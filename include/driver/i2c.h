#ifndef __I2C_H__
#define __I2C_H__

#define I2CD_NO_ERROR               0x00   /**< @brief No error.            */
#define I2CD_BUS_ERROR              0x01   /**< @brief Bus Error.           */
#define I2CD_ARBITRATION_LOST       0x02   /**< @brief Arbitration Lost
                                                                                             (master mode).              */
#define I2CD_ACK_FAILURE            0x04   /**< @brief Acknowledge Failure. */
#define I2CD_OVERRUN                0x08   /**< @brief Overrun/Underrun.    */
#define I2CD_PEC_ERROR              0x10   /**< @brief PEC Error in
                                                                                             reception.                  */
#define I2CD_TIMEOUT                0x20   /**< @brief Hardware timeout.    */
#define I2CD_SMB_ALERT              0x40   /**< @brief SMBus Alert.         */

typedef enum {
    I2C_UNINIT = 0,                           /**< Not initialized.           */
    I2C_STOP = 1,                             /**< Stopped.                   */
    I2C_READY = 2,                            /**< Ready.                     */
    I2C_ACTIVE_TX = 3,                        /**< Transmitting.              */
    I2C_ACTIVE_RX = 4,                        /**< Receiving.                 */
    I2C_LOCKED = 5                            /**> Bus or driver locked.      */
} i2cstate_t;

struct bscdevice_t {
    volatile unsigned int control;
    volatile unsigned int status; 
    volatile unsigned int dataLength;
    volatile unsigned int slaveAddress;
    volatile unsigned int dataFifo;
    volatile unsigned int clockDivider;
    volatile unsigned int dataDelay;
    volatile unsigned int clockStretchTimeout;
};

typedef struct bscdevice_t bscdevice_t;

/**
 * @brief   Type of a structure representing an I2C driver.
 */
typedef struct I2CDriver I2CDriver;

/**
 * @brief   I2C completion callback type.
 *
 * @param[in] i2cp      pointer to the @p I2CDriver object
 * @param[in] sts       operation status
 */
typedef void (*i2ccallback_t)(I2CDriver *i2cp, u32 sts);

/**
 * @brief   Driver configuration structure.
 * @note    Implementations may extend this structure to contain more,
 *          architecture dependent, fields.
 */
typedef struct {
  /** @brief I2C bus bit rate.*/
  u32                  ic_speed;
  /* End of the mandatory fields.*/
} I2CConfig;

/**
 * @brief   Structure representing an I2C driver.
 * @note    Implementations may extend this structure to contain more,
 *          architecture dependent, fields.
 */
struct I2CDriver {
  /** @brief Driver state.*/
  i2cstate_t                state;
  /** @brief Current configuration data.*/
  const I2CConfig           *config;
  /** @brief Error flags.*/
  u32                errors;
  /** @brief BSC device registers.*/
  bscdevice_t               *device;
#if I2C_USE_MUTUAL_EXCLUSION
#if CH_USE_MUTEXES
  Mutex                     mutex;
#endif /* CH_USE_MUTEXES */
#endif /* I2C_USE_MUTUAL_EXCLUSION */
  /* End of the mandatory fields.*/
  /**
   * @brief   Thread waiting for I/O completion.
   */
  /* Thread                    *thread; */
  /**
   * @brief   Address of slave device.
   */
  u16                 addr;
  /**
   * @brief   Pointer to the buffer with data to send.
   */
  const u8             *txbuf;
  /**
   * @brief   Number of bytes of data to send.
   */
  u32                    txbytes;
  /**
   * @brief   Current index in buffer when sending data.
   */
  u32                    txidx;
  /**
   * @brief   Pointer to the buffer to put received data.
   */
  u8                   *rxbuf;
  /**
   * @brief   Number of bytes of data to receive.
   */
  u32                    rxbytes;
  /**
   * @brief   Current index in buffer when receiving data.
   */
  u32                    rxidx;
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

#define i2c_lld_master_start(i2cp, header)

#define i2c_lld_master_stop(i2cp)

#define i2c_lld_master_restart(i2cp)

#define i2c_lld_get_errors(i2cp) ((i2cp)->errors)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

extern I2CDriver I2C0;

void i2c_lld_init(void);
void i2c_lld_start(I2CDriver *i2cp);
void i2c_lld_stop(I2CDriver *i2cp);

s32 i2c_lld_master_transmit_timeout(I2CDriver *i2cp, u16 addr, 
        const u8 *txbuf, u32 txbytes, 
        u8 *rxbuf, const u8 rxbytes, 
        u32 timeout);

s32 i2c_lld_master_receive_timeout(I2CDriver *i2cp, u16 addr, 
        u8 *rxbuf, u32 rxbytes, 
        u32 timeout);

void i2c_lld_serve_interrupt(I2CDriver *i2cp);

#endif /* __I2C_H__ */
