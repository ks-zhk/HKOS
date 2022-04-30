#include "rbtreeTest.h"
#include "assert.h"
#include <stdio.h>
#include <stdlib.h>
void rbtree_left_rotate(struct rbtree* rbtree, struct rbtree_elem* x){
    struct rbtree_elem* y = x->rson;
    x->rson = y->lson;
    if(y->lson != rbtree->nil){
        y->lson->fa = x;
    }
    y->fa = x->fa;
    if(x->fa == rbtree->nil){
        rbtree->root = y;
    }else if(x == x->fa->lson){
        x->fa->lson = y;
    }else{
        x->fa->rson = y;
    }
    y->lson = x;
    x->fa = y;

}
void rbtree_right_rotate(struct rbtree* rbtree, struct rbtree_elem* elem){
    struct rbtree_elem* y = elem->lson;
    elem->lson = y->rson;
    if(y->rson != (rbtree->nil)){
        y->rson->fa = elem;
    }
    y->fa = elem->fa;
    if(elem->fa == (rbtree->nil)){
        
        rbtree->root = y;
    }else if(elem == elem->fa->lson){
        
        elem->fa->lson = y;
    }else{
        elem->fa->rson = y;
    }
    y->rson = elem;
    elem->fa = y;
}
struct rbtree_elem* rbtree_find_min(struct rbtree* rbtree){
    struct rbtree_elem* now = (rbtree->root);
    while(now->lson != (rbtree->nil)){
        now = now -> lson;
    }
    return now;
    assert(now -> lson == (rbtree->nil));
}
struct rbtree_elem* rbtree_find_max(struct rbtree* rbtree){
    struct rbtree_elem* now = (rbtree->root);
    while(now->rson != (rbtree->nil)){
        now = now -> rson;
    }
    return now;
    assert(now -> rson == (rbtree->nil));
}
struct rbtree_elem* rbtree_find_pre(struct rbtree* rbtree,struct rbtree_elem* node){
    assert(node->lson != (rbtree->nil));
    struct rbtree_elem* now = node->lson;
    while(now -> rson != (rbtree->nil)){
        now = now -> rson;
    }
    assert(now->rson == (rbtree->nil));
    return now;
}//寻找前驱节点
struct rbtree_elem* rbtree_find_lat(struct rbtree* rbtree,struct rbtree_elem* node){
    assert(node->rson != (rbtree->nil));
    struct rbtree_elem* now = node ->rson;
    while(now->lson != (rbtree->nil)){
        now = now -> lson;
    }
    assert(now->lson == (rbtree->nil));
    return now;
}//寻找后继节点
int rbtree_search(struct rbtree* rbtree, struct rbtree_elem* elem){
    // printf("in search!\n");
    assert(elem != (rbtree->nil));
    struct rbtree_elem* now = (rbtree->root);
    while(elem != now && now != (rbtree->nil)){
        assert(elem->cmp != 0);
        if(elem->cmp(elem,now) == -1){
            //去左边找
            if(now->lson == rbtree->nil){
                return 0;
            }
            now = now->lson;
        }else{
            if(now->rson == rbtree->nil){
                return 0;
            }
            now = now->rson;
        }
    }
    if(now == elem){
        assert(now == elem);
        assert(now != rbtree->nil);
        return 1;
    }else{
        assert(now != elem);
        return 0;
    }
}//寻找节点
void rbtree_init(struct rbtree* rbtree){
    rbtree->nil = (struct rbtree_elem*)malloc(sizeof(struct rbtree_elem));
    rbtree->root = (struct rbtree_elem*)malloc(sizeof(struct rbtree_elem));
    rbtree->nil->color = BLACK;
    rbtree->nil->fa = (rbtree->nil);
    rbtree->nil->lson = (rbtree->nil);
    rbtree->nil->rson = (rbtree->nil);
    rbtree->nil->cmp = 0;
    rbtree->nil->value = (rbtree->nil);

    rbtree->root->lson = (rbtree->nil);
    rbtree->root->rson = (rbtree->nil);
    rbtree->root->color = BLACK;
    rbtree->root->fa = (rbtree->nil);
    rbtree->size = 0;
}
void rbtree_insert(struct rbtree* rbtree,struct rbtree_elem* elem){
    //重量级，插入节点
    //step1 找到要插入节点的位置
    struct rbtree_elem* now = (rbtree->root);
    elem->color = RED;
    elem->lson = (rbtree->nil);
    elem->rson = (rbtree->nil);
    // assert(now != 0);
    // printf("size = %d\n",rbtree->size);
    if((rbtree->size) == 0){
        //表示此时树为空
        elem->color = BLACK;
        elem->fa = (rbtree->nil);
        rbtree->root = elem;
        (rbtree->size)++;
        // printf("inin**********\n");
        return;
    }
    //printf("now->lson = %d and now->rson = %d\n",now->lson,now->rson);
    // assert((rbtree->size) > 0);
    // elem->color = RED;
    // elem->lson = (rbtree->nil);
    // elem->rson = (rbtree->nil);
    //printf("to be inserted is %d\n",*(int*)(elem->value));
    while(1){
        //printf("now = %d\n",*(int*)(now->value));
        //printf("in insert : now_value = %d\n",*(int*)(now->value));
        if(elem->cmp(elem,now) == -1){
            //printf("smaller\n");
            if(now->lson == (rbtree->nil)){
                now->lson = elem;
                elem->fa = now;
                break;
            }
            now = now->lson;
        }else{
            //printf("in bigger\n");
            if(now->rson == (rbtree->nil)){
                now->rson = elem;
                //printf("now->roson = elem\n");
                elem->fa = now;
                break;
            }
            //printf("nil = %d and now = %d and now_rson = %d\n",(rbtree->nil),now,now->rson);
            now = now->rson;
        }
    }
    rbtree_insert_fixup(rbtree,elem);
    rbtree->size++;
}
void rbtree_delete(struct rbtree* rbtree,struct rbtree_elem* elem){
    // printf("in delete\n");
    rbtree->size--;
    struct rbtree_elem* origin = elem;
    struct rbtree_elem* x;//要被移动的节点的原始位置
    struct rbtree_elem* y;//要被移动的节点
    enum COLOR y_origin_color = elem->color;
    if(elem->lson == (rbtree->nil)){
        x = elem->rson;
        rbtree_translate(rbtree,elem,elem->rson);
        
    }else if(elem->rson == (rbtree->nil)){
        x = elem->lson;
        rbtree_translate(rbtree,elem,elem->lson);
    }else{
        y = rbtree_find_lat(rbtree,elem);
        // printf("lat value = %d\n",*(int*)(y->value));
        assert(y->lson == (rbtree->nil));
        y_origin_color = y->color;
        x = y->rson;
        if(y->fa == elem)
            x->fa = y;
        else{
            rbtree_translate(rbtree,y,y->rson);
            y->rson = elem->rson;
            y->rson->fa = y;
        }
        rbtree_translate(rbtree,elem,y);
        y->lson = elem->lson;
        y->lson->fa = y;
        y->color = elem->color;
    }
    if(y_origin_color == BLACK){
        rbtree_delete_fixup(rbtree,x);
    }
    return;
}
void rbtree_insert_fixup(struct rbtree* rbtree, struct rbtree_elem* elem){
    //printf("in insert fixup to be insert = %d\n",*(int*)(elem->value));
    struct rbtree_elem* now = elem;
    while(now->fa->color == RED){
        assert(now->fa->color == RED);
        if(now == rbtree->root){
            break;
        }
        if(now->fa->fa->lson == now->fa){
            if(now->fa->fa->rson->color == RED){
                // printf("ininleft\n");
                now->fa->fa->color = RED;
                now->fa->color = BLACK;
                now->fa->fa->rson->color = BLACK;
                now = now->fa->fa;
                continue;
            }
            if(now->fa->rson == now){
                now = now->fa;
                rbtree_left_rotate(rbtree,now);
            }
            now->fa->color = BLACK;
            now->fa->fa->color = RED;
            rbtree_right_rotate(rbtree,now->fa->fa);
        }else{
            // printf("** in right fixup\n");
            if(now->fa->fa->lson->color == RED){
                // printf("** fa->fa->lson is RED right fixup\n");
                now->fa->fa->color = RED;
                now->fa->fa->lson->color = BLACK;
                now->fa->color = BLACK;
                now = now->fa->fa;
                continue;
            }
            if(now->fa->lson == now){
                now = now->fa;
                rbtree_right_rotate(rbtree,now);
            }
            now->fa->color = BLACK;
            now->fa->fa->color = RED;
            rbtree_left_rotate(rbtree,now->fa->fa);
        }
    }
    rbtree->root->color = BLACK;
    assert(rbtree->root->color = BLACK);
    assert(rbtree->root->fa = (rbtree->nil));
    return;
}
void rbtree_delete_fixup(struct rbtree* rbtree, struct rbtree_elem* elem){
    //超级重量级，删除后的重平衡
    // printf("in delete fixup\n");
    struct rbtree_elem* x = elem;
    while(x != (rbtree->root) && x->color == BLACK){
        assert(x->color == BLACK);
        assert(x != (rbtree->root));
        if(x->fa->lson == x){
            struct rbtree_elem* w = x->fa->rson;
            if(w->color == RED){
                if(w->lson->color == RED || w->rson->color == RED){
                    printf("error in left\n");
                    exit(0);
                }
                w->color = BLACK;
                x->fa->color = RED;
                rbtree_left_rotate(rbtree,x->fa);
                w = x->fa->rson;
            }
            if(w->lson->color == BLACK && w->rson->color == BLACK){
                w->color = RED;
                x = x->fa;
                continue;
            }else if(w->rson->color == BLACK){
                w->lson->color = BLACK;
                w->color = RED;
                rbtree_right_rotate(rbtree,w);
                w = x->fa->rson;
            }
            w->color = x->fa->color;
            x->fa->color = BLACK;
            w->rson->color = BLACK;
            rbtree_left_rotate(rbtree,x->fa);
            x = rbtree->root;
        }else{
            struct rbtree_elem* w = x->fa->lson;
            if(w->color == RED){
                if(w->lson->color == RED || w->rson->color == RED){
                    printf("error in right\n");
                    exit(0);
                }
                w->color = BLACK;
                x->fa->color = RED;
                rbtree_right_rotate(rbtree,x->fa);
                w = x->fa->lson;
            }
            if(w->lson->color == BLACK && w->rson->color == BLACK){
                w->color = RED;
                x = x->fa;
                continue;
            }else if(w->lson->color == BLACK){
                //printf("case3 -> case4 \n");
                //printf("x->fa->value = %d\n",*(int*)(x->fa->value));
                w->rson->color = BLACK;
                w->color = RED;
                assert(w->fa->lson == w || w->fa->rson == w);
                rbtree_left_rotate(rbtree,w);
                //printf("after left x->fa->value = %d\n",*(int*)(x->fa->value));
                w = x->fa->lson;
            }
            //printf("case4\n");
            //printf("1root in fixup = %d\n",*(int*)(rbtree->root->value));
            w->color = x->fa->color;
            //("2root in fixup = %d\n",*(int*)(rbtree->root->value));
            //printf("x->fa->value = %d\n",*(int*)(x->fa->value));
            x->fa->color = BLACK;
            //printf("3root in fixup = %d\n",*(int*)(rbtree->root->value));
            w->lson->color = BLACK;
            //printf("4root in fixup = %d\n",*(int*)(rbtree->root->value));
            //printf("x->fa->value = %d\n",*(int*)(x->fa->value));
            rbtree_right_rotate(rbtree,x->fa);
            //printf("x->fa->value = %d\n",*(int*)(x->fa->value));
            //printf("5root in fixup = %d\n",*(int*)(rbtree->root->value));
            x = rbtree->root;
            //printf("6root in fixup = %d\n",*(int*)(rbtree->root->value));
        }
    }
    x->color = BLACK;
}
void rbtree_translate(struct rbtree* rbtree,struct rbtree_elem* x,struct rbtree_elem* y){
    if(x->fa == (rbtree->nil))
        rbtree->root = y;
    else if(x == x->fa->lson){
        x->fa->lson = y;
    }else{
        x->fa->rson = y;
    }
    y->fa = x->fa;
    // assert(rbtree->root->fa == (rbtree->nil));
    // assert(x->lson->fa == y);
    // assert(x->rson->fa == y);
    // assert(x->fa == y->fa);
}