#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef COLOR
    #define BLACK "\033[30m"
    #define RED "\033[31m"
    #define GREEN "\033[32m"
    #define YELLOW "\033[33m"
    #define BLUE "\033[34m"
    #define MAGENTA "\033[35m"
    #define CYAN "\033[36m"
    #define WHITE "\033[37m"
    #define CLEAR "\033[0m"
#else
    #define BLACK 
    #define RED 
    #define GREEN 
    #define YELLOW 
    #define BLUE 
    #define MAGENTA 
    #define CYAN 
    #define WHITE 
    #define CLEAR
#endif

#if defined(DEBUG) && defined(INFO)
    #define info(fmt_str, ...) { fprintf(stderr, CYAN "[INFO | %s:%-4d] " CLEAR fmt_str "\n", __FILE__, __LINE__, ##__VA_ARGS__); }
#else
    #define info(...)
#endif

#if defined(DEBUG) && defined(WARN)
    #define warn(fmt_str, ...) { fprintf(stderr, YELLOW "[WARN | %s:%-4d] " CLEAR fmt_str "\n", __FILE__, __LINE__, ##__VA_ARGS__); }
#else
    #define warn(...)
#endif

#if defined(DEBUG) && defined(SUCCESS)
    #define success(fmt_str, ...) { fprintf(stderr, GREEN "[SUCCESS | %s:%-4d] " CLEAR fmt_str "\n", __FILE__, __LINE__, ##__VA_ARGS__); }
#else
    #define success(...)
#endif

#if defined(DEBUG) && defined(ERROR)
    #define error(fmt_str, ...) { fprintf(stderr, RED "[ERROR | %s:%-4d] " CLEAR fmt_str "\n", __FILE__, __LINE__, ##__VA_ARGS__); }
#else
    #define error(...)
#endif

#endif