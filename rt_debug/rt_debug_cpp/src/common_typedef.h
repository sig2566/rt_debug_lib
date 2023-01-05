/**********************************************************************
*
* INTEL CONFIDENTIAL
* Copyright 2009-2018 Intel Corporation All Rights Reserved.
* 
* The source code contained or described herein and all documents related to the
* source code ("Material") are owned by Intel Corporation or its suppliers or
* licensors. Title to the Material remains with Intel Corporation or its
* suppliers and licensors. The Material may contain trade secrets and proprietary
* and confidential information of Intel Corporation and its suppliers and
* licensors, and is protected by worldwide copyright and trade secret laws and
* treaty provisions. No part of the Material may be used, copied, reproduced,
* modified, published, uploaded, posted, transmitted, distributed, or disclosed
* in any way without Intel's prior express written permission.
* 
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or delivery
* of the Materials, either expressly, by implication, inducement, estoppel or
* otherwise. Any license under such intellectual property rights must be
* express and approved by Intel in writing.
* 
* Unless otherwise agreed by Intel in writing, you may not remove or alter this
* notice or any other notice embedded in Materials by Intel or Intel's suppliers
* or licensors in any way.
* 
*  version: RefPHY-flexran_18.12
*
**********************************************************************/

/**
 * @brief This file has the common type definitions and #defines used across the code
 * @file common_typedef.h
 * @ingroup group_lte_source_common
 * @author Intel Corporation
 **/

#ifndef _COMMON_TYPEDEF_H_
#define _COMMON_TYPEDEF_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include <stdint.h>

#ifndef _RESTRICT_
#define _RESTRICT_
#define RESTRICT restrict
#endif  /* _RESTRICT_ */

#ifndef _ALIGN_
#define _ALIGN_
#ifdef _WIN64
#define __align(x) __declspec(align(x))
#else
#define __align(x) __attribute__((aligned(x)))
#endif
#endif /* _ALIGN_ */

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#ifndef TRUE
/** TRUE = 1 */
#define TRUE 1
#endif /* #ifndef TRUE */

#ifndef FALSE
/** FALSE = 0 */
#define FALSE 0
#endif /* #ifndef FALSE */


#ifndef SUCCESS
/** SUCCESS = 0 */
#define SUCCESS     0
#endif /* #ifndef SUCCESS */

#ifndef FAILURE
/** FAILURE = 1 */
#define FAILURE     1
#endif /* #ifndef FAILURE */

#ifndef __INLINE__
#define __INLINE__
#if defined (WIN32)
#define INLINE
#elif defined (_MSC_VER)
#define INLINE                      _inline
#else
#define INLINE                      inline
#endif
#endif /* #ifndef __INLINE__ */

#ifndef _atomic_t_
#define _atomic_t_
/** atomic */
typedef volatile unsigned long atomic_t;
#endif /* #ifndef _atomic_t_ */

#ifdef _WIN64
#define ALIGN64 __declspec(align(64))
#else
#define ALIGN64 __attribute__((aligned(64)))
#endif

#ifndef _COMPLEX_
#define _COMPLEX_
/** complex type for int16_t */
typedef struct
{
    int16_t re;                      /**< real part */
    int16_t im;                      /**< imaginary  part */
}Complex;
#endif

#ifndef _COMPLEX8_
#define _COMPLEX8_
/** complex type for int16_t */
typedef struct
{
    signed char re;                      /**< real part */
    signed char im;                      /**< imaginary  part */
}Complex8;
#endif


#ifndef _COMPLEX32_
#define _COMPLEX32_
/** @struct Complex
 *  @brief this sturcture is a complex data structure
 */
typedef struct
{
    float re;                      // real part
    float im;                      // imag part
}Complex32;
#endif

//Default buffer size
#define GEN_BUF_SIZE (1000)



#ifdef PHY_GCC
#define IVDEP "GCC ivdep"
#define __assume_aligned(x,y) x=__builtin_assume_aligned(x,y)
#else
#define IVDEP "ivdep"
#endif

#ifdef __cplusplus
    }
#endif
#endif /* #ifndef _COMMON_TYPEDEF_H_ */

