#include <libc.h>
#include "common.h"
#include "mailbox.h"
#include "sdhci.h"
#include "mmc.h"
#include "log.h"

/* Set block count limit because of 16 bit register limit on some hardware*/
#ifndef CONFIG_SYS_MMC_MAX_BLK_COUNT
#define CONFIG_SYS_MMC_MAX_BLK_COUNT 65535
#endif

static struct list_head mmc_devices;
static int curr_dev_num = -1;

static int mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
                    struct mmc_data *data)
{
    struct mmc_data backup;
    int ret;

    memset(&backup, 0, sizeof(backup));

#ifdef CONFIG_MMC_TRACE
    int i;
    u8 *ptr;

    PRINT_ERR("CMD_SEND:%d\n", cmd->cmdidx);
    PRINT_ERR("\t\tARG\t\t\t 0x%08X\n", cmd->cmdarg);
    ret = mmc->send_cmd(mmc, cmd, data);
    switch (cmd->resp_type) {
        case MMC_RSP_NONE:
            PRINT_ERR("\t\tMMC_RSP_NONE\n");
            break;
        case MMC_RSP_R1:
            PRINT_ERR("\t\tMMC_RSP_R1,5,6,7 \t 0x%08X \n",
                    cmd->response[0]);
            break;
        case MMC_RSP_R1b:
            PRINT_ERR("\t\tMMC_RSP_R1b\t\t 0x%08X \n",
                    cmd->response[0]);
            break;
        case MMC_RSP_R2:
            PRINT_ERR("\t\tMMC_RSP_R2\t\t 0x%08X \n",
                    cmd->response[0]);
            PRINT_ERR("\t\t          \t\t 0x%08X \n",
                    cmd->response[1]);
            PRINT_ERR("\t\t          \t\t 0x%08X \n",
                    cmd->response[2]);
            PRINT_ERR("\t\t          \t\t 0x%08X \n",
                    cmd->response[3]);
            PRINT_ERR("\n");
            PRINT_ERR("\t\t\t\t\tDUMPING DATA\n");
            for (i = 0; i < 4; i++) {

                int j;
                PRINT_ERR("\t\t\t\t\t%03d - ", i*4);
                ptr = (u8 *)&cmd->response[i];
                ptr += 3;
                for (j = 0; j < 4; j++)
                    PRINT_ERR("%02X ", *ptr--);
                PRINT_ERR("\n");
            }
            break;
        case MMC_RSP_R3:
            PRINT_ERR("\t\tMMC_RSP_R3,4\t\t 0x%08X \n",
                    cmd->response[0]);
            break;
        default:
            PRINT_ERR("\t\tERROR MMC rsp not supported\n");
            break;
    }
#else
    ret = mmc->send_cmd(mmc, cmd, data);
#endif
    return ret;
}

static int mmc_set_blocklen(struct mmc *mmc, int len)
{
    struct mmc_cmd cmd;

    cmd.cmdidx = MMC_CMD_SET_BLOCKLEN;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmdarg = len;

    return mmc_send_cmd(mmc, &cmd, NULL); 
}

static int mmc_read_blocks(struct mmc *mmc, void *dst, u32 start,
                       unsigned long blkcnt)
{
    struct mmc_cmd cmd;
    struct mmc_data data;

    if (blkcnt > 1)
        cmd.cmdidx = MMC_CMD_READ_MULTIPLE_BLOCK;
    else
        cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;

    if (mmc->high_capacity)
        cmd.cmdarg = start;
    else
        cmd.cmdarg = start * mmc->read_bl_len;

    cmd.resp_type = MMC_RSP_R1;

    data.dest = dst;
    data.blocks = blkcnt;
    data.blocksize = mmc->read_bl_len;
    data.flags = MMC_DATA_READ;

    if (mmc_send_cmd(mmc, &cmd, &data))
        return 0;

    if (blkcnt > 1) {
        cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
        cmd.cmdarg = 0;
        cmd.resp_type = MMC_RSP_R1b;
        if (mmc_send_cmd(mmc, &cmd, NULL)) {
            PRINT_ERR("mmc fail to send stop cmd\n");
            return 0;
        }
    }

    return blkcnt;
}

static int mmc_send_status(struct mmc *mmc, int timeout)
{
    struct mmc_cmd cmd;
    int err, retries = 5;
#ifdef CONFIG_MMC_TRACE
    int status;
#endif

    cmd.cmdidx = MMC_CMD_SEND_STATUS;
    cmd.resp_type = MMC_RSP_R1;
    if (!mmc_host_is_spi(mmc))
        cmd.cmdarg = mmc->rca << 16;

    do {
        err = mmc_send_cmd(mmc, &cmd, NULL);
        if (!err) {
            if ((cmd.response[0] & MMC_STATUS_RDY_FOR_DATA) &&
                    (cmd.response[0] & MMC_STATUS_CURR_STATE) !=
                    MMC_STATE_PRG)
                break;
            else if (cmd.response[0] & MMC_STATUS_MASK) {
                PRINT_ERR("Status Error: 0x%x\n",
                        cmd.response[0]);
                return COMM_ERR;
            }
        } else if (--retries < 0)
            return err;

        udelay(1000);

    } while (timeout--);

#ifdef CONFIG_MMC_TRACE
    status = (cmd.response[0] & MMC_STATUS_CURR_STATE) >> 9;
    PRINT_ERR("CURR STATE:%d\n", status);
#endif
    if (timeout <= 0) {
        PRINT_ERR("Timeout waiting card ready\n");
        return TIMEOUT;

    }

    return 0;
}

static u32 mmc_erase_t(struct mmc *mmc, u32 start, unsigned long blkcnt)
{
    struct mmc_cmd cmd;
    u32 end;
    int err, start_cmd, end_cmd;

    if (mmc->high_capacity)
        end = start + blkcnt - 1;
    else {
        end = (start + blkcnt - 1) * mmc->write_bl_len;
        start *= mmc->write_bl_len;
    }

    if (IS_SD(mmc)) {
        start_cmd = SD_CMD_ERASE_WR_BLK_START;
        end_cmd = SD_CMD_ERASE_WR_BLK_END;
    } else {
        start_cmd = MMC_CMD_ERASE_GROUP_START;
        end_cmd = MMC_CMD_ERASE_GROUP_END;
    }

    cmd.cmdidx = start_cmd;
    cmd.cmdarg = start;
    cmd.resp_type = MMC_RSP_R1;

    err = mmc_send_cmd(mmc, &cmd, NULL);
    if (err)
        goto err_out;

    cmd.cmdidx = end_cmd;
    cmd.cmdarg = end;

    err = mmc_send_cmd(mmc, &cmd, NULL);
    if (err)
        goto err_out;

    cmd.cmdidx = MMC_CMD_ERASE;
    cmd.cmdarg = SECURE_ERASE;
    cmd.resp_type = MMC_RSP_R1b;


    err = mmc_send_cmd(mmc, &cmd, NULL);
    if (err)
        goto err_out;

    return 0;

err_out:
    PRINT_ERR("mmc erase failed\n");
    return err;
}

static u32
mmc_write_blocks(struct mmc *mmc, u32 start, unsigned long blkcnt, const void*src)
{
    struct mmc_cmd cmd;
    struct mmc_data data;
    int timeout = 1000;

    if ((start + blkcnt) > mmc->block_dev.lba) {
        PRINT_ERR("MMC: block number 0x%x exceeds max(0x%x)\n",
                start + blkcnt, mmc->block_dev.lba);
        return 0;
    }

    if (blkcnt > 1)
        cmd.cmdidx = MMC_CMD_WRITE_MULTIPLE_BLOCK;
    else
        cmd.cmdidx = MMC_CMD_WRITE_SINGLE_BLOCK;

    if (mmc->high_capacity)
        cmd.cmdarg = start;
    else
        cmd.cmdarg = start * mmc->write_bl_len;

    cmd.resp_type = MMC_RSP_R1;

    data.src = src;
    data.blocks = blkcnt;
    data.blocksize = mmc->write_bl_len;
    data.flags = MMC_DATA_WRITE;

    if (mmc_send_cmd(mmc, &cmd, &data)) {
        PRINT_ERR("mmc write failed\n");
        return 0;
    }

    /* SPI multiblock writes terminate using a special
     * token, not a STOP_TRANSMISSION request.
     */
    if (!mmc_host_is_spi(mmc) && blkcnt > 1) {
        cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
        cmd.cmdarg = 0;
        cmd.resp_type = MMC_RSP_R1b;
        if (mmc_send_cmd(mmc, &cmd, NULL)) {
            PRINT_ERR("mmc fail to send stop cmd\n");
            return 0;
        }
    }

    /* Waiting for the ready status */
    if (mmc_send_status(mmc, timeout))
        return 0;

    return blkcnt;
}

struct mmc *find_mmc_device(int dev_num)
{ 
#if 1
    struct mmc *m;
    struct list_head *entry;

    list_for_each(entry, &mmc_devices) {
        m = list_entry(entry, struct mmc, link);

        if (m->block_dev.dev == dev_num)
            return m;
    }    

    PRINT_ERR("MMC Device %d not found\n", dev_num);
#endif
    return NULL;
}


static u32 mmc_bwrite(s32 dev_num, u32 start, u32 blkcnt, const void*src)
{
    unsigned long cur, blocks_todo = blkcnt;

    struct mmc *mmc = find_mmc_device(dev_num);
    if (!mmc)
        return 0;

    if (mmc_set_blocklen(mmc, mmc->write_bl_len))
        return 0;

    do {
        cur = (blocks_todo > mmc->b_max) ?  mmc->b_max : blocks_todo;
        if(mmc_write_blocks(mmc, start, cur, src) != cur)
            return 0;
        blocks_todo -= cur;
        start += cur;
        src += cur * mmc->write_bl_len;
    } while (blocks_todo > 0);

    return blkcnt;
}

static u32 mmc_berase(s32 dev_num, u32 start, u32 blkcnt)
{
    int err = 0;
    struct mmc *mmc = find_mmc_device(dev_num);
    unsigned long blk = 0, blk_r = 0;
    int timeout = 1000;

    if (!mmc)
        return -1;

    if ((start % mmc->erase_grp_size) || (blkcnt % mmc->erase_grp_size))
        PRINT_ERR("\n\nCaution! Your devices Erase group is 0x%x\n"
                "The erase range would be change to 0x%x~0x%x\n\n",
                mmc->erase_grp_size, start & ~(mmc->erase_grp_size - 1),
                ((start + blkcnt + mmc->erase_grp_size)
                 & ~(mmc->erase_grp_size - 1)) - 1);

    while (blk < blkcnt) {
        blk_r = ((blkcnt - blk) > mmc->erase_grp_size) ?
            mmc->erase_grp_size : (blkcnt - blk);
        err = mmc_erase_t(mmc, start + blk, blk_r);
        if (err)
            break;

        blk += blk_r;

        /* Waiting for the ready status */
        if (mmc_send_status(mmc, timeout))
            return 0;
    }

    return blk;
}

static u32 mmc_bread(s32 dev_num, u32 start, u32 blkcnt, void *dst)
{
    unsigned long cur, blocks_todo = blkcnt;

    if (blkcnt == 0)
        return 0;

    struct mmc *mmc = find_mmc_device(dev_num);
    if (!mmc)
        return 0;

    if ((start + blkcnt) > mmc->block_dev.lba) {
        PRINT_ERR("MMC: block number 0x%x exceeds max(0x%x)\n",
                start + blkcnt, mmc->block_dev.lba);
        return 0;
    }

    if (mmc_set_blocklen(mmc, mmc->read_bl_len))
        return 0;

    do {
        cur = (blocks_todo > mmc->b_max) ?  mmc->b_max : blocks_todo;
        if(mmc_read_blocks(mmc, dst, start, cur) != cur)
            return 0;
        blocks_todo -= cur;
        start += cur;
        dst += cur * mmc->read_bl_len;
    } while (blocks_todo > 0);

    return blkcnt;
}

s32 mmc_register(struct mmc *mmc)
{
    /* Setup the universal parts of the block interface just once */
    mmc->block_dev.if_type = IF_TYPE_MMC;
    mmc->block_dev.dev = curr_dev_num++;
    mmc->block_dev.removable = 1; 
    mmc->block_dev.block_read = mmc_bread;
    mmc->block_dev.block_write = mmc_bwrite;
    mmc->block_dev.block_erase = mmc_berase;
    if (!mmc->b_max)
        mmc->b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;

    INIT_LIST_HEAD (&mmc->link);

    list_add_tail (&mmc->link, &mmc_devices);

    return 0;
}

int board_mmc_init(void)
{
    ALLOC_ALIGN_BUFFER(struct msg_set_power_state, msg_pwr, 1, 16);
    ALLOC_ALIGN_BUFFER(struct msg_get_clock_rate, msg_clk, 1, 16);
    int ret;

    BCM2835_MBOX_INIT_HDR(msg_pwr);
    BCM2835_MBOX_INIT_TAG(&msg_pwr->set_power_state,
            SET_POWER_STATE);
    msg_pwr->set_power_state.body.req.device_id =
        BCM2835_MBOX_POWER_DEVID_SDHCI;
    msg_pwr->set_power_state.body.req.state =
        BCM2835_MBOX_SET_POWER_STATE_REQ_ON |
        BCM2835_MBOX_SET_POWER_STATE_REQ_WAIT;

    ret = bcm2835_mbox_call_prop(BCM2835_MBOX_PROP_CHAN,
            &msg_pwr->hdr);
    if (ret) {
        PRINT_ERR("bcm2835: Could not set SDHCI power state\n");
        return -1; 
    }   

    BCM2835_MBOX_INIT_HDR(msg_clk);
    BCM2835_MBOX_INIT_TAG(&msg_clk->get_clock_rate, GET_CLOCK_RATE);
    msg_clk->get_clock_rate.body.req.clock_id = BCM2835_MBOX_CLOCK_ID_EMMC;

    ret = bcm2835_mbox_call_prop(BCM2835_MBOX_PROP_CHAN, &msg_clk->hdr);
    if (ret) {
        PRINT_ERR("bcm2835: Could not query eMMC clock rate\n");
        return -1; 
    }   

    return bcm2835_sdhci_init(BCM2835_SDHCI_BASE,
            msg_clk->get_clock_rate.body.resp.rate_hz);
}

#if 1
int mmc_getcd(struct mmc *mmc)
{
    int cd;

    /* cd = board_mmc_getcd(mmc); */
    cd = -1;

    if ((cd < 0) && mmc->getcd)
        cd = mmc->getcd(mmc);

    return cd;
}

static int mmc_switch(struct mmc *mmc, u8 set, u8 index, u8 value)
{
    struct mmc_cmd cmd;
    int timeout = 1000;
    int ret;

    cmd.cmdidx = MMC_CMD_SWITCH;
    cmd.resp_type = MMC_RSP_R1b;
    cmd.cmdarg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
                 (index << 16) |
                 (value << 8);

    ret = mmc_send_cmd(mmc, &cmd, NULL);

    /* Waiting for the ready status */
    if (!ret)
        ret = mmc_send_status(mmc, timeout);

    return ret;

}

static u32 __be32_to_cpu(u32 x)
{
    u8 *pc;
    u32 rv; 

    pc = (u8 *)&x;

    rv = pc[0] << 24 | pc[1] << 16 | pc[2] << 8 | pc[3];
    return rv;
}

u32 __div64_32(u64 *n, u32 base)
{
    u64 rem = *n;
    u64 b = base;
    u64 res, d = 1;
    u32 high = rem >> 32;

    /* Reduce the thing a bit first */
    res = 0;
    if (high >= base) {
        high /= base;
        res = (u64) high << 32;
        rem -= (u64) (high*base) << 32;
    }

    while ((s64)b > 0 && b < rem) {
        b = b+b;
        d = d+d;
    }

    do {
        if (rem >= b) {
            rem -= b;
            res += d;
        }
        b >>= 1;
        d >>= 1;
    } while (d);

    *n = res;
    return rem;
}

static int mmc_go_idle(struct mmc *mmc)
{
    struct mmc_cmd cmd;
    int err;

    udelay(1000);

    cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
    cmd.cmdarg = 0;
    cmd.resp_type = MMC_RSP_NONE;

    err = mmc_send_cmd(mmc, &cmd, NULL);

    if (err)
        return err;

    udelay(2000);

    return 0;
}

static int mmc_send_op_cond(struct mmc *mmc)
{
    int timeout = 10000;
    struct mmc_cmd cmd;
    int err;

    /* Some cards seem to need this */
    mmc_go_idle(mmc);

    /* Asking to the card its capabilities */
    cmd.cmdidx = MMC_CMD_SEND_OP_COND;
    cmd.resp_type = MMC_RSP_R3;
    cmd.cmdarg = 0;

    err = mmc_send_cmd(mmc, &cmd, NULL);

    if (err)
        return err;

    udelay(1000);

    do {
        cmd.cmdidx = MMC_CMD_SEND_OP_COND;
        cmd.resp_type = MMC_RSP_R3;
        cmd.cmdarg = (mmc_host_is_spi(mmc) ? 0 :
                (mmc->voltages &
                (cmd.response[0] & OCR_VOLTAGE_MASK)) |
                (cmd.response[0] & OCR_ACCESS_MODE));

        if (mmc->host_caps & MMC_MODE_HC)
            cmd.cmdarg |= OCR_HCS;

        err = mmc_send_cmd(mmc, &cmd, NULL);

        if (err)
            return err;

        udelay(1000);
    } while (!(cmd.response[0] & OCR_BUSY) && timeout--);

    if (timeout <= 0)
        return UNUSABLE_ERR;

    if (mmc_host_is_spi(mmc)) { /* read OCR for spi */
        cmd.cmdidx = MMC_CMD_SPI_READ_OCR;
        cmd.resp_type = MMC_RSP_R3;
        cmd.cmdarg = 0;

        err = mmc_send_cmd(mmc, &cmd, NULL);

        if (err)
            return err;
    }

    mmc->version = MMC_VERSION_UNKNOWN;
    mmc->ocr = cmd.response[0];

    mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
    mmc->rca = 0;

    return 0;
}

static int sd_switch(struct mmc *mmc, int mode, int group, u8 value, u8 *resp)
{
    struct mmc_cmd cmd;
    struct mmc_data data;

    /* Switch the frequency */
    cmd.cmdidx = SD_CMD_SWITCH_FUNC;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmdarg = (mode << 31) | 0xffffff;
    cmd.cmdarg &= ~(0xf << (group * 4));
    cmd.cmdarg |= value << (group * 4);

    data.dest = (char *)resp;
    data.blocksize = 64;
    data.blocks = 1;
    data.flags = MMC_DATA_READ;

    return mmc_send_cmd(mmc, &cmd, &data);
}

static int sd_change_freq(struct mmc *mmc)
{
    int err;
    struct mmc_cmd cmd;
    ALLOC_ALIGN_BUFFER(u32, scr, 2, 64);
    ALLOC_ALIGN_BUFFER(u32, switch_status, 16, 64);
    struct mmc_data data;
    int timeout;

    mmc->card_caps = 0;

    if (mmc_host_is_spi(mmc))
        return 0;

    /* Read the SCR to find out if this card supports higher speeds */
    cmd.cmdidx = MMC_CMD_APP_CMD;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmdarg = mmc->rca << 16;

    err = mmc_send_cmd(mmc, &cmd, NULL);

    if (err)
        return err;

    cmd.cmdidx = SD_CMD_APP_SEND_SCR;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmdarg = 0;

    timeout = 3;

retry_scr:
    data.dest = (char *)scr;
    data.blocksize = 8;
    data.blocks = 1;
    data.flags = MMC_DATA_READ;

    err = mmc_send_cmd(mmc, &cmd, &data);

    if (err) {
        if (timeout--)
            goto retry_scr;

        return err;
    }

    mmc->scr[0] = __be32_to_cpu(scr[0]);
    mmc->scr[1] = __be32_to_cpu(scr[1]);

    switch ((mmc->scr[0] >> 24) & 0xf) {
        case 0:
            mmc->version = SD_VERSION_1_0;
            break;
        case 1:
            mmc->version = SD_VERSION_1_10;
            break;
        case 2:
            mmc->version = SD_VERSION_2;
            break;
        default:
            mmc->version = SD_VERSION_1_0;
            break;
    }

    if (mmc->scr[0] & SD_DATA_4BIT)
        mmc->card_caps |= MMC_MODE_4BIT;

    /* Version 1.0 doesn't support switching */
    if (mmc->version == SD_VERSION_1_0)
        return 0;

    timeout = 4;
    while (timeout--) {
        err = sd_switch(mmc, SD_SWITCH_CHECK, 0, 1,
                (u8 *)switch_status);

        if (err)
            return err;

        /* The high-speed function is busy.  Try again */
        if (!(__be32_to_cpu(switch_status[7]) & SD_HIGHSPEED_BUSY))
            break;
    }

    /* If high-speed isn't supported, we return */
    if (!(__be32_to_cpu(switch_status[3]) & SD_HIGHSPEED_SUPPORTED))
        return 0;

    /*
     * If the host doesn't support SD_HIGHSPEED, do not switch card to
     * HIGHSPEED mode even if the card support SD_HIGHSPPED.
     * This can avoid furthur problem when the card runs in different
     * mode between the host.
     */
    if (!((mmc->host_caps & MMC_MODE_HS_52MHz) &&
        (mmc->host_caps & MMC_MODE_HS)))
        return 0;

    err = sd_switch(mmc, SD_SWITCH_SWITCH, 0, 1, (u8 *)switch_status);

    if (err)
        return err;

    if ((__be32_to_cpu(switch_status[4]) & 0x0f000000) == 0x01000000)
        mmc->card_caps |= MMC_MODE_HS;

    return 0;
}
/* frequency bases */
/* divided by 10 to be nice to platforms without floating point */
static const int fbase[] = {
    10000,
    100000,
    1000000,
    10000000,
};

/* Multiplier values for TRAN_SPEED.  Multiplied by 10 to be nice
 * to platforms without floating point.
 */
static const int multipliers[] = {
    0,  /* reserved */
    10,
    12,
    13,
    15,
    20,
    25,
    30,
    35,
    40,
    45,
    50,
    55,
    60,
    70,
    80,
};

static void mmc_set_ios(struct mmc *mmc)
{
    mmc->set_ios(mmc);
}

static void mmc_set_bus_width(struct mmc *mmc, u32 width)
{
    mmc->bus_width = width;

    mmc_set_ios(mmc);
}

void mmc_set_clock(struct mmc *mmc, u32 clock)
{
    if (clock > mmc->f_max)
        clock = mmc->f_max;

    if (clock < mmc->f_min)
        clock = mmc->f_min;

    mmc->clock = clock;

    mmc_set_ios(mmc);
}

static int mmc_send_if_cond(struct mmc *mmc)
{
    struct mmc_cmd cmd;
    int err;

    cmd.cmdidx = SD_CMD_SEND_IF_COND;
    /* We set the bit if the host supports voltages between 2.7 and 3.6 V */
    cmd.cmdarg = ((mmc->voltages & 0xff8000) != 0) << 8 | 0xaa;
    cmd.resp_type = MMC_RSP_R7;

    err = mmc_send_cmd(mmc, &cmd, NULL);

    if (err)
        return err;

    if ((cmd.response[0] & 0xff) != 0xaa)
        return UNUSABLE_ERR;
    else
        mmc->version = SD_VERSION_2;

    return 0;
}

static int sd_send_op_cond(struct mmc *mmc)
{
    int timeout = 1000;
    int err;
    struct mmc_cmd cmd;

    do {
        cmd.cmdidx = MMC_CMD_APP_CMD;
        cmd.resp_type = MMC_RSP_R1;
        cmd.cmdarg = 0;

        err = mmc_send_cmd(mmc, &cmd, NULL);

        if (err)
            return err;

        cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
        cmd.resp_type = MMC_RSP_R3;

        /*
         * Most cards do not answer if some reserved bits
         * in the ocr are set. However, Some controller
         * can set bit 7 (reserved for low voltages), but
         * how to manage low voltages SD card is not yet
         * specified.
         */
        cmd.cmdarg = mmc_host_is_spi(mmc) ? 0 :
            (mmc->voltages & 0xff8000);

        if (mmc->version == SD_VERSION_2)
            cmd.cmdarg |= OCR_HCS;

        err = mmc_send_cmd(mmc, &cmd, NULL);

        if (err)
            return err;

        udelay(1000);
    } while ((!(cmd.response[0] & OCR_BUSY)) && timeout--);

    if (timeout <= 0)
        return UNUSABLE_ERR;

    if (mmc->version != SD_VERSION_2)
        mmc->version = SD_VERSION_1_0;

    if (mmc_host_is_spi(mmc)) { /* read OCR for spi */
        cmd.cmdidx = MMC_CMD_SPI_READ_OCR;
        cmd.resp_type = MMC_RSP_R3;
        cmd.cmdarg = 0;

        err = mmc_send_cmd(mmc, &cmd, NULL);

        if (err)
            return err;
    }

    mmc->ocr = cmd.response[0];

    mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
    mmc->rca = 0;

    return 0;
}

static int mmc_send_ext_csd(struct mmc *mmc, u8 *ext_csd)
{
    struct mmc_cmd cmd;
    struct mmc_data data;
    int err;

    /* Get the Card Status Register */
    cmd.cmdidx = MMC_CMD_SEND_EXT_CSD;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmdarg = 0;

    data.dest = (char *)ext_csd;
    data.blocks = 1;
    data.blocksize = 512;
    data.flags = MMC_DATA_READ;

    err = mmc_send_cmd(mmc, &cmd, &data);

    return err;
}

static int mmc_change_freq(struct mmc *mmc)
{
    ALLOC_ALIGN_BUFFER(u8, ext_csd, 512, 64);
    char cardtype;
    int err;

    mmc->card_caps = 0;

    if (mmc_host_is_spi(mmc))
        return 0;

    /* Only version 4 supports high-speed */
    if (mmc->version < MMC_VERSION_4)
        return 0;

    err = mmc_send_ext_csd(mmc, ext_csd);

    if (err)
        return err;

    cardtype = ext_csd[EXT_CSD_CARD_TYPE] & 0xf;

    err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_HS_TIMING, 1);

    if (err)
        return err;

    /* Now check to see that it worked */
    err = mmc_send_ext_csd(mmc, ext_csd);

    if (err)
        return err;

    /* No high-speed support */
    if (!ext_csd[EXT_CSD_HS_TIMING])
        return 0;

    /* High Speed is set, there are two types: 52MHz and 26MHz */
    if (cardtype & MMC_HS_52MHZ)
        mmc->card_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;
    else
        mmc->card_caps |= MMC_MODE_HS;

    return 0;
}

/* The unnecessary pointer compare is there
 * to check for type safety (n must be 64bit)
 */
# define do_div(n,base) ({              \
    u32 __base = (base);           \
    u32 __rem;                 \
    (void)(((typeof((n)) *)0) == ((u64 *)0));  \
    if (((n) >> 32) == 0) {         \
        __rem = (u32)(n) % __base;     \
        (n) = (u32)(n) / __base;       \
    } else                      \
        __rem = __div64_32(&(n), __base);   \
    __rem;                      \
 })

/* Wrapper for do_div(). Doesn't modify dividend and returns
 * the result, not reminder.
 */
static inline u64 lldiv(u64 dividend, u32 divisor)
{
    u64 __res = dividend;
    do_div(__res, divisor);
    return(__res);
}


static int mmc_startup(struct mmc *mmc)
{
    int err, width;
    u32 mult, freq;
    u64 cmult, csize, capacity;
    struct mmc_cmd cmd;
    ALLOC_ALIGN_BUFFER(u8, ext_csd, 512, 64);
    ALLOC_ALIGN_BUFFER(u8, test_csd, 512, 64);
    int timeout = 1000;

#ifdef CONFIG_MMC_SPI_CRC_ON
    if (mmc_host_is_spi(mmc)) { /* enable CRC check for spi */
        cmd.cmdidx = MMC_CMD_SPI_CRC_ON_OFF;
        cmd.resp_type = MMC_RSP_R1;
        cmd.cmdarg = 1;
        err = mmc_send_cmd(mmc, &cmd, NULL);

        if (err)
            return err;
    }
#endif

    /* Put the Card in Identify Mode */
    cmd.cmdidx = mmc_host_is_spi(mmc) ? MMC_CMD_SEND_CID :
        MMC_CMD_ALL_SEND_CID; /* cmd not supported in spi */
    cmd.resp_type = MMC_RSP_R2;
    cmd.cmdarg = 0;

    err = mmc_send_cmd(mmc, &cmd, NULL);

    if (err)
        return err;

    memcpy(mmc->cid, cmd.response, 16);

    /*
     * For MMC cards, set the Relative Address.
     * For SD cards, get the Relatvie Address.
     * This also puts the cards into Standby State
     */
    if (!mmc_host_is_spi(mmc)) { /* cmd not supported in spi */
        cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
        cmd.cmdarg = mmc->rca << 16;
        cmd.resp_type = MMC_RSP_R6;

        err = mmc_send_cmd(mmc, &cmd, NULL);

        if (err)
            return err;

        if (IS_SD(mmc))
            mmc->rca = (cmd.response[0] >> 16) & 0xffff;
    }

    /* Get the Card-Specific Data */
    cmd.cmdidx = MMC_CMD_SEND_CSD;
    cmd.resp_type = MMC_RSP_R2;
    cmd.cmdarg = mmc->rca << 16;

    err = mmc_send_cmd(mmc, &cmd, NULL);

    /* Waiting for the ready status */
    mmc_send_status(mmc, timeout);

    if (err)
        return err;

    mmc->csd[0] = cmd.response[0];
    mmc->csd[1] = cmd.response[1];
    mmc->csd[2] = cmd.response[2];
    mmc->csd[3] = cmd.response[3];

    if (mmc->version == MMC_VERSION_UNKNOWN) {
        int version = (cmd.response[0] >> 26) & 0xf;

        switch (version) {
            case 0:
                mmc->version = MMC_VERSION_1_2;
                break;
            case 1:
                mmc->version = MMC_VERSION_1_4;
                break;
            case 2:
                mmc->version = MMC_VERSION_2_2;
                break;
            case 3:
                mmc->version = MMC_VERSION_3;
                break;
            case 4:
                mmc->version = MMC_VERSION_4;
                break;
            default:
                mmc->version = MMC_VERSION_1_2;
                break;
        }
    }

    /* divide frequency by 10, since the mults are 10x bigger */
    freq = fbase[(cmd.response[0] & 0x7)];
    mult = multipliers[((cmd.response[0] >> 3) & 0xf)];

    mmc->tran_speed = freq * mult;

    mmc->read_bl_len = 1 << ((cmd.response[1] >> 16) & 0xf);

    if (IS_SD(mmc))
        mmc->write_bl_len = mmc->read_bl_len;
    else
        mmc->write_bl_len = 1 << ((cmd.response[3] >> 22) & 0xf);

    if (mmc->high_capacity) {
        csize = (mmc->csd[1] & 0x3f) << 16
            | (mmc->csd[2] & 0xffff0000) >> 16;
        cmult = 8;
    } else {
        csize = (mmc->csd[1] & 0x3ff) << 2
            | (mmc->csd[2] & 0xc0000000) >> 30;
        cmult = (mmc->csd[2] & 0x00038000) >> 15;
    }

    mmc->capacity = (csize + 1) << (cmult + 2);
    mmc->capacity *= mmc->read_bl_len;

    if (mmc->read_bl_len > 512)
        mmc->read_bl_len = 512;

    if (mmc->write_bl_len > 512)
        mmc->write_bl_len = 512;

    /* Select the card, and put it into Transfer Mode */
    if (!mmc_host_is_spi(mmc)) { /* cmd not supported in spi */
        cmd.cmdidx = MMC_CMD_SELECT_CARD;
        cmd.resp_type = MMC_RSP_R1;
        cmd.cmdarg = mmc->rca << 16;
        err = mmc_send_cmd(mmc, &cmd, NULL);

        if (err)
            return err;
    }

    /*
     * For SD, its erase group is always one sector
     */
    mmc->erase_grp_size = 1;
    mmc->part_config = MMCPART_NOAVAILABLE;
    if (!IS_SD(mmc) && (mmc->version >= MMC_VERSION_4)) {
        /* check  ext_csd version and capacity */
        err = mmc_send_ext_csd(mmc, ext_csd);
        if (!err && (ext_csd[EXT_CSD_REV] >= 2)) {
            /*
             * According to the JEDEC Standard, the value of
             * ext_csd's capacity is valid if the value is more
             * than 2GB
             */
            capacity = ext_csd[EXT_CSD_SEC_CNT] << 0
                    | ext_csd[EXT_CSD_SEC_CNT + 1] << 8
                    | ext_csd[EXT_CSD_SEC_CNT + 2] << 16
                    | ext_csd[EXT_CSD_SEC_CNT + 3] << 24;
            capacity *= 512;
            if ((capacity >> 20) > 2 * 1024)
                mmc->capacity = capacity;
        }

        /*
         * Check whether GROUP_DEF is set, if yes, read out
         * group size from ext_csd directly, or calculate
         * the group size from the csd value.
         */
        if (ext_csd[EXT_CSD_ERASE_GROUP_DEF])
            mmc->erase_grp_size =
                  ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE] * 512 * 1024;
        else {
            int erase_gsz, erase_gmul;
            erase_gsz = (mmc->csd[2] & 0x00007c00) >> 10;
            erase_gmul = (mmc->csd[2] & 0x000003e0) >> 5;
            mmc->erase_grp_size = (erase_gsz + 1)
                * (erase_gmul + 1);
        }

        /* store the partition info of emmc */
        if ((ext_csd[EXT_CSD_PARTITIONING_SUPPORT] & PART_SUPPORT) ||
            ext_csd[EXT_CSD_BOOT_MULT])
            mmc->part_config = ext_csd[EXT_CSD_PART_CONF];
    }

    if (IS_SD(mmc))
        err = sd_change_freq(mmc);
    else
        err = mmc_change_freq(mmc);

    if (err)
        return err;

    /* Restrict card's capabilities by what the host can do */
    mmc->card_caps &= mmc->host_caps;

    if (IS_SD(mmc)) {
        if (mmc->card_caps & MMC_MODE_4BIT) {
            cmd.cmdidx = MMC_CMD_APP_CMD;
            cmd.resp_type = MMC_RSP_R1;
            cmd.cmdarg = mmc->rca << 16;

            err = mmc_send_cmd(mmc, &cmd, NULL);
            if (err)
                return err;

            cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
            cmd.resp_type = MMC_RSP_R1;
            cmd.cmdarg = 2;
            err = mmc_send_cmd(mmc, &cmd, NULL);
            if (err)
                return err;

            mmc_set_bus_width(mmc, 4);
        }

        if (mmc->card_caps & MMC_MODE_HS)
            mmc->tran_speed = 50000000;
        else
            mmc->tran_speed = 25000000;
    } else {
        width = ((mmc->host_caps & MMC_MODE_MASK_WIDTH_BITS) >>
             MMC_MODE_WIDTH_BITS_SHIFT);
        for (; width >= 0; width--) {
            /* Set the card to use 4 bit*/
            err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
                    EXT_CSD_BUS_WIDTH, width);

            if (err)
                continue;

            if (!width) {
                mmc_set_bus_width(mmc, 1);
                break;
            } else
                mmc_set_bus_width(mmc, 4 * width);

            err = mmc_send_ext_csd(mmc, test_csd);
            if (!err && ext_csd[EXT_CSD_PARTITIONING_SUPPORT] \
                    == test_csd[EXT_CSD_PARTITIONING_SUPPORT]
                 && ext_csd[EXT_CSD_ERASE_GROUP_DEF] \
                    == test_csd[EXT_CSD_ERASE_GROUP_DEF] \
                 && ext_csd[EXT_CSD_REV] \
                    == test_csd[EXT_CSD_REV]
                 && ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE] \
                    == test_csd[EXT_CSD_HC_ERASE_GRP_SIZE]
                 && memcmp(&ext_csd[EXT_CSD_SEC_CNT], \
                    &test_csd[EXT_CSD_SEC_CNT], 4) == 0) {

                mmc->card_caps |= width;
                break;
            }
        }

        if (mmc->card_caps & MMC_MODE_HS) {
            if (mmc->card_caps & MMC_MODE_HS_52MHz)
                mmc->tran_speed = 52000000;
            else
                mmc->tran_speed = 26000000;
        }
    }

    mmc_set_clock(mmc, mmc->tran_speed);

    /* fill in device description */
    mmc->block_dev.lun = 0;
    mmc->block_dev.type = 0;
    mmc->block_dev.blksz = mmc->read_bl_len;
    mmc->block_dev.lba = lldiv(mmc->capacity, mmc->read_bl_len);
    snprintf(mmc->block_dev.vendor, sizeof(mmc->block_dev.vendor),
            "Man %x Snr %x", mmc->cid[0] >> 8,
            (mmc->cid[2] << 8) | (mmc->cid[3] >> 24));
    snprintf(mmc->block_dev.product, sizeof(mmc->block_dev.product), 
            "%c%c%c%c%c", mmc->cid[0] & 0xff,
            (mmc->cid[1] >> 24), (mmc->cid[1] >> 16) & 0xff,
            (mmc->cid[1] >> 8) & 0xff, mmc->cid[1] & 0xff);
    snprintf(mmc->block_dev.revision, sizeof(mmc->block_dev.revision),
            "%d.%d", mmc->cid[2] >> 28,
            (mmc->cid[2] >> 24) & 0xf);
#if 0
#if !defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_LIBDISK_SUPPORT)
    init_part(&mmc->block_dev);
#endif
#endif
    return 0;
}

int driver_mmc_init(struct mmc *mmc)
{
    int err;

    if (mmc_getcd(mmc) == 0) {
        mmc->has_init = 0;
        PRINT_ERR("MMC: no card present\n");
        return NO_CARD_ERR;
    }

    if (mmc->has_init)
        return 0;

    err = mmc->init(mmc);

    if (err)
        return err;

    mmc_set_bus_width(mmc, 1);
    mmc_set_clock(mmc, 1);

    /* Reset the Card */
    err = mmc_go_idle(mmc);

    if (err)
        return err;

    /* The internal partition reset to user partition(0) at every CMD0*/
    mmc->part_num = 0;

    /* Test for SD version 2 */
    err = mmc_send_if_cond(mmc);

    /* Now try to get the SD card's operating condition */
    err = sd_send_op_cond(mmc);

    /* If the command timed out, we check for an MMC card */
    if (err == TIMEOUT) {
        err = mmc_send_op_cond(mmc);

        if (err) {
            PRINT_ERR("Card did not respond to voltage select!\n");
            return UNUSABLE_ERR;
        }
    }

    err = mmc_startup(mmc);
    if (err)
        mmc->has_init = 0;
    else
        mmc->has_init = 1;
    return err;
}
#endif

void print_mmc_devices(char separator)
{
    struct mmc *m;
    struct list_head *entry;

    list_for_each(entry, &mmc_devices) {
        m = list_entry(entry, struct mmc, link);

        PRINT_ERR("%s: %d\n", m->name, m->block_dev.dev);

        if (entry->next != &mmc_devices)
            PRINT_ERR("%c ", separator);
    }

    PRINT_ERR("\n");
}

s32 mmc_init()
{
    u8 buf[512];
    struct mmc *mmc;
    INIT_LIST_HEAD (&mmc_devices);
    curr_dev_num = 0;

    board_mmc_init();
    mmc = find_mmc_device(0);
    driver_mmc_init(find_mmc_device(0));
    PRINT_ERR("lba: %x\n", mmc->block_dev.lba);
    print_mmc_devices(',');

#define DEFAULT_MMC_DEVNUM (0)
    mmc = find_mmc_device(DEFAULT_MMC_DEVNUM);
    mmc->block_dev.block_read(DEFAULT_MMC_DEVNUM, 0, 1, buf);
    dump_mem(buf, sizeof(buf) / 4);
    return 0;
}
