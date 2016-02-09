#ifndef __ASSERT_H__
#define __ASSERT_H__
#include <int.h>
#define assert(exp) ((exp) || __assert(__FILE__, __func__, __LINE__, "assert(" #exp ")failed!\n"))
#endif /* __ASSERT_H__ */
