//********************************************************************************
//*
//*	 	Attention Interrupt Handler
//*
//* 2014 Jan: V1 /bh
//*
//********************************************************************************
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include "util.h"
#include "attn.h"


static void
Attn__handler( s, siginfo, context )
	int			s;
	siginfo_t*		siginfo;
	void*			context;
 {
	Attn = TRUE;

	printf( "<ATTN>\n" );
 }


//* END Private
//********************************************************************************
//* Public


int				Attn;


void
Attn_Init()
 {
	struct sigaction	sa;

	Attn = FALSE;

	sa.sa_sigaction = &Attn__handler;
	sa.sa_flags = SA_SIGINFO | SA_RESTART;
	sigemptyset( &sa.sa_mask );

	if( sigaction(SIGINT,&sa,NULL) < 0 )
	 {	printf( "  !!! Can't establish SIGINT signal handler !!!\n" );
		exit( -1 );
	 }
 }


//* EOF
//********************************************************************************
