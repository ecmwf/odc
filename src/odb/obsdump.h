/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef _OBSDUMP_H
#define _OBSDUMP_H

typedef struct obsdump obsdump;

int      obsdump_close(obsdump*);


#define OBSDUMP_IGNORE  0
#define OBSDUMP_INTEGER 1
#define OBSDUMP_REAL    2
#define OBSDUMP_STRING  3

/* dump */

obsdump* obsdump_create(const char* path);

int obsdump_pass1_init(obsdump* dump,int no_of_colums);
int obsdump_pass1_values(obsdump* dump,const double* values,int count);
int obsdump_pass1_end(obsdump* dump);

int obsdump_pass2_init(obsdump* dump);
int obsdump_pass2_values(obsdump* dump,const double* values,int count);
int obsdump_pass2_end(obsdump* dump);

int obs_dump_set_column(obsdump* dump,int index,const char* name,int type,double missing_value);


/* load */

obsdump* obsdump_open(const char* path);
int obs_dump_get_no_of_columns(obsdump* dump);
int obs_dump_get_column_type(obsdump* dump,int index);
const char* obs_dump_get_column_name(obsdump* dump,int index);
double obs_dump_get_column_missing_value(obsdump* dump,int index);
int obs_dump_get_next_row(obsdump* dump,double* data,int count);


#endif
