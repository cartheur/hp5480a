//********************************************************************************
//*
//*			HP 5480 Controller Program
//*
//* 2022 Apr   : V1
//*
//********************************************************************************
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <time.h>
#include "../util/util.h"
#include "../util/attn.h"
#include "../util/cp.h"
#include "ifDSA.h"


//* END Predefs
//********************************************************************************
//* Command Table

//--------------------------------------------------------------------------------


#define CMD_REMOTE		1	
#define CMD_LOCAL		2

#define CMD_ACC_CLEAR		3
#define CMD_ACC_LOAD		4
#define CMD_ACC_COUNT		5
#define CMD_ACC_SHIFT		6
#define CMD_ACC_DISPLAY		7

#define CMD_MEM_R		8
#define CMD_MEM_W		9
#define CMD_MEM_C		10
#define CMD_MEM_DISPLAY		11
#define CMD_MEM_SET		12

#define CMD_PAR_CLEAR		13
#define CMD_PAR_INC		14
#define CMD_MAR_LOADPAR		15

#define CMD_VHR_LOAD		16

#define CMD_TEST_ADAPTER	17


static CP_TBL			cmdTbl[] =
 {	{ "r*emote",	"",		"take remote control of 5480",	CMD_REMOTE	},
	{ "l*ocal",	"",		"release remote control",	CMD_LOCAL	},

	{ "az*ero",	"",		"accumulator clear",		CMD_ACC_CLEAR	},
	{ "al*oad",	"<num>",	"accumulator load",		CMD_ACC_LOAD	},
	{ "ac*ount",	"<num>",	"accumulator count",		CMD_ACC_COUNT	},
	{ "as*hift",	"<num>",	"accumulator shift",		CMD_ACC_SHIFT	},
	{ "ad*isplay",	"",		"accumulator display",		CMD_ACC_DISPLAY	},

	{ "mr*ead",	"",		"memory read",			CMD_MEM_R	},
	{ "mw*rite",	"",		"memory write",			CMD_MEM_W	},
	{ "mc*ycle",	"",		"memory read-write",		CMD_MEM_C	},
	{ "m*display",	"<addr> [<count>]","memory display",		CMD_MEM_DISPLAY	},
	{ "ms*et",	"<value> <addr> [<count>]", "memory set", 	CMD_MEM_SET	},

	{ "parz*ero",	"",		"PAR clear",			CMD_PAR_CLEAR	},
	{ "pari*nc",	"<count>",	"PAR increment",		CMD_PAR_INC	},
	{ "part*omar",	"",		"PAR to MAR",			CMD_MAR_LOADPAR	},

	{ "vhrl*oad",	"",		"load V & H Registers",		CMD_VHR_LOAD	},
	{ "testadapter","",		"cycle adapter outputs to 5480",CMD_TEST_ADAPTER},

	{ NULL,		NULL,		NULL,				0		}
 };


//* END Command Table
//********************************************************************************
//* Interactive mode

void
Interactive( clockWidth )
	int			clockWidth;
 {
	CP*			cp  = Cp_Open(NULL);
	DSA*			dsa = DSA_Open(clockWidth);
	int			cmd;
	int			num, addr, count, par, i;

	printf( "   -- Note: To manipulate PAR and memory, 5480 must be in DISPLAY mode\n   -- and 'remote' command executed.\n" );

	while( (cmd=Cp_Cmd(cp,"5480> ",cmdTbl)) ) switch( cmd )				// command loop
	 {	case CMD_REMOTE:							// CMD: take remote control (assert SVQ_SUB)
			if( !Cp_Check(cp) )
				continue;

			DSA_SigSet( dsa, DSA_SIG_SVQ_SUB, 0 );
			continue;

		case CMD_LOCAL:								// CMD: release remote control
			if( !Cp_Check(cp) )
				continue;

			DSA_SigSet( dsa, DSA_SIG_SVQ_SUB, 1 );
			continue;

		case CMD_ACC_CLEAR:							// CMD: clear ACC
			if( !Cp_Check(cp) )
				continue;

			DSA_SigPulse( dsa, DSA_SIG_ACC_CLR );
			continue;

		case CMD_ACC_LOAD:							// CMD: set ACC to specified value
			num = Cp_Num( cp, -1 );
			if( !Cp_Check(cp) )
				continue;

			DSA_AccPut( dsa, num );
			continue;

		case CMD_ACC_COUNT:							// CMD: count ACC up/down
			num = Cp_Num( cp, 1 );
			if( !Cp_Check(cp) )
				continue;

			DSA_SigPulse( dsa, DSA_SIG_ACC_ENCNT );

			if( num < 0 )
			 {	while( num++ )
					DSA_SigPulse( dsa, DSA_SIG_ACC_DEC );
			 }
			else
			 {	while( num-- )
					DSA_SigPulse( dsa, DSA_SIG_ACC_INC );
			 }
			continue;

		case CMD_ACC_SHIFT:							// CMD: shift ACC left/right
			num = Cp_Num( cp, 1 );
			if( !Cp_Check(cp) )
				continue;

			if( num < 0 )
			 {	DSA_SigPulse( dsa, DSA_SIG_ACC_ENSHR );
				num = -num;
			 }
			else if( num > 0 )
			 {	DSA_SigPulse( dsa, DSA_SIG_ACC_ENSHL );
			 }
			else continue;

			while( num-- )
				DSA_SigPulse( dsa, DSA_SIG_ACC_SHIFT );

			continue;

		case CMD_ACC_DISPLAY:							// CMD: get ACC value and display it
			if( !Cp_Check(cp) )
				continue;

			printf( "  ACC: 0x%06X\n", DSA_AccGet(dsa) );
			continue;

		case CMD_MEM_R:								// CMD: memory read
			if( !Cp_Check(cp) )
				continue;

			DSA_SigPulse( dsa, DSA_SIG_MEM_R );
			continue;

		case CMD_MEM_W:								// CMD: memory write
			if( !Cp_Check(cp) )
				continue;

			DSA_SigPulse( dsa, DSA_SIG_MEM_W );
			continue;

		case CMD_MEM_C:								// CMD: memory read-write cycle
			if( !Cp_Check(cp) )
				continue;

			DSA_SigPulse( dsa, DSA_SIG_MEM_C );
			continue;

		case CMD_MEM_DISPLAY:							// CMD: list memory contents
			addr  = Cp_Num( cp, 0 );
			count = Cp_Num( cp, 1 );
			if( !Cp_Check(cp) )
				continue;

			DSA_SigPulse( dsa, DSA_SIG_PAR_CLRENCH );			// 0==>PAR, enable count & enable PAR to HOLD (MAR)

			for( par=0; par<addr; ++par )					// increment to first address
				DSA_SigPulse( dsa, DSA_SIG_PAR_INC );

			for( i=0; i<count; ++i )
			 {	if( (i&3) == 0 )
					printf( "\n   0x%03X: ", par );

				DSA_SigPulse( dsa, DSA_SIG_MAR_LOADPAR );		// load mem addr reg from PAR
				DSA_SigPulse( dsa, DSA_SIG_ACC_CLR );			// clear ACC in prep for mem read
				DSA_SigPulse( dsa, DSA_SIG_MEM_C );			// read and restore mem

				printf( " 0x%06X", DSA_AccGet(dsa) );			// display ACC contents

				DSA_SigPulse( dsa, DSA_SIG_PAR_INC );			// next location
				++par;
			 }

			printf( "\n" );
			continue;

		case CMD_MEM_SET:							// CMD: set memory contents
			num   = Cp_Num( cp, 0 );					// value to set locations to
			addr  = Cp_Num( cp, 0 );					// address of first location
			count = Cp_Num( cp, 1 );					// count of locations to set, default 1
			if( !Cp_Check(cp) )
				continue;

			DSA_SigPulse( dsa, DSA_SIG_PAR_CLRENCH );			// 0==>PAR, enable count & enable PAR to MAR

			for( par=0; par<addr; ++par )					// increment to first address
				DSA_SigPulse( dsa, DSA_SIG_PAR_INC );

			for( i=0; i<count; ++i )
			 {	DSA_SigPulse( dsa, DSA_SIG_MAR_LOADPAR );		// load Mem Addr Reg from PAR

				DSA_SigPulse( dsa, DSA_SIG_MEM_R );			// read mem to clear location, this modifies ACC
				DSA_AccPut( dsa, num );					// set ACC to target value
				DSA_SigPulse( dsa, DSA_SIG_MEM_W );			// write ACC to mem

				DSA_SigPulse( dsa, DSA_SIG_PAR_INC );			// next location
				++par;
			 }

			continue;

		case CMD_PAR_CLEAR:							// CMD: clear process address register
			if( !Cp_Check(cp) )
				continue;

			DSA_SigPulse( dsa, DSA_SIG_PAR_CLRENCH );			// clear PAR, enable count & enable PAR to HOLD (MAR)
			continue;

		case CMD_MAR_LOADPAR:							// CMD: load MAR from PAR
			if( !Cp_Check(cp) )
				continue;

			DSA_SigPulse( dsa, DSA_SIG_MAR_LOADPAR );
			continue;

		case CMD_PAR_INC:							// CMD: increment process address register,
			num = Cp_Num( cp, 1 );						// assumes ENCNT has been triggerred by PAR_CLR
			if( !Cp_Check(cp) )
				continue;

			if( num < 0 )
			 {	printf( "  -- decrement not implemented\n" );
			 }
			else
			 {	while( num-- )
					DSA_SigPulse( dsa, DSA_SIG_PAR_INC );
			 }

			DSA_SigPulse( dsa, DSA_SIG_MAR_LOADPAR );			// load Mem Address Register from PAR
			continue;

		case CMD_VHR_LOAD:							// CMD: load the V & H Hold Registers from ACC & MAR respectively
			if( !Cp_Check(cp) )
				continue;

			DSA_SigPulse( dsa, DSA_SIG_VHR_LOAD );
			continue;

		case CMD_TEST_ADAPTER:							// CMD: cycle outputs
			if( !Cp_Check(cp) )
				continue;

			while( !Attn )
			 {	DSA_SigPulse( dsa, DSA_SIG_ACC_CLR	);
				DSA_SigPulse( dsa, DSA_SIG_ACC_ENSHL	);
				DSA_SigPulse( dsa, DSA_SIG_ACC_ENSHR	);
				DSA_SigPulse( dsa, DSA_SIG_ACC_SHIFT	);
				DSA_SigPulse( dsa, DSA_SIG_ACC_ENCNT	);
				DSA_SigPulse( dsa, DSA_SIG_ACC_INC	);
				DSA_SigPulse( dsa, DSA_SIG_ACC_DEC	);

				DSA_SigPulse( dsa, DSA_SIG_SVQ_SUB	);

				DSA_SigPulse( dsa, DSA_SIG_MEM_R	);
				DSA_SigPulse( dsa, DSA_SIG_MEM_W	);
				DSA_SigPulse( dsa, DSA_SIG_MEM_C	);

				DSA_SigPulse( dsa, DSA_SIG_PAR_CLRENCH	);
				DSA_SigPulse( dsa, DSA_SIG_PAR_INC	);
				DSA_SigPulse( dsa, DSA_SIG_MAR_LOADPAR	);

				DSA_SigPulse( dsa, DSA_SIG_VHR_LOAD	);
			 }
			continue;
	 }

	DSA_Close( dsa );
	Cp_Close( cp );
 }


//* END Interactive
//********************************************************************************
//* Main

int
main( argc, argv )
	int			argc;
	char**			argv;
 {
	int			i, clockWidth;

	//------------------------------
	// Command line configuration

	for( i=1; i<argc; ++i )
		if( strcmp(argv[i],"-c") == 0 )
		 {	if( ++i == argc ) goto useFail;
			if( sscanf( argv[i], "%d", &clockWidth ) != 1 ) goto useFail;
		 }
		else if( strcmp(argv[i],"-u") == 0 )
		 {useFail:
			printf( "  -- Use: %s [-c clockWidth][-u]\n", argv[0] );
			exit( -1 );
		 }

	//------------------------------
	// Execution

	Interactive( clockWidth );

	//------------------------------
	// Termination

	m_check();
	return( 0 );
}


//** END Main
//********************************************************************************
