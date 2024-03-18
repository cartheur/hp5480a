//********************************************************************************
//*
//*		Command Prompt Module
//*
//*	2014 Feb: V1 adapted from .py version
//*	2020 May: include command added, -1 for return-to-user on undef cmd
//*	2021 May: quotes and +++ multi-line added 
//*
//********************************************************************************

#include <stdio.h>
#include "util.h"
#include "attn.h"
#include "cp.h"


//-----------------------------------------------
// Internal Commands

#define PCMD_HELP		1
#define PCMD_CMDS		2
#define PCMD_NUM		3
#define PCMD_INCLUDE		4
#define PCMD_MSG		5


private void
Cp__Help( helpSrc, arg )
	char*		helpSrc;
	char*		arg;
 {
	char		ln[200];
	char		topic[50];
	char		*hs, *ls;
	int		all, display, cmd, i;

	if( !*arg )
		printf( "  Do 'help full' or 'help <topic>' for more info:\n" );

	all = s_kwMatch("all",arg) || s_kwMatch("full",arg);
	display = all;

	hs = helpSrc;
	while( *hs )
	 {	for( ls=ln; (*ls=*hs); ++hs,++ls )				// collect a line from doc src
			if( *hs == '\n' )
			 {	*ls = 0;
				++hs;
				break;
			 }

		ls = ln;
		if( *ls == '#' )						// check for topic line
		 {	if( (cmd = (*++ls=='C')) ) ++ls;			// check for command topic

			if( !*arg )						// if no arg, print all topics
			 {	printf( "  %s\n", ls );
				continue;
			 }

			if( display && !all )					// check for end of topic section
				break;

			i = sscanf( ls, " %s ", topic );			// look for topic match
			if( i==1 && s_kwMatch(topic,arg) )
				display = TRUE;					// switch to displaying

			if( cmd && (display || all) ) printf( "  Command: %s\n", ls );	// if command, print use
		 }
		else if( display )						// if display mode print section lines
			printf( "  %s\n", ln );
	 }

	if( *arg && !display )
		printf( "  -- no such topic. Do 'help' for topics.\n" );
 }


private CP_TBL			Cp__predefTbl[] =
 {
	{ "q*uit",	"",		"quit this program",				0		},
	{ "h*elp",	"",		"help if doc present, else ?",			PCMD_HELP	},
	{ "?",		"",		"display list of known commands",		PCMD_CMDS	},
	{ "inc*lude",	"<file>",	"read commands from <file>",			PCMD_INCLUDE	},
	{ "n*umber",	"<num>",	"display number in decimal, hex and octal",	PCMD_NUM	},
	{ ".",		"<num>",	"same as number",				PCMD_NUM	},
	{ "message",	"<string>",	"display a message",				PCMD_MSG	},
	{ NULL,		NULL,		NULL,						0		},
 };


private void
Cp__predefCmds( self, cmd, cmdTbl )
	CP*			self;
	int			cmd;
	CP_TBL*			cmdTbl;
 {
	CP_TBL*			ct;
	char*			s;
	int			n;

	switch( cmd )
	 {	case PCMD_HELP:						// CMD: help
			if( self->helpSrc )
			 {	s = Cp_Str( self, "" );
				if( !Cp_Check(self) )
					return;
				Cp__Help( self->helpSrc, s );
				return;
			 }						// if no help file, fall through to list commands

		case PCMD_CMDS:						// CMD: command list
			for( ct=cmdTbl;        ct->cmd; ++ct ) printf( "  %-15s %-30s - %s\n",  ct->cmd, ct->helpArgs, ct->helpNote );
			for( ct=Cp__predefTbl; ct->cmd; ++ct ) printf( "  %-15s %-30s - %s\n",  ct->cmd, ct->helpArgs, ct->helpNote );
			return;

		case PCMD_INCLUDE:					// CMD: read commands from a file
			Cp_PushCmdFile( self, NULL );
			return;

		case PCMD_NUM:						// CMD: number conversion
			n = Cp_Num( self, 0 );
			if( !Cp_Check(self) )
				return;
			printf( "  %d 0x%X 0%o\n", n, n, n );
			return;

		case PCMD_MSG:						// CMD: display message
			printf( "  --" );
			while( (s=Cp_Str( self,NULL)) )
				printf( " %s", s );
			printf( "\n" );
			return;
	 }
 }


private int
Cp__getCmdLine( fd, argv, buf )
	FILE*				fd;
	char**				argv;
	char*				buf;
 {
	int				i, argc, c, state;

	state = 'R';
	i = 0;
	argc = 0;
	argv[0] = buf;

	while( i < CP__MAXLINE )
	 {	c = fgetc( fd );
		switch( c )
		 {	case ' ':
			case '\t':
				if( state == 'Q' )				// in quote, add to buffer
				 {	buf[i++] = c;
					continue;
				 }
				else if( state!='D' && &(buf[i])==argv[argc] )
					continue;				// skip blanks at beginning of token

				state = 'R';					// back to regular reading
				buf[i++] = 0;					// terminate token
				argv[++argc] = &(buf[i]);			// start new token
				break;

			case '\n':
				if( state == '3' )				// if 3 plusses before newline, ignore those and newline, and continue reading
				 {	i -= 3;
					state = 'R';
					continue;
				 }
				goto fin;

			case EOF:
				if( i == 0 )
					return( 0 );				// no chars and EOF
				goto fin;

			case '"':
				if( state == 'Q' )
					state = 'D';				// 1st dbl-q seen while in quote
				else if( state == 'D' )
				 {	buf[i++] = '"';				// 2nd dbl-q in quote, add a quote char to buffer
					state = 'Q';				// continue quoting
				 }
				else
					state = 'Q';				// start quoting
				break;

			case '+':
				if( state == 'R' || state=='D' ) state = '1';
				else if( state == '1' )		 state = '2';
				else if( state == '2' )		 state = '3';
				// fall thru
			default:
				if( state == 'D' ) state = 'R';			// dbl-q was end of quote
				buf[i++] = c;					// add to buffer
				break;
		 }
	 }

	return( -1 );

 fin:	if( state!='D' && &(buf[i])==argv[argc] ) --argc;			// dismiss trailing blanks

	buf[i] = 0;								// terminate token and arg vector
	argv[++argc] = NULL;

	return( 1 );								// we have a command
 }


//** END Private
//********************************************************************************
//** Public


//-----------------------------------------------
// Initialisation

CP*
Cp_Open( helpSrc )
	char*			helpSrc;
 {
	CP*			self= r_alloc( CP );

	self->helpSrc = helpSrc;

	Attn_Init();

	self->inStkIdx = 0;
	self->inStk[0] = stdin;

	return( self );
 }


//-----------------------------------------------
// Termination

void
Cp_Close( self )
	CP*			self;
 {
	r_free( self );
 }


//-----------------------------------------------
// Display <prompt> and wait for a command
// Return the command code for a match, 0 for quit
// Prepare for following argument parsing

int
Cp_Cmd( self, prompt, cmdTbl )
	CP*			self;
	char*			prompt;
	CP_TBL			cmdTbl[];
 {
	CP_TBL			*uct, *pct;
	//char*			parse;
	char*			token;
	//int			i;

	repeat
	 {	Attn = FALSE;

		if( self->inStkIdx == 0 ) printf( "%s", prompt );

		switch( Cp__getCmdLine(self->inStk[self->inStkIdx],self->argv,self->cmdLn) )	// read and parse command line, check for EOF
		 {	case 0:
				if( self->inStkIdx == 0 )				// check for bottom of stack	??should break include loop on ATTN??
				 {	printf( "<EOF>\n" );				// force newline and quit
					goto fin;
				 }
				fclose( self->inStk[self->inStkIdx--] );		// pop stack
				continue;						// try again with input from lower stack

			case -1:
				printf( "  -- line too long\n" );
				continue;
		 }

		//if( !fgets(self->cmdLn,CP__MAXLINE,self->inStk[self->inStkIdx]) )	// prompt for command, check for EOF
		//if( self->inStkIdx > 0 ) printf( "%s\n", self->cmdLn ); //DEBUG

		//parse = self->cmdLn;						// parse command line into null-terminated vector of tokens
		//for( i=0; (self->argv[i]=s_token(&parse)); ++i );

		self->argp = self->argv;					// arg pointer starts at first arg
		self->err = 0;							// command status starts as good

		token = *self->argp++;
		if( !token || !*token )						// no command
			continue;

		for( uct=cmdTbl; uct->cmd; ++uct )				// check for user command
		 {	if( s_kwMatch(uct->cmd,token) )
				return( uct->code );				// found, return to user with command code
		 }

		for( pct=Cp__predefTbl; pct->cmd; ++pct )			// check for internal command
			if( s_kwMatch(pct->cmd,token) )
				break;
		if( pct->cmd )
		 {	if( pct->code == 0 )
				break;						// quit command
			Cp__predefCmds( self, pct->code, cmdTbl );		// execute internal command
			continue;
		 }

		if( uct->code < 0 )						// if EOT entry in user table had code<0, user will deal with unknown command
		 {	--self->argp;
			return( -1 );
		 }

		printf( "  -- unknown command\n" );				// no match
	 }
 fin:
	return( 0 );								// return to user to quit
 }


//-----------------------------------------------
// Check for failure during arg parsing
// if ok = TRUE, good
// if ok = 0

int
Cp_Check( self )
	CP*			self;
 {
	if( *self->argp ) self->err = -1;					// check for extraneous arguments

	if( self->err )
	 {	if( self->err < 0 ) printf( "  -- bad argument or syntax\n" );
		return( FALSE );
	 }

	return( TRUE );
 }


//-----------------------------------------------
// Parse off a free-form string argument

char*
Cp_Str( self, deflt )
	CP*			self;
	char*			deflt;
 {
	if( !*self->argp )
		return( deflt );

	return( *self->argp++ );
 }


//-----------------------------------------------
// Parse off an optional numeric argument

int
Cp_Num( self, deflt )
	CP*			self;
	int			deflt;
 {
	char*			token;
	int			n;

	token = *self->argp;
	if( !token )
		return( deflt );

	if( sscanf(token,"%i",&n) != 1 )
		return deflt;

	++self->argp;

	return( n );
 }


//-----------------------------------------------
// Parse off an optional keyword argument

int
Cp_Kw( self, kw )
	CP*			self;
	char*			kw;
 {
	char*			token;

	token = *self->argp;
	if( !token )
		return( FALSE );

	if( !s_kwMatch(kw,token) )
		return( FALSE );

	++self->argp;

	return( TRUE );
 }


//-----------------------------------------------
// Parse off a file argument, return file opened in specified mode.
// User is responsible for closing the file.

FILE*	
Cp_File( self, mode )
	CP*			self;
	char*			mode;
 {
	char*			fname;
	FILE*			fd;

	fname = Cp_Str( self, NULL );
	if( !fname )
	 {	printf( "  -- no file specified\n" );
		self->err = CP_FAILED;
		return( NULL );
	 }

	fd = fopen( fname, mode );
	if( !fd )
	 {	printf( "  -- error opening file\n" );
		self->err = CP_FAILED;
		return( NULL );
	 }

	return( fd );
 }


//-----------------------------------------------
// Open the specified file and redirect command input to come from it
// by pushing it onto the command input stack.

int
Cp_PushCmdFile( self, fName )
	CP*			self;
	char*			fName;
 {
	FILE*			fd;

	if( self->inStkIdx >= CP__INSTKMAX )
	 {	printf( "  -- too many nested includes\n" );
		return( FALSE );
	 }

	if( fName )
	 {	fd = fopen( fName, "r" );
		if( !fd )
		 {	printf( "  -- error opening file: %s\n", fName );
			self->err = CP_FAILED;
			return( FALSE );
		 }
	 }
	else
	 {	fd = Cp_File( self, "r" );
		if( !fd )
			return( FALSE );
	 }

	self->inStk[++self->inStkIdx] = fd;		// push onto input stk
	return( TRUE );
 }


//* END Command Prompt
//********************************************************************************
