#include "string.h"
#include "stdint.h"
#include "debug.h"
//set dst's size bytes with value
void memset(void* dst_,uint8_t value,uint32_t size){
    ASSERT(dst_ != NULL);
    uint8_t* dst = (uint8_t*)dst_;
    while(size-- > 0){
        *dst++ = value;
    }
    return;
}

//copy src_'s size bytes to dst_
void memcpy(void* dst_,void* src_,uint32_t size){
    ASSERT(dst_ != NULL && src_ != NULL);
    uint8_t* dst = (uint8_t*)dst_;
    uint8_t* src = (uint8_t*)src_;
    while(size-- > 0){
        *dst++ = *src++;
    }
    return;
}
//compare a with b (size bytes),if a > b return 1 if a==b return 0 else return -1;
int memcmp(const void* a_,const void* b_,uint32_t size){
    ASSERT(a_ != NULL && b_ != NULL);
    uint8_t* a = (uint8_t*)a_;
    uint8_t* b = (uint8_t*)b_;
    while(size-- > 0){
        if(*a > *b) return 1;
        if(*a < *b) return -1;
        a++;b++;
    }
    return 0;
}

//copy src_'s bytes until 0 to dst
char* strcpy(char* dst_,const char* src_){
    ASSERT(dst_ != NULL && src_ != NULL);
    uint8_t* dst = (uint8_t*)dst_;
    const uint8_t* src = (const uint8_t*)src_;
    while((*src) != 0){
        *dst++ = *src++;
    }
    *dst = 0;
    return;
}

//return the count of bytes of str until 0x00
uint32_t strlen(const char* str){
    ASSERT(str != NULL);
    const char* strt = str;
    while((*strt++));
    return (strt - str - 1);
}

//compare a with b (count of bytes and byte value)
int8_t strcmp(const char* a,const char* b){
    ASSERT(a != NULL && b != NULL);
    while(*a != 0 && *a == *b){
        a++;b++;
    }
    return (*a < *b) ? -1 : (*a > *b);
}

//find the first pos of ch in str_a
char* strchr(const char* a,const uint8_t ch){
    ASSERT(a != NULL);
    while(*a != 0){
        if(*a == ch) return (char *)a;
        a++;
    }
    return NULL;
}

//find the last pos of ch in str_a
char* strrchr(const char* a,const uint8_t ch){
    ASSERT(a != NULL);
    char* ans = NULL;
    while(*a != 0){
        if(*a == ch) ans = (char *)a;
        a++;
    }
    return ans;
}

//connect dst with src(dst pre,src later);
char* strcat(char* dst_,const char* src_){
    ASSERT(dst_ != NULL && src_ != NULL);
    char * ans = dst_;
    while(*dst_ != 0){
        dst_++;
    }
    while((*dst_++ = *src_++));
    return ans;
}

//count the numbers of occurances of ch in str
uint32_t strchrs(const char* a,const uint8_t ch){
    ASSERT( a != NULL);
    uint32_t cnt = 0;
    while(*a != 0){
        if(*a == ch){
            cnt++;
        }
        a++;
    }
    return cnt;
}
