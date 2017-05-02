#include <libc.h>
#include "common.h"
#include "mailbox.h"
#include "log.h"

u32 dram_init(void)
{
    u32 ram_size;
    s32 ret;
    ALLOC_ALIGN_BUFFER(struct msg_get_arm_mem, msg, 1, 16);

    BCM2835_MBOX_INIT_HDR(msg);
    BCM2835_MBOX_INIT_TAG(&msg->get_arm_mem, GET_ARM_MEMORY);

    ret = bcm2835_mbox_call_prop(BCM2835_MBOX_PROP_CHAN, &msg->hdr);
    if (ret) {
        PRINT_ERR("bcm2835: Could not query ARM memory size\n");
        return -1;
    }

    ram_size = msg->get_arm_mem.body.resp.mem_size;
    
    PRINT_ERR("DRAM: [0, 0x%x]\n", ram_size);

    return 0;
}
