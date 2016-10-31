#include <libc.h>
#include "mailbox.h"
#include "sdhci.h"
#include "mmc.h"
#include "log.h"

/* Set block count limit because of 16 bit register limit on some hardware*/
#ifndef CONFIG_SYS_MMC_MAX_BLK_COUNT
#define CONFIG_SYS_MMC_MAX_BLK_COUNT 65535
#endif

static struct list_head mmc_devices;
static int cur_dev_num = -1;

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

static int mmc_read_blocks(struct mmc *mmc, void *dst, ulong start,
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
                PRINT_ERR("Status Error: 0x%08X\n",
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

static ulong mmc_erase_t(struct mmc *mmc, ulong start, unsigned long blkcnt)
{
    struct mmc_cmd cmd;
    ulong end;
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

static ulong
mmc_write_blocks(struct mmc *mmc, ulong start, unsigned long blkcnt, const void*src)
{
    struct mmc_cmd cmd;
    struct mmc_data data;
    int timeout = 1000;

    if ((start + blkcnt) > mmc->block_dev.lba) {
        PRINT_ERR("MMC: block number 0x%lx exceeds max(0x%lx)\n",
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


static ulong
mmc_bwrite(int dev_num, ulong start, unsigned long blkcnt, const void*src)
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

static unsigned long
mmc_berase(int dev_num, unsigned long start, unsigned long blkcnt)
{
    int err = 0;
    struct mmc *mmc = find_mmc_device(dev_num);
    unsigned long blk = 0, blk_r = 0;
    int timeout = 1000;

    if (!mmc)
        return -1;

    if ((start % mmc->erase_grp_size) || (blkcnt % mmc->erase_grp_size))
        PRINT_ERR("\n\nCaution! Your devices Erase group is 0x%x\n"
                "The erase range would be change to 0x%lx~0x%lx\n\n",
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

static ulong mmc_bread(int dev_num, ulong start, unsigned long blkcnt, void *dst)
{
    unsigned long cur, blocks_todo = blkcnt;

    if (blkcnt == 0)
        return 0;

    struct mmc *mmc = find_mmc_device(dev_num);
    if (!mmc)
        return 0;

    if ((start + blkcnt) > mmc->block_dev.lba) {
        PRINT_ERR("MMC: block number 0x%lx exceeds max(0x%lx)\n",
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

int mmc_register(struct mmc *mmc)
{
    /* Setup the universal parts of the block interface just once */
    mmc->block_dev.if_type = IF_TYPE_MMC;
    mmc->block_dev.dev = cur_dev_num++;
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

int mmc_init(void)
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
