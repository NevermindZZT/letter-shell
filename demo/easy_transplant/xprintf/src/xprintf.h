/*------------------------------------------------------------------------*/
/* Universal string handler for user console interface  (C)ChaN, 2021     */
/*------------------------------------------------------------------------*/

#ifndef XPRINTF_DEF
#define XPRINTF_DEF
#include <string.h>

// clang-format off

#ifdef __cplusplus
extern "C" {
#endif

#define XF_USE_OUTPUT	1	/* 1: Enable output functions */
#define	XF_CRLF			1	/* 1: Convert \n ==> \r\n in the output char */
#define	XF_USE_DUMP		1	/* 1: Enable put_dump function */
#define	XF_USE_LLI		1	/* 1: Enable long long integer in size prefix ll */
#define	XF_USE_FP		1	/* 1: Enable support for floating point in type e and f */
#define XF_DPC			'.'	/* Decimal separator for floating point */
#define XF_USE_INPUT	0	/* 1: Enable input functions */
#define	XF_INPUT_ECHO	0	/* 1: Echo back input chars in xgets function */

    // clang-format on

#if defined(__GNUC__) && __GNUC__ >= 10
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

#if XF_USE_OUTPUT
#define xdev_out(func) xfunc_output = (void (*)(int))(func)
    extern void (*xfunc_output)(int);
    void xputc(int chr);
    void xfputc(void (*func)(int), int chr);
    void xputs(const char *str);
    void xfputs(void (*func)(int), const char *str);
    void xprintf(const char *fmt, ...);
    void xsprintf(char *buff, const char *fmt, ...);
    void xfprintf(void (*func)(int), const char *fmt, ...);
    void put_dump(const void *buff, unsigned long addr, int len, size_t width);
#endif

#if XF_USE_INPUT
#define xdev_in(func) xfunc_input = (int (*)(void))(func)
    extern int (*xfunc_input)(void);
    int xgets(char *buff, int len);
    int xatoi(char **str, long *res);
    int xatof(char **str, double *res);
#endif

#ifdef __cplusplus
}
#endif

#endif
