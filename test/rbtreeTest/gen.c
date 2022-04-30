#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
int n,d;
int main(void){
    freopen("data.in","w",stdout);
    srand(time(NULL));
    n = 50;
    printf("%d\n",n);
    for(int i=1;i<=n;i++){
        printf("%d ",rand()%200);
    }
    printf("\n");
    d = rand()%n + 1;
    printf("%d\n",d);
    for(int i=1;i<=d;i++){
        printf("%d ",rand()%n);
    }
    return 0;
}