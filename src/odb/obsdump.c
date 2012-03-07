/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "obsdump.h"

#define MEGA(x) (x*1024*1024)
#define SIZE    ((int)65535)


typedef struct hashrec {
	struct hashrec  *next;
	char   *name;
	int    cnt;
	int index;
} hashrec;

typedef struct column column;
typedef struct codec  codec;


struct column {

	obsdump *owner;

	char *name;
	int type;

	double missing_value;

	codec* coder;

	int has_missing;
	double min;
	double max;
};

struct obsdump {
	hashrec* table[SIZE];
	FILE*    f;
	char *path;

	column* columns[1024];

	int next_index;
	int nrows;
	unsigned char* buffer;
	double* last_values;
	int no_of_columns;
	char *io_buffer;

	char** strings;
};

static int hash(const char *name)
{
	int n = 0;

	while(*name)
		n +=  (*name++ - 'A') + (n << 5);

	if(n<0)  {
		int m = -n/SIZE;
		n += (m+1)*SIZE;
	}
	return n % SIZE;
}


static void store(obsdump* dump,const char *name)
{
	hashrec *h;
	int      n;


	n = hash(name);
	h = dump->table[n];

	while(h)
	{
		if(strcmp(h->name,name) == 0)
		{
			h->cnt++;
			return;
		}
		h = h->next;
	}

	h        = calloc(sizeof(hashrec),1);
	h->name  = strdup(name);
	h->next  = dump->table[n];
	h->cnt   = 1;
	h->index = dump->next_index++;
	dump->table[n] = h;


}

static int find_index(obsdump *dump,const char *name)
{
	hashrec *h;
	int      n;


	n = hash(name);
	h = dump->table[n];

	while(h)
	{
		if(strcmp(h->name,name) == 0)
			return h->index;
		h = h->next;
	}

	printf("[%s] not in hash\n",name);
	abort();

}

static void dump_table(obsdump* dump)
{
	int      i;

	for(i = 0; i < SIZE; i++)
	{
		hashrec *h  = dump->table[i];

		while(h)
		{
			printf("[%s] -> %d %d\n",h->name,h->cnt,h->index);
			h = h->next;
		}
	}
}

static void write_table(obsdump* dump)
{
	int i;
	int n = 0;
	int len;


	assert(fwrite(&dump->next_index,sizeof(dump->next_index),1,dump->f) == 1);


	for(i = 0; i < SIZE; i++)
	{
		hashrec *h  = dump->table[i];

		while(h)
		{
			int len;
			len = strlen(h->name);
			assert(fwrite(&len,sizeof(len),1,dump->f) == 1);
			assert(fwrite(h->name,1,len,dump->f) == len);
			assert(fwrite(&h->cnt,sizeof(h->cnt),1,dump->f) == 1);
			assert(fwrite(&h->index,sizeof(h->index),1,dump->f) == 1);
			n++;
			h = h->next;
		}
	}

	assert(n == dump->next_index);
}

static void read_table(obsdump* dump)
{
	int i;
	int n = 0;
	int len;
	hashrec* h;

	assert(fread(&dump->next_index,sizeof(dump->next_index),1,dump->f) == 1);

	dump->strings = calloc(sizeof(char*),dump->next_index);

	for(i = 0; i < dump->next_index; i++)
	{
		int len;
		int cnt, index;
		char buf[1024];

		assert(fread(&len,sizeof(len),1,dump->f) == 1);
		assert(fread(buf,1,len,dump->f) == len);
		buf[len]  = 0;

		assert(fread(&cnt,sizeof(cnt),1,dump->f) == 1);
		assert(fread(&index,sizeof(index),1,dump->f) == 1);

		assert(index < dump->next_index);
		dump->strings[index] = strdup(buf);
		assert(dump->strings[index]);

	}

}

/*=====================*/


static unsigned char* encode_short_real(unsigned char* p,double d,column* col)
{
	float s = d;
	memcpy(p,&s,sizeof(s));
	return p+sizeof(s);
}

static double decode_short_real(column* col)
{
	float s;
	assert(fread(&s,sizeof(s),1,col->owner->f) == 1);
	return s;
}

static unsigned char* encode_int32(unsigned char* p,double d,column* col)
{
	/* unsigned long s = d; */
	long s = d;
	memcpy(p,&s,sizeof(s));
	return p+sizeof(s);
}

static double decode_int32(column* col)
{
	/* unsigned long s; */
	long s;
	assert(fread(&s,sizeof(s),1,col->owner->f) == 1);
	return s;
}

static unsigned char* encode_int16(unsigned char* p,double d,column* col)
{
	unsigned short s = d-col->min;
	memcpy(p,&s,sizeof(s));
	return p+sizeof(s);
}

static double decode_int16(column* col)
{
	unsigned short s;
	assert(fread(&s,sizeof(s),1,col->owner->f) == 1);
	return s+col->min;
}

static unsigned char* encode_int16_missing(unsigned char* p,double d,column* col)
{
	unsigned short s = (d==col->missing_value)?0xffff:d-col->min;
	memcpy(p,&s,sizeof(s));
	return p+sizeof(s);
}

static double decode_int16_missing(column* col)
{
	unsigned short s;
	assert(fread(&s,sizeof(s),1,col->owner->f) == 1);
	return s == 0xffff? (col->missing_value) : (s+col->min);
}

static unsigned char* encode_int8(unsigned char* p,double d,column* col)
{
	unsigned char s = d-col->min;
	memcpy(p,&s,sizeof(s));
	return p+sizeof(s);
}

static double decode_int8(column* col)
{
	unsigned char s;
	assert(fread(&s,sizeof(s),1,col->owner->f) == 1);
	return s+col->min;
}

static unsigned char* encode_int8_missing(unsigned char* p,double d,column* col)
{
	unsigned char s = (d==col->missing_value)?0xff:d-col->min;
	memcpy(p,&s,sizeof(s));
	return p+sizeof(s);
}

static double decode_int8_missing(column* col)
{
	unsigned char s;
	assert(fread(&s,sizeof(s),1,col->owner->f) == 1);
	return s == 0xff ? col->missing_value : s+col->min;
}

static unsigned char* encode_int8_string(unsigned char* p,double d,column* col)
{
	char buf[255];
	int j;
	memcpy(buf,&d,sizeof(double));
	/* for(j=0;j<sizeof(double);j++) if(!isprint(buf[j])) buf[j] = ' '; */
	buf[sizeof(double)] = 0;
	return encode_int8(p,find_index(col->owner,buf),col);
}

static double decode_int8_string(column* col)
{
	int i    = decode_int8(col);;
	double d;
	char buf[255] = {0,};

	const char* s ;
	assert(i < col->owner->next_index);
	
	s = col->owner->strings[i];
	strncpy(buf,s,sizeof(d));

	memcpy(&d,buf,sizeof(d));
	return d;

}

static unsigned char* encode_int16_string(unsigned char* p,double d,column* col)
{
	char buf[255];
	int j;
	memcpy(buf,&d,sizeof(double));
	/* for(j=0;j<sizeof(double);j++) if(!isprint(buf[j])) buf[j] = ' '; */
	buf[sizeof(double)] = 0;
	return encode_int16(p,find_index(col->owner,buf),col);
}

static double decode_int16_string(column* col)
{
	int i    = decode_int16(col);;
	double d;
	char buf[255] = {0,};

	const char* s ;
	assert(i < col->owner->next_index);
	
	s = col->owner->strings[i];
	strncpy(buf,s,sizeof(d));

	memcpy(&d,buf,sizeof(d));
	return d;
}

static unsigned char* encode_long_real(unsigned char* p,double s,column* col)
{
	memcpy(p,&s,sizeof(s));
	return p+sizeof(s);
}

static double decode_long_real(column* col)
{
	double s;
	assert(fread(&s,sizeof(s),1,col->owner->f) == 1);
	return s;
}

static unsigned char* encode_chars(unsigned char* p,double s,column* col)
{
	memcpy(p,&s,sizeof(s));
	return p+sizeof(s);
}

static double decode_chars(column* col)
{
	double s;
	assert(fread(&s,sizeof(s),1,col->owner->f) == 1);
	return s;
}

static unsigned char* encode_constant(unsigned char* p,double d,column* col)
{
	return p;
}

static double decode_constant(column* col)
{
	return col->min;
}

static unsigned char* encode_constant_or_missing(unsigned char* p,double d,column* col)
{
	return encode_int8(p,d != col->missing_value,col);
}

static double decode_constant_or_missing(column* col)
{
	double n = decode_int8(col);
	return n ? col->min: col->missing_value;
}

/*==============*/
typedef unsigned char* (*encode_proc)(unsigned char* p,double d,column *c);
typedef double         (*decode_proc)(column *c);

struct codec {
	const char* name;
	encode_proc encode;
	decode_proc decode;
};

static codec codecs[] = {
	{ "constant", encode_constant, decode_constant, },
	{ "constant_or_missing", encode_constant_or_missing, decode_constant_or_missing, },

	{ "chars", encode_chars, decode_chars, },
	{ "long_real", encode_long_real, decode_long_real, },
	{ "short_real", encode_short_real, decode_short_real, },

	{ "int32", encode_int32, decode_int32, },
	{ "int16", encode_int16, decode_int16, },
	{ "int8", encode_int8, decode_int8, },

	{ "int16_missing", encode_int16_missing, decode_int16_missing, },
	{ "int8_missing", encode_int8_missing, decode_int8_missing, },

	{ "int16_string", encode_int16_string, decode_int16_string, },
	{ "int8_string", encode_int8_string, decode_int8_string, },
};


codec* find_codec(const char* name)
{
	int count = sizeof(codecs)/sizeof(codec);
	int i;

	for(i=0;i<count;i++)
		if(strcmp(name,codecs[i].name) == 0)
			return &codecs[i];

	printf("cannot find codec %s\n",name);
	abort();
}


static void create_columns(obsdump *dump,int no_of_columns)
{
	int i;
	dump->no_of_columns = no_of_columns;
	assert(no_of_columns < 1024);
	dump->nrows = 0;

	for(i = 0 ; i< no_of_columns ; i++)
	{
		column* col = dump->columns[i] = calloc(sizeof(column),1);
		assert(col);
		col->owner  = dump;
	}
}

int obsdump_pass1_init(obsdump *dump,int no_of_columns)
{
	create_columns(dump,no_of_columns);
	return 0;
}

int obsdump_pass1_values(obsdump *dump,const double* values,int count)
{
	int i;
	assert(count == dump->no_of_columns);
	for(i = 0 ; i< count ; i++)
	{
		column* col = dump->columns[i];
		if(col->type == OBSDUMP_STRING)
		{
			char buf[255];
			int j;
			memcpy(buf,&values[i],sizeof(double));
			/* for(j=0;j<sizeof(double);j++) if(!isprint(buf[j])) buf[j] = ' '; */
			buf[sizeof(double)] = 0;
			store(dump,buf);
		}
		else
		{
			if(values[i] == col->missing_value)
			{
				col->has_missing = 1;
			}
			else 
			{

				if(values[i]<col->min || col->min == col->missing_value) 
					col->min=values[i];

				if(values[i]>col->max || col->max == col->missing_value) 
					col->max=values[i];
			}
		}
	}
	dump->nrows++;
	return 0;
} 

int obsdump_pass1_end(obsdump *dump)
{
	int i;

	for (i=0; i<dump->no_of_columns; i++) {
		column* col = dump->columns[i];

		col->coder = find_codec("long_real");

		switch(col->type)
		{
			case OBSDUMP_REAL:
				col->coder = find_codec("long_real");
				col->coder = find_codec("short_real");
				if(col->max == col->min)
					if(col->has_missing)
						col->coder = find_codec("constant_or_missing");
					else
						col->coder = find_codec("constant");
				break;

			case OBSDUMP_STRING:
				col->coder = find_codec("chars");
				if(dump->next_index < 65536) 
					col->coder = find_codec("int16_string");
				if(dump->next_index < 256) 
					col->coder = find_codec("int8_string");
				break;

			case OBSDUMP_INTEGER:
				if(col->has_missing)
				{
					double n = col->max - col->min;
					col->coder = find_codec("int32");
					if(n < 32768) 
						col->coder = find_codec("int16_missing");
					if(n < 128) 
						col->coder = find_codec("int8_missing");
					if(n == 0) 
						col->coder = find_codec("constant_or_missing");
				}
				else {
					double n = col->max - col->min;
					col->coder = find_codec("int32");
					if(n < 65536) 
						col->coder = find_codec("int16");
					if(n < 255) 
						col->coder = find_codec("int8");
					if(n == 0) 
						col->coder = find_codec("constant");
				}
				break;


			default:
				printf("Unsupported type: [%d]\n",col->type);
				abort();
				break;

		}
	}

	return 0; 
}

int obsdump_pass2_init(obsdump *dump)
{
	int i;
	int count = dump->no_of_columns;
	double* last = dump->last_values = calloc(sizeof(double),count);
	char c = 0xff;
	assert(last);

	for (i=0; i<count; i++) {
		column* col = dump->columns[i];
		last[i]     = col->missing_value;
	}


	dump->buffer  = malloc(1 + sizeof(double) * count);
	assert(dump->buffer);

	dump->nrows   = 0;


	/* Header */
	assert(fwrite(&c,1,1,dump->f) == 1);
	assert(fwrite(&count,sizeof(count),1,dump->f) == 1);
	for(i = 0; i<count; i++) {
		int len;
		column* col = dump->columns[i];

		len = strlen(col->name);
		assert(fwrite(&len,sizeof(len),1,dump->f) == 1);
		assert(fwrite(col->name,len,1,dump->f) == 1);
		assert(fwrite(&col->type,sizeof(col->type),1,dump->f) == 1);

		len = strlen(col->coder->name);
		assert(fwrite(&len,sizeof(len),1,dump->f) == 1);
		assert(fwrite(col->coder->name,len,1,dump->f) == 1);

		assert(fwrite(&col->has_missing,sizeof(col->has_missing),1,dump->f) == 1);

		assert(fwrite(&col->min,sizeof(col->min),1,dump->f) == 1);
		assert(fwrite(&col->max,sizeof(col->max),1,dump->f) == 1);
		assert(fwrite(&col->missing_value,sizeof(col->missing_value),1,dump->f) == 1);

	}
	write_table(dump);

	return 0;
}

int obsdump_pass2_values(obsdump *dump,const double* values,int count)
{
	double* last = dump->last_values;
	unsigned char* p;

	int k = 0;
	assert(count    == dump->no_of_columns);

	while(values[k] == last[k] &&  k < count ) k++;

	assert(k<128); // for now

	p = dump->buffer;
	*p++ = k;

	while(k<count) 
	{
		column* col = dump->columns[k];
		p = col->coder->encode(p,values[k],col);
		last[k] = values[k];
		k++;
	}

	assert(fwrite(dump->buffer,p-dump->buffer,1,dump->f) == 1);

	dump->nrows++;

	return 0;

} 


int obsdump_pass2_end(obsdump *dump)
{
	int n = fclose(dump->f);
	free(dump->buffer);
	free(dump->last_values);

	free(dump->io_buffer);

	dump->buffer = NULL;
	dump->last_values = NULL;
	dump->f = NULL;
	return n;
}


obsdump* obsdump_create(const char* path)
{
	obsdump *dump = calloc(sizeof(obsdump),1);
	if(!dump) return NULL;

	dump->path = strdup(path);
	assert(dump->path);

	dump->f = fopen64(path,"w");
	assert(dump->f);

	dump->io_buffer = malloc(MEGA(20));
	assert(dump->io_buffer);
	setbuffer(dump->f,dump->io_buffer,MEGA(20));

	return dump;
}

int obsdump_close(obsdump *dump)
{
	int i;

	free(dump->path);
	if(dump->f) fclose(dump->f); 

	/*TODO:free hash */
	/*TODO:free strings */
	if(dump->buffer) free(dump->buffer);
	if(dump->last_values) free(dump->last_values);

	for(i = 0 ; i< dump->no_of_columns ; i++)
	{
		column* col = dump->columns[i];
		if(col) {
			free(col->name);
			free(col);
		}
	}

	free(dump);

	return 0;
}


int obs_dump_set_column(obsdump* dump,int index,const char* name,int type,double missing_value)
{
	column* col;
	assert(index < dump->no_of_columns);
	col = dump->columns[index];
	assert(col);
	assert(!col->name);

	col->name = strdup(name); assert(col->name);
	col->type = type;
	col->missing_value = missing_value;

	return 0;
}

obsdump* obsdump_open(const char* path)
{
	unsigned char c;
	int i;

	obsdump *dump = calloc(sizeof(obsdump),1);
	if(!dump) return NULL;

	dump->path = strdup(path);
	assert(dump->path);

	dump->f = fopen64(path,"r");
	assert(dump->f);
	dump->io_buffer = malloc(MEGA(20));
	assert(dump->io_buffer);
	setbuffer(dump->f,dump->io_buffer,MEGA(20));

	/* Header */
	assert(fread(&c,1,1,dump->f) == 1); assert(c == 0xff);
	assert(fread(&dump->no_of_columns,sizeof(dump->no_of_columns),1,dump->f) == 1);
	create_columns(dump,dump->no_of_columns);

	dump->last_values = calloc(sizeof(double),dump->no_of_columns);
	for(i = 0; i<dump->no_of_columns; i++) {
		int len;
		char buf[1024];
		column* col = dump->columns[i];

		assert(fread(&len,sizeof(len),1,dump->f) == 1);
		assert(fread(buf,len,1,dump->f) == 1);
		buf[len]  = 0;
		col->name = strdup(buf);
		assert(col->name);

		assert(fread(&col->type,sizeof(col->type),1,dump->f) == 1);

		assert(fread(&len,sizeof(len),1,dump->f) == 1);
		assert(fread(buf,len,1,dump->f) == 1);
		buf[len]  = 0;
		col->coder = find_codec(buf);


		assert(fread(&col->has_missing,sizeof(col->has_missing),1,dump->f) == 1);

		assert(fread(&col->min,sizeof(col->min),1,dump->f) == 1);
		assert(fread(&col->max,sizeof(col->max),1,dump->f) == 1);
		assert(fread(&col->missing_value,sizeof(col->missing_value),1,dump->f) == 1);

		dump->last_values[i] = col->missing_value;

	}
	read_table(dump);


	return dump;
}

int obs_dump_get_no_of_columns(obsdump* dump)
{
	return dump->no_of_columns;
}

int obs_dump_get_column_type(obsdump* dump,int index)
{
	return dump->columns[index]->type;
}

const char* obs_dump_get_column_name(obsdump* dump,int index)
{
	return dump->columns[index]->name;
}

const char* obs_dump_get_codec_name(obsdump* dump,int index)
{
	return dump->columns[index]->coder->name;
}

double obs_dump_get_column_missing_value(obsdump* dump,int index)
{
	return dump->columns[index]->missing_value;
}

int obs_dump_get_next_row(obsdump* dump,double* data,int count)
{
	unsigned char c;
	int i = fread(&c,1,1,dump->f);

	if(i == 0 && feof(dump->f))
		return 0;

	assert(i == 1);

	assert(count >= dump->no_of_columns);

	for(i = c; i < dump->no_of_columns; i++)
	{
		column* col = dump->columns[i];
		dump->last_values[i] = col->coder->decode(col);	
	}

	memcpy(data, dump->last_values,dump->no_of_columns * sizeof(double));

	return dump->no_of_columns;
}
