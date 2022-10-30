/******************************************************************
 * i_cpp_sys_api.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef I_COMP_API_H_
#define I_COMP_API_H_
//#include <asm/cachectl.h>
//#include <asm/unistd.h>



//	Use this macro in order to ignore compiler alignment of data structures
//	that are not aligned:
#define PACK_STRUCT __attribute__((packed))

#include "i_sys_utils.h"
#include "i_osa_api.h"
#include "rt_debug_api.h"
//API definitions for 5G simulator modules
#define MDO_NAME_SIZE    (80)
namespace RT_DEBUG
{
	enum EResultT
	{
		E_OK,
		E_FAIL
	};

	struct TraceDataT
	{
		uint32_t trace_id;
		uint32_t val0, val1, val2, val3;
	};
	enum EInterfaceT
	{
		E_CONTROL_API,
		E_DEBUG_API
	};

	enum ECallReasonT
	{
		E_WAKEUP,
		E_DEBUG,
		E_DIRECT,
		E_RESPONSE,
		E_MAX_CALL_REASON
	};

	enum EConfigId
	{
	    e_GENERAL,
	    e_GET_API,
		e_GET_TEST_MODULE_API,
	    e_TEST_CONFIG,
	    e_START_TCP_SRV,
	    e_TCP_CONNECT,
	    e_PUSCH_SLOT_API,
	    e_PDSCH_SLOT_API,
		e_PUT_INIT_DATA
	};

	enum EMemAllocBit
	{
		e_DEFINE_WITHOUT_ALLOCATE_BIT_NUM,
		e_SHR_CONSUMER,
		e_SHR_PRODUCER,
		e_NOT_IN_USE_3,
		e_NOT_IN_USE_4,
		e_NOT_IN_USE_5,
		e_NOT_IN_USE_6,
		e_NOT_IN_USE_7,
		e_NOT_IN_USE_8,
		e_NOT_IN_USE_9,
		e_NOT_IN_USE_10,
		e_NOT_IN_USE_11,
		e_NOT_IN_USE_12,
		e_NOT_IN_USE_13,
		e_NOT_IN_USE_14,
		e_NOT_IN_USE_15,
		e_NOT_IN_USE_16,
		e_NOT_IN_USE_17,
		e_NOT_IN_USE_18,
		e_NOT_IN_USE_19,
		e_NOT_IN_USE_20,
		e_NOT_IN_USE_21,
		e_NOT_IN_USE_22,
		e_NOT_IN_USE_23,
		e_NOT_IN_USE_24,
		e_NOT_IN_USE_25,
		e_NOT_IN_USE_26,
		e_NOT_IN_USE_27,
		e_NOT_IN_USE_28,
		e_NOT_IN_USE_29,
		e_NOT_IN_USE_30,
		e_NOT_IN_USE_31
	};



	//Profiler counter data
	struct ProfilerCntD : public ProfileData
	{

		void Reset()
		{
			max_cnt_ = last_cnt_ = average_cnt_ = max_cnt_time_ = 0;
			meas_num_ = 0;
		}
	};


#if		(ARCH == INTEL)

	#define FLUSH_POOL_CHUNK(chunk_start_ptr, chunk_end_ptr)		//nothing to do on Intel CPU
	#define	INVALIDATE_POOL_CHUNK(chunk_start_ptr, chunk_end_ptr)	//nothing to do on Intel CPU

#elif 	(ARCH == ARM)

	#define FLUSH_POOL_CHUNK(chunk_start_ptr, chunk_end_ptr) 	__builtin___clear_cache(chunk_start_ptr, chunk_end_ptr)
	#define	INVALIDATE_POOL_CHUNK(chunk_start_ptr, chunk_end_ptr)	//should find-out how to perform that on ARM processor
#else

	#error	No valid architecture detected !!!

#endif

	class ITarget;
	class IGL_ControlAPI;
	class IModuleControlAPI;
	class CProfileCnt;

	/*******************************************************************************************//*!
	*@class CMemArea
	*@brief The purpose of this class is :
	*@brief Memory management. The class provides the following services:
	*@brief Explicit memory allocation by Framework.
	*@brief Inter-module thread safe data transfer.
	*@brief FIFO support
	*@brief Dual Buffer support
	*@brief Dynamic memory allocation of memory chunks with the same size.
	***********************************************************************************************/
	class CMemArea
	{
	protected:
	  void *allocated_ptr_; // for saving the original allocated pointer (in case of release required)
	  void *ptr_producer_;
	  void *ptr_consumer_;
	  uint32_t number_chunks_;
	  uint32_t single_chunk_size_;
	  volatile __align(CACHE_ALIGNMENT) uint32_t wr_ind_; // make write pointer be aligned to the cache-line size of the processor
	  volatile __align(CACHE_ALIGNMENT) uint32_t rd_ind_; // make read pointer be aligned to the cache-line size of the processor
	  __align(CACHE_ALIGNMENT) EAccessT access_;
	  uint32_t attrib_;
	  OSA_spinlock_t in_lock_;
	  OSA_spinlock_t out_lock_;
	public:
	  char	name_[MDO_NAME_SIZE];
	  CMemArea()
	  {
		  Init();
	  }
	  void Setup(uint32_t size, uint32_t inc_size, char *name, EAccessT access= E_READ, uint32_t attrtib=0 )
	  {
		  rd_ind_ = 0;
		  wr_ind_ = 1;
		  number_chunks_= size;
		  single_chunk_size_= ALIGN_SIZE(inc_size, CACHE_ALIGNMENT);
		  access_ = access;
		  attrib_ = attrtib;
		  strcpy(name_, name);
		  OSA_spinlock_create(&in_lock_);
		  OSA_spinlock_create(&out_lock_);
	  }
	  void MemAlloc(void *mem_buf = NULL)
	  {
		  uint64_t pointer_int;
		  uint32_t total_size= single_chunk_size_*number_chunks_;
		  total_size = ADD_SIZE_FOR_MEM_ALIGN(total_size, CACHE_ALIGNMENT);
		  ASSERT(total_size >0);

		  if(mem_buf== NULL)
		  {
			  if(!(attrib_>>e_DEFINE_WITHOUT_ALLOCATE_BIT_NUM)&0x1)
			  {
				  allocated_ptr_= new char [total_size];
				  ASSERT(allocated_ptr_!=NULL);
				  pointer_int = ALIGN_POINTER((uint64_t)allocated_ptr_, CACHE_ALIGNMENT); // align the memory pointer to the Cache line size
				  ptr_producer_ = ptr_consumer_= (void*)pointer_int;
			  }
		  }
		  else
		  {
			  bool consumer = (attrib_>> e_SHR_CONSUMER) && 1;
			  bool producer = (attrib_>> e_SHR_PRODUCER) && 1;

			  ASSERT(ALIGN_64BIT((uint64_t)mem_buf)==(uint64_t)mem_buf); //Check that buffer address is aligned to 64 bytes (cache line size)

			  if((producer== false) && (consumer== false))
			  {
				  //No care producer/consumer means both
				  producer= true;
				  consumer= true;
			  }

			  if(producer)
			  {
				  ptr_producer_ = mem_buf;
			  }
			  if(consumer)
			  {
				  ptr_consumer_= mem_buf;
			  }

		  }

	  }
	  uint32_t GetAttrib()
	  {
		  return attrib_;
	  }
	  char* GetName()
	  {
		  return name_;
	  }
	  void SetAccess(EAccessT access)
	  {
		  access_ = access;
	  }
	  EAccessT GetAccess()
	  {
		  return access_;
	  }
	  void Init()
	  {
		  ptr_producer_= NULL;
		  ptr_consumer_= NULL;
	  }

	  uint32_t GetDataSize()
	  {
		 return single_chunk_size_;
	  }


	  uint32_t GetTotalDataSize()
	  {
		 return single_chunk_size_*number_chunks_;
	  }

	  // Methods below support access to the memory area as double buffer (used for situation when one thread update data and many other threads read updated data)
	  //***********************************************************************************************************************************************************
	  void *GetReadBufP()
	  {

		  return (void*)((char*)ptr_consumer_ + single_chunk_size_ * rd_ind_);
	  }

	  void *GetWriteBufP()
	  {
		 return (void*)((char*)ptr_producer_ + single_chunk_size_ * wr_ind_);;
	  }

	  void DualBufSwap()
	  {
		  PushFIFO();
		  PopFIFO();
	  }
	  //*******************************************************************************************************

	  // Methods below allow access to the memory area as FIFO (one direction data exchange between two threads)
	  //********************************************************************************************************
	  void *AllocFIFO()
	  {
		  return GetWriteBufP();
	  }
	  void *PushFIFO()
	  {
		  uint32_t tmp = (wr_ind_+1)%number_chunks_;
		  if(tmp == rd_ind_)
		  {
			  return NULL;
		  }

		  wr_ind_ = tmp;
		  void* chunk_start_ptr = AllocFIFO();
		  uint64_t chunk_end = (uint64_t)chunk_start_ptr+single_chunk_size_-1;
		  void* chunk_end_ptr = (void*)chunk_end;
		  //flush the cache lines of the new pushed chunk
		  FLUSH_POOL_CHUNK(chunk_start_ptr, chunk_end_ptr);
		  return chunk_start_ptr;
	  }
	  void *PopFIFO()
	  {
		  uint32_t tmp = (rd_ind_+1)%number_chunks_;
		  if(tmp == wr_ind_)
		  {
			  return NULL;
		  }
		  rd_ind_ = tmp;

		  void* chunk_start_ptr = GetReadBufP();
		  uint64_t chunk_end_ptr = (uint64_t)chunk_start_ptr+single_chunk_size_-1;
		  //void* chunk_end_ptr = (void*)chunk_end;
		  //invalidate the cache lines of the new chunk poped
		  INVALIDATE_POOL_CHUNK(chunk_start_ptr, chunk_end_ptr);
		  return chunk_start_ptr;

	  }
	  //**********************************************************************************************************
	  //The methods below allow use the memory area as dynamic heap
	  void *AllocPoolChunk()
	  {
		  void *ptr;
	      //spinlock_push check/allocate
	      OSA_spinlock_lock(&out_lock_);

		  wr_ind_ = (wr_ind_+1)%number_chunks_;
		  ptr = AllocFIFO();
		  OSA_spinlock_unlock(&out_lock_);
		  return ptr;
	  }

	  void ReleaseChunk(void *ptr)
	  {

	  }

	  void *GetPrevChunk(uint32_t prev_idx)
	  {
	      uint32_t prev_abs_indx;
		  bool consumer = (attrib_>> e_SHR_CONSUMER) && 1;
		  bool producer = (attrib_>> e_SHR_PRODUCER) && 1;
		  char *ptr= NULL;
		  if((producer== false) && (consumer== false))
		  {
			  //No care producer/consumer means both
			  producer= true;
			  consumer= true;
		  }

		  if(producer)
		  {
			  ptr= (char*)ptr_producer_ ;
		  }
		  else
		  {
			  ptr =(char*)ptr_consumer_;
		  }

	      ASSERT(prev_idx < number_chunks_);
	      prev_abs_indx = (number_chunks_ + wr_ind_ - prev_idx) % single_chunk_size_;
	      return (void*)(ptr + single_chunk_size_ * prev_abs_indx);;
	  }
	  //************************************************************************************************************
	  //   Methods below is used for multi-threaded FIFO, where number threads may run simultaneously on producer and consumer side.
	  //************************************************************************************************************
	  EResultT PushFIFO_MT(void *data_p, uint32_t data_size)
      {
	      //spinlock_push check/allocate
	      OSA_spinlock_lock(&in_lock_);
	      EResultT res = E_OK;
          uint32_t tmp = (wr_ind_+1)%number_chunks_;
          if(tmp == rd_ind_)
          {
              res = E_FAIL;
          }
          else
          {
              void *dest = GetWriteBufP();
              ASSERT(single_chunk_size_ >= data_size);
              memcpy(dest, data_p, data_size);
              wr_ind_ = tmp;
          }
          //Release spinlock_push
          OSA_spinlock_unlock(&in_lock_);
          return res;
      }
      void *PopFIFO_MT(void *data_p, uint32_t data_size)
      {
          // Add spinlock_pop check/allocate
          OSA_spinlock_lock(&out_lock_);
          uint32_t tmp = (rd_ind_+1)%number_chunks_;
          void *res = NULL;
          if(tmp != wr_ind_)
          {
        	  rd_ind_ = tmp;
              res = GetReadBufP();
              ASSERT(single_chunk_size_ >= data_size);
              memcpy(data_p, res, data_size);
          }
          //Release spinlock_pop
          OSA_spinlock_unlock(&out_lock_);
          return res;
      }


	} ;

	/*******************************************************************************************//*!
	*@class StaticMemArea template
	*@brief The purpose of this class is :
	*@brief Memory management. The class provides the following services:
	*@brief Static memory allocation by Framework.
	*@brief Inter-module thread safe data transfer.
	*@brief FIFO support
	*@brief Dual Buffer support
	*@brief Dynamic memory allocation of memory chunks with the same size.
	***********************************************************************************************///
	template<class T, int32_t CHUNKS_NUM>class StaticMemArea : public CMemArea
	{
		alignas(CACHE_ALIGNMENT) T buf_[CHUNKS_NUM];
	public:
		  void *GetReadBufP()
		  {

			  return (void*)((char*)buf_ + single_chunk_size_ * rd_ind_);
		  }

		  void *GetWriteBufP()
		  {
			 return (void*)((char*)buf_ + single_chunk_size_ * wr_ind_);;
		  }
		  void Setup(uint32_t size, uint32_t inc_size, char *name, EAccessT access= E_READ, uint32_t attrib=0 )
		  {
			  CMemArea::Setup(CHUNKS_NUM, sizeof(T), name, access, attrib);
		  }
		  void MemAlloc(void *mem_buf = NULL)
		  {
			  CMemArea::MemAlloc((void*)buf_);
		  }

		  T* GetEntry(uint32_t num)
		  {
			  ASSERT(num<CHUNKS_NUM);
			  return &buf_[num];
		  }
		  EResultT PushFIFO_MT(void *data_p, uint32_t data_size)
	      {
		      //spinlock_push check/allocate
		      OSA_spinlock_lock(&in_lock_);
		      EResultT res = E_OK;
	          uint32_t tmp = (wr_ind_+1)%number_chunks_;
	          if(tmp == rd_ind_)
	          {
	              res = E_FAIL;
	          }
	          else
	          {
	              void *dest = GetWriteBufP();
	              ASSERT(single_chunk_size_ >= data_size);
	              memcpy(dest, data_p, data_size);
	              wr_ind_ = tmp;
	          }
	          //Release spinlock_push
	          OSA_spinlock_unlock(&in_lock_);
	          return res;
	      }
	      void *PopFIFO_MT(void *data_p, uint32_t data_size)
	      {
	          // Add spinlock_pop check/allocate
	          OSA_spinlock_lock(&out_lock_);
	          uint32_t tmp = (rd_ind_+1)%number_chunks_;
	          void *res = NULL;
	          if(tmp != wr_ind_)
	          {
	        	  rd_ind_ = tmp;
	              res = GetReadBufP();
	              ASSERT(single_chunk_size_ >= data_size);
	              memcpy(data_p, res, data_size);
	          }
	          //Release spinlock_pop
	          OSA_spinlock_unlock(&out_lock_);
	          return res;
	      }
		  void *AllocPoolChunk()
		  {
			  void *ptr;
		      //spinlock_push check/allocate
		      OSA_spinlock_lock(&out_lock_);

			  wr_ind_ = (wr_ind_+1)%number_chunks_;
			  ptr = GetWriteBufP();
			  OSA_spinlock_unlock(&out_lock_);
			  return ptr;
		  }


	};
	typedef CMemArea* CMemAreaP;
	struct CModuleInfo
	{
		IModuleControlAPI*  module_p_;
		char				module_name_[MDO_NAME_SIZE];
		void Init()
		{
			module_name_[0]= 0;
			module_p_ = NULL;
		}
	};


	typedef CMemArea* CMemAreaP;

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

	class alignas(CACHE_ALIGNMENT) CProfileCnt
	{

		ProfilerCntD prof_cnt_;
		uint64_t acc_cnt_; //Support multiple start stop during one measurement
		uint32_t cnt_id_;
		uint32_t max_calls_;
		uint64_t start_val_;
		uint64_t delta_;
		uint32_t	prof_id = -1;
		CProfileCnt  *next_prof_p;
		EProfileEval_type 	evalType_;
		CMemArea *mem_ptr_ = NULL;


	public:
	CProfileCnt()
	{
		cnt_id_ =0;
		prof_cnt_.Reset();
		max_calls_= 500;
		acc_cnt_ = 0;
		start_val_ = 0;
		delta_ = 0;
		next_prof_p = NULL;
		evalType_ = TIME_EVAL;//TIME_EVAL;//CYCLE_EVAL;
	}
	CProfileCnt* GetNextProf()
	{
		return next_prof_p;
	}
	void ConnectProf(CProfileCnt *prof_addr)
	{
		next_prof_p = prof_addr;
	}

	void SetMaxCalls(uint32_t val)
	{
		max_calls_= val;
	}
	void Init(char *name, CMemArea *mem_ptr)
	{
		struct timespec t1, t0;
		mem_ptr_ = mem_ptr;
		prof_cnt_.Reset();
		Reset();
		if(evalType_ == TIME_EVAL)
		{
			int8_t number_delta_calls_iterations = 10;
			delta_ = 0;
			for(int8_t i=0; i<number_delta_calls_iterations; i++)
			{
				clock_gettime(CLOCK_MONOTONIC, &t0);
				clock_gettime(CLOCK_MONOTONIC, &t1);
				delta_ += (((uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec) - ((uint64_t)t0.tv_sec * BILLION + (uint64_t)t0.tv_nsec));
			}
			delta_ /= number_delta_calls_iterations; //average amount of clocks for calling gettime

		}
		else
		{
			// call twice in order to ensure that the tsc code is located in cash and its running time is more correct
			delta_ 		= tsc();
			start_val_	= tsc();
			delta_ 		= tsc();
			start_val_	= tsc();
			delta_ = start_val_ - delta_;
			start_val_ = 0;

		}

	}
	void Update()
	{
		ProfilerCntD tmp;
		if(prof_cnt_.meas_num_ == 0)
			return;
		prof_cnt_.average_cnt_ = prof_cnt_.average_cnt_/prof_cnt_.meas_num_;

		while(mem_ptr_->PushFIFO_MT(&prof_cnt_, sizeof(prof_cnt_))==E_FAIL )
		{
			mem_ptr_->PopFIFO_MT(&prof_cnt_, sizeof(prof_cnt_));
		}

		prof_cnt_.Reset();
	}
	void Start()
	{
		ASSERT(start_val_== 0);
		//__sync_synchronize();
		struct timespec t1, t0;
		prof_cnt_.last_cnt_ =1; //Informs that the Start function was called

		if(evalType_ == TIME_EVAL)
		{
			//clock_gettime(CLOCK_MONOTONIC, &t0);
			clock_gettime(CLOCK_MONOTONIC, &t1);
			//delta_ = (uint64_t)t0.tv_sec * BILLION + (uint64_t)t0.tv_nsec;
			start_val_ = (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec;
		}
		else
		{
			// call twice in order to ensure that the tsc code is located in cash and its running time is more correct
			start_val_	= tsc();
		}
		//__sync_synchronize();
	}

#if defined(__x86_64__) || defined(__amd64__)
inline	uint64_t tsc(void)
	{
		register uint32_t lo, hi;
		asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));
		return ((uint64_t)hi << 32UL) | (uint32_t)lo;
	}
#elif defined(__aarch64__)
inline	uint64_t tsc(void)
	{

 /*
// this code should work on our ARM processor since it's ver8 architecture, but it does not recognize "mrc" assembly command for some reason
#if (__ARM_ARCH >= 6)
		uint32_t pmccntr;
		uint32_t pmuseren;
		uint32_t pmcntenset;
		  // Read the user mode perf monitor counter access permissions.
		  asm volatile("mrc p15, 0, %0, c9, c14, 0" : "=r"(pmuseren));
		  if (pmuseren & 1) {  // Allows reading perfmon counters for user mode code.
			asm volatile("mrc p15, 0, %0, c9, c12, 1" : "=r"(pmcntenset));
			if (pmcntenset & 0x80000000ul) {  // Is it counting?
			  asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(pmccntr));
			  // The counter is set up to count every 64th cycle
			  return static_cast<uint64_t>(pmccntr) * 64;  // Should optimize to << 6
			}
		  }
#endif
 */

		  // System timer of ARMv8 runs at a different frequency than the CPU's.
		  // The frequency is fixed, typically in the range 1-50MHz.  It can be
		  // read at CNTFRQ special register.  We assume the OS has set up
		  // the virtual timer properly.

		int64_t virtual_timer_value;
		asm volatile("mrs %0, cntvct_el0" : "=r"(virtual_timer_value));
		return (uint32_t)virtual_timer_value;
	}
#endif


	void StopContinue()
	{
		struct timespec t1;
		uint64_t t64;
		if(evalType_ == TIME_EVAL)
		{
			clock_gettime(CLOCK_MONOTONIC, &t1);
			t64= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec;
		}
		else
		{
			t64 = tsc();
		}
		acc_cnt_ += t64 - start_val_;
		start_val_ = 0;

	}
	void ForceStop()
	{
		Update();
		Reset();
	}
	void Stop(uint64_t *prof_valp = NULL)
	{
		//__sync_synchronize();
		struct timespec t1;
		volatile uint64_t t64;
		if(prof_valp==NULL)
		{
			//Calculate time profiling between calling start and stop functions
			if(evalType_ == TIME_EVAL)
			{
				clock_gettime(CLOCK_MONOTONIC, &t1);
				t64= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec;
			}
			else
			{
				t64 = tsc();
			}

			int64_t tmp_val = t64 - start_val_ + acc_cnt_ - delta_;// check if calling to both, the tested code and gettime function, took less time than delta_ time (it can happen in case of caching issues)
			if(tmp_val < 0)
			{
				prof_cnt_.last_cnt_ = 20; //add artificial small number of nsecs (20nsec is about 30 cycles for 1.6 Ghz CPU clock)
			}
			else
			{
				prof_cnt_.last_cnt_= tmp_val;
			}
		}
		else
		{
			//Calculate profiling of any values
			prof_cnt_.last_cnt_= *prof_valp;
		}
		//prof_cnt_.last_cnt_=  t64 - start_val_ + acc_cnt_ - delta_;
		//prof_cnt_.last_cnt_=  t64 - start_val_ + acc_cnt_; // force delta_ = 0 since there is sometime values of the counter less than delta and then we get negative measurement
		//ASSERT(delta_ < 10000L)
		//ASSERT((t64 - start_val_) < 100000000000L)
		//ASSERT(acc_cnt_ == 0)
		//ASSERT(prof_cnt_.last_cnt_ < 100000000000L)
#ifdef yafit
		if(prof_cnt_.last_cnt_ > 100000000000L)
		{
			printf("t64=%llu, start_val_=%llu, acc_cnt_=%llu, delta_=%llu\n", t64, start_val_, acc_cnt_, delta_);
		}
#endif
		acc_cnt_ = 0;
		if(prof_cnt_.last_cnt_ > prof_cnt_.max_cnt_)
		{
			prof_cnt_.max_cnt_ = prof_cnt_.last_cnt_;
			prof_cnt_.max_cnt_time_ = start_val_;
		}
		prof_cnt_.average_cnt_ += prof_cnt_.last_cnt_;
		start_val_ = 0;
		prof_cnt_.meas_num_++;
		if((max_calls_ !=0) &&(  max_calls_ <= prof_cnt_.meas_num_))
		{
			Update();
		}
		//__sync_synchronize();
	}


	void Reset()
	{
		prof_cnt_.Reset();
		start_val_ = 0;
		acc_cnt_ = 0;
	}

	};
}; //Namespace definition
//API function to connect with ITarget API
typedef uint32_t (*IGetConnectAPI_t)(void **target_ptr);
extern "C" uint32_t IGetConnectAPI(void **target_ptr);
#endif /* I_COMP_API_H_ */
