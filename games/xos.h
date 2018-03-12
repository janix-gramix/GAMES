#if !defined(_XOS_H)
#define _XOS_H

#if defined(__APPLE__)
#   define linux
#   define fopen64  fopen
#endif

#if defined(linux) 
#	include <stdint.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long long uint64_t;
#endif
typedef float   float32_t ;
typedef double  float64_t ;

#if defined(linux)
#   include <unistd.h>
/* 2014-06 Inutile #define FOPEN64 fopen64 */
#   define  FOPEN64 fopen
#else
#   define FOPEN64 fopen

#   ifdef __cplusplus
        extern "C" {
#   endif

        extern int opterr;
        extern int optind;
        extern int optopt;
        extern char *optarg;

        int getopt(int argc, char **argv, char *opts);

#   ifdef __cplusplus
    }
#   endif
#endif

#if defined(__GNUC__)
#   define PACKED__  __attribute__((__packed__))
#   define __PACKED
#elif defined(_MSC_VER) 
#       define PACKED__
#       define __PACKED  __declspec(align(1))
#else
#error  "UNKNOWN COMPILER for PACKED definitions"
#endif

// pour prendre en compte le Big endian 
// definit X_BIG_ENDIAN
#ifdef __BYTE_ORDER
#  if __BYTE_ORDER == __BIG_ENDIAN
#       define  X_BIG_ENDIAN
#  endif
#endif /* __BYTE_ORDER */ 

// pour prendre en compte les contraintes d'alignement
#ifdef sparc
#   define  X_ALIGN_CONSTRAINT  8
#endif
#endif