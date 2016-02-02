#ifndef __GPIO_H__
#define __GPIO_H__

enum {
	GPIO_0 = 0,
	GPIO_1,
	GPIO_2,
	GPIO_3,
	GPIO_4,
	GPIO_5,
	GPIO_6,
	GPIO_8,
	GPIO_9,
	GPIO_10,
	GPIO_11,
	GPIO_12,
	GPIO_13,
	GPIO_14,
	GPIO_15,
	GPIO_16,
	GPIO_18,
	GPIO_19,
	GPIO_20,
	GPIO_21,
	GPIO_22,
	GPIO_23,
	GPIO_24,
	GPIO_25,
	GPIO_26,
	GPIO_28,
	GPIO_29,
	GPIO_30,
	GPIO_31,
	GPIO_32,
	GPIO_33,
	GPIO_34,
	GPIO_35,
	GPIO_36,
	GPIO_38,
	GPIO_39,
	GPIO_40,
	GPIO_41,
	GPIO_42,
	GPIO_43,
	GPIO_44,
	GPIO_45,
	GPIO_46,
	GPIO_48,
	GPIO_49,
	GPIO_50,
	GPIO_51,
	GPIO_52,
	GPIO_53,
    GPIO_NR_MAX,
};

enum {
    INPUT = 0,
    OUTPUT,
    ALT_FUNC_4, /* yes, the arrange is strange */
    ALT_FUNC_5,
    ALT_FUNC_0,
    ALT_FUNC_1,
    ALT_FUNC_2,
    ALT_FUNC_3,
    ALT_FUNC_MAX,
};

struct _gpio_fsel_reg {
        u32 fsel0 : 3;
        u32 fsel1 : 3;
        u32 fsel2 : 3;
        u32 fsel3 : 3;
        u32 fsel4 : 3;
        u32 fsel5 : 3;
        u32 fsel6 : 3;
        u32 fsel7 : 3;
        u32 fsel8 : 3;
        u32 fsel9 : 3;
        u32 reserved : 2;
}__attribute__((__packed__));

union gpio_fsel_reg {
    u32 value;
    struct _gpio_fsel_reg reg;
};

#endif /* __GPIO_H__ */
