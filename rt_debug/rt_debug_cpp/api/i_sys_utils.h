/******************************************************************
 * i_sys_utils.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/


#ifndef _I_SYS_UTILS_GLOBAL_H
#define _I_SYS_UTILS_GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------
 * Include section
 * -----------------------------------------------------------
 */

//
/*
 * -----------------------------------------------------------
 * MACRO (define) section
 * -----------------------------------------------------------
 */
#define UNIX

// in bytes
#define CACHE_ALIGNMENT			(64)

// align the size to the nearest aligned value
#define ALIGN_SIZE(x, ALIGNMENT_VALUE) (((x)+ALIGNMENT_VALUE-1)&(~(ALIGNMENT_VALUE-1)))

// add to the size an additional section with alignment value size in order to be able to align the allocated memory to the alignment value
#define ADD_SIZE_FOR_MEM_ALIGN(x, ALIGNMENT_VALUE) ((x)+ALIGNMENT_VALUE-1)
// align the pointer to the nearest aligned value
#define ALIGN_POINTER(x, ALIGNMENT_VALUE) ((uint64_t)((((uint64_t)(x))+ALIGNMENT_VALUE-1)&(~(ALIGNMENT_VALUE-1))))

#define ALIGN_128BIT(n)				((n) & ~0x0000000f)
#define INC_128BIT(n)				((n) + 0x00000010)
#define CHECK_ALIGN_128BIT(n)		(!((n) & 0x0000000c))

#define ALIGN_64BIT(n)              ((n) & ~0x00000007)
#define INC_64BIT(n)                ((n) + 0x00000008)
#define CHECK_ALIGN_64BIT(n)	    (!((n) & 0x00000007))

#define ALIGN_32BIT(n)              ((n) & ~0x00000003)
#define INC_32BIT(n)                ((n) + 0x00000004)
#define CHECK_ALIGN_32BIT(n)		(!((n) & 0x00000003))

#define ALIGN_4SAMPLES(n)			((n) & ~0x00000003)
#define CHECK_ALIGN_4SAMPLES(n)		(!((n) & 0x00000003))

#define BITS_TO_BYTES(n)            ((((n) + 7)) >> 3)
#define BITS_TO_WORDS(n)            ((((n) + 31)) >> 5)
#define ALIGN_TO_4(n)				(((n) + 3) & ~3)


#define W32_TO_BYTES(n)				((n) << 2)
#define BYTES_TO_W32(n)				((n) >> 2)

#define PHY_SAMPLE_TO_BYTES(n)		W32_TO_BYTES(n)
#define BYTES_TO_PHY_SAMPLE(n)		BYTES_TO_W32(n)

#define BPS_TO_KBPS(bw)             ((bw)/1000)
#define KBPS_TO_BPS(bw)             ((bw)*1000ll)

#define KBPS_TO_MBPS(bw)            ((bw)/1000)
#define MBPS_TO_KBPS(bw)            ((bw)*1000)

#define BPS_TO_MBPS(bw)             ((bw)/1000000)
#define MBPS_TO_BPS(bw)             ((bw)*1000000ll)

#define HZ_TO_KHZ(bw)               ((bw)/1000)
#define KHZ_TO_HZ(bw)               ((bw)*1000ll)

#define KHZ_TO_MHZ(bw)              ((bw)/1000)
#define MHZ_TO_KHZ(bw)              ((bw)*1000ll)

#define KHZ_TO_GHZ(bw)              ((bw)/1000000.0)
#define GHZ_TO_KHZ(bw)              ((bw)*1000000ll)

#define HZ_TO_MHZ(bw)               ((bw)/1000000)
#define MHZ_TO_HZ(bw)               ((bw)*1000000ll)

//the speed of light is 299,792,458 meters per second,
//meaning that it takes ~ 3 nano seconds to pass 1 meter
//RTD (round trip delay) is for both directions,
//thus the formula is (meters * 2_directions * 3_nano_secs_per_meter):

#define DISTANCE_METERS_TO_RTD_NANO_SECS(dist)     ((dist)*6)

#define	TRUE	1
#define	FALSE	0
#ifndef NULL
#define	NULL		((void *)0)
#endif
#define	PTR(x)		((void *)(x))
#define	UNUSED(x)	((void)(x))
#define	SIGN(a)	 (((a) < 0) ? -1 : 1)

#define DAN_FREQUENT	_Pragma("frequency_hint FREQUENT")
#define DAN_NEVER		_Pragma("frequency_hint NEVER")
#define DAN_FLUSH		_Pragma("flush")
#define DAN_ALIGNED(val)	__attribute__((aligned(val)))

//********************************************************************************************
//********************************************************************************************
// KS 22/04/12 TODO it in DS
// Definitions from DS_CLN_api.h
// Standard print routine support. The printf may be substituted for it using a linker option.

typedef enum
{
	LOG_LEVEL_CRIT = 1,                
	LOG_LEVEL_ERROR = 2,               
	LOG_LEVEL_WARNING = 3,               
	LOG_LEVEL_NOTICE = 4,               
	LOG_LEVEL_INFO = 5,               
	LOG_LEVEL_DEBUG = 6,              
	LOG_LEVEL_DEVEL = 7,              
}LOG_LEVEL;


typedef enum
{
	FAPI_REL_CODE_REL8 = 1,                
	FAPI_REL_CODE_REL10 = 2,
}FAPI_REL_CODE;

enum Constellation
{
    e_BPSK = 1,
    e_QPSK = 2,
    e_QAM16 = 4,
    e_QAM64 = 6,
    e_QAM256 = 8
};
#define	MAX_Constellation	e_QAM256



int  dsPrint(cchar* sformat, ...);
void dsLog(int level, const char* dsname, const char* sformat, ...);

// NORETURN void OS_assert(cchar* modname, cchar* func, cchar* file, int line) NOINLINE;
// NORETURN void OS_assert_ext(cchar* modname, cchar* func, cchar* file, int line, const char * p_format_str, ...) NOINLINE;
//NORETURN void OS_assert_single_arg(const OS_assert_params_t_ *assert_prms) NOINLINE;
// NORETURN void OS_assert_ext(const OS_assert_params_t_ *assert_prms, const char * p_format_str, ...) NOINLINE;

#define OS_printf(...)			dsLog(LOG_LEVEL_INFO, MODULENAME, __VA_ARGS__)
#define OS_printf_debug(...)    dsLog(LOG_LEVEL_INFO, MODULENAME, __VA_ARGS__)
#define OS_printf_level(l,...)  dsLog(l, MODULENAME, __VA_ARGS__) 

												
//#define ASSERT_ARGS		__attribute__ ((section(".rodata.assrt")))


// #define ASSERT_SINGLE_ARG



#define CHECK_PARAMS(cond)      assert(cond);
#define ASSERT(cond) 			assert(cond);


#define ASSERT_EXT(cond,...)												\
	if (!(cond))															\
	{																		\
		DAN_NEVER;															\
		OS_printf(__VA_ARGS__);												\
	    OS_assert(MODULENAME, __FUNCTION__, __FILENAME__, __LINE__);		\
	}

#define ASSERT_0()  		{OS_assert(MODULENAME, __FUNCTION__, __FILENAME__, __LINE__);}														
#define ASSERT_0_EXT(...)  	{OS_printf(__VA_ARGS__); OS_assert(MODULENAME, __FUNCTION__, __FILENAME__, __LINE__);}		




#define PRINT_ASSERT(x) {OS_printf("Assertion \"%s\" failed at line %d in %s\n", \
                                     x, __LINE__, __FUNCTION__);}
#define TEXT_ASSERT(x,y) {if(!(y)) {DAN_NEVER;PRINT_ASSERT(x); ASSERT(FALSE);}}

// Use the below macro for the debug purposes only, 
// it prints the current function name and line where it is placed.
#define DEBUG_PRINT_FUNC_N_LINE	OS_printf("#%u DBG: %s[%d]", PLATFORM_my_ipc_id, __FUNCTION__, __LINE__)
	
//********************************************************************************************
//********************************************************************************************


#define	LIMIT(x,l,h)		XT_MIN(XT_MAX(x,l),h)

#define	POW2(n)				(0x1<<(n))
#define	POW2_ll(n)			(0x1ll<<(n))
#define	POW2_F(n)			(1.0 * (float)POW2(n))

#define	F_POW2_INT(n)	((n) >= 0 ? (float)POW2(n) : 1.0/(float)POW2(-n))


//#define	INC_MOD(a,b,m)		(((a)+(b))<(m) ? ((a)+(b)) : ((a)+(b)-(m)))
#define	INC_MOD(a,b,m)		(((a)+(b)) %(m))
#define	MASK_BIT(b)			(POW2(b))
#define	MASK_N_BITS(n)		(POW2(n)-1)

#ifndef MIN
//#if !defined(UNIX) && !defined(WIN32) && !defined(ARM)
//#define MIN(x,y)            XT_MINU (x, y)
//#else
#define MIN(x,y)			((x)<(y) ? (x):(y))
//#endif
#endif

#ifndef MAX
#if !defined(UNIX) && !defined(WIN32) && !defined(ARM)
#define MAX(x,y)            XT_MAXU (x, y)
#else
#define MAX(x,y)			((x)>(y) ? (x):(y))
#endif
#endif

#define ABS(x)          (((x)>=0) ? (x) : -(x))

#define BIT(num, i)         ((num>>i)&0x1)
#define BUILD_WORD(halfword1,halfword2)		(((halfword2) << 16) | halfword1)
#define HALFWORD_LOW(word)					((word) & 0x0000FFFF)
#define HALFWORD_HIGH(word)					((word) >> 16)


#define UINT64_LOW(word)					(uint32_t)((word) & 0xFFFFFFFFll)
#define UINT64_HIGH(word)					(uint32_t)(UINT64_LOW((word) >> 32))

#define CEIL(X)         ( (X - (int)X)==0 ? (int)X : (int)X+1 )		// Eitan: define CEIL macro which replace the ceil()

// Get an array size
#define TABLESIZE(table) 	(sizeof(table)/sizeof((table)[0]))

#define OFFSETOF(s,m)		(int)&(((s *)0)->m)

#define	ADDR_MOD_4K(n)		((n) & MASK_N_BITS(12))
#define	ADDR_4K				(POW2(12))

#define	SIZE_OF_4_REs		(16)	// 16 bytes

#define	IQ_FIXP_MANTISSA	(16)


#define	SQRT_2				(1.4142135623730950488016887242097)
#define	SQRT_0_DOT_5		(0.70710678118654752440084436210485)
#define LOG10_2             (0.30102999566398119521373889472449)

#define UNITY_GAIN_FLOAT    (0.9999999999999999999999999999999)

#define SFBC_BOOST_OFFSET_DB      (-12)
#define PHICH_BOOST_OFFSET_DB     (-4)
#define RS_BOOST_OFFSET_DB        (-6)
#define MBSFN_RS_BOOST_OFFSET_DB  (-6)
#define PRS_BOOST_OFFSET_DB       (-6)
#define CSI_RS_BOOST_OFFSET_DB       (0)
#define CSI_RS_BOOST_OFFSET_FLOAT   (UNITY_GAIN_FLOAT)


#define PSS_BOOST_OFFSET_DB       (-6)
#define SSS_BOOST_OFFSET_DB       (-6)
#define	PBCH_BOOST_OFFSET_DB      (SFBC_BOOST_OFFSET_DB - 3)
#define	PCFICH_BOOST_OFFSET_DB    (SFBC_BOOST_OFFSET_DB - 3)
#define PDSCH_BOOST_OFFSET_DB     (-12)
#define PDCCH_BOOST_OFFSET_DB     (-15)


#define POW10_0_0_DB        (1.0)
#define POW10_0_5_DB        (1.1220184543019634355910389464779)
#define POW10_1_0_DB        (POW10_0_5_DB*POW10_0_5_DB)
#define POW10_1_5_DB        (POW10_1_0_DB*POW10_0_5_DB)
#define POW10_2_0_DB        (POW10_1_5_DB*POW10_0_5_DB)
#define POW10_2_5_DB        (POW10_2_0_DB*POW10_0_5_DB)
#define POW10_3_0_DB        (POW10_2_5_DB*POW10_0_5_DB)
#define POW10_3_5_DB        (POW10_3_0_DB*POW10_0_5_DB)
#define POW10_4_0_DB        (POW10_3_5_DB*POW10_0_5_DB)
#define POW10_4_5_DB        (POW10_4_0_DB*POW10_0_5_DB)
#define POW10_5_0_DB        (POW10_4_5_DB*POW10_0_5_DB)
#define POW10_5_5_DB        (POW10_5_0_DB*POW10_0_5_DB)
#define POW10_6_0_DB        (POW10_5_5_DB*POW10_0_5_DB)
#define POW10_6_5_DB        (POW10_6_0_DB*POW10_0_5_DB)
#define POW10_7_0_DB        (POW10_6_5_DB*POW10_0_5_DB)
#define POW10_7_5_DB        (POW10_7_0_DB*POW10_0_5_DB)
#define POW10_8_0_DB        (POW10_7_5_DB*POW10_0_5_DB)
#define POW10_8_5_DB        (POW10_8_0_DB*POW10_0_5_DB)
#define POW10_9_0_DB        (POW10_8_5_DB*POW10_0_5_DB)
#define POW10_9_5_DB        (POW10_9_0_DB*POW10_0_5_DB)
#define POW10_10_0_DB       (POW10_9_5_DB*POW10_0_5_DB)
#define POW10_10_5_DB       (POW10_10_0_DB*POW10_0_5_DB)
#define POW10_11_0_DB       (POW10_10_5_DB*POW10_0_5_DB)
#define POW10_11_5_DB       (POW10_11_0_DB*POW10_0_5_DB)
#define POW10_12_0_DB       (POW10_11_5_DB*POW10_0_5_DB)
#define POW10_12_5_DB       (POW10_12_0_DB*POW10_0_5_DB)
#define POW10_13_0_DB       (POW10_12_5_DB*POW10_0_5_DB)
#define POW10_13_5_DB       (POW10_13_0_DB*POW10_0_5_DB)
#define POW10_14_0_DB       (POW10_13_5_DB*POW10_0_5_DB)
#define POW10_14_5_DB       (POW10_14_0_DB*POW10_0_5_DB)
#define POW10_15_0_DB       (POW10_14_5_DB*POW10_0_5_DB)
#define POW10_15_5_DB       (POW10_15_0_DB*POW10_0_5_DB)
#define POW10_16_0_DB       (POW10_15_5_DB*POW10_0_5_DB)
#define POW10_16_5_DB       (POW10_16_0_DB*POW10_0_5_DB)
#define POW10_17_0_DB       (POW10_16_5_DB*POW10_0_5_DB)
#define POW10_17_5_DB       (POW10_17_0_DB*POW10_0_5_DB)
#define POW10_18_0_DB       (POW10_17_5_DB*POW10_0_5_DB)
#define POW10_18_5_DB       (POW10_18_0_DB*POW10_0_5_DB)
#define POW10_19_0_DB       (POW10_18_5_DB*POW10_0_5_DB)
#define POW10_19_5_DB       (POW10_19_0_DB*POW10_0_5_DB)
#define POW10_20_0_DB       (POW10_19_5_DB*POW10_0_5_DB)



#define POW10_M0_5_DB        (1.0/(POW10_0_5_DB))
#define POW10_M1_0_DB        (1.0/(POW10_1_0_DB))
#define POW10_M1_5_DB        (1.0/(POW10_1_5_DB))
#define POW10_M2_0_DB        (1.0/(POW10_2_0_DB))
#define POW10_M2_5_DB        (1.0/(POW10_2_5_DB))
#define POW10_M3_0_DB        (1.0/(POW10_3_0_DB))
#define POW10_M3_5_DB        (1.0/(POW10_3_5_DB))
#define POW10_M4_0_DB        (1.0/(POW10_4_0_DB))
#define POW10_M4_5_DB        (1.0/(POW10_4_5_DB))
#define POW10_M5_0_DB        (1.0/(POW10_5_0_DB))
#define POW10_M5_5_DB        (1.0/(POW10_5_5_DB))
#define POW10_M6_0_DB        (1.0/(POW10_6_0_DB))
#define POW10_M6_5_DB        (1.0/(POW10_6_5_DB))
#define POW10_M7_0_DB        (1.0/(POW10_7_0_DB))
#define POW10_M7_5_DB        (1.0/(POW10_7_5_DB))
#define POW10_M8_0_DB        (1.0/(POW10_8_0_DB))
#define POW10_M8_5_DB        (1.0/(POW10_8_5_DB))
#define POW10_M9_0_DB        (1.0/(POW10_9_0_DB))
#define POW10_M9_5_DB        (1.0/(POW10_9_5_DB))
#define POW10_M10_0_DB       (1.0/(POW10_10_0_DB))
#define POW10_M10_5_DB       (1.0/(POW10_10_5_DB))
#define POW10_M11_0_DB       (1.0/(POW10_11_0_DB))
#define POW10_M11_5_DB       (1.0/(POW10_11_5_DB))
#define POW10_M12_0_DB       (1.0/(POW10_12_0_DB))
#define POW10_M12_5_DB       (1.0/(POW10_12_5_DB))
#define POW10_M13_0_DB       (1.0/(POW10_13_0_DB))
#define POW10_M13_5_DB       (1.0/(POW10_13_5_DB))
#define POW10_M14_0_DB       (1.0/(POW10_14_0_DB))
#define POW10_M14_5_DB       (1.0/(POW10_14_5_DB))
#define POW10_M15_0_DB       (1.0/(POW10_15_0_DB))
#define POW10_M15_5_DB       (1.0/(POW10_15_5_DB))
#define POW10_M16_0_DB       (1.0/(POW10_16_0_DB))
#define POW10_M16_5_DB       (1.0/(POW10_16_5_DB))
#define POW10_M17_0_DB       (1.0/(POW10_17_0_DB))
#define POW10_M17_5_DB       (1.0/(POW10_17_5_DB))
#define POW10_M18_0_DB       (1.0/(POW10_18_0_DB))
#define POW10_M18_5_DB       (1.0/(POW10_18_5_DB))
#define POW10_M19_0_DB       (1.0/(POW10_19_0_DB))
#define POW10_M19_5_DB       (1.0/(POW10_19_5_DB))
#define POW10_M20_0_DB       (1.0/(POW10_20_0_DB))




#define POW20_0_0_DB        (1.0)
#define POW20_0_5_DB        (1.0592537251772888788092803732781)
#define POW20_1_0_DB        (POW20_0_5_DB*POW20_0_5_DB)
#define POW20_1_5_DB        (POW20_1_0_DB*POW20_0_5_DB)
#define POW20_2_0_DB        (POW20_1_5_DB*POW20_0_5_DB)
#define POW20_2_5_DB        (POW20_2_0_DB*POW20_0_5_DB)
#define POW20_3_0_DB        (POW20_2_5_DB*POW20_0_5_DB)
#define POW20_3_5_DB        (POW20_3_0_DB*POW20_0_5_DB)
#define POW20_4_0_DB        (POW20_3_5_DB*POW20_0_5_DB)
#define POW20_4_5_DB        (POW20_4_0_DB*POW20_0_5_DB)
#define POW20_5_0_DB        (POW20_4_5_DB*POW20_0_5_DB)
#define POW20_5_5_DB        (POW20_5_0_DB*POW20_0_5_DB)
#define POW20_6_0_DB        (POW20_5_5_DB*POW20_0_5_DB)
#define POW20_6_5_DB        (POW20_6_0_DB*POW20_0_5_DB)
#define POW20_7_0_DB        (POW20_6_5_DB*POW20_0_5_DB)
#define POW20_7_5_DB        (POW20_7_0_DB*POW20_0_5_DB)
#define POW20_8_0_DB        (POW20_7_5_DB*POW20_0_5_DB)
#define POW20_8_5_DB        (POW20_8_0_DB*POW20_0_5_DB)
#define POW20_9_0_DB        (POW20_8_5_DB*POW20_0_5_DB)
#define POW20_9_5_DB        (POW20_9_0_DB*POW20_0_5_DB)
#define POW20_10_0_DB       (POW20_9_5_DB*POW20_0_5_DB)
#define POW20_10_5_DB       (POW20_10_0_DB*POW20_0_5_DB)
#define POW20_11_0_DB       (POW20_10_5_DB*POW20_0_5_DB)
#define POW20_11_5_DB       (POW20_11_0_DB*POW20_0_5_DB)
#define POW20_12_0_DB       (POW20_11_5_DB*POW20_0_5_DB)
#define POW20_12_5_DB       (POW20_12_0_DB*POW20_0_5_DB)
#define POW20_13_0_DB       (POW20_12_5_DB*POW20_0_5_DB)
#define POW20_13_5_DB       (POW20_13_0_DB*POW20_0_5_DB)
#define POW20_14_0_DB       (POW20_13_5_DB*POW20_0_5_DB)
#define POW20_14_5_DB       (POW20_14_0_DB*POW20_0_5_DB)
#define POW20_15_0_DB       (POW20_14_5_DB*POW20_0_5_DB)
#define POW20_15_5_DB       (POW20_15_0_DB*POW20_0_5_DB)
#define POW20_16_0_DB       (POW20_15_5_DB*POW20_0_5_DB)
#define POW20_16_5_DB       (POW20_16_0_DB*POW20_0_5_DB)
#define POW20_17_0_DB       (POW20_16_5_DB*POW20_0_5_DB)
#define POW20_17_5_DB       (POW20_17_0_DB*POW20_0_5_DB)
#define POW20_18_0_DB       (POW20_17_5_DB*POW20_0_5_DB)
#define POW20_18_5_DB       (POW20_18_0_DB*POW20_0_5_DB)
#define POW20_19_0_DB       (POW20_18_5_DB*POW20_0_5_DB)
#define POW20_19_5_DB       (POW20_19_0_DB*POW20_0_5_DB)
#define POW20_20_0_DB       (POW20_19_5_DB*POW20_0_5_DB)
#define POW20_20_5_DB       (POW20_20_0_DB*POW20_0_5_DB)
#define POW20_21_0_DB       (POW20_20_5_DB*POW20_0_5_DB)
#define POW20_21_5_DB       (POW20_21_0_DB*POW20_0_5_DB)
#define POW20_22_0_DB       (POW20_21_5_DB*POW20_0_5_DB)
#define POW20_22_5_DB       (POW20_22_0_DB*POW20_0_5_DB)
#define POW20_23_0_DB       (POW20_22_5_DB*POW20_0_5_DB)
#define POW20_23_5_DB       (POW20_23_0_DB*POW20_0_5_DB)
#define POW20_24_0_DB       (POW20_23_5_DB*POW20_0_5_DB)
#define POW20_24_5_DB       (POW20_24_0_DB*POW20_0_5_DB)
#define POW20_25_0_DB       (POW20_24_5_DB*POW20_0_5_DB)
#define POW20_25_5_DB       (POW20_25_0_DB*POW20_0_5_DB)
#define POW20_26_0_DB       (POW20_25_5_DB*POW20_0_5_DB)
#define POW20_26_5_DB       (POW20_26_0_DB*POW20_0_5_DB)
#define POW20_27_0_DB       (POW20_26_5_DB*POW20_0_5_DB)
#define POW20_27_5_DB       (POW20_27_0_DB*POW20_0_5_DB)
#define POW20_28_0_DB       (POW20_27_5_DB*POW20_0_5_DB)
#define POW20_28_5_DB       (POW20_28_0_DB*POW20_0_5_DB)
#define POW20_29_0_DB       (POW20_28_5_DB*POW20_0_5_DB)
#define POW20_29_5_DB       (POW20_29_0_DB*POW20_0_5_DB)
#define POW20_30_0_DB       (POW20_29_5_DB*POW20_0_5_DB)
#define POW20_30_5_DB       (POW20_30_0_DB*POW20_0_5_DB)
#define POW20_31_0_DB       (POW20_30_5_DB*POW20_0_5_DB)
#define POW20_31_5_DB       (POW20_31_0_DB*POW20_0_5_DB)
#define POW20_32_0_DB       (POW20_31_5_DB*POW20_0_5_DB)
#define POW20_32_5_DB       (POW20_32_0_DB*POW20_0_5_DB)
#define POW20_33_0_DB       (POW20_32_5_DB*POW20_0_5_DB)
#define POW20_33_5_DB       (POW20_33_0_DB*POW20_0_5_DB)
#define POW20_34_0_DB       (POW20_33_5_DB*POW20_0_5_DB)
#define POW20_34_5_DB       (POW20_34_0_DB*POW20_0_5_DB)
#define POW20_35_0_DB       (POW20_34_5_DB*POW20_0_5_DB)



#define POW20_M0_5_DB        (1.0/(POW20_0_5_DB))
#define POW20_M1_0_DB        (1.0/(POW20_1_0_DB))
#define POW20_M1_5_DB        (1.0/(POW20_1_5_DB))
#define POW20_M2_0_DB        (1.0/(POW20_2_0_DB))
#define POW20_M2_5_DB        (1.0/(POW20_2_5_DB))
#define POW20_M3_0_DB        (1.0/(POW20_3_0_DB))
#define POW20_M3_5_DB        (1.0/(POW20_3_5_DB))
#define POW20_M4_0_DB        (1.0/(POW20_4_0_DB))
#define POW20_M4_5_DB        (1.0/(POW20_4_5_DB))
#define POW20_M5_0_DB        (1.0/(POW20_5_0_DB))
#define POW20_M5_5_DB        (1.0/(POW20_5_5_DB))
#define POW20_M6_0_DB        (1.0/(POW20_6_0_DB))
#define POW20_M6_5_DB        (1.0/(POW20_6_5_DB))
#define POW20_M7_0_DB        (1.0/(POW20_7_0_DB))
#define POW20_M7_5_DB        (1.0/(POW20_7_5_DB))
#define POW20_M8_0_DB        (1.0/(POW20_8_0_DB))
#define POW20_M8_5_DB        (1.0/(POW20_8_5_DB))
#define POW20_M9_0_DB        (1.0/(POW20_9_0_DB))
#define POW20_M9_5_DB        (1.0/(POW20_9_5_DB))
#define POW20_M10_0_DB       (1.0/(POW20_10_0_DB))
#define POW20_M10_5_DB       (1.0/(POW20_10_5_DB))
#define POW20_M11_0_DB       (1.0/(POW20_11_0_DB))
#define POW20_M11_5_DB       (1.0/(POW20_11_5_DB))
#define POW20_M12_0_DB       (1.0/(POW20_12_0_DB))
#define POW20_M12_5_DB       (1.0/(POW20_12_5_DB))
#define POW20_M13_0_DB       (1.0/(POW20_13_0_DB))
#define POW20_M13_5_DB       (1.0/(POW20_13_5_DB))
#define POW20_M14_0_DB       (1.0/(POW20_14_0_DB))
#define POW20_M14_5_DB       (1.0/(POW20_14_5_DB))
#define POW20_M15_0_DB       (1.0/(POW20_15_0_DB))
#define POW20_M15_5_DB       (1.0/(POW20_15_5_DB))
#define POW20_M16_0_DB       (1.0/(POW20_16_0_DB))
#define POW20_M16_5_DB       (1.0/(POW20_16_5_DB))
#define POW20_M17_0_DB       (1.0/(POW20_17_0_DB))
#define POW20_M17_5_DB       (1.0/(POW20_17_5_DB))
#define POW20_M18_0_DB       (1.0/(POW20_18_0_DB))
#define POW20_M18_5_DB       (1.0/(POW20_18_5_DB))
#define POW20_M19_0_DB       (1.0/(POW20_19_0_DB))
#define POW20_M19_5_DB       (1.0/(POW20_19_5_DB))
#define POW20_M20_0_DB       (1.0/(POW20_20_0_DB))
#define POW20_M20_5_DB       (1.0/(POW20_20_5_DB))
#define POW20_M21_0_DB       (1.0/(POW20_21_0_DB))
#define POW20_M21_5_DB       (1.0/(POW20_21_5_DB))
#define POW20_M22_0_DB       (1.0/(POW20_22_0_DB))
#define POW20_M22_5_DB       (1.0/(POW20_22_5_DB))
#define POW20_M23_0_DB       (1.0/(POW20_23_0_DB))
#define POW20_M23_5_DB       (1.0/(POW20_23_5_DB))
#define POW20_M24_0_DB       (1.0/(POW20_24_0_DB))
#define POW20_M24_5_DB       (1.0/(POW20_24_5_DB))
#define POW20_M25_0_DB       (1.0/(POW20_25_0_DB))
#define POW20_M25_5_DB       (1.0/(POW20_25_5_DB))
#define POW20_M26_0_DB       (1.0/(POW20_26_0_DB))
#define POW20_M26_5_DB       (1.0/(POW20_26_5_DB))
#define POW20_M27_0_DB       (1.0/(POW20_27_0_DB))
#define POW20_M27_5_DB       (1.0/(POW20_27_5_DB))
#define POW20_M28_0_DB       (1.0/(POW20_28_0_DB))
#define POW20_M28_5_DB       (1.0/(POW20_28_5_DB))
#define POW20_M29_0_DB       (1.0/(POW20_29_0_DB))
#define POW20_M29_5_DB       (1.0/(POW20_29_5_DB))
#define POW20_M30_0_DB       (1.0/(POW20_30_0_DB))
#define POW20_M30_5_DB       (1.0/(POW20_30_5_DB))
#define POW20_M31_0_DB       (1.0/(POW20_31_0_DB))
#define POW20_M31_5_DB       (1.0/(POW20_31_5_DB))
#define POW20_M32_0_DB       (1.0/(POW20_32_0_DB))
#define POW20_M32_5_DB       (1.0/(POW20_32_5_DB))
#define POW20_M33_0_DB       (1.0/(POW20_33_0_DB))
#define POW20_M33_5_DB       (1.0/(POW20_33_5_DB))
#define POW20_M34_0_DB       (1.0/(POW20_34_0_DB))
#define POW20_M34_5_DB       (1.0/(POW20_34_5_DB))
#define POW20_M35_0_DB       (1.0/(POW20_35_0_DB))


// additional boosting values complient with:
//DAN_E_STD_PA_MINUS_6dB      = 0L,
//DAN_E_STD_PA_MINUS_4_77dB   = 1L,
//DAN_E_STD_PA_MINUS_3dB      = 2L,
//DAN_E_STD_PA_MINUS_1_77dB   = 3L,
//DAN_E_STD_PA_0dB            = 4L,
//DAN_E_STD_PA_1dB            = 5L,
//DAN_E_STD_PA_2dB            = 6L,
//DAN_E_STD_PA_3dB            = 7L,


#define POW10_M4_77_DB       (0.33342641276323495361951214379204)
#define POW10_M1_77_DB       (0.66527315620174135756297280349963)


#define POW20_M4_77_DB       (0.57743087271398552544808931972263)
#define POW20_M1_77_DB       (0.81564278713278728874809527338256)

// ro_B / ro_A power ratios

// single anttena port
#define Pb_Inx_0_AntPorts_1    (1.0)                                // sqrt(1.0)
#define Pb_Inx_1_AntPorts_1    (0.89442719099991587856366946749251) // sqrt(4.0/5.0)
#define Pb_Inx_2_AntPorts_1    (0.77459666924148337703585307995648) // sqrt(3.0/5.0)
#define Pb_Inx_3_AntPorts_1    (0.63245553203367586639977870888654) // sqrt(2.0/5.0)


// 2 or 4 anttena ports
#define Pb_Inx_0_AntPorts_2_4    (1.1180339887498948482045868343656)    // sqrt(5.0/4.0)
#define Pb_Inx_1_AntPorts_2_4    (1.0)                                  // sqrt(1.0)
#define Pb_Inx_2_AntPorts_2_4    (0.86602540378443864676372317075294)   // sqrt(3.0/4.0)
#define Pb_Inx_3_AntPorts_2_4    (0.70710678118654752440084436210485)   // sqrt(1.0/2.0)



#define PI					(3.1415926535897932384626433832795)

//#define	F_TO_FIXP(x,n)			((int32_t)((x)*POW2_F((n)-1)))
#define	F_TO_FIXP(x,n)			((int16_t)((x)*POW2_F((n)-1)))
#define	F_TO_FIXP16(x)			F_TO_FIXP(x,IQ_FIXP_MANTISSA)

#define	FIX_NORM(x,n)			((x)>>((n)-1))
#define	FIXP16_NORM(x)			FIX_NORM(x,IQ_FIXP_MANTISSA)


#define	ImRe_PACK(i,r)			(((((int16_t)(i)) & MASK_N_BITS(IQ_FIXP_MANTISSA)) << IQ_FIXP_MANTISSA)	\
									| (((int16_t)(r)) & MASK_N_BITS(IQ_FIXP_MANTISSA))	)
#define	ReIm_PACK(r,i)			ImRe_PACK(i,r)

#define	ImRe_FIXP_PACK(i,r,n)		ImRe_PACK(F_TO_FIXP(i,n),F_TO_FIXP(r,n))
#define	ImRe_FIXP16_PACK(i,r)		ImRe_FIXP_PACK(i,r,IQ_FIXP_MANTISSA)


#define	EXTRACT_IMAG(n)	(((n) >> (IQ_FIXP_MANTISSA)) & MASK_N_BITS(IQ_FIXP_MANTISSA))
#define	EXTRACT_REAL(n)	((n) & MASK_N_BITS(IQ_FIXP_MANTISSA))


/* Used as termination value for initialization list of HW registers */
#define HW_REG_OFFSET_LAST			(0xFFFFFFFF)

#define HW_REG_CONF_TABLE_PTR(name) &HW_reg_conf_##name[0]
// Create a configuration registers table for specific HW_REG entity
#define HW_START_REG_CONF_TABLE(name)		\
		const HW_reg_conf_array_t RODATA_INIT HW_reg_conf_##name[] = {
// Configuration registers table entity
#define HW_REG_CONF_ENTRY(_offset,_val)	{_offset,_val},
// End of configuration registers table
#define HW_END_REG_CONF_TABLE {HW_REG_OFFSET_LAST,0}};

/* INTERNAL: Read/Write access to APB memory mapped registers */
#define HW_SET_REG(ba,x,y) (*((vuint32_t *)((uint32_t)(ba)+(uint32_t)(x))) =(uint32_t)(y))
#define HW_GET_REG(ba,x)	 (*((vuint32_t *)((uint32_t)(ba)+(uint32_t)(x))))


#define HW_PRINT 1
#define HW_DEBUG 2

//#define HW_REG_DEBUG  HW_DEBUG

#if (HW_REG_DEBUG == HW_PRINT) 
#define HW_SET_REG_PRINT(module,ba,x,y) (*((vuint32_t *)((uint32_t)(ba)+(uint32_t)(x))) =(uint32_t)(y)),	\
								OS_printf("Module %s  Reg Address = 0X%08x 0X%08x \n",module,(vuint32_t *)((uint32_t)(ba)+(uint32_t)(x)),y)
#define HW_SET_REG_DEBUG(ba,x,y,d_addres) (*((vuint32_t *)((uint32_t)(ba)+(uint32_t)(x))) =(uint32_t)(y)),	\
								(*((vuint32_t *)((uint32_t)(d_addres))) =(uint32_t)(y))
#else
#define HW_SET_REG_PRINT(module,ba,x,y) (*((vuint32_t *)((uint32_t)(ba)+(uint32_t)(x))) =(uint32_t)(y))

#endif

#if (HW_REG_DEBUG == HW_DEBUG) 
#define HW_SET_REG_DEBUG(ba,x,y,d_addres) (*((vuint32_t *)((uint32_t)(ba)+(uint32_t)(x))) =(uint32_t)(y)),	\
								(*((vuint32_t *)((uint32_t)(d_addres))) =(uint32_t)(y))

#endif
#ifndef HW_REG_DEBUG
#define HW_SET_REG_DEBUG(ba,x,y,d_addres) (*((vuint32_t *)((uint32_t)(ba)+(uint32_t)(x))) =(uint32_t)(y))
#endif



#define	ROUND_DIV(x,y)			(((x) + (y) / 2) / (y))
#define	ROUND_UP_DIV(x,y)		(((x) + (y) - 1) / (y))
#define	ROUND_UP_POW_2(x,y)		(((x) + ((y) - 1)) & (~((y) - 1)))
#define ROUND_UP(x,y)		    (ROUND_UP_DIV(x,y) * (y))
#define	ROUND_DOWN_POW_2(x,y)	((x) & (~((y) - 1)))
#define ROUND_SCALE(x,scale)	(  (x + (1 << (scale-1)) )>> scale)
#define	BITS_IN_WORD			(sizeof(uint32_t)*8)
#define ROUND_NUM(x,rnd_num)	((x + (rnd_num>>1)) & (~(rnd_num-1)))


#define SECTION(_sec)  __attribute__ ((section(_sec)))

#define    outb(x,y)    (*((char *)(x)) = y)
#define    outw(x,y)    (*((short *)(x)) = y)
#define    outl(x,y)    (*((long *)(x)) = y)

#define    inb(x)       (*((char *)(x)))
#define    inw(x)       (*((short *)(x)))
#define    inl(x)       (*((long *)(x)))

#define    outvb(x,y)    (*((volatile char *)(x)) = y)
#define    outvw(x,y)    (*((volatile short *)(x)) = y)
#define    outvl(x,y)    (*((volatile long *)(x)) = y)

#define    invb(x)       (*((volatile char *)(x)))
#define    invw(x)       (*((volatile short *)(x)))
#define    invl(x)       (*((volatile long *)(x)))
#define    RegWrite32(x,y)	(outvl(x,y))
#define    RegRead32(x)		(invl(x))

/* Embed a bit field (LEN) bits wide from a source byte (SRC)
*  where the field is in the (LEN) least significant bits, into
*  a destination byte (DST) starting at bit position (POS) and
*  return the modified version of (DST).
*/
#define PACK_W32(SRC,DST,POS,LEN) \
((((~(0xFFFFFFFF<<(LEN)))&(SRC))<<(POS))|((DST)&(~((~(0xFFFFFFFF<<(LEN)))<<(POS)))))

/* Extract a bit field (LEN) bits wide, starting at bit (POS) from
*  within a source byte (SRC) and return it as the least significant
*  bits of an otherwise zero byte.
*
*/
#define SUB_W32(SRC,POS,LEN) (((SRC)>>(POS))&(~(0xFFFFFFFF<<(LEN))))

#define	FIELD(val,field)	(((val) & MASK_N_BITS(field##_LEN)) << (field##_POS))

/*
 * Cyclic increment and decrement x with respect to base
 */
#define CYCLIC_INC(x,base)      ((x)<(base-1)) ? (x+1) : 0
#define CYCLIC_DEC(x,base)      (x)            ? (x-1) : (base-1)



#ifndef DEBUG_OUT
#ifdef SIMULATION
	#define	DEBUG_OUT(x)	WRITE_OUTQ0_32(x)
#endif
#endif

#define	COUNT_W128_TO_W32(n)	((n)<<2)
#define	COUNT_W32_TO_W128(n)	((n)>>2)

#define	ADDR_W128_TO_ADDR_W32(n)	(COUNT_W128_TO_W32(n))
#define	ADDR_W32_TO_ADDR_W128(n)	(COUNT_W32_TO_W128(n))

/*Entry size in aseert strings table in shared memory*/
#define MAX_FAULT_STR_SIZE           (256)
#define MAX_FAULT_STR_NUM            (400)

#define VALIDATE_AT_COMPILE_TIME(type, cond) \
    extern int DATA_SLOWEST g_dummy_##type[((cond) == 0) - 1];

#define VALIDATE_TYPE_ALIGNMENT_ARRAY(type, size) \
    VALIDATE_AT_COMPILE_TIME(type, ((sizeof(type) * (size)) % AG_PLT_PARAM_CACHE_LINE_SIZE));

#define VALIDATE_TYPE_ALIGNMENT(type) VALIDATE_TYPE_ALIGNMENT_ARRAY(type, 1)


/*
 * -----------------------------------------------------------
 * TYPEDEF section
 * -----------------------------------------------------------
 */
/*
 * -----------------------------------------------------------
 * MACRO to set memory allocation attributes for code & data
 * -----------------------------------------------------------
 */
#if !defined(UNIX) && !defined(WIN32)

#define RODATA_SLOWEST			__attribute__ ((section("." MODULENAME "_slowest.rodata")))
#define DATA_SLOWEST			__attribute__ ((section("." MODULENAME "_slowest.data")))
#define CODE_SLOWEST			__attribute__ ((section("." MODULENAME "_slowest.text")))
#define RODATA_SLOWEST_ALIGN(x)	__attribute__ ((section("." MODULENAME "_slowest.rodata"),aligned(x)))
#define DATA_SLOWEST_ALIGN(x)	__attribute__ ((section("." MODULENAME "_slowest.data"),aligned(x)))
#define CODE_SLOWEST_ALIGN(x)	__attribute__ ((section("." MODULENAME "_slowest.text"),aligned(x)))

#ifndef DEBUG
#if defined(ECLIPSE)

// this option should never happen, it is put there just to help the eplips indexer
// the indexer treats the __attribute__ lines as errors
#error how did we get here? did someone defined ECLIPS?

#define RODATA_FASTEST
#define DATA_FASTEST
#define CODE_FASTEST
#define RODATA_FAST
#define DATA_FAST
#define CODE_FAST
#define RODATA_MEDIUM
#define DATA_MEDIUM
#define CODE_MEDIUM
#define RODATA_SLOW
#define DATA_SLOW
#define CODE_SLOW
#define RODATA_DEFAULT
#define DATA_DEFAULT
#define CODE_DEFAULT

#define RODATA_INIT
#define DATA_INIT
#define CODE_INIT

#define RODATA_FASTEST_ALIGN(x)
#define DATA_FASTEST_ALIGN(x)
#define CODE_FASTEST_ALIGN(x)
#define RODATA_FAST_ALIGN(x)
#define DATA_FAST_ALIGN(x)
#define CODE_FAST_ALIGN(x)
#define RODATA_MEDIUM_ALIGN(x)
#define DATA_MEDIUM_ALIGN(x)
#define CODE_MEDIUM_ALIGN(x)
#define RODATA_SLOW_ALIGN(x)
#define DATA_SLOW_ALIGN(x)
#define CODE_SLOW_ALIGN(x)
#define RODATA_DEFAULT_ALIGN(x)
#define DATA_DEFAULT_ALIGN(x)
#define CODE_DEFAULT_ALIGN(x)

#define RODATA_INIT_ALIGN(x)
#define DATA_INIT_ALIGN(x)
#define CODE_INIT_ALIGN(x)

#else
#define RODATA_FASTEST			__attribute__ ((section("." MODULENAME "_fastest.rodata")))
#define DATA_FASTEST			__attribute__ ((section("." MODULENAME "_fastest.data")))
#define CODE_FASTEST			__attribute__ ((section("." MODULENAME "_fastest.text")))
#define RODATA_FAST				__attribute__ ((section("." MODULENAME "_fast.rodata")))
#define DATA_FAST				__attribute__ ((section("." MODULENAME "_fast.data")))
#define CODE_FAST				__attribute__ ((section("." MODULENAME "_fast.text")))
#define RODATA_MEDIUM			__attribute__ ((section("." MODULENAME "_medium.rodata")))
#define DATA_MEDIUM				__attribute__ ((section("." MODULENAME "_medium.data")))
#define CODE_MEDIUM				__attribute__ ((section("." MODULENAME "_medium.text")))
#define RODATA_SLOW				__attribute__ ((section("." MODULENAME "_slow.rodata")))
#define DATA_SLOW				__attribute__ ((section("." MODULENAME "_slow.data")))
#define CODE_SLOW				__attribute__ ((section("." MODULENAME "_slow.text")))
#define RODATA_DEFAULT			__attribute__ ((section("." MODULENAME "_default.rodata")))
#define DATA_DEFAULT			__attribute__ ((section("." MODULENAME "_default.data")))
#define CODE_DEFAULT			__attribute__ ((section("." MODULENAME "_default.text")))

#define RODATA_INIT				__attribute__ ((section("." MODULENAME "_init.rodata")))
#define DATA_INIT				__attribute__ ((section("." MODULENAME "_init.data")))
#define CODE_INIT				__attribute__ ((section("." MODULENAME "_init.text")))

#define RODATA_FASTEST_ALIGN(x)	__attribute__ ((section("." MODULENAME "_fastest.rodata"),aligned(x)))
#define DATA_FASTEST_ALIGN(x)	__attribute__ ((section("." MODULENAME "_fastest.data"),aligned(x)))
#define CODE_FASTEST_ALIGN(x)	__attribute__ ((section("." MODULENAME "_fastest.text"),aligned(x)))
#define RODATA_FAST_ALIGN(x)	__attribute__ ((section("." MODULENAME "_fast.rodata"),aligned(x)))
#define DATA_FAST_ALIGN(x)		__attribute__ ((section("." MODULENAME "_fast.data"),aligned(x)))
#define CODE_FAST_ALIGN(x)		__attribute__ ((section("." MODULENAME "_fast.text"),aligned(x)))
#define RODATA_MEDIUM_ALIGN(x)	__attribute__ ((section("." MODULENAME "_medium.rodata"),aligned(x)))
#define DATA_MEDIUM_ALIGN(x)	__attribute__ ((section("." MODULENAME "_medium.data"),aligned(x)))
#define CODE_MEDIUM_ALIGN(x)	__attribute__ ((section("." MODULENAME "_medium.text"),aligned(x)))
#define RODATA_SLOW_ALIGN(x)	__attribute__ ((section("." MODULENAME "_slow.rodata"),aligned(x)))
#define DATA_SLOW_ALIGN(x)		__attribute__ ((section("." MODULENAME "_slow.data"),aligned(x)))
#define CODE_SLOW_ALIGN(x)		__attribute__ ((section("." MODULENAME "_slow.text"),aligned(x)))
#define RODATA_DEFAULT_ALIGN(x)	__attribute__ ((section("." MODULENAME "_default.rodata"),aligned(x)))
#define DATA_DEFAULT_ALIGN(x)	__attribute__ ((section("." MODULENAME "_default.data"),aligned(x)))
#define CODE_DEFAULT_ALIGN(x)	__attribute__ ((section("." MODULENAME "_default.text"),aligned(x)))

#define RODATA_INIT_ALIGN(x)	__attribute__ ((section("." MODULENAME "_init.rodata"),aligned(x)))
#define DATA_INIT_ALIGN(x)		__attribute__ ((section("." MODULENAME "_init.data"),aligned(x)))
#define CODE_INIT_ALIGN(x)		__attribute__ ((section("." MODULENAME "_init.text"),aligned(x)))

#endif // defined eclipse

#else
#define RODATA_FASTEST			RODATA_SLOWEST
#define DATA_FASTEST			DATA_SLOWEST
#define CODE_FASTEST			CODE_SLOWEST
#define RODATA_FAST				RODATA_SLOWEST
#define DATA_FAST				DATA_SLOWEST
#define CODE_FAST				CODE_SLOWEST
#define RODATA_MEDIUM			RODATA_SLOWEST
#define DATA_MEDIUM				DATA_SLOWEST
#define CODE_MEDIUM				CODE_SLOWEST
#define RODATA_SLOW				RODATA_SLOWEST
#define DATA_SLOW				DATA_SLOWEST
#define CODE_SLOW				CODE_SLOWEST
#define RODATA_DEFAULT			RODATA_SLOWEST
#define DATA_DEFAULT			DATA_SLOWEST
#define CODE_DEFAULT			CODE_SLOWEST

#define RODATA_INIT			RODATA_SLOWEST
#define DATA_INIT				DATA_SLOWEST
#define CODE_INIT				CODE_SLOWEST

#define RODATA_FASTEST_ALIGN	RODATA_SLOWEST_ALIGN
#define DATA_FASTEST_ALIGN		DATA_SLOWEST_ALIGN
#define CODE_FASTEST_ALIGN	    CODE_SLOWEST_ALIGN
#define RODATA_FAST_ALIGN		RODATA_SLOWEST_ALIGN
#define DATA_FAST_ALIGN			DATA_SLOWEST_ALIGN
#define CODE_FAST_ALIGN		    CODE_SLOWEST_ALIGN
#define RODATA_MEDIUM_ALIGN		RODATA_SLOWEST_ALIGN
#define DATA_MEDIUM_ALIGN		DATA_SLOWEST_ALIGN
#define CODE_MEDIUM_ALIGN	    CODE_SLOWEST_ALIGN
#define RODATA_SLOW_ALIGN		RODATA_SLOWEST_ALIGN
#define DATA_SLOW_ALIGN			DATA_SLOWEST_ALIGN
#define CODE_SLOW_ALIGN		    CODE_SLOWEST_ALIGN
#define RODATA_DEFAULT_ALIGN	RODATA_SLOWEST_ALIGN
#define DATA_DEFAULT_ALIGN		DATA_SLOWEST_ALIGN
#define CODE_DEFAULT_ALIGN	    CODE_SLOWEST_ALIGN

#define RODATA_INIT_ALIGN		RODATA_SLOWEST_ALIGN
#define DATA_INIT_ALIGN			DATA_SLOWEST_ALIGN
#define CODE_INIT_ALIGN	    	CODE_SLOWEST_ALIGN
#endif

#else
#define RODATA_FASTEST
#define DATA_FASTEST
#define CODE_FASTEST
#define RODATA_FAST
#define DATA_FAST
#define CODE_FAST
#define RODATA_MEDIUM
#define DATA_MEDIUM
#define CODE_MEDIUM
#define RODATA_SLOW
#define DATA_SLOW
#define CODE_SLOW
#define RODATA_SLOWEST
#define DATA_SLOWEST
#define CODE_SLOWEST
#define RODATA_DEFAULT
#define DATA_DEFAULT
#define CODE_DEFAULT

#define RODATA_INIT
#define DATA_INIT
#define CODE_INIT

#define RODATA_FASTEST_ALIGN(x)
#define DATA_FASTEST_ALIGN(x)
#define CODE_FASTEST_ALIGN(x)
#define RODATA_FAST_ALIGN(x)
#define DATA_FAST_ALIGN(x)
#define CODE_FAST_ALIGN(x)
#define RODATA_MEDIUM_ALIGN(x)
#define DATA_MEDIUM_ALIGN(x)
#define CODE_MEDIUM_ALIGN(x)
#define RODATA_SLOW_ALIGN(x)
#define DATA_SLOW_ALIGN(x)
#define CODE_SLOW_ALIGN(x)
#define RODATA_SLOWEST_ALIGN(x)
#define DATA_SLOWEST_ALIGN(x)
#define CODE_SLOWEST_ALIGN(x)
#define RODATA_DEFAULT_ALIGN(x)
#define DATA_DEFAULT_ALIGN(x)
#define CODE_DEFAULT_ALIGN(x)

#define RODATA_INIT_ALIGN(x)
#define DATA_INIT_ALIGN(x)
#define CODE_INIT_ALIGN(x)
#endif // #if !defined(UNIX) && !defined(WIN32)

#undef BIG_ENDIAN
#ifndef BIG_ENDIAN
//#define	BIG_ENDIAN				0
#if !defined(LITTLE_ENDIAN)
#define	LITTLE_ENDIAN			1
#endif
#else
#error "Supported only little endian"
#endif

#define	ENDIANESS				LITTLE_ENDIAN


#define __UINT8_MAX			(0xFF)			// = 255
#define __UINT16_MAX		(0xFFFF)			// = 65535
#define __UINT32_MAX		(0xFFFFFFFF)		// = 4294967295
#define __INT16_MAX			(32767)			// = 32767
#define __INT16_MIN			(-(__INT16_MAX) - 1)// = -32768
#define __INT32_MAX			(2147483647)	// = 2147483647
#define __INT32_MIN			(-(__INT32_MAX) - 1)// = -2147483648

#define	CEIL_LOG2_INT(x)		LOG2_INT(2*(x)-1)

// get log 2 of integer
#define	LOG2_INT(x)			LOG2_INT_RNG32(x,0)
#define	LOG2_INT_RNG32(x,n)	(	(x)>=POW2_INT(16+(n))? LOG2_INT_RNG16(x,16+(n)) : LOG2_INT_RNG16(x,n)	)
#define	LOG2_INT_RNG16(x,n)	(	(x)>=POW2_INT(8+(n)) ? LOG2_INT_RNG8(x,8+(n)) 	: LOG2_INT_RNG8(x,n)	)
#define	LOG2_INT_RNG8(x,n)	(	(x)>=POW2_INT(4+(n)) ? LOG2_INT_RNG4(x,4+(n)) 	: LOG2_INT_RNG4(x,n)	)
#define	LOG2_INT_RNG4(x,n)	(	(x)>=POW2_INT(2+(n)) ? LOG2_INT_RNG2(x,2+(n)) 	: LOG2_INT_RNG2(x,n)	)
#define	LOG2_INT_RNG2(x,n)	(	(x)>=POW2_INT(1+(n)) ? LOG2_INT_RNG1(x,1+(n))	: LOG2_INT_RNG1(x,n)	)
#define	LOG2_INT_RNG1(x,n)	(	(x)>=POW2_INT(0+(n)) ? n : -1	)


// modulo 2^n
#define	MOD_POW2(x,n)					((x) & MASK_N_BITS(n))
#define	MOD_POW2_SIGN(x,n)				((x) & MASK_BIT((n)-1))

#define	UINT_A_LT_B_MODnBIT(a,b,n)		MOD_POW2_SIGN((a)-(b),n)
#define	UINT_A_EQ_B_MODnBIT(a,b,n)		(MOD_POW2((a)-(b),n) == 0)

#define	UINT16_A_LT_B_MOD32BIT(a,b)		UINT_A_LT_B_MODnBIT(a,b,16)
#define	UINT16_A_LE_B_MOD32BIT(a,b)		UINT_A_LT_B_MODnBIT(((a)-1),b,16)
#define	UINT16_A_GE_B_MOD32BIT(a,b)		(!UINT_A_LT_B_MODnBIT(a,b,16))
#define	UINT16_A_GT_B_MOD32BIT(a,b)		UINT_A_LT_B_MODnBIT(b,a,16)
#define	UINT16_A_EQ_B_MOD32BIT(a,b)		((a)==(b))

#define	UINT32_A_LT_B_MOD32BIT(a,b)		UINT_A_LT_B_MODnBIT(a,b,32)
#define	UINT32_A_LE_B_MOD32BIT(a,b)		UINT_A_LT_B_MODnBIT(((a)-1),b,32)
#define	UINT32_A_GE_B_MOD32BIT(a,b)		(!UINT_A_LT_B_MODnBIT(a,b,32))
#define	UINT32_A_GT_B_MOD32BIT(a,b)		UINT_A_LT_B_MODnBIT(b,a,32)
#define	UINT32_A_EQ_B_MOD32BIT(a,b)		((a)==(b))

#define	UINT64_A_LT_B_MOD64BIT(a,b)		UINT_A_LT_B_MODnBIT(a,b,64)
#define	UINT64_A_LE_B_MOD64BIT(a,b)		UINT_A_LT_B_MODnBIT(((a)-1),b,64)
#define	UINT64_A_GE_B_MOD64BIT(a,b)		(!UINT_A_LT_B_MODnBIT(a,b,64))
#define	UINT64_A_GT_B_MOD64BIT(a,b)		UINT_A_LT_B_MODnBIT(b,a,64)
#define	UINT64_A_EQ_B_MOD64BIT(a,b)		((a)==(b))



#define W16_MSB(x) (((x)>>8)& 0xff)
#define W16_LSB(x) ((x)& 0xff)




#define	POW2_INT(n)		(1ll << (n))
#define	POW2_FLOAT_U(n)	((float)(POW2_INT(n)))

#define IS_POW2(val)    ((((val) & ((val) - 1)) == 0) ? TRUE : FALSE)     // Returns TRUE is val is power of 2

/*
 * Represent number (16.16 bits) as 32 bit
 */

#define	FIX_16DOT16_M				(16-1)
#define	FIX_16DOT16_N  				(16+16)

#define FIX_16DOT16_CONST(x)	    ((int32_t)((x)*(int)POW2(16)))
#define INT_TO_FIX_16DOT16(x)	    ((int32_t)(x) << 16)
#define FIX_16DOT16_TO_INT(x)	    ((x) >> 16)
#define FIX_16DOT16_TO_FRAC(x)		((int32_t)((uint32_t)(x) << 16))
#define FIX_16DOT16_TO_FLOAT(x)		((x) / POW2_FLOAT_U(16))

#define FIX_10DOT5_CONST(x)	        ((int16_t)((x)*(int16_t)POW2(5)))
#define INT16_TO_FIX_10DOT5(x)	    ((int16_t)(x) << 5)
#define FIX_10DOT5_TO_INT(x)	    ((x) >> 5)
#define FIX_10DOT5_TO_FRAC(x)		((int16_t)((uint16_t)(x) << 5))
#define FIX_10DOT5_TO_FLOAT(x)		((x) / POW2_FLOAT_U(5))
#define FIX_10DOT5_TO_16DOT16(x)     ((int32_t)(x) << 11)
#define FIX_16DOT16_TO_10DOT5(x)     ((int16_t)((x) >> 11))

 
#define ROUND_0_5    FIX_16DOT16_CONST(0.5)

// shifts
#define	SHIFT(x,n)		((n) >= 0 ? ((x) << (n)) : ((x) >> (-(n))))


// pow2(x) in the interval x:0..1
#define	POW2_FLOAT_0_1(x)	(1.0 + (1-3.430654244690814e-001)*(x) + (3.430654244690814e-001) *(x)*(x))
#define	POW2_FLOAT_POS(x)	((x) > 1 ? POW2_FLOAT_U(FLOOR(x))*POW2_FLOAT_0_1((x)-FLOOR(x)) : POW2_FLOAT_0_1(x))
#define	POW2_FLOAT(x)		((x) >= 0 ? POW2_FLOAT_POS(x) : 1.0/POW2_FLOAT_POS(-x))
#define	POW10_FLOAT(x)		POW2_FLOAT((x)/LOG10_2)
#define DB20_TO_LIN(x)      POW10_FLOAT((x)/20.0)
#define DB10_TO_LIN(x)      POW10_FLOAT((x)/10.0)

// general fixpoint quantization
#define	LSB_LOG2_FIX_MqN(m,n)					((m)-(n)+1)
#define	LSB_FIX_MqN_2_FLOAT(m,n)				POW2_FLOAT(LSB_LOG2_FIX_MqN(m,n))

#define	M_FIX_M1qN1_X_FIX_M2qN2(m1,n1,m2,n2)	((m1)+(m2)+1)
#define	N_FIX_M1qN1_X_FIX_M2qN2(m1,n1,m2,n2)	((n1)+(n2))

#define	M_INT(n)								((n)-1)
#define	N_INT(n)								(n)

#define FLOOR(x)	((int32_t)(x))
#define RND(x)      (int32_t) ((x)+0.5)

#define	FLOAT_2_FIX_MqN(x,m,n)					((int32_t)((x)/LSB_FIX_MqN_2_FLOAT(m,n)))
#define	FLOAT_2_FIX_MqN_RND(x,m,n)				((int32_t)RND((x)/LSB_FIX_MqN_2_FLOAT(m,n)))

#define	FIX_M1qN1_2_FIX_M2qN2(x,m1,n1,m2,n2)	SHIFT(x,(LSB_LOG2_FIX_MqN(m1,n1) - LSB_LOG2_FIX_MqN(m2,n2)))

#define	FIX_MqN_2_INT(x,m,n)					FIX_M1qN1_2_FIX_M2qN2(x,m,n,n-1,n)
#define	FIX_MqN_2_UFRAC(x,m,n)					FIX_M1qN1_2_FIX_M2qN2(((unsigned)x),m,n,-1,n)

#define ConvToUINT128(Data)						*((UINT128*)(&(Data)))
#define ConvToUINT32(Data)						*((uint32_t*)(&(Data)))


#define	UINT32_IS_A_LT_B(a,b)	((((uint32_t)(a))-((uint32_t)(b))) & MASK_BIT(31))
#define	UINT32_IS_A_GE_B(a,b)	(!UINT32_IS_A_LT_B(a,b))

/* ************************************************************************
 * PROFILER types & configuration data struct
 */
enum {
	PROFILER_CFG_FLAG_ENABLED  = 0x00001,
	PROFILER_CFG_FLAG_STARTED  = 0x00002,
	PROFILER_CFG_FLAG_FINISHED = 0x10000
};


/* Per-node profiler data structure */
typedef struct {
	uint32_t	buf_start;		// Buffer start address
	uint32_t	buf_size;		// Buffer size
	int32_t	cur_offs;		// Current offset (run-time variable, initiated by 0)
	uint32_t	flags;			// Flags bitfields (see PROFILER_CFG_FLAG_...)
	uint32_t	period;			// Profiling interrupt period, in microseconds
} PROFILER_CFG;

extern PROFILER_CFG	ProfilerCfg;


// fast euclid algoroithm
static inline uint32_t calc_gcd(uint32_t a,uint32_t b)
{
	uint32_t temp;

	while(b>0) {
		temp = b;
		b = a % b;		// b=a mod b
		a = temp;
	}
	return(a);
}

static inline uint64_t calc_lcm(uint32_t a,uint32_t b)
{
	uint32_t gcd;

    // calculate GCD of each number
	gcd = calc_gcd(a,b);

    // reduct each number before multiplication
	return (uint64_t)((a / gcd)*(b / gcd))*gcd;
}

static inline uint32_t calc_lcm_div_gcd(uint32_t a,uint32_t b)
{
	uint32_t gcd, lcm_div_gcd;

    // calculate GCD of each number
	gcd = calc_gcd(a,b);

	lcm_div_gcd = (a / gcd)*(b / gcd);

	ASSERT ((a / gcd) <= lcm_div_gcd);
	ASSERT ((b / gcd) <= lcm_div_gcd);

    // reduct each number before multiplication
	return (lcm_div_gcd);
}


// For some memories it is possible only 4-byte access
static inline void memcpy_4aligned(int *to, int *from, int bytecount)
{
	int *end = to + (bytecount / sizeof(int));
	while (to < end)
		*to++ = *from++;
}

static inline uint8_t calc_ceil_log2(uint32_t a)
{
    uint8_t cnt = 0;

    if(a > POW2(31))
        return 32;
    while (a >= POW2(cnt))
    {
        cnt++;
    }
    return cnt;
}





/*************************************************************************/



/*******************************************************************************************\
								WIreShark Logging support 
\*******************************************************************************************/


#ifdef WS_LOG_DS_CLT

// #define WS_LOG_TEST  // VYS - Open for WS Logging test example

#define WS_LOG_FRM_0(str) 									  dsWsLogFrm((str), 0,     0,      0,      0,      0,      0,      0)
#define WS_LOG_FRM_1(str, arg0) 							  dsWsLogFrm((str), 1,(uint32_t)(arg0),      0,      0,      0,      0,      0)
#define WS_LOG_FRM_2(str, arg0, arg1) 						  dsWsLogFrm((str), 2,(uint32_t)(arg0), (uint32_t)(arg1),      0,      0,      0,      0)
#define WS_LOG_FRM_3(str, arg0, arg1, arg2) 				  dsWsLogFrm((str), 3,(uint32_t)(arg0), (uint32_t)(arg1), (uint32_t)(arg2),      0,      0,      0)
#define WS_LOG_FRM_4(str, arg0, arg1, arg2, arg3) 			  dsWsLogFrm((str), 4,(uint32_t)(arg0), (uint32_t)(arg1), (uint32_t)(arg2), (uint32_t)(arg3),      0,      0)
#define WS_LOG_FRM_5(str, arg0, arg1, arg2, arg3, arg4) 	  dsWsLogFrm((str), 5,(uint32_t)(arg0), (uint32_t)(arg1), (uint32_t)(arg2), (uint32_t)(arg3), (uint32_t)(arg4),      0)
#define WS_LOG_FRM_6(str, arg0, arg1, arg2, arg3, arg4, arg5) dsWsLogFrm((str), 6,(uint32_t)(arg0), (uint32_t)(arg1), (uint32_t)(arg2), (uint32_t)(arg3), (uint32_t)(arg4), (uint32_t)(arg5))



void  dsWsLogFrm (cchar* sformat, uint8_t argn, uint32_t arg0, uint32_t arg1,uint32_t arg2,uint32_t arg3, uint32_t arg4,uint32_t arg5);

#else

#define WS_LOG_FRM_0(str) 									
#define WS_LOG_FRM_1(str, arg0) 								
#define WS_LOG_FRM_2(str, arg0, arg1) 						
#define WS_LOG_FRM_3(str, arg0, arg1, arg2) 					
#define WS_LOG_FRM_4(str, arg0, arg1, arg2, arg3) 			
#define WS_LOG_FRM_5(str, arg0, arg1, arg2, arg3, arg4) 		
#define WS_LOG_FRM_6(str, arg0, arg1, arg2, arg3, arg4, arg5) 

#endif

#ifdef __cplusplus
}
#endif


#endif

/*
 * -----------------------------------------------------------
 * End of file
 * -----------------------------------------------------------
 */
