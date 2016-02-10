#ifndef __INT_H__
#define __INT_H__
#include <types.h>
#include <mmio.h>

/* CPSR bit */
#define FIQ_DISABLE_BIT 6
#define IRQ_DISABLE_BIT 7

enum irq_e {
    /* arm basic irq */
    IRQ_CORE_TIMER     = 0,
    IRQ_CORE_MAILBOX   = 1,
    IRQ_CORE_DOORBELL0 = 2,
    IRQ_CORE_DOORBELL1 = 3,
    IRQ_GPU0_HALT      = 4,
    IRQ_GPU1_HALT      = 5,
    IRQ_ILEGAL_ACCESS0 = 6,
    IRQ_ILEGAL_ACCESS1 = 7,

    /* arm peripherals irq */
    IRQ_AUX         = 32 + 29,
    IRQ_I2C_SPI_SLV = 32 + 43,
    IRQ_SMI         = 32 + 48,
    IRQ_GPIO0       = 32 + 49,
    IRQ_GPIO1       = 32 + 50,
    IRQ_GPIO2       = 32 + 51,
    IRQ_GPIO3       = 32 + 52,
    IRQ_I2C         = 32 + 53,
    IRQ_SPI         = 32 + 54,
    IRQ_PCM         = 32 + 55,
    IRQ_UART        = 32 + 57,
    IRQ_MAX         = 32 * 3,
};

s32 request_irq(u32 irq_nr, func_1 irq_handler);
s32 release_irq(u32 irq_nr);
s32 enable_irq(u32 irq_nr);
s32 disable_irq(u32 irq_nr);
s32 lockup();
s32 int_init();

#endif /* __INT_H__ */
