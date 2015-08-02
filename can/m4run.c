#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>

#include <ti/ipc/MultiProc.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/ipc/rpmsg/MessageQCopy.h>
#include <ti/srvmgr/NameMap.h>
#include <ti/resmgr/IpcResource.h>

#include "sysbios.h"
#include "io.h"

#define APP_NUM_ITERATIONS 100

#define REG32(A)   (*(volatile UInt32 *) (A))
#define HOST_TO_SYSM3_MBX       0 /* Rx on SysM3 from Host */

#define MAILBOX_BASEADDR (0xAA0F4000)

#define MAILBOX_MESSAGE(M)      (MAILBOX_BASEADDR + 0x040 + (0x4 * M))
#define MAILBOX_FIFOSTATUS(M)   (MAILBOX_BASEADDR + 0x080 + (0x4 * M))
#define MAILBOX_STATUS(M)       (MAILBOX_BASEADDR + 0x0C0 + (0x4 * M))

/* L4 phys and virt address */
/* L4_PER_XLATE_INC -- M4 MMU memory translating increment */
#define L4_PER_XLATE_INC	0x60000000UL /* 0x48000000 -> 0xA8000000 */
#define L4_PER			0x48000000UL 			/* phys */
#define L4_PER_VIRT		(L4_PER + L4_PER_XLATE_INC)	/* virt */

#define SAR_BANK1		0x4AE26000UL	/* SAR 1st page phys */
#define SAR_BANK1_VIRT	(SAR_BANK1 + L4_PER_XLATE_INC)

static Void startExchange(Uint32 arg);
Void task1(UArg arg0, UArg arg1);

static Void startExchange(Uint32 arg)
{
	MessageQCopy_Handle    handle;
	Char                   buffer[128];
	UInt32                 myEndpoint = 0;
	UInt32                 remoteEndpoint;
	UInt16                 len;
	UInt16				   dstProc;
	Int                    i;

	sb_printf("copyTask %d: Entered...:\n", arg);

	dstProc = MultiProc_getId("HOST");

	MessageQCopy_init(dstProc);

    /* Create the messageQ for receiving (and get our endpoint for sending). */
	handle = MessageQCopy_create(arg, &myEndpoint);

	NameMap_register("fuckit-all", arg);

	for (i = 0; i < APP_NUM_ITERATIONS; i++) {
		/* Await a character message: */
		MessageQCopy_recv(handle, (Ptr)buffer, &len, &remoteEndpoint,
							MessageQCopy_FOREVER);

		buffer[len] = '\0';

		len = 8;
		memcpy((Ptr)buffer, "response", 8);
		/* Send data back to remote endpoint: */
		MessageQCopy_send(dstProc, remoteEndpoint, myEndpoint,
							(Ptr)buffer, len);
	}

	/* Teardown our side: */
	MessageQCopy_delete(&handle);

	/* Free MessageQCopy module wide resources: */
	MessageQCopy_finalize();
}

Void task1(UArg arg0, UArg arg1)
{
	sb_printf("\nWaiting for message.\n");

	for (;;);

#if 0
	/* Waiting for a ping request from the remote (host) side */
	while (!REG32(MAILBOX_STATUS(HOST_TO_SYSM3_MBX)));

	startExchange((Uint32)arg0);
#endif
}

Void main(Int argc, Char *argv[])
{
	Task_Params params;
	volatile uint32_t *addr = (uint32_t *)SAR_BANK1_VIRT;

	serial_init();

	sb_printf("Sysbios main\r\n");

	*addr = 0xAABBCCDD;


	Task_Params_init(&params);
	params.instance->name = "test_copy";
	params.priority = 3;
	params.arg0 = 50;
	Task_create(task1, &params, NULL);

	BIOS_start();
}

