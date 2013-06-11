/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ODB_C_API_H


/**
 * \file odbcapi.h
 *
 * @author Piotr Kuchta, March 2009
 *
 */

#if defined(__cplusplus) || defined(c_plusplus) || defined(SWIGPYTHON)
extern "C" {
#endif

typedef void* oda_ptr;
typedef void* oda_read_iterator_ptr;
typedef void* oda_select_iterator_ptr;

typedef void* oda_writer_ptr;
typedef void* oda_write_iterator_ptr;

//typedef void oda;
typedef void oda_read_iterator;
typedef void oda_select_iterator;

typedef void oda_writer;
typedef void oda_write_iterator;


void odb_start();

unsigned int odb_get_headerBufferSize();
void odb_set_headerBufferSize(unsigned int );

unsigned int odb_get_setvbufferSize();
void odb_set_setvbufferSize(unsigned int);

const char* odb_api_version();
const char* odb_api_git_sha1();
unsigned int odb_api_format_version_major();
unsigned int odb_api_format_version_minor();

oda_ptr odb_create(const char *, int *);
int odb_destroy(oda_ptr);

oda_read_iterator_ptr odb_create_read_iterator(oda_ptr, const char *, int *);
int odb_read_iterator_destroy(oda_read_iterator_ptr);
int odb_read_iterator_get_no_of_columns(oda_read_iterator_ptr, int*);
int odb_read_iterator_get_column_type(oda_read_iterator_ptr, int, int*);
int odb_read_iterator_get_column_name(oda_read_iterator_ptr, int, char**, int*);
int odb_read_iterator_get_bitfield(oda_read_iterator_ptr, int, char**, char**, int*, int*);
int odb_read_iterator_get_next_row(oda_read_iterator_ptr, int, double*, int*);

oda_select_iterator_ptr odb_create_select_iterator(oda_ptr, const char *, int *);
oda_select_iterator_ptr odb_create_select_iterator_from_file(oda_ptr, const char *, const char *, int *);
int odb_select_iterator_destroy(oda_select_iterator_ptr);
int odb_select_iterator_get_no_of_columns(oda_select_iterator_ptr, int*);
int odb_select_iterator_get_column_type(oda_select_iterator_ptr, int, int *);
int odb_select_iterator_get_column_name(oda_select_iterator_ptr, int, char **, int*);
int odb_select_iterator_get_bitfield(oda_select_iterator_ptr, int, char**, char**, int*, int*);
int odb_select_iterator_get_next_row(oda_select_iterator_ptr, int, double*, int*);

oda_writer_ptr odb_writer_create(const char *, int *);
int odb_writer_destroy(oda_writer_ptr);
int odb_write_iterator_destroy(oda_write_iterator_ptr);

oda_write_iterator_ptr odb_create_write_iterator(oda_writer_ptr, const char* , int *);
oda_write_iterator_ptr odb_create_append_iterator(oda_writer_ptr, const char* , int *);
int odb_write_iterator_set_no_of_columns(oda_write_iterator_ptr, int);

int odb_write_iterator_set_column(oda_write_iterator_ptr, int, int, const char *);
int odb_write_iterator_set_bitfield(oda_write_iterator_ptr, int, int, const char *, const char*, const char *);

int odb_write_iterator_set_missing_value(oda_write_iterator_ptr, int, double);

int odb_write_iterator_write_header(oda_write_iterator_ptr);
int odb_write_iterator_set_next_row(oda_write_iterator_ptr, double *, int);

// FIXME: This needs to be changed: return error code like all the rest of the functions
double odb_count(const char *);

int get_blocks_offsets(const char* fileName, size_t* numberOfBlocks, off_t** offsets, size_t** sizes);
int release_blocks_offsets(off_t**);
int release_blocks_sizes(size_t**);

#if defined(__cplusplus) || defined(c_plusplus) || defined(SWIGPYTHON)
}
#endif
#endif

