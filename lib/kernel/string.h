#ifndef __KERNEL_STRING_H__
#define __KERNEL_STRING_H__
#include "stdint.h"
//set dst's size bytes with value
void memset(void* dst_,uint8_t value,uint32_t size);
//copy src_'s size bytes to dst_
void memcpy(void* dst_,void* src_,uint32_t size);
//compare a with b (size bytes),if a > b return 1 if a==b return 0 else return -1;
int memcmp(const void* a_,const void* b_,uint32_t size);

//copy src_'s bytes until 0 to dst
char* strcpy(char* dst_,const char* src_);

//return the count of bytes of str until 0x00
uint32_t strlen(const char* str);
//compare a with b (count of bytes and byte value)
int8_t  strcmp(const char* a,const char* b);
//find the first pos of ch in str_a
char* strchr(const char* a,const uint8_t ch);

//find the last pos of ch in str_a
char* strrchr(const char* a,const uint8_t ch);

//connect dst with src(dst pre,src later);
char* strcat(char* dst_,const char* src_);

//count the numbers of occurances of ch in str
uint32_t strchrs(const char* a,const uint8_t ch);


#endif