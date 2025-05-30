/**
 * @file trace.h
 *
 * @brief
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-03-24
 * @copyright Copyright (c) 2025
 */
#ifndef _TRACE_H_
#define _TRACE_H_

#ifdef USE_TRACE
#include <stdio.h>
#include <stdlib.h>

// defined in trace.c
extern int trace_depth;

#define TRACE(...)                                      \
    do {                                                \
        fprintf(stdout, "%*sTRACE: ", trace_depth, ""); \
        fprintf(stdout, __VA_ARGS__);                   \
        fprintf(stdout, "\n");                          \
    } while(0)

#define ENTER                                                                         \
    do {                                                                              \
        fprintf(stdout, "%*sENTER: %s: %s()\n", trace_depth, "", __FILE__, __func__); \
        trace_depth += 2;                                                             \
    } while(0)

#define RETURN(...)                                                                       \
    do {                                                                                  \
        trace_depth -= 2;                                                                 \
        fprintf(stdout, "%*sRETURN(%s) %s()\n", trace_depth, "", #__VA_ARGS__, __func__); \
        return __VA_ARGS__;                                                               \
    } while(0)

#define SEPARATOR printf("------------------------\n")

#else

#define TRACE(...)
#define ENTER
#define RETURN(...)         \
    do {                    \
        return __VA_ARGS__; \
    } while(0)
#define SEPARATOR

#endif

#endif /* _TRACE_H_ */
