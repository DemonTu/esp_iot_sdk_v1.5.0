/* /////////////////////////////////////////////////////////////////////////////////////////////////
//                     Copyright (c) Philips Semiconductors
//
//                       (C)PHILIPS Electronics N.V.2004
//         All rights are reserved. Reproduction in whole or in part is
//        prohibited without the written consent of the copyright owner.
//    Philips reserves the right to make changes without notice at any time.
//   Philips makes no warranty, expressed, implied or statutory, including but
//   not limited to any implied warranty of merchantability or fitness for any
//  particular purpose, or that the use will not infringe any third party patent,
//   copyright or trademark. Philips must not be liable for any loss or damage
//                            arising from its use.
///////////////////////////////////////////////////////////////////////////////////////////////// */

/*!
 * \if cond_generate_filedocu
 * \file phcsTypeDefs.h
 *
 * Project: Basic Function Library type definitions.
 *
 * Workfile: phcsTypeDefs.h
 * $Author: mha $
 * $Revision: 1.3 $
 * $Date: Wed Jul  5 12:56:59 2006 $
 *
 * Comment:
 *  None.
 *
 * History:
 *  MHa: Generated September 2005
 * \endif
 *
*/

#ifndef PHCSTYPEDEFS_H
#define PHCSTYPEDEFS_H
//#include <stdint.h>
#include "c_types.h"
#if 0
/* unsigned types */
#ifndef __uint8_t_defined
#define __uint8_t_defined
    typedef unsigned char       uint8_t;
    #ifndef __int32_t_defined
    #define __int32_t_defined
        typedef long                int32_t;
    #endif
#endif

#ifndef __uint16_t_defined
#define __uint16_t_defined
    typedef unsigned short      uint16_t;
#endif

#ifndef __uint32_t_defined
#define __uint32_t_defined
    typedef unsigned long       uint32_t;
#endif

#ifndef __CX51__ /* Keil 8051 define */
    #ifndef __uint64_t_defined
    #define __uint64_t_defined
        typedef unsigned long long  uint64_t;
    #endif
#endif


/* signed types */
/* Exclude int8_t and int32_t defintion if used in Linux environment.
   The same name is already defined in the includes for serial interface. */
#ifndef __int8_t_defined
#define __int8_t_defined
    typedef char                int8_t;
#endif

#ifndef __int16_t_defined
#define __int16_t_defined
    typedef short               int16_t;
#endif

#ifndef __CX51__ /* Keil 8051 define */
    #ifndef __int64_t_defined
    #define __int64_t_defined
        typedef long long           int64_t;
    #endif
#endif

#ifndef __bool_defined
#define __bool_defined
    #ifdef __CX51__ /* Keil 8051 define */
        typedef bit			        bool;
    #else
        #ifndef bool
        #define bool                unsigned char; */
        #endif
    #endif
#endif
#endif

#ifndef true
    #define true            1
#endif

#ifndef false
    #define false           0
#endif

#endif /* PHCSTYPEDEFS_H */
