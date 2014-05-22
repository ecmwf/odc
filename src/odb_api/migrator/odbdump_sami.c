/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


/* odbdump.c */

#include "odb.h"
#include "info.h"
#include "result.h"
#include "odbdump.h"
#include "pcma_extern.h"

PUBLIC Bool odbdump_on = false;

typedef struct {
  char *dbname;

  char *database;
  char *sql_query;
  char *poolmask;

  int db_handle;
  int maxcols;
  int npools_all;

  int *poolnos;
  int npools;
  int curpoolno_idx;
  int currow;

  int nrows;

  info_t *info;
  info_t *curinfo;

  set_t *setvar;
  int nsetvar;

  result_t *r;
} odbdump_t;


PRIVATE FILE *f_stderr = NULL; /* not thread safe */


PUBLIC void
odbdump_reset_stderr(FILE *fp, const char *filename, const char *mode)
{
  if (!f_stderr) {
    /* Get rid of unnecessary stderr-output by
       redirecting "stderr" to the filename (or /dev/null if cannot open).
       Do only once */
    if (!fp) fp = stderr;
    if (!filename) filename = "odbdump.stderr";
    if (!mode) mode = "w";
    f_stderr = freopen(filename, mode, fp);
    if (!f_stderr) f_stderr = freopen("/dev/null", mode, fp);
  }
}


PRIVATE char *
SlurpMe(const char *path) /* Nearly as in odb/compiler/odb98.c */
{
  char *s = NULL;
  if (path) {
    extern int IOgetsize(const char *path, int *filesize, int *blksize);
    int filesize = 0;
    int rc = IOgetsize(path, &filesize, NULL);
    if (rc == 0 && filesize > 0) {
      FILE *fp = fopen(path,"r");
      if (fp) {
	int len = filesize + 1;
	ALLOC(s, len);
	if (s) {
	  rc = fread(s, sizeof(*s), filesize, fp); /* slurp!! */
	  if (rc == filesize) { /* Success */
	    char *ps = s;
	    s[filesize] = '\0';
	    while (*ps) {
	      if (isspace(*ps) || !isprint(*ps)) *ps = ' ';
	      ++ps;
	    } /* while (*ps) */
	  }
	  else {
	    FREE(s); /* Failure */
	  }
	}
	fclose(fp);
      } /* if (fp) */
    }
  }
  if (!s) {
    fprintf(stderr,"***Error: Unable to inline from the non-existent (or empty) file '%s'\n",
	    path ? path : NIL);
  }
  return s;
}


PUBLIC void *
odbdump_open(const char *database,
	     const char *sql_query, /* Precedence on this over the queryfile, if both present */
	     const char *queryfile,
	     const char *poolmask,
	     const char *varvalue,
	     int *ncols)
{
  static Bool do_once = true; /* Not thread safe */
  odbdump_t *h = NULL;
  if (ncols) *ncols = 0;

  if (!database) {
    /* Try current dir for database */
    database = ".";
  }

  if (do_once) {
    /* 
       Avoid hangs in gdb/dbx debugger called by Dr.Hook in odb98.x :
       export GNUDEBUGGER=0
       export DBXDEBUGGER=0
    */
    char *env1, *env2;
    env1 = STRDUP("GNUDEBUGGER=0");
    putenv(env1); /* "env1" remains allocated ; cannot be free'd due to putenv() */
    env2 = STRDUP("DBXDEBUGGER=0");
    putenv(env2); /* "env1" remains allocated ; cannot be free'd due to putenv() */
    do_once = false;
  }

  if (!f_stderr) odbdump_reset_stderr(NULL, NULL, NULL);

  if (database && (sql_query || queryfile)) {
    int npools, ntables, db_handle;
    odbdump_on = true;
    db_handle = ODBc_open(database, "r", &npools, &ntables, poolmask);
    odbdump_on = false;
    if (db_handle >= 1) {
      int nsetvar = 0;
      set_t *setvar = ODBc_make_setvars(varvalue, &nsetvar);
      void *(*sql_prepare_func)(int , const char *, const set_t *, int) = /* showing off ? */
	sql_query ? ODBc_sql_prepare : ODBc_sql_prepare_via_sqlfile;
      info_t *info = sql_prepare_func(db_handle, sql_query ? sql_query : queryfile, setvar, nsetvar);
      info_t *info_chain = info;
      int maxcols = 0;

      while (info) {
	if (info->create_index == 0) {
	  /* We skip ANY CREATE/DROP INDEX -statements */
	  maxcols = MAX(maxcols, info->ncols_true);
/*
	  maxcols = MAX(maxcols, info->ncols);
      printf("ncols = %d %d %d %d %d %d %d\n", info->ncols, info->ncols_true, info->ncols_pure,
                                      info->ncols_formula, info->ncols_aggr_formula, 
                                      info->ncols_aux, info->ncols_nonassoc); 
*/
	}
	else {
	  fprintf(stderr,"***Warning: odbdump currently ignores CREATE/DROP INDEX -statements\n");
	}
	info = ODBc_next(info);
      } /* while (info) */
      info = info_chain;

      if (maxcols >= 1) {
	ODBc_print_info(ODBc_get_debug_fp(), info);

	CALLOC(h, 1);
	h->dbname = STRDUP(info->ph->dbname);
	h->database = STRDUP(database);
	h->sql_query = sql_query ? STRDUP(sql_query) : SlurpMe(queryfile);
	h->poolmask = poolmask ? STRDUP(poolmask) : NULL;
	h->db_handle = db_handle;
	h->maxcols = maxcols;
	h->npools_all = h->npools = npools;
	h->poolnos = ODB_get_permanent_poolmask(h->db_handle, &h->npools);
	h->curpoolno_idx = 0;
	h->currow = 0;
	h->nrows = 0;
	h->info = info;
	h->curinfo = NULL;
	h->setvar = setvar;
	h->nsetvar = nsetvar;
	h->r = NULL;
      }
      else {
	info = ODBc_sql_cancel(info);
      }
    } /* if (db_handle >= 1) */
  } /* if (database && sql_query) */

  if (h) {
    if (h->npools == 0) {
      odbdump_close(h);
      h = NULL;
    }
    else if (ncols) {
      *ncols = h->maxcols; 
    }
  }
	    
  return h;
}


PUBLIC int
odbdump_nextrow_packed(void *Handle, 
		       void *v, /* at least npk elements of sizeof(char) */
		       int npk,
		       int *new_dataset)
{
  int rc = 0;
  int nbytes = 0;
  int nd = npk/sizeof(double);
  double *d = v;
  rc = odbdump_nextrow(Handle, d, nd, new_dataset);
  if (rc > 0) {
    void *pk = PackDoubles(d, rc, &nbytes);
    if (pk && nbytes > 0 && nbytes <= npk) {
      memcpy(v, pk, nbytes);
    }
    else { /* Do not pack at all */
      uint *Pk = NULL;
      double *tmp = NULL;
      const int ratio = sizeof(*tmp)/sizeof(*Pk); /* Usually 2 */
      const int offset = 2/ratio;
      int ntmp = offset + rc;
      ALLOC(tmp, ntmp);
      Pk = (uint *)tmp;
      Pk[0] = (uint)-1;
      Pk[1] = rc;
      memcpy(&tmp[offset], d, rc * sizeof(*d));
      nbytes = ntmp * sizeof(*tmp);
      if (nbytes <= npk) memcpy(d, tmp, nbytes);
      else nbytes = -nbytes;
      FREE(tmp);
    }
    FREE(pk);
  }
  return nbytes;
}


PUBLIC int
odbdump_nextrow(void *Handle, 
		void *v, /* at least nd elements of sizeof(double) */
		int nd,
		int *new_dataset)
{
  double *d = v;
  int rc = 0;
  if (new_dataset) *new_dataset = 0;
  if (Handle && d && nd > 0) {
    DEF_IT;
    const Bool row_wise_preference = true;
    odbdump_t *h = Handle;
    info_t *info = h->curinfo ? h->curinfo : h->info;

  again:

    while (info && info->create_index != 0) {
      /* We skip ANY CREATE/DROP INDEX -statements */
      info = ODBc_next(info);
    }
    
    if (h->curinfo != info) {
      /* Reset previous infos */
      h->r = ODBc_unget_data(h->r);
      h->currow = 0;
      h->nrows = 0;
      h->curpoolno_idx = 0;
      if (h->curinfo && h->curinfo->need_hash_lock) {
	/* UNIQUEBY-clause present (or has SELECT DISTINCT) */
	codb_hash_init_();
	codb_hash_unset_lock_();
      }
      h->curinfo = info;
      if (h->curinfo && h->curinfo->need_hash_lock) {
	/* UNIQUEBY-clause present (or has SELECT DISTINCT) */
	codb_hash_init_();
	codb_hash_unset_lock_();
      }
      if (new_dataset) *new_dataset = info ? 1 : 0;
    }

    if (!info) goto finish;

    if (info->need_global_view) {
      /* A global view */
      if (!h->r && h->curpoolno_idx < h->npools) {
	int ip, npools = h->npools;
	int nrows = 0;
	result_t *res = NULL;
	
	h->r = ODBc_unget_data(h->r);
	
	for (ip=0; ip<npools; ip++) {
	  /* This is a loop over effective pools i.e. those which
	     were left in the poolmask */
	  int nr, jp = h->poolnos[ip];
	  if (info->has_thin) codb_thin_reset_(&it);
	  nr = ODBc_sql_exec(h->db_handle, info, jp, NULL, NULL);
	  if (nr > 0) {
	    nrows += nr;
	    res = ODBc_get_data(h->db_handle, res, info, jp, 1, nr, false, &row_wise_preference);
	  }
	  if (info->has_thin) codb_thin_reset_(&it);
	  info = ODBc_reset_info(info);
	  if (info->is_bc && nrows >= 1) break;
	} /* for (ip=0; ip<npools; ip++) */
	
	h->nrows = 0;
	if (res && nrows > 0) {
	  if (info->has_aggrfuncs) res = ODBc_aggr(res, 1);
	  res = ODBc_operate(res, "count(*)");
	  res = ODBc_sort(res, NULL, 0);
	  h->r = res;
	  h->nrows = res->nrows_out;
	}
	h->currow = 0;
	h->curpoolno_idx = h->npools; /* means: don't redo this block, since active pools already "looped over" */
      } /* if (!h->r && h->curpoolno_idx < h->npools) */
    }
    else {
      /* NOT a global view */
      while (!h->r && h->curpoolno_idx < h->npools) {
	int jp = h->poolnos[(h->curpoolno_idx)++];
	if (info->has_thin) codb_thin_reset_(&it);
	h->nrows = ODBc_sql_exec(h->db_handle, info, jp, NULL, NULL);
	if (h->nrows > 0) {
	  h->r = ODBc_get_data(h->db_handle, NULL, info, jp, 1, h->nrows, false, &row_wise_preference);
	  h->currow = 0;
	} /* if (h->nrows > 0) */
      } /* while (!h->r && h->curpoolno_idx < h->poolnos) */
    }

    if (h->r) {
      if (h->currow >= h->nrows) {
	h->r = ODBc_unget_data(h->r);
	h->currow = 0;
	h->nrows = 0;
	if (info->has_thin) codb_thin_reset_(&it);
	info = ODBc_reset_info(info);
	if (info->need_global_view) info = ODBc_next(info);
	goto again;
      }
      else {
	int currow = h->currow;
	result_t *res = h->r;
	Bool row_wise = res->row_wise;
	nd = MIN(nd, res->ncols_out);

	if (row_wise) { /* Stride == 1 */
	  const double *in = &res->d[currow][0];
	  memcpy(d, in, nd * sizeof(*d));
	}
	else { /* Access with stride >> 1 */
	  int i;
	  for (i=0; i<nd; i++) d[i] = res->d[i][currow];
	}

	h->currow = ++currow;
	rc = nd;
      }
    } /* h->r */
  }
 finish:
  return rc;
}


PUBLIC colinfo_t *
odbdump_create_colinfo(void *Handle,
		       int *ncols)
{
  colinfo_t *ci = NULL;
  odbdump_t *h = Handle;
  if (ncols) *ncols = 0;
  if (h && h->curinfo) {
    const info_t *info = h->curinfo;
    int ncols_true = info->ncols_true;
    int j, nc = 0;
    for (j=0; j<ncols_true; j++) {
      const col_t *colthis = &info->c[j];
      if (colthis->kind == 1 || colthis->kind == 2 || colthis->kind == 4) ++nc;
    } /* for (j=0; j<ncols_true; j++) */
    if (nc > 0) {
      int k = 0;
      colinfo_t *pci;
      CALLOC(ci, nc);
      pci = ci;
      for (j=0; j<ncols_true; j++) {
	const col_t *colthis = &info->c[j];
	if (colthis->kind == 1 || colthis->kind == 2 || colthis->kind == 4) {
	  pci->id = ++k;
	  pci->name = STRDUP(colthis->name);
	  pci->nickname = colthis->nickname ? STRDUP(colthis->nickname) : NULL;
	  pci->type_name = STRDUP(colthis->dtype);
	  pci->dtnum = colthis->dtnum;
	  ++pci;
	}
      } /* for (j=0; j<ncols_true; j++) */
      if (ncols) *ncols = nc;
    } /* if (nc > 0) */
  }
  return ci;
}


PUBLIC colinfo_t *
odbdump_destroy_colinfo(colinfo_t *ci, 
			int ncols)
{
  if (ci && ncols > 0) {
    colinfo_t *pci = ci;
    int j;
    for (j=0; j<ncols; j++) {
      FREE(pci->name);
      FREE(pci->nickname);
      FREE(pci->type_name);
      ++pci;
    }
    FREE(ci);
  }
  return NULL;
}


PUBLIC int
odbdump_close(void *Handle)
{
  int rc = -1;
  if (Handle) {
    odbdump_t *h = Handle;
    int nsetvar = h->nsetvar;
    if (h->setvar && nsetvar > 0) {
      int j;
      for (j=0; j<nsetvar; j++) {
	FREE(h->setvar[j].name);
      }
    }
    FREE(h->setvar);
    FREE(h->dbname);
    FREE(h->database);
    FREE(h->sql_query);
    FREE(h->poolmask);
    h->info = ODBc_sql_cancel(h->info);
    h->r = ODBc_unget_data(h->r);
    FREE(h->poolnos);
    rc = ODBc_close(h->db_handle);
    FREE(h);
  }
  return rc;
}


