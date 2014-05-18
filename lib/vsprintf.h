#ifndef VSNPRINTF_H
#define VSNPRINTF_H


typedef unsigned int size_t;
typedef char* va_list;

#define __va_size(type) \
   (((sizeof(type) + sizeof(long) - 1) / sizeof (long)) * sizeof (long))

#define va_start(ap, last) \
   ((ap) = ((char*)&last) + __va_size(last))

#define va_arg(ap, type) \
   (*(type*)((ap)+= __va_size(type), (ap) - __va_size(type)))

#define va_end(va_list) ((void)0)

# define do_div(n,base) ({                  \
            unsigned int __base = (base);               \
            unsigned int __rem;                     \
            __rem = ((unsigned int)(n)) % __base;           \
            (n) = ((unsigned int)(n)) / __base;             \
            __rem;                          \
         })

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */

int isdigit(char c);

static int skip_atoi(const char **s);


static char * number(char * buf, char * end, long long num, int base, int size, int precision, int type);

/**
* vsnprintf - Format a string and place it in a buffer
* @buf: The buffer to place the result into
* @size: The size of the buffer, including the trailing null space
* @fmt: The format string to use
* @args: Arguments for the format string
*
* Call this function if you are already dealing with a va_list.
* You probably want snprintf instead.
 */
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);

/**
 * snprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @...: Arguments for the format string
 */
int snprintf(char * buf, size_t size, const char *fmt, ...);

/**
 * vsprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @fmt: The format string to use
 * @args: Arguments for the format string
 *
 * Call this function if you are already dealing with a va_list.
 * You probably want sprintf instead.
 */
int vsprintf(char *buf, const char *fmt, va_list args);


/**
 * sprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @fmt: The format string to use
 * @...: Arguments for the format string
 */
int sprintf(char * buf, const char *fmt, ...);

#endif //VSNPRINTF_H
