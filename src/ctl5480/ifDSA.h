//**********************************************
//**	5480 Interface Definitions
//**********************************************

// Control signals to the 5480 from the adapter
// These are bit positions in the adapter output shift register

#define DSA_SIG_ACC_CLR		0x8000
#define DSA_SIG_ACC_ENSHL	0x4000
#define DSA_SIG_ACC_ENSHR	0x2000
#define DSA_SIG_ACC_SHIFT	0x1000
#define DSA_SIG_ACC_ENCNT	0x0800
#define DSA_SIG_ACC_INC		0x0400
#define DSA_SIG_ACC_DEC		0x0200
#define DSA_SIG_SVQ_SUB		0x0100

#define DSA_SIG_MEM_R		0x0080
#define DSA_SIG_MEM_W		0x0040
#define DSA_SIG_MEM_C		0x0020
#define DSA_SIG_PAR_CLRENCH	0x0010
#define DSA_SIG_PAR_INC		0x0008
#define DSA_SIG_MAR_LOADPAR	0x0004
#define DSA_SIG_VHR_LOAD	0x0002
// not in use			0x0001


//----------------------------------------------
// Interface Descriptor Type

typedef struct
 {
	int			clockWidth;					// loop count for pulse signals
	int			sigState;					// current state of output signals
 }
	DSA;


//----------------------------------------------
// Interface Routines

extern DSA*			DSA_Open();
extern void			DSA_Close();

extern void			DSA_SigPulse();
extern void			DSA_SigSet();

extern void			DSA_AccPut();
extern int			DSA_AccGet();
