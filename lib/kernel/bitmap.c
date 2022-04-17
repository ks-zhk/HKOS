#include "bitmap.h"
#include "stdint.h"
#include "global.h"
#include "string.h"
#include "interrupt.h"
#include "debug.h"
void bitmap_init(struct bitmap* btmp){
    memset(btmp ->bits,0,btmp->btmp_bytes_len);
}
uint8_t bitmap_scan_test(struct bitmap* btmp,uint32_t bit_idx){
    uint32_t byte_idx = (bit_idx >> 3);//right 3 bit = divide 8
    uint8_t byte_bit_idx = (uint8_t)(bit_idx - (byte_idx << 3));
    return ((btmp ->bits)[byte_idx] & (BITMAP_MASK << byte_bit_idx));
}
int bitmap_scan(struct bitmap* btmp,uint32_t cnt){
    //require for cnt bits
    //first find the byte != 0xff
    uint32_t byte_idx = 0;
    while((0xff == btmp ->bits[byte_idx]) && (byte_idx < btmp ->btmp_bytes_len)){
        byte_idx++;
    }
    ASSERT(byte_idx < btmp -> btmp_bytes_len);
    if(byte_idx == btmp -> btmp_bytes_len){
        return -1;
    }

    uint32_t bit_idx = 0;
    while((uint8_t)(BITMAP_MASK << bit_idx) & btmp ->bits[byte_idx]){
        bit_idx++;
    }

    int bit_idx_start = (byte_idx << 3) + bit_idx;
    if(cnt == 1) return bit_idx_start;

    uint32_t bit_left = (((btmp -> btmp_bytes_len) << 3) - bit_idx_start);
    uint32_t next_bit = bit_idx_start + 1;
    uint32_t count = 1;
    bit_idx_start = -1;
    while(bit_left-- > 0){
        if(!(bitmap_scan_test(btmp,next_bit))){
            count++;
        }else{
            count = 0;
        }
        if (count == cnt)
        {
            bit_idx_start = next_bit - cnt + 1;
            return bit_idx_start;
        }
        next_bit++;
        
    }
    return bit_idx_start;
}
void bitmap_set(struct bitmap* btmp,uint32_t idx,uint8_t value){
    ASSERT(value==0 || value == 1);
    uint32_t byte_idx = (idx >> 3);
    uint8_t bit_odd = (idx - (byte_idx << 3));
    // ASSERT(byte_idx <= btmp->btmp_bytes_len);
    if(value){
        btmp -> bits[byte_idx] |= (BITMAP_MASK << bit_odd);
    }else{
        btmp -> bits[byte_idx] &= ~(BITMAP_MASK << bit_odd);
    }
}