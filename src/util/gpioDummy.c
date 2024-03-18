#include <stdio.h>
#include "gpioDummy.h"


void
wiringPiSetup()
 {	printf( "  -- GPIO not available\n" );
 }


void
pinMode()
 {
 }


void
pullUpDnControl()
 {
 }


void
digitalWrite()
 {
 }


int
digitalRead()
 {	return( 1 );
 }
