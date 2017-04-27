/*
 * (C) Copyright 2012 Stephen Warren
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <libc.h>
#include "log.h"
#include "timer.h"
#include "mailbox.h"

#define TIMEOUT (100 * 1000) /* 100mS in uS */

static s32 read_reg()
{
	struct bcm2835_mbox_regs *regs =
		(struct bcm2835_mbox_regs *)BCM2835_MBOX_PHYSADDR;

    readl((u32)(&(regs->read)));
    return 0;
}

int bcm2835_mbox_call_raw(u32 chan, u32 send, u32 *recv)
{
	struct bcm2835_mbox_regs *regs =
		(struct bcm2835_mbox_regs *)BCM2835_MBOX_PHYSADDR;
	u32 val;

	if (send & BCM2835_CHAN_MASK) {
		PRINT_ERR("mbox: Illegal mbox data 0x%x\n", send);
		return -1;
	}

	/* Drain any stale responses */

    if (wait_value(&regs->status, BCM2835_MBOX_STATUS_RD_EMPTY, 1, 100, read_reg) == -1) {
        PRINT_ERR("mbox: Timeout draining stale responses\n");
        return -1;
    }

	/* Wait for space to send */
    if (readl((u32)(&regs->status)) & BCM2835_MBOX_STATUS_WR_FULL) {
        PRINT_ERR("mbox: Timeout waiting for send space\n");
        return -1;
    }

	/* Send the request */

	val = BCM2835_MBOX_PACK(chan, send);
	PRINT_ERR("mbox: TX raw: 0x%x\n", val);
	writel((u32)(&regs->write), val);

	/* Wait for the response */
    if (wait_value(&regs->status, BCM2835_MBOX_STATUS_RD_EMPTY, 0, 100, NULL) == -1) {
			PRINT_ERR("mbox: Timeout waiting for response\n");
			return -1;
    }

	/* Read the response */

	val = readl((u32)(&regs->read));
	PRINT_ERR("mbox: RX raw: 0x%x\n", val);

	/* Validate the response */

	if (BCM2835_MBOX_UNPACK_CHAN(val) != chan) {
		PRINT_ERR("mbox: Response channel mismatch\n");
		return -1;
	}

	*recv = BCM2835_MBOX_UNPACK_DATA(val);

	return 0;
}

#ifdef DEBUG
void dump_buf(struct bcm2835_mbox_hdr *buffer)
{
	u32 *p;
	u32 words;
	int i;

	p = (u32 *)buffer;
	words = buffer->buf_size / 4;
	for (i = 0; i < words; i++)
		PRINT_ERR("    0x%x: 0x%x\n", i * 4, p[i]);
}
#endif

int bcm2835_mbox_call_prop(u32 chan, struct bcm2835_mbox_hdr *buffer)
{
	int ret;
	u32 rbuffer;
	struct bcm2835_mbox_tag_hdr *tag;
	int tag_index;

#ifdef DEBUG
	PRINT_ERR("mbox: TX buffer\n");
	dump_buf(buffer);
#endif

	ret = bcm2835_mbox_call_raw(chan, (u32)buffer, &rbuffer);
	if (ret)
		return ret;
	if (rbuffer != (u32)buffer) {
		PRINT_ERR("mbox: Response buffer mismatch (0x%x 0x%x)\n", rbuffer, buffer);
		return -1;
	}

#ifdef DEBUG
	PRINT_ERR("mbox: RX buffer\n");
	dump_buf(buffer);
#endif

	/* Validate overall response status */

	if (buffer->code != BCM2835_MBOX_RESP_CODE_SUCCESS) {
		PRINT_ERR("mbox: Header response code invalid\n");
		return -1;
	}

	/* Validate each tag's response status */

	tag = (void *)(buffer + 1);
	tag_index = 0;
	while (tag->tag) {
		if (!(tag->val_len & BCM2835_MBOX_TAG_VAL_LEN_RESPONSE)) {
			PRINT_ERR("mbox: Tag %d missing val_len response bit\n",
				tag_index);
			return -1;
		}
		/*
		 * Clear the reponse bit so clients can just look right at the
		 * length field without extra processing
		 */
		tag->val_len &= ~BCM2835_MBOX_TAG_VAL_LEN_RESPONSE;
		tag = (void *)(((u8 *)tag) + sizeof(*tag) + tag->val_buf_size);
		tag_index++;
	}

	return 0;
}
