//********************************************************************************
//*	 	5480 DSA Interface
//*
//*	Low-level interface to the 5480 adapter hardware.
//*	Note the delay loops in the Transfer routine. The loop count used is valid for
//*	running on an RPi.
//*	The Linux nanosleep routine results in an overly long minimum delay, so is not used here.
//*
//*	2022 Apr: V1 /bh
//*
//********************************************************************************
#include <time.h>
#include <stdio.h>
#include "../util/util.h"
#ifdef __linux__
	#include <wiringPi.h>
#else
	#include "../util/gpioDummy.h"
#endif
#include "ifDSA.h"


// Signals to/from the 5480 adapter
// These are wiringPi pin numbers, different than both the header pin numbers and the BCM GPIO numbers

#define DSA__WPP_SDI		13
#define DSA__WPP_SDO		12
#define DSA__WPP_SCP		14
#define DSA__WPP_UPDATE		2


// Shift desired signal states into output shift register of adapter, then load output buffer so 5480 sees them.
private void
DSA__update( self, sigState )
	DSA*			self;
	int			sigState;
 {
	int			i, j;

	for( i=0; i<16; ++i )							// 16 bits in output shift register
	 {	digitalWrite( DSA__WPP_SDO, sigState&1 );			// set serial data output state to signal state

		digitalWrite( DSA__WPP_SCP, 0 );				// pulse to shift register
		for( j=self->clockWidth; j; --j );
		digitalWrite( DSA__WPP_SCP, 1 );
		for( j=self->clockWidth; j; --j );

		sigState >>= 1;
	 }

	digitalWrite( DSA__WPP_UPDATE, 0 );					// pulse to load buffer register from shift register
	for( j=self->clockWidth; j; --j );
	digitalWrite( DSA__WPP_UPDATE, 1 );
	for( j=self->clockWidth; j; --j );
 }


//* END Private
//********************************************************************************
//* Public

DSA*
DSA_Open( clockWidth )
	int			clockWidth;
 {
	DSA*			self = r_alloc( DSA );

	self->clockWidth = clockWidth>=0 ? clockWidth : 100;			// loop count for pulse signals

	self->sigState = 0;							// set initial state for signals to 5480
	self->sigState |= DSA_SIG_ACC_CLR;
	self->sigState |= DSA_SIG_ACC_ENSHL;
	self->sigState |= DSA_SIG_ACC_ENSHR;
	self->sigState |= DSA_SIG_ACC_SHIFT;
	self->sigState |= DSA_SIG_ACC_ENCNT;
	self->sigState |= DSA_SIG_ACC_INC;
	self->sigState |= DSA_SIG_ACC_DEC;

	self->sigState |= DSA_SIG_SVQ_SUB;

	self->sigState |= DSA_SIG_MEM_R;
	self->sigState |= DSA_SIG_MEM_W;
	self->sigState |= DSA_SIG_MEM_C;

	self->sigState |= DSA_SIG_PAR_CLRENCH;
	self->sigState |= DSA_SIG_PAR_INC;
	self->sigState |= DSA_SIG_MAR_LOADPAR;

	self->sigState |= DSA_SIG_VHR_LOAD;

	wiringPiSetup();							// initialize GPIO library

	pinMode(	 DSA__WPP_SDI, INPUT	);				// initialize signals to/from 5480/adapter
	pullUpDnControl( DSA__WPP_SDI, PUD_DOWN	);

	pinMode(	DSA__WPP_SCP, OUTPUT	);
	digitalWrite(	DSA__WPP_SCP, 1		);

	pinMode(	DSA__WPP_SDO, OUTPUT	);
	digitalWrite(	DSA__WPP_SCP, 0		);

	pinMode(	DSA__WPP_UPDATE, OUTPUT	);
	digitalWrite(	DSA__WPP_UPDATE, 1	);

	DSA__update( self, self->sigState );					// set signals state in adapter

	return( self );
 }


//----------------------------------------------

void
DSA_Close( self )
	DSA*			self;
 {
	r_free( self );
 }


//----------------------------------------------
// Pulse a signal on the 5480.

void
DSA_SigPulse( self, sig )
	DSA*			self;
	int			sig;
 {
	DSA__update( self, self->sigState ^ sig );				// flip state of specified bit
	DSA__update( self, self->sigState );					// flip it back;
 }


//----------------------------------------------
// Set a signal state on the 5480.

void
DSA_SigSet( self, sig, state )
	DSA*			self;
	int			sig;
	int			state;
 {
	if( (self->sigState&sig) != (state?sig:0) )				// does current state differ from desired state?
	 {	self->sigState ^= sig;						// if so, flip state of specified bit
		DSA__update( self, self->sigState );
	 }
 }


//----------------------------------------------
// Set ACC in 5480 to value <send>

void
DSA_AccPut( self, send )
	DSA*			self;
	int			send;
 {
	int			b;

	DSA_SigPulse( self, DSA_SIG_ACC_CLR );					// prepare by clearing ACC

	for( b=0x800000; ; b>>=1 )						// 24 bits
	 {	if( send & b )							// if bit of new value = 1 then increment LSB of ACC to 1
		 {	DSA_SigPulse( self, DSA_SIG_ACC_ENCNT );
			DSA_SigPulse( self, DSA_SIG_ACC_INC   );
		 }

		DSA_SigPulse( self, DSA_SIG_ACC_ENSHL );			// shift ACC up 1 bit

		if( b == 1 )
			break;

		DSA_SigPulse( self, DSA_SIG_ACC_SHIFT );
	 }
}


//----------------------------------------------
// Get the value of ACC from the 5480

int
DSA_AccGet( self )
	DSA*			self;
 {
	int			recv, b;

	recv = 0;

	for( b=1; b<=0x800000; b<<=1 )						// 24 bits
	 {	if( digitalRead(DSA__WPP_SDI) )					// receive bit
			recv |= b;						// and place it into recv

		DSA_SigPulse( self, DSA_SIG_ACC_ENSHR );			// shift (rotate?) ACC down 1 bit
		DSA_SigPulse( self, DSA_SIG_ACC_SHIFT );
	 }

	return( recv );
 }


//* EOF
//********************************************************************************
