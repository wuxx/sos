#include <libc.h>
#include "gpio.h"
#include "timer.h"
#include "pwm.h"

PUBLIC void pwm_init(void)
{
    return;
}

PUBLIC void pwm_start(u32 period)
{
  /* Set PWM pin function.*/
  gpio_set_function(18, ALT_FUNC_5);

  /* Stop PWM.*/
  writel(PWM_CTL, 0);

  /* Disable clock generator (reset bit 4).*/
  writel(GPIO0_CLK_CTL, GPIO_CLK_PWD | 0x01);
  mdelay(110);

  /* Wait for clock to be !BUSY.*/
  while ((readl(GPIO0_CLK_CTL) & 0x80) != 0);

  /* set pwm div to 32 (19.2/32 = 600KHz).*/
  writel(GPIO0_CLK_DIV, GPIO_CLK_PWD | (32 << 12));

  /* enable clock generator.*/
  writel(GPIO0_CLK_CTL, GPIO_CLK_PWD | 0x11);

  /* N/M -- N = DATA, M = RANGE.*/
  /* M/S -- M = DATA, S = RANGE.*/
  writel(PWM0_DATA, 0);
  writel(PWM0_RANGE, period);

}

PUBLIC void pwm_stop()
{
  writel(PWM_CTL, ~PWM0_ENABLE);
}

PUBLIC void pwm_set_period(u32 period)
{
  writel(PWM0_RANGE, period);
}

PUBLIC void pwm_enable_channel(u8 channel, u32 width)
{
  writel(PWM_CTL, readl(PWM_CTL) | PWM0_ENABLE);
  writel(PWM0_DATA, width);
}

PUBLIC void pwm_disable_channel(u8 channel)
{
  writel(PWM_CTL, readl(PWM_CTL) & ~PWM0_ENABLE);
}
