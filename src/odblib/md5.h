// File md5.h
// Baudouin Raoult - (c) ECMWF Sep 11

#ifndef md5_H
#define md5_H

#include <stdlib.h>
#include <stdint.h>


typedef struct md5_state {
    uint64_t size;

    unsigned long words[64];
    unsigned long word_count;

    unsigned char bytes[4];
    unsigned long byte_count;

    unsigned long h0;
    unsigned long h1;
    unsigned long h2;
    unsigned long h3;

} md5_state;

void md5_init(md5_state* s);
void md5_add(md5_state* s,const void* data,size_t len);
void md5_end(md5_state* s, char *digest);



#endif
