#        $(DRIVER_DIR)/usb/eth/smsc95xx.c        \
        $(DRIVER_DIR)/usb/eth/usb_ether.c       \
        $(DRIVER_DIR)/usb/host/dwc_otg.c        \
        $(DRIVER_DIR)/usb/host/dwc_otg-hcd.c

DRIVER_SRCS = \
        $(DRIVER_DIR)/gpio/gpio.c               \
        $(DRIVER_DIR)/timer/timer.c             \
        $(DRIVER_DIR)/i2c/i2c.c                 \
        $(DRIVER_DIR)/watchdog/watchdog.c       \
        $(DRIVER_DIR)/mailbox/mailbox.c         \
        $(DRIVER_DIR)/mmc/mmc.c                 \
        $(DRIVER_DIR)/mmc/sdhci.c               \
        $(DRIVER_DIR)/pwm/pwm.c                 \
        $(DRIVER_DIR)/log/log.c                 \
        $(DRIVER_DIR)/shell/shell.c             \
        $(DRIVER_DIR)/uart/uart.c
