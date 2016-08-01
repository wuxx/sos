SOS
===

Simple OS for raspberry pi model B <br>
1. SOS use u-boot as bootloader (https://github.com/wuxx/u-boot-pi) <br>
U-Boot> loady 0x200000 (because sos is link to 0x200000, please checkout the sos.ld) <br>
U-Boot> go 0x200000 <br>

2. use u-boot boot normal linux kernel <br>
(U-Boot> fatls mmc 0:1) <br>
(U-Boot> fatload mmc 0:1 0x8000 kernel.img.linux) <br>
(U-Boot> bootz 0x8000) <br>


3. more information about rpi <br>
https://www.raspberrypi.org/documentation/hardware/raspberrypi/schematics/ <br>

4. arm inline asm <br>
http://www.ethernut.de/en/documents/arm-inline-asm.html<br>

5. from linux kernel document<br>
The interrupt sources are as follows:<br>
Bank 0: <br>
0: ARM_TIMER <br>
1: ARM_MAILBOX  <br>
2: ARM_DOORBELL_0 <br>
3: ARM_DOORBELL_1 <br>
4: VPU0_HALTED <br>
5: VPU1_HALTED <br>
6: ILLEGAL_TYPE0 <br>
7: ILLEGAL_TYPE1 <br>

Bank 1: <br>
0: TIMER0 <br>
1: TIMER1 <br>
2: TIMER2 <br>
3: TIMER3 <br>
4: CODEC0 <br>
5: CODEC1 <br>
6: CODEC2 <br>
7: VC_JPEG <br>
8: ISP  <br>
9: VC_USB <br>
10: VC_3D <br>
11: TRANSPOSER <br>
12: MULTICORESYNC0 <br>
13: MULTICORESYNC1 <br>
14: MULTICORESYNC2 <br>
15: MULTICORESYNC3 <br>
16: DMA0 <br>
17: DMA1 <br>
18: VC_DMA2 <br>
19: VC_DMA3 <br>
20: DMA4 <br>
21: DMA5 <br>
22: DMA6 <br>
23: DMA7 <br>
24: DMA8 <br>
25: DMA9 <br>
26: DMA10 <br>
27: DMA11 <br>
28: DMA12 <br>
29: AUX <br>
30: ARM <br>
31: VPUDMA <br>

Bank 2: <br>
0: HOSTPORT <br>
1: VIDEOSCALER <br>
2: CCP2TX <br>
3: SDC <br>
4: DSI0 <br>
5: AVE <br>
6: CAM0 <br>
7: CAM1 <br>
8: HDMI0 <br>
9: HDMI1 <br>
10: PIXELVALVE1 <br>
11: I2CSPISLV <br>
12: DSI1 <br>
13: PWA0 <br>
14: PWA1 <br>
15: CPR <br>
16: SMI <br>
17: GPIO0 <br>
18: GPIO1 <br>
19: GPIO2 <br>
20: GPIO3 <br>
21: VC_I2C <br>
22: VC_SPI <br>
23: VC_I2SPCM <br>
24: VC_SDIO <br>
25: VC_UART <br>
26: SLIMBUS <br>
27: VEC <br>
28: CPG <br>
29: RNG <br>
30: VC_ARASANSDIO <br>
31: AVSPMON <br>
