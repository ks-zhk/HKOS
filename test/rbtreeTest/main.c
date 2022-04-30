#include "rbtreeTest.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#define MAXN 1200
void preVis(struct rbtree* rbtree,struct rbtree_elem* now);
int cmp(struct rbtree_elem* x, struct rbtree_elem* y);
void bfs(struct rbtree* rbtree);
int check(struct rbtree* rbtree,struct rbtree_elem* now);
struct Num{
    int w;
    struct rbtree_elem node;
};
int n,x,d;
struct rbtree rbtree;
struct rbtree_elem* data[MAXN];
struct rbtree_elem* q[MAXN];
int vis[MAXN];
int c[MAXN];
FILE* out;
int main(void){
    srand(time(NULL));
    freopen("data.in","r",stdin);
    freopen("res.out","w",stdout);
    out = fopen("check.out","w");
    rbtree_init(&rbtree);
    scanf("%d",&n);
    int cnt = 0;
    while(n--){
        scanf("%d",&x);
        struct Num* num = ((struct Num*)malloc(sizeof(struct Num)));
        num->w = x;
        num->node.cmp = cmp;
        num->node.value = (void*)(&(num->w));
        num->node.color = RED;
        data[cnt++] = &(num->node);
        rbtree_insert(&rbtree,&(num->node));
        check(&rbtree,rbtree.root);
        if(rbtree.root->color == RED){
            printf("root error!\n");
            exit(0);
        }
        // bfs(&rbtree);
        // printf("\n\n");
        // printf("insert ok\n");
        // printf("root_value = %d\n",*(int*)(rbtree.root->value));
    }
    preVis(&rbtree,(rbtree.root));
    bfs(&rbtree);
    printf("ok\n");
    scanf("%d",&d);
    // printf("d = %d\n",d);
    int tobedel;
    for(int i=1;i<=d;i++){
        scanf("%d",&tobedel);
        // printf("root = %d\n",*(int*)(rbtree.root->value));
        if(vis[tobedel]==0 && rbtree_search(&rbtree,data[tobedel])){
            //printf("del  %d\n",*(int*)(data[tobedel]->value));
            rbtree_delete(&rbtree,data[tobedel]);
            vis[tobedel] = 1;
            bfs(&rbtree);
            check(&rbtree,rbtree.root);
        }else{
            if(vis[tobedel] && rand()%2 == 0 && !rbtree_search(&rbtree,data[tobedel])){
                //printf("insert  %d\n",*(int*)(data[tobedel]->value));
                vis[tobedel] = 0;
                rbtree_insert(&rbtree,data[tobedel]);
                
                bfs(&rbtree);
                check(&rbtree,rbtree.root);
            }
        }
        //printf("\n");
    }
    bfs(&rbtree);
    // printf("\n");
    // preVis(&rbtree,(rbtree.root));
    fclose(out);
    return 0;
}
void preVis(struct rbtree* rbtree,struct rbtree_elem* now){
    if(now == (rbtree->nil)){
        return;
    }
    // printf("%p\n",now);
    preVis(rbtree,now->lson);
    // printf("%d ",*((int*)(now->value)));
    preVis(rbtree,now->rson);
    return;
}
void bfs(struct rbtree* rbtree){
    int front = 0;
    int tail = 0;
    int ceng = 1;
    c[tail] = 1;
    q[tail] = rbtree->root;
    tail++;
    int pre = 0;
    while(front < tail){
        struct rbtree_elem* now = q[front];
        if(c[front] > pre){
            fprintf(out,"\n");
        }
        // // printf("ceng = %d  ",c[front]);
        // printf("%c:%d  ",((now->color == RED)?'R':'B'),*(int*)(now->value));
        fprintf(out,"%c:%d  ",((now->color == RED)?'R':'B'),*(int*)(now->value));
        if(now->lson != rbtree->nil){
            if(now->color == RED){
                if(now->lson->color == RED){
                    printf("in bfs lson\n");
                }
            }
            q[tail] = now->lson;
            c[tail] = c[front] + 1;
            tail++;
        }
        if(now->rson != rbtree->nil){
            if(now->color == RED){
                if(now->rson->color == RED){
                    printf("in bfs rson\n");
                }
            }
            q[tail] = now->rson;
            c[tail] = c[front] + 1;
            tail++;
        }

        pre = c[front];
        front++;
    }
    fprintf(out,"\n\n");
    return;
}
int check(struct rbtree* rbtree,struct rbtree_elem* now){
    if(now == rbtree->nil){
        return 1;
    }else{
        int lsum = check(rbtree,now->lson);
        int rsum = check(rbtree,now->rson);
        if(lsum == rsum){
            if(now->color == BLACK){
                return lsum + 1;
            }else{
                return lsum;
            }
        }else{
            printf("in check error\n");
            exit(0);
        }
    }
}
int cmp(struct rbtree_elem* x, struct rbtree_elem* y){
    // printf("in cmp\n");
    if(*((int*)(x->value)) < *((int*)(y->value))){
        // printf("out cmp\n");
        return -1;
    }else{
        // printf("out cmp\n");
        return 1;
    }
}