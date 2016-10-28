#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

enum wdt_cmd {
    WDT_START = 0,
    WDT_STOP,
    WDT_FEED,
    WDT_SET_TIMEOUT,
    WDT_CMD_MAX,
};

#define WDT_PASSWORD    (0x5a000000)

#define WDT_TICK2S(tick)   ((tick) >> 16)
#define WDT_S2TICK(us)     ((us)   << 16)

u32 watchdog_init();
s32 watchdog_ctrl(u32 cmd, ...);
#endif /* __WATCHDOG_H__ */
