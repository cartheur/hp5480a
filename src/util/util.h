//-------------------------------------
// Some c extensionas

#define private			static
#define public

#define boolean			int
#define TRUE			1
#define FALSE			0

#define repeat			for(;;)

#define PANICIF(test)		if( (test) ) { fprintf( stderr, "PANIC!! %s\n", #test ); exit(-1); }

extern int			min();
extern int			max();


//-------------------------------------
// fix stupid stdio

#define fput(f,c)		fputc( (c), (f) )


//-------------------------------------
// Memory

extern void*			m_alloc();
extern void			m_free();
extern void			m_check();

#define r_alloc(type)		( (type*) m_alloc(sizeof(type)) )
#define r_free(r)		m_free( r )


//-------------------------------------
// Buffer

typedef struct
 {	int*		p;
	int		l;
	int		size;
 }
	BF;

extern BF*			bf_alloc();
extern void			bf_free();
extern void			bf_resize();
extern void			bf_append();
extern void			bf_put();
extern void			bf_fill();
extern void			bf_shift();


//-------------------------------------
// Time

extern void			t_delay_uS();


//-------------------------------------
// String Routines

#define s_eql(s1,s2)		( strcmp((s1),(s2)) == 0 )
#define s_len(s)		( strlen(s) )

extern int			s_kwMatch();
extern char*			s_token();


//-------------------------------------
// XNS Routines

extern void			XNS_Decode();
extern int			XNS_Encode();
