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

#include "rt_debug_api.h"

//	Use this macro in order to ignore compiler alignment of data structures
//	that are not aligned:
#define PACK_STRUCT __attribute__((packed))


#include "i_osa_api.h"
//API definitions for 5G simulator modules
#define MDO_NAME_SIZE    (80)
namespace ns_5g_phy
{
	enum EResultT
	{
		E_OK,
		E_FAIL
	};
	struct SysTimeT : public GenSysTime
	{
	private:
		uint32_t nsec_correction;
	public:

		//Add time offset in microseconds to the current system time.
		void SysTimeUpdate(int32_t usec_diff, uint32_t nsec_remainder= 0)
		{
			nsec_correction+= nsec_remainder;

			int32_t offset_tmp = usec_diff  + offset + (nsec_correction/1000);
			nsec_correction= nsec_correction%1000;
			int32_t borrow_nsf = offset_tmp <0 ? -1: 0;
			int32_t nsf_tmp = (offset_tmp / SUBFRAME_USECS + nsf + borrow_nsf);
			int32_t borrow_nf = nsf_tmp<0 ? -1: 0;

			nf = ( (nsf_tmp/NUM_OF_SF_IN_FRAME  + nf + borrow_nf) % (SYS_FRAMES_NUM+1) + (SYS_FRAMES_NUM+1)) & SYS_FRAMES_NUM;


			offset = (SUBFRAME_USECS + offset_tmp%SUBFRAME_USECS ) % SUBFRAME_USECS;
			nsf = (nsf_tmp % NUM_OF_SF_IN_FRAME + NUM_OF_SF_IN_FRAME) % NUM_OF_SF_IN_FRAME;
		}
		void SetNsecCorrection(uint32_t nsec_correction_init)
		{
			 nsec_correction = nsec_correction_init;
		}

		uint32_t GetNsecCorrection()
		{
			return nsec_correction ;
		}

		uint32_t GetSlotNum()
		{
			return nsf*NUM_SLOTS_SF + offset/SLOT_DURATION;
		}
		uint32_t GetSlotOffset()
		{
			return offset%SLOT_DURATION;
		}
		void reset()
		{
			nf = nsf = offset = 0;
			nsec_correction = 0;
		}
		SysTimeT()
		{
			reset();
		}
		SysTimeT(GenSysTime &sys_time)
		{
			 nf= sys_time.nf;
			 nsf= sys_time.nsf;
			 offset= sys_time.offset;
			 nsec_correction = 0;
		}
	};
	enum EAccessT
	{
		E_READ,
		E_WRITE
	};
	enum ESeverityT
	{
		E_CRITICAL,
		E_HIGH,
		E_MEDIUM,
		E_LOW,
		E_BP,
		E_INFO
	};
	enum ECallMethodT
	{
		E_PERIODIC,
		E_ONCE
	};

	enum EDebugOperationT
	{
		E_VERSION,
		E_PROFILER,
		E_MAX_OP
	};

	/******************************************************************************************//*!
	 *@class TimerEvent_api
	 *@brief The purpose of this class is to callback after the timer event:
	 *@brief
	 *********************************************************************************************/
	struct TimerEvent_api {
		//Run the data engine function
		/******************************************************************************************//*!
		 *@function: TimerEvCallback
		 *@brief The purpose of this function is callback after the timer event:
		 *@brief Parameters:
		 *@brief SysTimeT *sys_time_p  -- Current time
		 *@brief int32_t param         -- Special parameter to indicate the event
		 *@brief int32_t seq_val       -- Additional parameter, which indicates additional sequential parameter. It may be used to set information
		 *@brief 						  about destination symbol number.
		 *@brief SysTimeT *dst_time	   -- Destinations system time. It indicates the system time of the event, which is called in advance.
		 *@brief
		 *********************************************************************************************/

		virtual void TimerEvCallback(SysTimeT *sys_time_p, int32_t param, int32_t seq_val, SysTimeT *dst_time =NULL)=	0;
	};
	enum E_EVENT_PERIOD
	{
		E_CUSTOM,
		E_EVERY_SLOT
	};
	struct TimerEventSchedulerT
	{
		 E_EVENT_PERIOD event_periodicy;
		 uint32_t		slot_offset; //Offset in slots. This offset is aligned with the slot start time. For example: 0 current slot, 1 start time of the next slot. Range: 2*(1<< Numerology)
		 int32_t        usec_offset; //Offset in usecs from the slot start time
		 uint32_t       *event_offsets; // Pointer to the table of offsets from the stall start time. The actual offset is calculated as sum of usec_offset with every event_offsets elements.
		 	 	 	 	 	 	 	 	// If it is NULL then only usec_offset value is used.
		 uint32_t		num_events;     // Number timer events in the event_offset array.
		 uint32_t       send_val;		// Value to send with timer event call.
		 uint32_t		*send_vals;
		 bool           is_permanent;   // Set if the timer event is permanent or not
		 TimerEvent_api* callback_timer_api;

		/******************************************************************************************//*!
		 *@function  SetPermanentTimerEvent
		 *@brief The purpose of this function is to configure the permanent timer events
		 *@brief Parameters:
		 *@brief int32_t u_offset   			-- Time advance offset (usecs). If the event should be called in advance, then this parameter should be negative.
		 *@brief const uint32_t  *ev_offsets    -- Pointer to list of events, which can be called per slot. If it is only one event per slot, then it may be set to NULL
		 *@brief 								   and u_offset parameter can be used to set the event time.
		 *@brief uint32_t	n_events			-- Number of events per slot.
		 *@brief uint64_t   gen_val				-- Value, which used to identify the timer event
		 *@brief TimerEvent_api* callback_api   -- Pointer to the class, which implements callback of timer event driver
		 *@brief const uint32_t *vals			-- Pointer to additional sequential values, used per sequential event. For example they may contain information about the symbol number.
		 *@brief
		 *********************************************************************************************/
		void SetPermanentTimerEvent(int32_t u_offset, const uint32_t  *ev_offsets, uint32_t	n_events,
									uint64_t gen_val, TimerEvent_api* callback_api, const uint32_t *vals=NULL)
		{
			ASSERT(callback_api);
			is_permanent= true;
			event_periodicy = E_EVERY_SLOT;
			slot_offset =0;
			usec_offset = u_offset;
			event_offsets = (uint32_t*)ev_offsets;
			num_events= n_events;
			send_val = gen_val;
			callback_timer_api = callback_api;
			send_vals = (uint32_t*)vals;
		}

		 //Temporary timer event is repeated every slot. It is possible to set single offset or vector of offsets.
		/******************************************************************************************//*!
		 *@function  SetPermanentTimerEvent
		 *@brief The purpose of this function is to configure the temporary timer events
		 *@brief Parameters:
		 *@brief uint32_t slot_ofst             -- Offset in slots for the next timer event
		 *@brief int32_t u_offset   			-- Time advance offset (usecs). If the event should be called in advance, then this parameter should be negative.
		 *@brief const uint32_t  *ev_offsets    -- Pointer to list of events, which can be called per slot. If it is only one event per slot, then it may be set to NULL
		 *@brief 								   and u_offset parameter can be used to set the event time.
		 *@brief uint32_t	n_events			-- Number of events per slot.
		 *@brief uint64_t   gen_val				-- Value, which used to identify the timer event
		 *@brief TimerEvent_api* callback_api   -- Pointer to the class, which implements callback of timer event driver
		 *@brief const uint32_t *vals			-- Pointer to additional sequential values, used per sequential event. For example they may contain information about the symbol number.
		 *@brief
		 *********************************************************************************************/
		void SetTempTimerEvent(uint32_t slot_ofst, const int32_t u_offset, uint32_t  *ev_offsets, uint32_t	n_events,
									uint64_t val, TimerEvent_api* callback_api, const uint32_t *vals=NULL)
		{
			ASSERT(callback_api);
			is_permanent= false;
			event_periodicy = E_CUSTOM;
			slot_offset =slot_ofst;
			usec_offset = u_offset;
			event_offsets = ev_offsets;
			num_events= n_events;
			send_val = val;
			callback_timer_api = callback_api;
			send_vals = (uint32_t*)vals;
		}

		void Reset()
		{
			is_permanent = false;
			usec_offset = slot_offset= send_val = 0;
		    num_events = 0;
		    event_offsets = NULL;
		    callback_timer_api = NULL;
		    event_periodicy = E_CUSTOM;
		    send_vals = NULL;
		}

		TimerEventSchedulerT()
		{
		    Reset();
		}
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
		__align(CACHE_ALIGNMENT) T buf_[CHUNKS_NUM];
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


	//Module APIs
	/*******************************************************************************************//*!
	*@class IModuleControlCallBackAPI
	*@brief The purpose of this class is :
	*@brief API for asynchronous services, provided by the framework to the 5G moduler
	***********************************************************************************************/
	class IModuleControlCallBackAPI
	{
	public:

		virtual EResultT IMemAreaDefine(CMemAreaP *mearea_ptr_) = 0;
		virtual EResultT IRegistryTraceEntry(char *format_str, uint32_t *id) = 0;
		virtual EResultT IRegistryProfileEntry(CProfileCnt *ptr, char *name, uint32_t *prof_id) = 0;
		virtual EResultT IRegisterTimerEvent(TimerEventSchedulerT *sched_info) = 0;
		virtual EResultT ILogData(ESeverityT severity, char *str) = 0;
		virtual EResultT ITraceData(uint32_t id, uint32_t line_num, uint64_t val0= 0, uint64_t val1=0, uint64_t val2=0, uint64_t val3=0) = 0;
		virtual EResultT IStopRequest(ESeverityT severity) = 0;
		virtual EResultT IGetSysTime(SysTimeT *sys_time_p) = 0;
		virtual EResultT IGetModule(char mod_name[], IModuleControlAPI **mod_ptr) = 0;
		virtual EResultT IMemAreaMount(CMemAreaP *mearea_ptr_, char area_name[], EAccessT ac_type) = 0;
		virtual EResultT ISyncTime(SysTimeT *sys_time_p = NULL, timespec *linux_time =NULL ) = 0;
		virtual EResultT IDelay_us(uint32_t usecs)= 0;
		virtual EResultT IAllocateEventCnt(char *cnt_name, volatile int64_t **cnt_ptr)= 0;
		virtual EResultT ISaveProfileInfo(uint32_t prof_id, ProfileData *data) = 0;
	};

	/*******************************************************************************************//*!
	*@class IModuleControlAPI
	*@brief The purpose of this class is :
	*@brief API for access to the 5G moduler
	***********************************************************************************************/
	class IModuleControlAPI
	{
	public:
		virtual EResultT IInit(IModuleControlCallBackAPI *callback_ptr, ITarget *target_api) = 0;
		virtual EResultT IColdStart() = 0;
		virtual EResultT IWarmStart() = 0;
		virtual EResultT IHotStart() = 0;
		virtual EResultT IStop(ESeverityT severity) = 0;
		virtual EResultT ICall(SysTimeT *sys_time_p, uint32_t param) = 0;
		virtual EResultT IConfigure(uint32_t id, void *in, void **out) = 0;
		virtual EResultT IGetInfo(char* module_name, uint32_t *major_ver, uint32_t *minor_ver, uint32_t *build_num, char* add_info) = 0;
	};

	//Front End APIs
	/*******************************************************************************************//*!
	*@class IGL_ControlCallBackAPI
	*@brief The purpose of this class is :
	*@brief API RSE callback API
	***********************************************************************************************/
	class IGL_ControlCallBackAPI
	{
	public:
		virtual EResultT IStopRequest(ESeverityT severity) = 0;
		virtual EResultT I_TTI_evt(SysTimeT *sys_time_p) = 0;
	};

	/*******************************************************************************************//*!
	*@class IGL_DebugAPI
	*@brief The purpose of this class is :
	*@brief Debugger API to the running support component. That API allows the system debugging from
	*@brief the externas application (front end).
	***********************************************************************************************/
	class IGL_DebugAPI	{
	public:
		virtual EResultT  	IDebugInit(char* add_info) = 0;
		virtual EResultT  	IGetMemAreasNum(uint32_t *areas_num) = 0;
		virtual CMemArea*  	IGetMemArea(uint32_t area_num) = 0;
		virtual EResultT 	IGetModulesNum(uint32_t *modules_num) = 0;
		virtual CModuleInfo IGetModule(uint32_t module_num) = 0;
		virtual EResultT 	ISetLogSeverity(ESeverityT severity) = 0;
		virtual EResultT 	IProfilerSave(char* file_name) = 0;
		virtual EResultT 	ITraceSave(char* file_name) = 0;
		virtual EResultT 	ICall(SysTimeT *sys_time_p, uint32_t param) = 0;
		virtual EResultT 	ISetBP(SysTimeT *sys_time_p, char *data, uint32_t *id) = 0;
		virtual EResultT 	IClearBP(uint32_t id) = 0;
		virtual EResultT 	ISendCLI(char* command_str, char **respond) = 0;
		virtual CMemArea** 	IGetProfilingData(uint32_t *nuentries_)= 0;
		virtual CMemArea**	IGetLogData(uint32_t *nuentries_) = 0;

	};


	/*******************************************************************************************//*!
	*@class IGL_ControlAPI
	*@brief The purpose of this class is :
	*@brief Running and control 5G enodev using external program (front end)
	***********************************************************************************************/
	class IGL_ControlAPI
	{
	public:
		virtual EResultT IInit(	IGL_ControlCallBackAPI *control_callback_ptr, const char* config_file, char* add_info) = 0;
		virtual EResultT IColdStart() = 0;
		virtual EResultT IWarmStart() = 0;
		virtual EResultT IHotStart() = 0;
		virtual EResultT IStop(ESeverityT severity) = 0;
		virtual EResultT IConfigure(uint32_t id, void *in, void **out) = 0;
		virtual EResultT IGetInfo(char* module_name, uint32_t major_ver, uint32_t minor_ver, uint32_t build_num, char* add_info) = 0;
		virtual EResultT IExitReq(ESeverityT severity) = 0;
		//If fapi_req_p==NULL than there is not other requests in this TTI
		virtual EResultT IFAPI_req_put(void* fapi_req_p) = 0;
		//fapi_evt_p == NULL means no events.
		virtual EResultT IFAPI_evt_get(void** fapi_evt_p) = 0;
	};

	//General factory API
	/*******************************************************************************************//*!
	*@class ITarget
	*@brief The purpose of this class is :
	*@brief Factory API using for connection to the classes implemented inside the library.
	***********************************************************************************************/
	class ITarget
	{
	public:
		virtual EResultT IGetTarget(EInterfaceT api_type, uint32_t id, void **gl_api_ptr) = 0;
		virtual EResultT IGetVersionInfo(char** module_name, uint32_t *major_ver, uint32_t *minor_ver, uint32_t *build_num, char** add_info) = 0;
	};
}; //Namespace definition
//API function to connect with ITarget API
typedef uint32_t (*IGetConnectAPI_t)(void **target_ptr);
extern "C" uint32_t IGetConnectAPI(void **target_ptr);
#endif /* I_COMP_API_H_ */
