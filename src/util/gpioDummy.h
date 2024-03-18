#define OUTPUT			0
#define INPUT			0
#define PUD_UP			0
#define PUD_DOWN		0

extern void			wiringPiSetup();
extern void			pinMode();
extern void			pullUpDnControl();
extern void			digitalWrite();
extern int			digitalRead();
