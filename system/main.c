#include "vsprintf.h"

#define NULL ((void*) 0)
typedef unsigned int u32;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef int int32_t;

typedef void (*start_func)(uint32_t para);
typedef void (*task_func)();
extern void __switch_to(start_func);
#define GPIO_ADDRESS 0x20200000

// write to MMIO register
static inline void mmio_write(uint32_t reg, uint32_t data) {
    uint32_t *ptr = (uint32_t*)reg;
    asm volatile("str %[data], [%[reg]]"
            : : [reg]"r"(ptr), [data]"r"(data));
}

// read from MMIO register
static inline uint32_t mmio_read(uint32_t reg) {
    uint32_t *ptr = (uint32_t*)reg;
    uint32_t data;
    asm volatile("ldr %[data], [%[reg]]"
            : [data]"=r"(data) : [reg]"r"(ptr));
    return data;
}

u32 gpio_address = GPIO_ADDRESS;

void set_gpio_function(u32 gpio, u32 function)
{
    u32 offset = 0;
    u32 offset0 = 0;
    u32 offset1 = 0;
    u32 mask = 0;
    u32 tmp = 0;
    if (gpio > 53 || function > 7) {
        return;
    }
    
    offset0 = gpio / 10;
    offset1 = gpio % 10;
    offset1 = offset1 * 3;

    tmp = *((u32*)gpio_address + offset0);
    mask = 0x7 << offset1;
    mask = ~mask;
    tmp &= mask;
    tmp |= (function << offset1);
    *((u32*)gpio_address + offset0) = tmp;
}


void set_gpio_value(u32 gpio, u32 val)
{
    u32 offset0 = 0;
    u32 offset1 = 0;
    u32 base = 0;
    u32 tmp = 0;
    u32 mask = 0;
    if (gpio > 53) {
        return;
    }
    
    switch(val) {
        case (0):
            base = 10;  /* offset 40  output 0 */
            break;

        case (1):
            base = 7;   /* offset 28 output 1 */
            break;

        default:
            base = 7;
            val = 1;
            break;
    }

    offset0 = gpio / 32;
    offset1 = gpio % 32;

    tmp = *((u32 *)gpio_address + base + offset0);

    tmp |= (1 << offset1);
    *((u32 *)gpio_address + base + offset0) = tmp;

}


void delay_gpio()
{
    uint32_t count = 400000;
    while(count--);
}


/* uart begin */
enum {
    // The GPIO registers base address.
    GPIO_BASE = 0x20200000,
 
    // The offsets for reach register.
 
    // Controls actuation of pull up/down to ALL GPIO pins.
    GPPUD = (GPIO_BASE + 0x94),
 
    // Controls actuation of pull up/down for specific GPIO pin.
    GPPUDCLK0 = (GPIO_BASE + 0x98),
 
    // The base address for UART.
    UART0_BASE = 0x20201000,
 
    // The offsets for reach register for the UART.
    UART0_DR     = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR     = (UART0_BASE + 0x18),
    UART0_ILPR   = (UART0_BASE + 0x20),
    UART0_IBRD   = (UART0_BASE + 0x24),
    UART0_FBRD   = (UART0_BASE + 0x28),
    UART0_LCRH   = (UART0_BASE + 0x2C),
    UART0_CR     = (UART0_BASE + 0x30),
    UART0_IFLS   = (UART0_BASE + 0x34),
    UART0_IMSC   = (UART0_BASE + 0x38),
    UART0_RIS    = (UART0_BASE + 0x3C),
    UART0_MIS    = (UART0_BASE + 0x40),
    UART0_ICR    = (UART0_BASE + 0x44),
    UART0_DMACR  = (UART0_BASE + 0x48),
    UART0_ITCR   = (UART0_BASE + 0x80),
    UART0_ITIP   = (UART0_BASE + 0x84),
    UART0_ITOP   = (UART0_BASE + 0x88),
    UART0_TDR    = (UART0_BASE + 0x8C),
};
 
/*
 * delay function
 * int32_t delay: number of cycles to delay
 *
 * This just loops <delay> times in a way that the compiler
 * wont optimize away.
*/

static void delay(int32_t count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
         : : [count]"r"(count) : "cc");
}
 
/*
 * Initialize UART0.
 */
void uart_init() {
    // Disable UART0.
    mmio_write(UART0_CR, 0x00000000);
    // Setup the GPIO pin 14 && 15.
 
    // Disable pull up/down for all GPIO pins & delay for 150 cycles.
    mmio_write(GPPUD, 0x00000000);
    delay(150);
 
    // Disable pull up/down for pin 14,15 & delay for 150 cycles.
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);
 
    // Write 0 to GPPUDCLK0 to make it take effect.
    mmio_write(GPPUDCLK0, 0x00000000);
 
    // Clear pending interrupts.
    mmio_write(UART0_ICR, 0x7FF);
 
    // Set integer & fractional part of baud rate.
    // Divider = UART_CLOCK/(16 * Baud)
    // Fraction part register = (Fractional part * 64) + 0.5
    // UART_CLOCK = 3000000; Baud = 115200.
 
    // Divider = 3000000/(16 * 115200) = 1.627 = ~1.
    // Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
    mmio_write(UART0_IBRD, 1);
    mmio_write(UART0_FBRD, 40);
 
    // Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
 
    // Mask all interrupts.
    mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) |
            (1 << 6) | (1 << 7) | (1 << 8) |
            (1 << 9) | (1 << 10));
 
    // Enable UART0, receive & transfer part of UART.
    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}
 
/*
 * Transmit a byte via UART0.
 * uint8_t Byte: byte to send.
 */
void uart_putc(uint8_t byte) {
    // wait for UART to become ready to transmit
    while (1) {
        if (!(mmio_read(UART0_FR) & (1 << 5))) {
        break;
    }
    }
    mmio_write(UART0_DR, byte);
}
 
/*
 * print a string to the UART one character at a time
 * const char *str: 0-terminated string
 */
void uart_puts(const char *str) {
    while (*str) {
        if (*str == '\n') {
            uart_putc('\r');
            uart_putc('\n');
            str++;
        }
        else {
            uart_putc(*str++);
        }
    }
}

char buf[1024] = {0};
void uart_printf(const char* fmt,...)
{
    va_list args;
    va_start(args,fmt);
    vsnprintf(buf,sizeof(buf), fmt, args);
    va_end(args);
    uart_puts(buf);
}
/* uart end*/

/* task*/
#define TASK_UNUSED -1
#define TASK_RUNNING 0
#define TASK_SUSPEND 1
#define TASK_READY   2

struct task *g_running_task;
struct task
{
    uint32_t sp;
    //uint32_t context[15]; 
    uint32_t task_id;
    uint32_t *stack;
    uint32_t stack_size;
    uint32_t state;
    start_func func;
};

uint32_t g_stack[5*1024];
struct task g_task[5];
uint32_t init_task()
{
    int i;
    for(i=0;i<5;i++) {
        g_task[i].task_id = -1;
    }

    uart_printf("in init_task, g_task: 0x%x; g_stack: 0x%x\n", g_task, g_stack);
}

/* 得到一个空闲的TCB, 偏移 == prio == task_id */
uint32_t get_free_task(uint32_t prio)
{
    uart_printf("prio: %d\n", prio);
    if (g_task[prio].task_id == -1)
    {
        uart_printf("in get_free_task: %d\n", prio);
        return prio;
    }
    return -1;
}

void dump_task(uint32_t tid)
{
    struct task *tsk = &g_task[tid];
    int i;
    uart_printf("task_id: %d\n", tid);
    uart_printf("task_func: 0x%x\n", tsk->func);
    uart_printf("stack: 0x%x\n", tsk->stack);
    uart_printf("stack_size: %d\n", tsk->stack_size);
    uart_printf("sp: 0x%x\n", tsk->sp);
    for(i=0;i<15;i++) {
        uart_printf("sp[%d]: 0x%x\n", i, ((int *)(tsk->sp))[i]);
    }
    uart_printf("cpsr: 0x%x\n", ((int *)(tsk->sp))[15]);
}

void launch(uint32_t para)
{
    void (*func)(void);
    func = (void (*)(void))para;
    uart_printf("launch start,  func: 0x%x \n", func);
    func();
    uart_printf("launch end\n");
}

uint32_t init_context(struct task *tsk, uint32_t task_id, task_func func)
{
    uint32_t i = 0;
    uint32_t *context = NULL;
    tsk->task_id = task_id;
    tsk->stack = &g_stack[task_id*1024];
    tsk->stack_size = 1024; /* 1024 * 4bytes */
    tsk->func = launch;
    context = tsk->stack + tsk->stack_size - 16;
    tsk->sp = (int)context;   /* R0-R12 SP, LR, CPSR */
    context[15] = 0x1d3; /* CPSR <ARM1176JZF-S TRM P97> */
    context[14] = (uint32_t)launch; /* LR */
    context[13] = ((uint32_t)tsk->stack + tsk->stack_size); /* SP */
    for(i=0;i<13;i++) { /* R0 - R12 */
        context[i] = 0;
    }

    context[0] = (int)func;

    tsk->state = TASK_READY;

    return task_id;
}



uint32_t task_create(uint32_t task_prio, start_func func)
{
    uint32_t tid = get_free_task(task_prio);
    if (tid == -1) {
        return -1;
    }
    uart_printf("task_create 0\n");
    tid = init_context(&g_task[tid], tid, func);
    dump_task(tid);
    uart_printf("task_create 1\n");
    return tid;
}

void start_first_task(uint32_t tid)
{
    struct task *tsk = &g_task[tid];
    uart_printf("in start_first_task, tsk: 0x%x\n", tsk);
    g_running_task = tsk;
    __start_first_task(tsk);
}


/* switch to new task */
void task_switch(uint32_t tid)
{
    struct task *new = &g_task[tid];
    struct task *tmp = g_running_task;
    g_running_task->state = TASK_READY;
    new->state = TASK_RUNNING;
//    uart_printf("task_switch 0\n");
    g_running_task = new;
    __switch_context(tmp, new);
//    uart_printf("task_switch 1\n");
}

/* tid 1 */
void testA();
void testB()
{
    while(1) {
        uart_puts("in testB\n");           
        delay_gpio();
        task_switch(0); /* switch to testA */
        //__switch_context(g_running_task, &g_task[0]);
    }
}

/* tid 0 */
void testA()
{
    while(1) {
        uart_puts("in testA\n");           
        delay_gpio();
        task_switch(1); /* switch to testA */
    }
}


void enable_int()
{
    __asm volatile (
            "cpsie i"
            );
}

void disable_int()
{
    __asm volatile (
            "cpsid i"
            );
}


void panic(const char *str)
{
    uart_printf("%s\n", str);
    while(1);
}

void undefined_execption()
{
    panic("not inplemented!");
}

typedef (*exception_entry)(void);
/* 16 (internal) + 32 (external) */
exception_entry ee[48] = {
    [0 ... 47] = (exception_entry)undefined_execption,
};
const uint32_t vector_table_base = 0x0;
void init_vector_table()
{
    int i;
    uint32_t *base = (uint32_t *)vector_table_base;
    for(i=0;i<48;i++) {
        base[i] = (uint32_t)ee[i];
        uart_printf("[%d]: 0x%x\n", i, base[i]);
    }
    return;
}

int main()
{
    uint32_t cpsr;
    uint32_t sp;
    uint32_t pc;
    uint32_t tid;
    uart_init();
    uart_printf("system start\n");
    init_task();
    set_gpio_function(16, 1);

    //tid = task_create(0, testA);
    //uart_printf("get tid %d\n", tid);
    //dump_task(0);

    //tid = task_create(1, testB);
    //uart_printf("get tid %d\n", tid);
    //dump_task(1);

    //start_first_task(0);

    init_vector_table();
    enable_int();
    uart_printf("call __die...");
    __die();
    uart_printf("after call __die...");
    while(1) {
        set_gpio_value(16, 0);  /* LED ON */
        delay_gpio();
        set_gpio_value(16, 1);
        delay_gpio();
        uart_puts("in main\n");
        cpsr = __get_cpsr();
        sp = __get_sp();
        pc = __get_pc();
        uart_printf("cpsr: 0x%x; sp: 0x%x; pc: 0x%x\n", cpsr, sp, pc);
        
    }

    return 0;
}
