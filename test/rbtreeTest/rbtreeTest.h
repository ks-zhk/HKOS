#ifndef __LIB__KERNEL__RBTREE_H__
#define __LIB__KERNEL__RBTREE_H__
/*
    规定比较
*/
#include "stdint.h"
#define rb_offset(struct_type,member) (int)(&((struct_type*)0)->member)
#define rb_elem2entry(struct_type,struct_member_name,elem_ptr) (struct_type*)((int)elem_ptr - rb_offset(struct_type,struct_member_name))
// #define rb_get_weight(struct_type,struct_member_name,elem_ptr,struct_weight_type,struct_weight_name) \
// (struct_weight_type*)((int)rb_elem2entry(struct_type,struct_member_name,elem_ptr) + rb_offset(struct_type,struct_weight_name))
// #define rb_get_cmpfunc(struct_type,struct_member_name,elem_ptr,member)
// #define rb_getweight(struct_type,struct_member_name,elem_ptr,struct_w_type,struct_w_name) \
// (struct_w_type*)(((int)rb_elem2entry(struct_type,struct_member_name,elem_ptr) + rb_offset(struct_type,struct_w_name)))
// //找出比较值。
// #define rb_getcmp(struct_type,struct_member_name,elem_ptr,function_cmp_type,function_cmp_name) \
// (function_cmp_type*)(((int)rb_elem2entry(struct_type,struct_member_name,elem_ptr) + rb_offset(struct_type,function_cmp_name)))
//找出比较函数的函数指针
enum COLOR{
    RED,
    BLACK
};
struct rbtree_elem{
    enum COLOR color;//红黑树节点的颜色
    struct rbtree_elem *fa;
    struct rbtree_elem *lson;//左孩子
    struct rbtree_elem *rson;//右孩子
    int (*cmp)(struct rbtree_elem *x,struct rbtree_elem *y);//指向比较函数，-1表示x<y,其他(0,1)表示 x>=y
    void* value;//指向权重数据。
};
struct rbtree{
    struct rbtree_elem* root;//root表示红黑树的根节点
    struct rbtree_elem* nil;
    uint32_t size;
};
void rbtree_left_rotate(struct rbtree* rbtree, struct rbtree_elem* elem);
void rbtree_right_rotate(struct rbtree* rbtree, struct rbtree_elem* elem);
struct rbtree_elem* rbtree_find_min(struct rbtree* rbtree);
struct rbtree_elem* rbtree_find_max(struct rbtree* rbtree);
struct rbtree_elem* rbtree_find_pre(struct rbtree* rbtree,struct rbtree_elem* node);//寻找前驱节点
struct rbtree_elem* rbtree_find_lat(struct rbtree* rbtree,struct rbtree_elem* node);//寻找后继节点
int rbtree_search(struct rbtree* rbtree, struct rbtree_elem* elem);//寻找节点
void rbtree_init(struct rbtree* rbtree);
void rbtree_insert(struct rbtree* rbtree,struct rbtree_elem* elem);
void rbtree_delete(struct rbtree* rbtree,struct rbtree_elem* elem);
void rbtree_insert_fixup(struct rbtree* rbtree, struct rbtree_elem* elem);
void rbtree_delete_fixup(struct rbtree* rbtree, struct rbtree_elem* elem);
void rbtree_translate(struct rbtree* rbtree,struct rbtree_elem* x,struct rbtree_elem* y);//用Y元素覆盖X元素，并且使得X消失。
#endif