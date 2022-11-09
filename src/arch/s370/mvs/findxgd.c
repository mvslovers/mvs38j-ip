/* findxgd.c - findxgd */

#include <conf.h>
#include <kernel.h>
#include <xgd.h>	// MVS-Xinu Global Data 

/*------------------------------------------------------------------------
 *  findxgd  --  locate MVS-Xinu XGD (Xinu Global Data)
 *------------------------------------------------------------------------
 */
void *
findxgd()
{
	sgd	*psgd;
	xgd	*pxgd;

	psgd = findsgd();		// track down SGD
	if (psgd == NULL) {
		panic("findxgd unable to locate SGD");
	}
	pxgd = psgd->sgd_xgd;		// addr of XGD
	if (pxgd == NULL) {
		panic("findxgd XGD not initialized");
	}
	return pxgd;
}
