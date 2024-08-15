/*
 ============================================================================
 Name        : example_c.c
 Author      : Igor Shoihet
 Version     :
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include <string.h>
#include "rt_debug_adap_api.h"

enum PROG_ACTION{
	E_NONE,
	E_APPLICATION,
	E_MONITOR,
	E_EXTRACT,
	E_MAX_ACTION
};

static char rt_debug_out[100] = "/tmp/rt_debug.txt";
void print_help()
{
	printf("Test program for the RT debug library\n\
Theare the options of the running:\n\
-a -- Running test applications	\n\
-m -- Monitoring the application running\n\
-e -- Extract the trace data\n\
-i <file> -- RT debugger data file, by default /tmp/rt_debug.dat\n\
-o <file> -- Output text file with extracted RT debugging information. by default /tmp/rt_debug.txt\n\
-h -- Print this help\n\n");
}
void WorkThread(void* param)
{
	const int num_iter= 10000;
	int i;
	int *delay= (int*)param;
	ProfilePoint prof_task, prof_task1;
	volatile uint64_t *watch_dog= RTDBG_GET_EVENT_CNTR_PTR(STATUS_GROUP,EV_WATCHDOG);
	RTDBG_INIT_PROF(GEN_GROUP, UI_PROF, &prof_task, 500);
	RTDBG_INIT_PROF(GEN_GROUP, TST_PROF, &prof_task1, 500);
	RTDBG_START_PROF(GEN_GROUP, TST_PROF, &prof_task1);
	for(i=0; i < num_iter; i++)
	{
		*watch_dog+=1;
		RTDBG_START_PROF(GEN_GROUP, UI_PROF, &prof_task);
		RTDBG_SAVE_TRACE(GEN_GROUP, PASSED, i, 0, 0, 0);
		usleep((int)*delay);

		RTDBG_STOP_PROF(GEN_GROUP, UI_PROF, &prof_task);
	}
	RTDBG_STOP_PROF(GEN_GROUP, TST_PROF, &prof_task1);

	RTDBG_FLUSH_DATA_PROF(GEN_GROUP, UI_PROF, &prof_task);
	RTDBG_FLUSH_DATA_PROF(GEN_GROUP, TST_PROF, &prof_task1);

	printf("+++Finish tracing collection after flush\n");
}
void Init()
{
	usleep(100);
}
void Run_application()
{
	const int thread_num= 10;
	const int delay_diff_max=50;
	const int delay_avg= 1000;
	ProfilePoint prof_task;
	int i;
	pthread_t tid[thread_num];
	volatile int64_t *ex_status= RTDBG_GET_EVENT_CNTR_PTR(STATUS_GROUP,EV_STATUS);
	*ex_status= 0;
	for(i=0;i<thread_num;i++)
	{
		int delay= delay_avg+ rand()%delay_diff_max;
	    printf("Before Thread\n");
	    pthread_create(&tid[i], NULL, WorkThread, (void*)&delay);
	    RTDBG_SAVE_TRACE(INIT_GROUP, CREATE_THREAD, i, (uint32_t)tid[i], 0, 0);
	}
	*ex_status= 1;
	printf("Wait for join\n");
	for(i=0;i<thread_num;i++)
	{

	    pthread_join(tid[i], NULL);
	}
	printf("Joined, Start Extraction\n");
	*ex_status= 2;
	RTDBG_SAVE_LOG(STATUS_GROUP,"Exit application");
	RT_debug_stop();
	RT_debug_ExtractDebugData(rt_debug_out);
}

void RunMonitor()
{

}

void ExtractData()
{
	RT_debug_ExtractDebugData(rt_debug_out);
}

int main(int argc, char *argv[]) {

	enum PROG_ACTION tst_action = E_NONE;
	char rt_debug_data[100]= "/tmp/rt_debug.dat";
	puts("!!!RT debug library running test!!!"); /* prints !!!Hello World!!! */

    size_t optind;
    for (optind = 1; optind < argc && argv[optind][0] == '-'; optind++) {
        switch (argv[optind][1]) {
        case 'a':
        	if(tst_action != E_NONE){
        		printf("The action may be only one and the other action was selected already\n");
                print_help();
                exit(EXIT_FAILURE);
        	}
        	tst_action = E_APPLICATION;
        	break;
        case 'm':
        	if(tst_action != E_NONE){
    		printf("The action may be only one and the other action was selected already\n");
            print_help();
            exit(EXIT_FAILURE);
			}
			tst_action = E_MONITOR;
			break;

        case 'e':
        	if(tst_action != E_NONE){
        		printf("The action may be only one and the other action was selected already\n");
                print_help();
                exit(EXIT_FAILURE);
        	}
        	tst_action = E_EXTRACT;
        	break;
        case 'i':
        	optind++;
        	strcpy(rt_debug_data, argv[optind]);
        	break;
        case 'o':
        	optind++;
        	strcpy(rt_debug_out, argv[optind]);
        	break;

        case 'h': print_help(); break;
        default:
            fprintf(stderr, "Wrong parameters. Usage:\n");
            print_help();
            exit(EXIT_FAILURE);
        }
    }
    argv += optind;
    if(tst_action == E_APPLICATION) {
    	//New test, remove the trace collection file
    	remove(rt_debug_data);
    }
    RT_debug_init(rt_debug_data);
    switch(tst_action){
    case E_APPLICATION:
    	RT_debug_start();
    	RTDBG_SAVE_LOG(INIT_GROUP,"Start running application %d", 10);
    	Run_application();
    	RTDBG_SAVE_LOG(GEN_GROUP,"Stop application");
    	break;
    case E_MONITOR:
    	RTDBG_Stop();
    	RunMonitor();
    case E_EXTRACT:
    	RT_debug_stop();
    	ExtractData();
    }

	return EXIT_SUCCESS;
}
