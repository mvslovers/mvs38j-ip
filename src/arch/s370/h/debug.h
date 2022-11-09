/* debug.h - debugging  */ 

#undef	dbug		// allow multiple <debug.h> inclusions in source
#undef	trace		// in case coder wishes to redefine DEBUG
#undef	snap		// for selected portions of code

#ifdef	DEBUG
	#define	dbug(...)	__VA_ARGS__
	#define	trace(...)	kprintf(__VA_ARGS__)	// variadic
#else
	#define	dbug(...)	// suppress
	#define	snap(x,y,z)	// suppress
	#define	trace(...)	// suppress
#endif




