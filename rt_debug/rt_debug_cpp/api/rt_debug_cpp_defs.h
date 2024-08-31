/******************************************************************
 * i_sys_utils.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/


#ifndef _RT_DEBUG_CPP_DEFS__
#define _RT_DEBUG_CPP_DEFS__

#ifdef __cplusplus
extern "C" {
#endif
#include "rt_debug_defs.h"
#include "rt_debug_api.h"
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

/******************************************************************************************//*!
 *@class CProfileCnt
 *@brief The purpose of this class is :
 *@brief This class implements the profiler counter.
 *********************************************************************************************/
	enum EProfileEval_type
	{
		TIME_EVAL,
		CYCLE_EVAL
	};



	class CProfilePoint: public ProfilePoint
	{
		uint32_t	group_id_;

	public:
	CProfilePoint()
	{
		cnt_id_ =0;
		max_calls_= 500;
		acc_cnt_ = 0;
		prof_id_ = -1;
		start_val_ = 0;
		delta_ = 0;
		evalType_ = TIME_EVAL;//TIME_EVAL;//CYCLE_EVAL;
		group_id_= -1;
	}

	void SetMaxCalls(uint32_t val)
	{
		max_calls_= val;
	}
	void Init(const char *name, const char *group_name)
	{
		ProfileData* read_p;
		group_id_= RTDBG_AllocRTDebugGroup(group_name);
		prof_id_= RTDBG_AddProfiler(group_id_, name);
	    struct timespec t1, t0;
		prof_data_.Reset();
		Reset();
		RTDBG_ProfInit(group_id_, prof_id_, static_cast<ProfilePoint*>(this), max_calls_);
	}
	void Update()
	{
		RTDBG_ProfFlushMeas(group_id_, prof_id_, static_cast<ProfilePoint*>(this));
	}
	void Start()
	{
		RTDBG_StartProfMeas(group_id_, prof_id_, static_cast<ProfilePoint*>(this));
	}


	void StopContinue()
	{
		RTDBG_StopStartProfMeas(group_id_, prof_id_, static_cast<ProfilePoint*>(this));
	}
	void ForceStop()
	{
	    Update();
	    Reset();
	}
	void Stop(uint64_t *prof_valp = NULL)
	{
		//__sync_synchronize();
		RTDBG_StopProfMeas(group_id_, prof_id_, static_cast<ProfilePoint*>(this));

	}


	void Reset()
	{
	    prof_data_.Reset();
		start_val_ = 0;
		acc_cnt_ = 0;
	}

	};
} //namespace

#endif

/*
 * -----------------------------------------------------------
 * End of file
 * -----------------------------------------------------------
 */
