#include <libc.h>
#include <memory_map.h>
#include "mmio.h"
#include "log.h"
#include "gpio.h"
#include "i2c.h"

PUBLIC s32 i2c_init()
{
    return 0;
}

#if 0
#define wakeup_isr(i2cp, msg) {                                             \
  chSysLockFromIsr();                                                       \
  if ((i2cp)->thread != NULL) {                                             \
    Thread *tp = (i2cp)->thread;                                            \
    (i2cp)->thread = NULL;                                                  \
    tp->p_u.rdymsg = (msg);                                                 \
    chSchReadyI(tp);                                                        \
  }                                                                         \
  chSysUnlockFromIsr();                                                     \
}
#else
#define wakeup_isr(i2cp, msg) 
#endif

/**
 * @brief   Handling of stalled I2C transactions.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */
static void i2c_lld_safety_timeout(void *p) 
{
#if 0
  I2CDriver *i2cp = (I2CDriver *)p;
  chSysLockFromIsr();
  if (i2cp->thread) {
    bscdevice_t *device = i2cp->device;

    i2cp->errors |= I2CD_TIMEOUT;
    if (device->status & BSC_CLKT)
      i2cp->errors |= I2CD_BUS_ERROR;
    if (device->status & BSC_ERR)
      i2cp->errors |= I2CD_ACK_FAILURE;

    device->control = 0;
    device->status = BSC_CLKT | BSC_ERR | BSC_DONE;

    Thread *tp = i2cp->thread;
    i2cp->thread = NULL;
    tp->p_u.rdymsg = RDY_TIMEOUT;
    chSchReadyI(tp);
  }
  chSysUnlockFromIsr();
#endif
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

void i2c_lld_serve_interrupt(I2CDriver *i2cp) 
{
#if 0
  UNUSED(i2cp);
  bscdevice_t *device = i2cp->device;
  u32 status = device->status;

  if (status & (BSC_CLKT | BSC_ERR)) {
    // TODO set error flags
    wakeup_isr(i2cp, RDY_RESET);
  }
  else if (status & BSC_DONE) {
    while ((status & BSC_RXD) && (i2cp->rxidx < i2cp->rxbytes))
      i2cp->rxbuf[i2cp->rxidx++] = device->dataFifo;
    device->control = 0;
    device->status = BSC_CLKT | BSC_ERR | BSC_DONE;
    wakeup_isr(i2cp, RDY_OK);
  }
  else if (status & BSC_TXW) {
    while ((i2cp->txidx < i2cp->txbytes) && (status & BSC_TXD))
      device->dataFifo = i2cp->txbuf[i2cp->txidx++];
  }
  else if (status & BSC_RXR) {
    while ((i2cp->rxidx < i2cp->rxbytes) && (status & BSC_RXD))
      i2cp->rxbuf[i2cp->rxidx++] = device->dataFifo;
  }
#endif
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level I2C driver initialization.
 *
 * @notapi
 */
void i2c_lld_init(void) 
{
#if 0
  I2C0.device = BSC0_ADDR;
  i2cObjectInit(&I2C0);
#endif
}

/**
 * @brief   Configures and activates the I2C peripheral.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */
void i2c_lld_start(I2CDriver *i2cp) 
{
#if 0
  /* Set up GPIO pins for I2C */
  bcm2835_gpio_fnsel(GPIO0_PAD, GPFN_ALT0);
  bcm2835_gpio_fnsel(GPIO1_PAD, GPFN_ALT0);

  u32 speed = i2cp->config->ic_speed;
  if (speed != 0 && speed != 100000)
    i2cp->device->clockDivider = BSC_CLOCK_FREQ / i2cp->config->ic_speed;

  i2cp->device->control |= BSC_I2CEN;
#endif
}

/**
 * @brief   Deactivates the I2C peripheral.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 *
 * @notapi
 */
void i2c_lld_stop(I2CDriver *i2cp) 
{
#if 0
  /* Set GPIO pin function to default */
  bcm2835_gpio_fnsel(GPIO0_PAD, GPFN_IN);
  bcm2835_gpio_fnsel(GPIO1_PAD, GPFN_IN);

  i2cp->device->control &= ~BSC_I2CEN;
#endif
}

/**
 * @brief   Master transmission.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 * @param[in] addr      slave device address (7 bits) without R/W bit
 * @param[in] txbuf     transmit data buffer pointer
 * @param[in] txbytes   number of bytes to be transmitted
 * @param[out] rxbuf     receive data buffer pointer
 * @param[in] rxbytes   number of bytes to be received
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 *
 * @notapi
 */
s32 i2c_lld_master_transmit_timeout(I2CDriver *i2cp, u16 addr, 
                                       const u8 *txbuf, u32 txbytes, 
                                       u8 *rxbuf, const u8 rxbytes, 
                                       u32 timeout) 
{
#if 0
  VirtualTimer vt;

  /* Global timeout for the whole operation.*/
  if (timeout != TIME_INFINITE)
    chVTSetI(&vt, timeout, i2c_lld_safety_timeout, (void *)i2cp);

  i2cp->addr = addr;
  i2cp->txbuf = txbuf;
  i2cp->txbytes = txbytes;
  i2cp->txidx = 0;
  i2cp->rxbuf = rxbuf;
  i2cp->rxbytes = rxbytes;
  i2cp->rxidx = 0;

  bscdevice_t *device = i2cp->device;
  device->slaveAddress = addr;
  device->dataLength = txbytes;
  device->status = CLEAR_STATUS;

  /* Enable Interrupts and start transfer.*/
  device->control |= (BSC_INTT | BSC_INTD | START_WRITE);

  /* Is this really needed? there is an outer lock already */
  chSysLock();

  i2cp->thread = chThdSelf();
  chSchGoSleepS(THD_STATE_SUSPENDED);
  if ((timeout != TIME_INFINITE) && chVTIsArmedI(&vt))
    chVTResetI(&vt);

  chSysUnlock();

  s32 status = chThdSelf()->p_u.rdymsg;

  if (status == RDY_OK && rxbytes > 0) {
    /* The TIMEOUT_INFINITE prevents receive from setting up it's own timer.*/
    status = i2c_lld_master_receive_timeout(i2cp, addr, rxbuf, 
					    rxbytes, TIME_INFINITE);
    if ((timeout != TIME_INFINITE) && chVTIsArmedI(&vt))
      chVTResetI(&vt);
  }

  return status;
#endif
}


/**
 * @brief   Master receive.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 * @param[in] addr      slave device address (7 bits) without R/W bit
 * @param[out] rxbuf     receive data buffer pointer
 * @param[in] rxbytes   number of bytes to be received
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 *
 * @notapi
 */
s32 i2c_lld_master_receive_timeout(I2CDriver *i2cp, u16 addr, 
				     u8 *rxbuf, u32 rxbytes, 
				     u32 timeout) 
{
#if 0
  VirtualTimer vt;

  /* Global timeout for the whole operation.*/
  if (timeout != TIME_INFINITE)
    chVTSetI(&vt, timeout, i2c_lld_safety_timeout, (void *)i2cp);

  i2cp->addr = addr;
  i2cp->txbuf = NULL;
  i2cp->txbytes = 0;
  i2cp->txidx = 0;
  i2cp->rxbuf = rxbuf;
  i2cp->rxbytes = rxbytes;
  i2cp->rxidx = 0;

  /* Setup device.*/
  bscdevice_t *device = i2cp->device;
  device->slaveAddress = addr;
  device->dataLength = rxbytes;
  device->status = CLEAR_STATUS;

  /* Enable Interrupts and start transfer.*/
  device->control = (BSC_INTR | BSC_INTD | START_READ);

  // needed? there is an outer lock already
  chSysLock();
  i2cp->thread = chThdSelf();
  chSchGoSleepS(THD_STATE_SUSPENDED);
  if ((timeout != TIME_INFINITE) && chVTIsArmedI(&vt))
    chVTResetI(&vt);
  chSysUnlock();

  return chThdSelf()->p_u.rdymsg;
#endif
  return 0;
}
