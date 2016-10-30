#ifndef __PWM_H__
#define __PWM_H__

#include <libc.h>

#define PWM_CTL        (0x2020C000)
#define PWM_STATUS     (0x2020C004)

#define PWM0_RANGE     (0x2020C010)
#define PWM0_DATA      (0x2020C014)

#define PWM1_RANGE     (0x2020C020)
#define PWM1_DATA      (0x2020C024)

#define PWM0_ENABLE    BIT(0)
#define PWM0_MODE_MS   BIT(7)

#define PWM1_ENABLE    BIT(8)
#define PWM1_MODE_MS   BIT(15)

#define PWM_MODE_MS    0xFF

#define GPIO_CLK_PWD   0x5a000000

#define GPIO0_CLK_CTL  (0x201010A0)
#define GPIO0_CLK_DIV  (0x201010A4)

void pwm_init(void);
void pwm_start(u32 period);
void pwm_stop();
void pwm_set_period(u32 period);
void pwm_enable_channel(u8 channel, u32 width);
void pwm_disable_channel(u8 channel);

#endif /* __PWM_H__ */
