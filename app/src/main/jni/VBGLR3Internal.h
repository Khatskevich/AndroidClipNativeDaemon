#ifndef IN_RING3
#define  IN_RING3
#endif
#ifndef VBOX_WITH_HGCM
#define VBOX_WITH_HGCM
#endif



#include "VBox/VMMDev.h"
#include "VBox/VBoxGuest.h"
#include "VBox/VBoxGuestLib.h"

/*
int     vbglR3DoIOCtl(unsigned iFunction, void *pvData, size_t cbData);
int     vbglR3GRAlloc(VMMDevRequestHeader **ppReq, uint32_t cb, VMMDevRequestType enmReqType);
int     vbglR3GRPerform(VMMDevRequestHeader *pReq);
void    vbglR3GRFree(VMMDevRequestHeader *pReq);
*/


void VbglHGCMParmUInt32Set(HGCMFunctionParameter *pParm, uint32_t u32)
{
    pParm->type = VMMDevHGCMParmType_32bit;
    pParm->u.value64 = 0; /* init unused bits to 0 */
    pParm->u.value32 = u32;
}


int VbglHGCMParmUInt32Get(HGCMFunctionParameter *pParm, uint32_t *pu32)
{
    if (pParm->type == VMMDevHGCMParmType_32bit)
    {
        *pu32 = pParm->u.value32;
        return VINF_SUCCESS;
    }
    return VERR_INVALID_PARAMETER;
}


void VbglHGCMParmUInt64Set(HGCMFunctionParameter *pParm, uint64_t u64)
{
    pParm->type      = VMMDevHGCMParmType_64bit;
    pParm->u.value64 = u64;
}


int VbglHGCMParmUInt64Get(HGCMFunctionParameter *pParm, uint64_t *pu64)
{
    if (pParm->type == VMMDevHGCMParmType_64bit)
    {
        *pu64 = pParm->u.value64;
        return VINF_SUCCESS;
    }
    return VERR_INVALID_PARAMETER;
}


void VbglHGCMParmPtrSet(HGCMFunctionParameter *pParm, void *pv, uint32_t cb)
{
    pParm->type                    = VMMDevHGCMParmType_LinAddr;
    pParm->u.Pointer.size          = cb;
    pParm->u.Pointer.u.linearAddr  = (uintptr_t)pv;
}



void VbglHGCMParmPtrSetString(HGCMFunctionParameter *pParm, const char *psz)
{
    pParm->type                    = VMMDevHGCMParmType_LinAddr_In;
    pParm->u.Pointer.size          = (uint32_t)strlen(psz) + 1;
    pParm->u.Pointer.u.linearAddr  = (uintptr_t)psz;
}



