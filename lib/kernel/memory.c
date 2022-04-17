#include "memory.h"
#include "stdint.h"
#include "print.h"
#include "debug.h"

#define PAGE_SIZE 4096
#define NULL 0
//一页的大小为4096byte
#define MEM_BITMAP_BASE 0xc009a000
//0xc009a000 - 0xc009dfff 一共4页，存储位图
#define K_HEAP_START 0xc0100000
//将0xc0100000 这个虚拟地址作为虚拟内存池的起始地址

#define PDE_IDX(addr) ((addr & 0xffc00000) >> 22)
//get idx of PDE
#define PTE_IDX(addr) ((addr & 0x003ff000) >> 12)
//get idx of PTE
struct pool{
    struct bitmap pool_bitmap;
    //物理地址位图
    uint32_t phy_addr_start;
    //物理内存池起始地址
    uint32_t pool_size;
    //内存池大小
};
struct pool kernel_pool,user_pool;
//物理内存，内核池子以及用户池子
struct virtual_addr kernel_vaddr;

static void mem_pool_init(uint32_t all_mem){
    put_str("   mem_pool_init start\n");
    uint32_t page_table_size = PAGE_SIZE * 256;
    //页表占用的内存数量
    uint32_t used_mem = page_table_size + 0x100000;
    //已用低端1MB内存，以及页表占用的内存，这两个内存是连续的，且是物理上连续。
    uint32_t free_mem = all_mem - used_mem;
    //可用的物理内存数量 = 总物理内存量 - 已用物理内存量
    uint16_t all_free_pages = free_mem / PAGE_SIZE;
    //计算总空闲页

    uint16_t kernel_free_pages = all_free_pages >> 1;
    //内核占用一半空闲页
    uint16_t user_free_pages = all_free_pages - kernel_free_pages;
    //用户进程占用一半空闲页

    uint32_t kbm_length = kernel_free_pages >> 3;
    //位图中的一位管理一页，因此除8（右移3位）可得位图需要的字节数量
    uint32_t ubm_length = user_free_pages >> 3;
    //用户位图同理

    uint32_t kp_start = used_mem;
    //内核内存池物理起始地址从已用页表开始
    uint32_t up_start = kp_start + PAGE_SIZE*kernel_free_pages;
    //用户内存池物理地址从内核占用的所有空间开始
    kernel_pool.phy_addr_start = kp_start;
    user_pool.phy_addr_start = up_start;

    kernel_pool.pool_size = PAGE_SIZE*kernel_free_pages;
    user_pool.pool_size = PAGE_SIZE*user_free_pages;
    
    kernel_pool.pool_bitmap.btmp_bytes_len = kbm_length;
    user_pool.pool_bitmap.btmp_bytes_len = ubm_length;

    kernel_pool.pool_bitmap.bits = (void *)MEM_BITMAP_BASE;
    user_pool.pool_bitmap.bits = (void *)(MEM_BITMAP_BASE + kbm_length);


    put_str("   ksernel_pool_bitmap start\n");
    put_int((uint32_t)kernel_pool.pool_bitmap.bits);
    put_str("\nkernel_pool_phy_addr_start:");
    put_int((uint32_t)kernel_pool.phy_addr_start);
    put_char('\n');

    put_str("   user_pool_bitmap start\n");
    put_int((uint32_t)user_pool.pool_bitmap.bits);
    put_str("\nuser_pool_phy_addr_start:");
    put_int((uint32_t)user_pool.phy_addr_start);
    put_char('\n');

    bitmap_init(&kernel_pool.pool_bitmap);
    bitmap_init(&user_pool.pool_bitmap);

    kernel_vaddr.vaddr_bitmap.btmp_bytes_len = kbm_length;
    kernel_vaddr.vaddr_bitmap.bits = \
    (void*)(MEM_BITMAP_BASE + kbm_length + ubm_length);

    kernel_vaddr.vaddr_start = K_HEAP_START;
    bitmap_init(&kernel_vaddr.vaddr_bitmap);
    put_str("   mem_pool_init done");
}
static void * vaddr_get(enum pool_flags pf, uint32_t pg_cnt){
    int vaddr_start = 0, bit_idx_start = -1;
    uint32_t cnt = 0;
    if(pf == PF_KERNEL){
        bit_idx_start = bitmap_scan(&kernel_vaddr.vaddr_bitmap,pg_cnt);
        if(bit_idx_start == -1) return NULL;
        while(cnt < pg_cnt){
            bitmap_set(&kernel_vaddr.vaddr_bitmap,bit_idx_start + cnt++,1);
        }
        vaddr_start = kernel_vaddr.vaddr_start + bit_idx_start*PAGE_SIZE;
    }else{
        //to be ok
    }
    return (void*)vaddr_start;
}
uint32_t* pte_ptr(uint32_t vaddr){
    uint32_t* pte = (uint32_t*)(0xffc00000 + ((vaddr & 0xffc00000) >> 10)+ (PTE_IDX(vaddr) << 2));
    return pte;
}

uint32_t* pde_ptr(uint32_t vaddr){
    uint32_t *pde = (uint32_t*)(0xfffff000 + (PDE_IDX(vaddr) << 2));
    return pde;
}
static void* palloc(struct pool* m_pool){
    int bit_idx = bitmap_scan(&m_pool->pool_bitmap,1);
    if(bit_idx == -1) return NULL;

    bitmap_set(&m_pool->pool_bitmap,bit_idx,1);
    uint32_t page_phyaddr = ((bit_idx*PAGE_SIZE) + m_pool->phy_addr_start);
    return (void*)page_phyaddr;
}
static void page_table_add(void* _vaddr,void* _page_phyaddr){
    uint32_t vaddr = (uint32_t)_vaddr,page_phyaddr = (uint32_t)(_page_phyaddr);
    uint32_t* pde = pde_ptr(vaddr);
    uint32_t* pte = pte_ptr(vaddr);

    if( *pde & 0x00000001){

        ASSERT(!(*pte & 0x00000001));
        if(!(*pte & 0x00000001)){
            *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
        }else{
            PANIC("pte repeat");
            *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
        }
    }else{
        uint32_t pde_phyaddr = (uint32_t)palloc(&kernel_pool);
        *pde = (pde_phyaddr | PG_US_U | PG_RW_W | PG_P_1);

        memset((void*)((int)pte & 0xfffff000),0,PAGE_SIZE);
        ASSERT(!(*pte & 0x00000001));
        *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
    }
}
void* malloc_page(enum pool_flags pf,uint32_t pg_cnt){
    ASSERT(pg_cnt > 0 && pg_cnt < 3840);
    void* vaddr_start = vaddr_get(pf,pg_cnt);
    if(vaddr_start == NULL){
        return NULL;
    }
    uint32_t vaddr = (uint32_t)vaddr_start,cnt = pg_cnt;
    struct pool* mem_pool = pf & PF_KERNEL? &kernel_pool: &user_pool;
    while(cnt-->0){
        void* page_phyaddr = palloc(mem_pool);
        if(page_phyaddr == NULL){
            return NULL;
        }
        page_table_add((void*)vaddr,page_phyaddr);
        vaddr += PAGE_SIZE;
    }
    return vaddr_start;
}
void* get_kernel_pages(uint32_t pg_cnt){
    void* vaddr = malloc_page(PF_KERNEL,pg_cnt);
    if(vaddr != NULL){
        memset(vaddr,0,pg_cnt*PAGE_SIZE);
    }
    return vaddr;
}
void mem_init(void){
    put_str("mem init start\n");
    uint32_t mem_bytes_total = *((uint32_t*)(0xb00));
    mem_pool_init(mem_bytes_total);
    put_str("mem_init done\n");
}