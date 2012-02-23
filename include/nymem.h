/* extern char *malloc(),*realloc(); */
extern char *ny_malloc(),*ny_realloc();
extern void ny_free();

extern unsigned ny_sbuf_size;
extern char *ny_sbuf;

/* #define MALLOC(x)	ny_malloc(x,&ny_sbuf,&ny_sbuf_size)
#define REALLOC(o,x)	ny_realloc(o,x,&ny_sbuf,&ny_sbuf_size)
#define FREE_M(x)	ny_free(x) */

#define MALLOC(x)	((char *)(((((((x>>2)+1)<<2)) > ny_sbuf_size)||(ny_sbuf == 0)) \
				?(ny_malloc((((x>>2)+1)<<2),&ny_sbuf,&ny_sbuf_size)) \
				:(ny_sbuf+=(((x>>2)+1)<<2),\
				  ny_sbuf_size-=(((x>>2)+1)<<2),\
				  ny_sbuf-((((x>>2)+1)<<2)))))
#define REALLOC(o,x)	ny_realloc(o,x,&ny_sbuf,&ny_sbuf_size)
#define FREE_M(x)
/* #define MALLOC(x)	malloc(x)
#define REALLOC(o,x)	realloc(o,x)
#define FREE_M(x)	free(x) */
#define NEW_ORDER()	(struct ny_Order *)MALLOC(sizeof(struct ny_Order))
#define NEW_SUBTREE()   (struct ny_subtree_t *)MALLOC(sizeof(struct ny_subtree_t))
#define NEW_FUNC()      (struct ny_func_t *)MALLOC(sizeof(struct ny_func_t))
#define NEW_BRANCH()    (struct ny_cond_t *)MALLOC(sizeof(struct ny_cond_t))
#define NEW_NODE()	(struct ny_Node *)MALLOC(sizeof(struct ny_Node))
#define NEW_TRANS()	(struct ny_Translation *) \
				MALLOC(sizeof(struct ny_Translation))
#define NEW_RHSITEM()	(struct ny_RhsItem *)MALLOC(sizeof(struct ny_RhsItem))
#define NEW_TEMP()      (struct ny_Register **)MALLOC(sizeof(struct ny_Register *))
#define	NEW_REG()	(struct ny_Register *)MALLOC(sizeof(struct ny_Register))
#define NEW_RSE()	(struct ny_RStackElem *)MALLOC(sizeof(struct ny_RStackElem))
#define NEW_RS()	(struct ny_RStack *)MALLOC(sizeof(struct ny_RStack))
#define NEW_CNS()	(struct ny_CNStack *)MALLOC(sizeof(struct ny_CNStack))
#define NEW_ARGS()	(struct ny_ArgStack *)MALLOC(sizeof(struct ny_ArgStack))
#define NEW_OS()	(struct ny_OStack *)MALLOC(sizeof(struct ny_OStack))
#define NEW_STR(x)	MALLOC(x)
#define RE_STR(o,x)	REALLOC(o,x)
#define FREE_CNSTACK(x)	FREE_M(x)
#define FREE_ASTACK(x)	FREE_M(x)
