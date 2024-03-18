//********************************************************************************
//**	 	Some Utility Routines
//**
//** 2014 Jan: V1 /bh
//**
//********************************************************************************
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "util.h"


int
min( a, b )
	int			a, b;
 {
	return a < b ? a : b;
 }


//-----------------------------------------------

int
max( a, b )
	int			a, b;
 {
	return a > b ? a : b;
 }


//* END Misc
//********************************************************************************
//* Memory Routines

static int			m__allocCnt;


void*
m_alloc( size )
	int			size;
 {
	++m__allocCnt;
	return malloc( size );
 }


//-----------------------------------------------

void
m_free( m )
	void*			m;
 {
	free( m );
	--m__allocCnt;
 }


//-----------------------------------------------

void
m_check()
 {
	if( m__allocCnt != 0 )
		printf( "  !!! Memory allocation imbalance, %i allocated !!!\n", m__allocCnt );
 }


//* END Memory
//********************************************************************************
//* Buffer Routines


BF*
bf_alloc( size )
	int			size;
 {
	BF*			bf = r_alloc( BF );

	bf->p = (int*) m_alloc( size*sizeof(int) );
	bf->l = 0;
	bf->size = size;

	return bf;
 }


//-----------------------------------------------

void
bf_free( bf )
	BF*			bf;
 {
	m_free( bf->p );
	r_free( bf );
 }


//-----------------------------------------------

void
bf_resize( bf, size )
	BF*			bf;
	int			size;
 {
	bf->p = (int*) realloc( bf->p, size*sizeof(int) );
	bf->size = size;

	if( bf->l > bf->size ) bf->l = bf->size;
 }


//-----------------------------------------------

void
bf_append( bf, n )
	BF*			bf;
	int			n;
 {
	if( bf->l >= bf->size )
		bf_resize( bf, bf->size+=10000 );

	bf->p[bf->l++] = n;
 }


//-----------------------------------------------

void
bf_put( bf, i, n )
	BF*			bf;
	int			i, n;
 {
	if( i >= bf->size )
		bf_resize( bf, bf->size+=10000 );

	bf->p[i] = n;

	if( i >= bf->l )
		bf->l = i+1;
 }


//-----------------------------------------------

void
bf_fill( bf, i, cnt, n )
	BF*			bf;
	int			i, cnt, n;
 {
	while( cnt-- > 0 )
		bf_put( bf, i++, n );
 }


//-----------------------------------------------
// Shift buffer contents starting at <bi> up <cnt> elements.
// <cnt> must be >= 0.
// Should improve so cnt<0 shifts down.

void
bf_shift( bf, bi, cnt )
	BF*			bf;
	int			bi;
	int			cnt;
 {
	int			biSrc, biDst;

	if( bi+cnt >= bf->size )
		bf_resize( bf, bf->size+=10000 );

	biSrc = bf->l;
	bf->l += cnt;
	biDst = bf->l;

	while( biSrc >= bi ) bf->p[--biDst] = bf->p[--biSrc];
 }


//* END Buffer
//********************************************************************************
//* Time Routines


//-----------------------------------------------
// Delay Routine

void
t_delay_uS( uS )
	int			uS;
{
	struct timespec	ts;

	ts.tv_sec  = 0;
	ts.tv_nsec = uS * 1000;

	nanosleep( &ts, NULL );
 }


//* END Time
//********************************************************************************
//* String Routines


//-----------------------------------------------
// String Keyword Matching

int
s_kwMatch( kw, str )
	char*			kw;
	char*			str;
 {
	int			required;
	char			*k, *s;

	required = TRUE;
	k = kw;
	s = str;

	repeat
	 {	if( *k == '*' )
		 {	required = FALSE;
			++k;
		 }
		if( !*s )
		 {	if( !required || !*k )
				return TRUE;
			break;
		 }
		if( *k != *s )
			break;
		++k;
		++s;
	 }

	return FALSE;
 }


//-----------------------------------------------
// String Token Parsing

char*
s_token( parse )
	char**			parse;
 {
	char			*s, *token;

	s = *parse;

	while( *s==' ' || *s=='\t' || *s=='\n' ) ++s;		// skip white space
	if( !*s )
		return( NULL );

	token = s;
	while( *s && *s!=' ' && *s!='\t' && *s!='\n' ) ++s;	// collect token
	if( *s ) *s++ = 0;

	*parse = s;

	return( token );
 }


//* END String
//********************************************************************************
//* XNS Routines


//-----------------------------------------------
// Basic Decode XNS from file

void
XNS_Decode( bf, fd )
	BF*			bf;
	FILE*			fd;
 {
	int			num, c;

	num = 0;

	while( (c=fgetc(fd)) != EOF )
	 {	if( c >= '0' && c <= '9' )
			num = (num<<4) | (c-'0');
		else if( c >= 'A' && c <= 'F' )
			num = (num<<4) | (c-'A'+10);
		else if( c == '~' )
		 {	bf_append( bf, num );
			num = 0;
		 }
		else if( c == '!' )
			break;
		else
			num = 0;
	 }
 }


//-----------------------------------------------
// Basic XNS Encode to file

int
XNS_Encode( fd, bf )
	FILE*			fd;
	BF*			bf;
 {
	int			i;

	for( i=0; i<bf->l; ++i )
	 {	fprintf( fd, "%X~", bf->p[i] );
		if( i%16 == 15 ) fprintf( fd, "\n" );
	 }

	fprintf( fd, "!" );

	return( i );
 }


//** EOF
//********************************************************************************
