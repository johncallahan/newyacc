#include <stdio.h>
#include <errno.h>

#define INITIAL  65000

extern char *malloc();
extern char *realloc();

#define PROFILESTOREMAXSIZE 30
unsigned ny_memprofilesize = 0;
struct {
  unsigned calls,size;
} ny_memprofile[PROFILESTOREMAXSIZE];

void
ny_profilememory(size)
int size;
{
  unsigned i;

  for(i=0;i < ny_memprofilesize;i++) {
    if(size == ny_memprofile[i].size) {
      ny_memprofile[i].calls++; return;
    }
  }
  if(ny_memprofilesize < PROFILESTOREMAXSIZE) {
    ny_memprofile[ny_memprofilesize].size = size;
    ny_memprofile[ny_memprofilesize++].calls = 1;
  }
}

void
ny_printmemoryprofile()
{
  unsigned i;

  fprintf(stderr,"size\tcalls\n"); fflush(stderr);
  for(i=0;i < ny_memprofilesize;i++) {
    fprintf(stderr,"%d\t%d\n",ny_memprofile[i].size,ny_memprofile[i].calls);
  }
}

char
*ny_malloc(size,buf,rem)
int size,*rem;
char **buf;
{
  int rsize;

  rsize = ((size>>2)+1)<<2;
#ifdef NYDEBUG
 	fprintf(stderr,"alloc(%lx,%d,%d)",*buf,rsize,*rem); fflush(stderr);
#endif
#ifdef MEMPROFILE
	ny_profilememory(rsize);
#endif
	if((rsize > *rem) || (*buf == 0)) {
#ifdef NYDEBUG
 		fprintf(stderr,"initial alloc ... "); fflush(stderr);
#endif
		if((*buf = malloc(INITIAL)) == 0) {
		  fprintf(stderr,"(malloc: not enough core)\n");
#ifdef MEMPROFILE
		  ny_printmemoryprofile();
#endif
		  fflush(stderr); exit(ENOMEM);
		}
		*rem = INITIAL - rsize;
		*buf += rsize;
		return(*buf-rsize);
	} else {
#ifdef NYDEBUG
 		fprintf(stderr,"allocing %d left %d ",rsize,*rem);
		fflush(stderr);
#endif
		*buf += rsize; *rem -= rsize;
		return(*buf-rsize);
	}
}

char
*ny_realloc(oldobj,size,buf,rem)
int size,*rem;
char **buf,*oldobj;
{
	char *tb;
	tb = ny_malloc(size,buf,rem);
	bcopy(oldobj,tb,size);
	return(tb);
}

void
ny_free()
{
}

void
ny_fputs(buf,count,str,fd)
char **buf,*str;
int *count;
FILE *fd;
{
	int l;
	if(*buf == 0) {
		*buf = malloc(INITIAL);
		*count = INITIAL;
	}
	if((l=strlen(str)) > *count) {
		fwrite(*buf,1,INITIAL-*count,fd); fflush(fd);
		*count = INITIAL;
	}
/*	fprintf(stderr,"putting %d\n",l); fflush(stderr); */
	bcopy(str,*buf+(INITIAL-*count),l);
	*count -= l;
}

ny_flush(buf,count,fd)
char **buf;
int *count;
FILE *fd;
{
/*	fprintf(stderr,"flushing %d\n",INITIAL-*count); fflush(stderr); */
	fwrite(*buf,1,INITIAL-*count,fd); fflush(fd);
	*count = INITIAL;
}
