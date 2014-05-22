#ifndef _ODBDUMP_H_
#define _ODBDUMP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* odbdump.h */

#include "result.h"

/* from aux/odbdump.c */

extern void
odbdump_reset_stderr(FILE *fp, const char *filename, const char *mode);

extern void *odbdump_open(const char *database,
			  const char *sql_query, /* Precedence on this over the queryfile, if both present */
			  const char *queryfile,
			  const char *poolmask,
			  const char *varvalue,
			  int *ncols);

extern int odbdump_nextrow(void *Handle, 
			   void *v, /* at least nd elements of sizeof(double) */
			   int nd,
			   int *new_dataset);

extern int odbdump_nextrow_packed(void *Handle,
				  void *v, /* at least npk elements sizeof(char) */
				  int npk,
				  int *new_dataset);

extern int odbdump_close(void *Handle);

extern colinfo_t *odbdump_create_colinfo(void *Handle, int *ncols);

extern colinfo_t *odbdump_destroy_colinfo(colinfo_t *ci, int ncols);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif

#endif /* _ODBDUMP_H_ */

