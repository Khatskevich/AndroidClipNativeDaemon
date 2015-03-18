/** @page pg_hostclip       The Shared Clipboard Host Service
 *
 * The shared clipboard host service provides a proxy between the host's
 * clipboard and a similar proxy running on a guest.  The service is split
 * into a platform-independent core and platform-specific backends.  The
 * service defines two communication protocols - one to communicate with the
 * clipboard service running on the guest, and one to communicate with the
 * backend.  These will be described in a very skeletal fashion here.
 *
 * @section sec_hostclip_guest_proto  The guest communication protocol
 *
 * The guest clipboard service communicates with the host service via HGCM
 * (the host service runs as an HGCM service).  The guest clipboard must
 * connect to the host service before all else (Windows hosts currently only
 * support one simultaneous connection).  Once it has connected, it can send
 * HGCM messages to the host services, some of which will receive replies from
 * the host.  The host can only reply to a guest message, it cannot initiate
 * any communication.  The guest can in theory send any number of messages in
 * parallel (see the descriptions of the messages for the practice), and the
 * host will receive these in sequence, and may reply to them at once
 * (releasing the caller in the guest) or defer the reply until later.
 *
 * There are currently four messages defined.  The first is
 * VBOX_SHARED_CLIPBOARD_FN_GET_HOST_MSG, which waits for a message from the
 * host.  Host messages currently defined are
 * VBOX_SHARED_CLIPBOARD_HOST_MSG_QUIT (unused),
 * VBOX_SHARED_CLIPBOARD_HOST_MSG_READ_DATA (request that the guest send the
 * contents of its clipboard to the host) and
 * VBOX_SHARED_CLIPBOARD_HOST_MSG_FORMATS (to notify the guest that new
 * clipboard data is available).  If a host message is sent while the guest is
 * not waiting, it will be queued until the guest requests it.  At most one
 * host message of each type will be kept in the queue.  The host code only
 * supports a single simultaneous VBOX_SHARED_CLIPBOARD_FN_GET_HOST_MSG call
 * from the guest.
 *
 * The second guest message is VBOX_SHARED_CLIPBOARD_FN_FORMATS, which tells
 * the host that the guest has new clipboard data available.  The third is
 * VBOX_SHARED_CLIPBOARD_FN_READ_DATA, which asks the host to send its
 * clipboard data and waits until it arrives.  The host supports at most one
 * simultaneous VBOX_SHARED_CLIPBOARD_FN_READ_DATA call from the guest - if a
 * second call is made before the first has returned, the first will be
 * aborted.
 *
 * The last guest message is VBOX_SHARED_CLIPBOARD_FN_WRITE_DATA, which is
 * used to send the contents of the guest clipboard to the host.  This call
 * should be used after the host has requested data from the guest.
 *
 * @section sec_hostclip_backend_proto  The communication protocol with the
 *                                      platform-specific backend
 *
 * This section may be written in the future :)
 */


//*****************************************************************************************


/*
 * The mode of operations.
 *
#define VBOX_SHARED_CLIPBOARD_MODE_OFF           0
#define VBOX_SHARED_CLIPBOARD_MODE_HOST_TO_GUEST 1
#define VBOX_SHARED_CLIPBOARD_MODE_GUEST_TO_HOST 2
#define VBOX_SHARED_CLIPBOARD_MODE_BIDIRECTIONAL 3

/*
 * Supported data formats. Bit mask.
 *
#define VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT   UINT32_C(0x01)
#define VBOX_SHARED_CLIPBOARD_FMT_BITMAP        UINT32_C(0x02)
#define VBOX_SHARED_CLIPBOARD_FMT_HTML          UINT32_C(0x04)

/*
 * The service functions which are callable by host.
 *
#define VBOX_SHARED_CLIPBOARD_HOST_FN_SET_MODE      1
/** Run headless on the host, i.e. do not touch the host clipboard. *
#define VBOX_SHARED_CLIPBOARD_HOST_FN_SET_HEADLESS  2

/*
 * The service functions which are called by guest.
 *
* Call host and wait blocking for an host event VBOX_SHARED_CLIPBOARD_HOST_MSG_* *
#define VBOX_SHARED_CLIPBOARD_FN_GET_HOST_MSG      1
/* Send list of available formats to host. *
#define VBOX_SHARED_CLIPBOARD_FN_FORMATS           2
/* Obtain data in specified format from host. *
#define VBOX_SHARED_CLIPBOARD_FN_READ_DATA         3
/* Send data in requested format to host. *
#define VBOX_SHARED_CLIPBOARD_FN_WRITE_DATA        4

*
 * The host messages for the guest.
 *
#define VBOX_SHARED_CLIPBOARD_HOST_MSG_QUIT        1
#define VBOX_SHARED_CLIPBOARD_HOST_MSG_READ_DATA   2
#define VBOX_SHARED_CLIPBOARD_HOST_MSG_FORMATS     3
*/





//*************************************************************************************




/** The different clipboard formats which we support. */
enum CLIPFORMAT
{
    INVALID = 0,
    TARGETS,
    TEXT,  /* Treat this as Utf8, but it may really be ascii */
    UTF8,
    BMP
};


#define RT_ARCH_X86
#define __X86__
#define ARCH_BITS 64
#define GC_ARCH_BITS 32
#define __gnu_linux__
#define IN_RING3
#define VBOX_WITH_HGCM

#include "VBox/VBoxGuest.h"
#include "VBox/VBoxGuestLib.h"
#include "VBox/HostServices/VBoxClipboardSvc.h"
#include "VBox/GuestHost/SharedClipboard.h"
#include "iprt/err.h"


#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "VBGLR3Internal.h"

int rc, des;


/**
 * Connects to the clipboard service.
 *
 * @returns VBox status code
 * @param   pu32ClientId    Where to put the client id on success. The client id
 *                          must be passed to all the other clipboard calls.
 */
VBGLR3DECL(int) VbglR3ClipboardConnect(uint32_t *pu32ClientId)
{
    VBoxGuestHGCMConnectInfo Info;
    Info.result = VERR_WRONG_ORDER;
    Info.Loc.type = VMMDevHGCMLoc_LocalHost_Existing;
    RT_ZERO(Info.Loc.u);
    strcpy(Info.Loc.u.host.achName, "VBoxSharedClipboard");
    Info.u32ClientID = UINT32_MAX;  /* try make valgrind shut up. */

    int rc = ioctl( des , VBOXGUEST_IOCTL_HGCM_CONNECT, &Info);
    if (1) //RT_SUCCESS(rc))
    {
        rc = Info.result;
        if (1) //RT_SUCCESS(rc))
            *pu32ClientId = Info.u32ClientID;
    }
    return rc;
}


int VbglR3ClipboardDisconnect(uint32_t u32ClientId)
{
    VBoxGuestHGCMDisconnectInfo Info;
    Info.result = VERR_WRONG_ORDER;
    Info.u32ClientID = u32ClientId;

    int rc = ioctl( des , VBOXGUEST_IOCTL_HGCM_DISCONNECT, &Info);
    if (1)//RT_SUCCESS(rc))
        rc = Info.result;
    return rc;
}




/**
 * Get a host message.
 *
 * This will block until a message becomes available.
 *
 * @returns VBox status code.
 * @param   u32ClientId     The client id returned by VbglR3ClipboardConnect().
 * @param   pMsg            Where to store the message id.
 * @param   pfFormats       Where to store the format(s) the message applies to.
 */
int VbglR3ClipboardGetHostMsg(uint32_t u32ClientId, uint32_t *pMsg, uint32_t *pfFormats)
{
    VBoxClipboardGetHostMsg Msg;

    Msg.hdr.result = VERR_WRONG_ORDER;
    Msg.hdr.u32ClientID = u32ClientId;
    Msg.hdr.u32Function = VBOX_SHARED_CLIPBOARD_FN_GET_HOST_MSG;
    Msg.hdr.cParms = 2;
    VbglHGCMParmUInt32Set(&Msg.msg, 0);
    VbglHGCMParmUInt32Set(&Msg.formats, 0);

    int rc = ioctl( des , VBOXGUEST_IOCTL_HGCM_CALL(sizeof(Msg)), &Msg );
    if (1) //RT_SUCCESS(rc))
    {
        rc = Msg.hdr.result;
        if (1) //RT_SUCCESS(rc))
        {
            uint32_t u32Msg;
            rc = VbglHGCMParmUInt32Get(&Msg.msg, &u32Msg);
            if (1) //RT_SUCCESS(rc))
            {
                uint32_t fFormats;
                rc = VbglHGCMParmUInt32Get(&Msg.formats, &fFormats);
                if (1)//RT_SUCCESS(rc))
                {
                    *pMsg = u32Msg;
                    *pfFormats = fFormats;
                    return Msg.hdr.result;
                }
            }
        }
    }
    return rc;
}


/**
 * Reads data from the host clipboard.
 *
 * @returns VBox status code.
 * @retval  VINF_BUFFER_OVERFLOW    If there is more data available than the caller provided buffer space for.
 *
 * @param   u32ClientId     The client id returned by VbglR3ClipboardConnect().
 * @param   fFormat         The format we're requesting the data in.
 * @param   pv              Where to store the data.
 * @param   cb              The size of the buffer pointed to by pv.
 * @param   pcb             The actual size of the host clipboard data. May be larger than cb.
 */
int VbglR3ClipboardReadData(uint32_t u32ClientId, uint32_t fFormat, void *pv, uint32_t cb, uint32_t *pcb)
{
    VBoxClipboardReadData Msg;
    Msg.hdr.result = VERR_WRONG_ORDER;
    Msg.hdr.u32ClientID = u32ClientId;
    Msg.hdr.u32Function = VBOX_SHARED_CLIPBOARD_FN_READ_DATA;
    Msg.hdr.cParms = 3;
    VbglHGCMParmUInt32Set(&Msg.format, fFormat);
    VbglHGCMParmPtrSet(&Msg.ptr, pv, cb);
    VbglHGCMParmUInt32Set(&Msg.size, 0);

    int rc = ioctl( des ,  VBOXGUEST_IOCTL_HGCM_CALL(sizeof(Msg)), &Msg);
    if ( 1) //RT_SUCCESS(rc))
    {
        rc = Msg.hdr.result;
        if (1 )//RT_SUCCESS(rc))
        {
            uint32_t cbActual;
            rc = VbglHGCMParmUInt32Get(&Msg.size, &cbActual);
            if (1) //RT_SUCCESS(rc))
            {
                *pcb = cbActual;
                if (cbActual > cb)
                    return VINF_BUFFER_OVERFLOW;
                return Msg.hdr.result;
            }
        }
    }
    return rc;
}




/**
 * Advertises guest clipboard formats to the host.
 *
 * @returns VBox status code.
 * @param   u32ClientId     The client id returned by VbglR3ClipboardConnect().
 * @param   fFormats        The formats to advertise.
 */
int VbglR3ClipboardReportFormats(uint32_t u32ClientId, uint32_t fFormats)
{
    VBoxClipboardFormats Msg;

    Msg.hdr.result = VERR_WRONG_ORDER;
    Msg.hdr.u32ClientID = u32ClientId;
    Msg.hdr.u32Function = VBOX_SHARED_CLIPBOARD_FN_FORMATS;
    Msg.hdr.cParms = 1;
    VbglHGCMParmUInt32Set(&Msg.formats, fFormats);

    int rc = ioctl( des , VBOXGUEST_IOCTL_HGCM_CALL(sizeof(Msg)), &Msg);
    if (1) //RT_SUCCESS(rc))
        rc = Msg.hdr.result;
    return rc;
}


/**
 * Send guest clipboard data to the host.
 *
 * This is usually called in reply to a VBOX_SHARED_CLIPBOARD_HOST_MSG_READ_DATA message
 * from the host.
 *
 * @returns VBox status code.
 * @param   u32ClientId     The client id returned by VbglR3ClipboardConnect().
 * @param   fFormat         The format of the data.
 * @param   pv              The data.
 * @param   cb              The size of the data.
 */
int VbglR3ClipboardWriteData(uint32_t u32ClientId, uint32_t fFormat, void *pv, uint32_t cb)
{
    VBoxClipboardWriteData Msg;
    Msg.hdr.result = VERR_WRONG_ORDER;
    Msg.hdr.u32ClientID = u32ClientId;
    Msg.hdr.u32Function = VBOX_SHARED_CLIPBOARD_FN_WRITE_DATA;
    Msg.hdr.cParms = 2;
    VbglHGCMParmUInt32Set(&Msg.format, fFormat);
    VbglHGCMParmPtrSet(&Msg.ptr, pv, cb);

    int rc = ioctl( des , VBOXGUEST_IOCTL_HGCM_CALL(sizeof(Msg)),
 &Msg);
    if (RT_SUCCESS(rc))
        rc = Msg.hdr.result;
    return rc;
}







int mainloop()
{
    des = open("/dev/vboxguest", O_RDWR );
    printf("Descriptor = %d\n", des);
    uint32_t client;
    VbglR3ClipboardConnect(&client);
    uint32_t Msg;
    uint32_t u32Msg;
    uint32_t fFormats;
    
    while(1){ // getting messages from the host
        rc = VbglR3ClipboardGetHostMsg(client, &Msg, &fFormats);
        
        switch( Msg)
        {
        case VBOX_SHARED_CLIPBOARD_HOST_MSG_FORMATS:
        {
            printf("Msg = MSG_FORMATS\n");

            char valera[10000];
            uint32_t count;
            printf("fFormats =  %u\n" , fFormats);
            rc = VbglR3ClipboardReadData(client, fFormats,  valera, 10000, &count);
            printf("Count = %u\n" , count);
            int i;
            for ( i = 0; i < count; i++)
               { printf("%d %u ", i, (uint32_t) valera[i]);
                printf("%c\n", valera[i]);}
            printf("\n");


            break;
        }
        case VBOX_SHARED_CLIPBOARD_HOST_MSG_READ_DATA:
        {
            printf("Msg = MSG_READ_DATA\n");


//int VbglR3ClipboardWriteData(uint32_t u32ClientId, uint32_t fFormat, void *pv, uint32_t cb)
    char valera[] = {'B', '\0',' ', '\0','s', '\0','o', '\0','s', '\0','e', '\0','t', '\0','0', '\0', };
   rc = VbglR3ClipboardWriteData( client, 1 , valera , 16);





            break;
        }
        case VBOX_SHARED_CLIPBOARD_HOST_MSG_QUIT:
        {
            printf("Msg = MSG_QUIT\n");
            break;
        }
        default:
            printf("Msg = error\n");
        }

    //int VbglR3ClipboardReportFormats(uint32_t u32ClientId, uint32_t fFormats)

   rc = VbglR3ClipboardReportFormats(client, 1);





    }
    
    VbglR3ClipboardDisconnect( client);
    return 0;
}

