/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/* File md5.h
 * Baudouin Raoult - (c) ECMWF Sep 11
 */

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
