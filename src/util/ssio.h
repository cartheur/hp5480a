//----------------------------------------------
// wiringPi pin numbers for Simple Serial IO
// Note these are different than both the header pin numbers and the BCM GPIO numbers

/*
#define SSIO_PIN_SDI		13				// P1.21, MISO		DB25.6
#define SSIO_PIN_SDO		12				// P1.19, MOSI		DB25.4
#define SSIO_PIN_SCP		14				// P1.23, SCLK		DB25.3
#define SSIO_PIN_CTL		3				// P1.15, GPIO22	DB25.2
#define SSIO_PIN_CTL2		2				// P1.13, GPIO21	DB25.1
								// P1.25, GND		DB25.7
								// P1.2, +5V		DB25.8
*/

#define SSIO_PIN_SDI		6				// P1.22		DB25.6
#define SSIO_PIN_SDO		5				// P1.18		DB25.4
#define SSIO_PIN_SCP		4				// P1.16		DB25.3
#define SSIO_PIN_CTL		1				// P1.12		DB25.2
#define SSIO_PIN_CTL2		10				// P1.24		DB25.1
								// P1.6, GND		DB25.7
								// P1.2, +5V		DB25.8

#ifdef __linux__
	#include <wiringPi.h>
#else
	#include "../util/gpioDummy.h"
#endif
