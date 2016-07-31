SOS
===

Simple OS for raspberry pi model B
1. SOS use u-boot as bootloader (https://github.com/wuxx/u-boot-pi)
U-Boot> loady 0x200000 (because sos is link to 0x200000, please checkout the sos.ld)
U-Boot> go 0x200000

2. use u-boot boot normal linux kernel
(U-Boot> fatls mmc 0:1)
(U-Boot> fatload mmc 0:1 0x8000 kernel.img.linux)
(U-Boot> bootz 0x8000)


2. more information about rpi
https://www.raspberrypi.org/documentation/hardware/raspberrypi/schematics/

3. arm inline asm 
http://www.ethernut.de/en/documents/arm-inline-asm.html

4. from linux kernel document
The interrupt sources are as follows:

Bank 0:
0: ARM_TIMER
1: ARM_MAILBOX
2: ARM_DOORBELL_0
3: ARM_DOORBELL_1
4: VPU0_HALTED
5: VPU1_HALTED
6: ILLEGAL_TYPE0
7: ILLEGAL_TYPE1

Bank 1:
0: TIMER0
1: TIMER1
2: TIMER2
3: TIMER3
4: CODEC0
5: CODEC1
6: CODEC2
7: VC_JPEG
8: ISP 
9: VC_USB
10: VC_3D
11: TRANSPOSER
12: MULTICORESYNC0
13: MULTICORESYNC1
14: MULTICORESYNC2
15: MULTICORESYNC3
16: DMA0
17: DMA1
18: VC_DMA2
19: VC_DMA3
20: DMA4
21: DMA5
22: DMA6
23: DMA7
24: DMA8
25: DMA9
26: DMA10
27: DMA11
28: DMA12
29: AUX
30: ARM
31: VPUDMA

Bank 2:
0: HOSTPORT
1: VIDEOSCALER
2: CCP2TX
3: SDC
4: DSI0
5: AVE
6: CAM0
7: CAM1
8: HDMI0
9: HDMI1
10: PIXELVALVE1
11: I2CSPISLV
12: DSI1
13: PWA0
14: PWA1
15: CPR
16: SMI
17: GPIO0
18: GPIO1
19: GPIO2
20: GPIO3
21: VC_I2C
22: VC_SPI
23: VC_I2SPCM
24: VC_SDIO
25: VC_UART
26: SLIMBUS
27: VEC
28: CPG
29: RNG
30: VC_ARASANSDIO
31: AVSPMON

