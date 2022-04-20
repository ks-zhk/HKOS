#include "list.h"
#include "interrupt.h"
#include "stdint.h"
#include "console.h"
#include "debug.h"
#define NULL 0

#define elem_pre(elem) (elem->pre)
void list_init(struct list* plist){
    plist->head.pre = NULL;
    plist->head.nxt = &(plist->tail);
    plist->tail.pre = &(plist->head);
    plist->tail.nxt = NULL;
}
void list_insert_before(struct list_elem* before,struct list_elem* elem){
    enum intr_status old_status = intr_disable();
    before->pre->nxt = elem;
    elem->pre = before->pre;
    elem->nxt = before;
    before->pre = elem;

    intr_set_status(old_status);
}
void list_push(struct list* plist,struct list_elem* elem){
    list_insert_before(plist->head.nxt,elem);
}
void list_iterate(struct list* plist){
    return;
}
void list_append(struct list* plist,struct list_elem* elem){
    list_insert_before(&plist->tail,elem);
}
void list_remove(struct list_elem* pelem){
    enum intr_status old_status = intr_disable();
    pelem->pre->nxt = pelem->nxt;
    pelem->nxt->pre = pelem->pre;
    intr_set_status(old_status);
}
struct list_elem* list_pop(struct list* plist){
    struct list_elem* elem = plist->head.nxt;
    list_remove(elem);
    return elem;
}
uint8_t list_empty(struct list* plist){
    if(plist->head.nxt == &plist->tail) return 1;
    return 0;
}
uint32_t list_len(struct list* plist){
    uint32_t length = 0;
    struct list_elem* tmp = plist->head.nxt;
    while(tmp != &plist->tail){
        tmp = tmp->nxt;
        length++;
    }
    return length;
}
struct list_elem* list_traversal(struct list* plist,function func,int arg){
    if(list_empty(plist)) return NULL;
    struct list_elem* elem = plist->head.nxt;
    while(elem != &plist->tail){
        if(func(elem,arg)) return elem;
        elem = elem->nxt;
    }
    return NULL;
}
uint8_t elem_find(struct list* plist,struct list_elem* objelem){
    ASSERT(plist->tail.nxt == NULL);
    struct list_elem* elem = plist->head.nxt;
    while(elem != &plist->tail){
        if(elem == objelem) return 1;
        elem = elem -> nxt;
    }
    return 0;
}