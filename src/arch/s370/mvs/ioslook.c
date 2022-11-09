
// IOSLOOK.c
//
// Note: Caller must be in supervisor state
//
// Returns Address of UCB common segment for specified device
// or NULL if device does not exist.

// I need to create a struct for the UCB,
// but for now I'll just define it as void
#define UCB void

UCB *Ioslook(unsigned nDev)
{
	__register(6) unsigned R6 = nDev; // IOSLOOK (in) device number
	__register(1) void *R1; // IOSLOOK (out) UCB address
	__register(15) int R15; // IOSLOOK (out) return code

	__asm {
	STM 	0,15,0(13) Save registers
	L 	15,16 Get CVT pointer
	L 	15,124(0,15) Get IOCM pointer
	L 	15,84(0,15) Get IECVGENA pointer
	BAL 	14,12(0,15) Call IECVGENA (IOSLOOK)
	ST 	0,4(0,13) Save result in R1 save area
	LM 	0,14,0(13) Restore registers
}
if (R15) {
	R1=0; // not found
}
return R1;
}
 

