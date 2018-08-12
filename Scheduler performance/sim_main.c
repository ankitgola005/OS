/*  sim_main.c
 *
 *  Ankit Gola              :    2017EET2296
 *
 *  This is the main function for simulation.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <time.h>

#include "sim.h"

/*  Main
 */
int main(int argc, char *argv[]) {
    if(argc != 5) {
        fprintf(stderr, "ERROR: USAGE sim <number of processes> <Percent of System processes> <Percent of System CPU process> <Percent of User CPU process>\n");
        exit(EXIT_FAILURE);
    }
    
    // Seed rand with time
    srand(time(NULL));

    // Get the numbers
    int numProc = strtol(argv[1], NULL, 10);
    int percentSys = numProc * strtol(argv[2], NULL, 10)/100;
    int percentUser = numProc - percentSys;
    
    int percentSysCPU = percentSys * strtol(argv[3], NULL, 10)/100;
    int percentSysIO = percentSys - percentSysCPU;

    int percentUserCPU = percentUser * strtol(argv[4], NULL, 10)/100;
    int percentUserIO = percentUser - percentUserCPU;

    printf("userCPU: %d, userIO: %d\n", percentUserCPU, percentUserIO);
    printf("sysCPU: %d, sysIO: %d\n", percentSysCPU, percentSysIO);

    //  Use rand with probability distribution to get procType, burstTime and priority.
    //while(1) {
    
    proc *list = NULL;
    int z = 0, o = 0, t = 0, th = 0;
    for(int i = numProc; i > 0; i--) {
        int procType = rand_gen(percentUserIO, percentUserCPU, percentSysIO, percentSysCPU);
        if(procType == 0) 
            z++;
        else if(procType == 1)
            o++;
        else if(procType == 2)
            t++;
        else if(procType == 3)
            th++;

        proc *new = create_proc(i, procType);
        insert_head(&list, new);
    }
    printf("UserIO: %d, UserCPU: %d\nSystemIO: %d, SystemCPU: %d\n", z, o, t, th);

    // Sort the list on the basis of arrival time
    merge_sort(&list);
    printf("Merged list\n");
    disp_list(list);

    // Get user input to do things
    while(1) {
        printf("\n\n");
        printf("Select option:\n");
        printf("1. FCFS\n");
        printf("2. SJF\n");
        printf("3. SJF with preemption (SRTF)\n");
        printf("4. Priority with ageing\n");
        printf("5. Priority with ageing and pre emption\n");
        printf("6. Round Robin\n");
        printf("7. Multi Level Queue, with ageing and pre emption between queues\n");
        printf("8. Multi Level Feedback Queue, with ageing and pre emption between queues\n");
        printf("9. Exit\n");

        int op;
        scanf("%d", &op);

        switch(op) {
            case 1:
                {
                    int *waitingTime = (int *)malloc(sizeof(int) * numProc);
                    int *turnAroundTime = (int *)malloc(sizeof(int) * numProc);
                    int *responseTime = (int *)malloc(sizeof(int) * numProc);
                    int *cpuUtilisation = (int *)malloc(sizeof(int) * numProc);
                    int *serviceTime = (int *)malloc(sizeof(int) * numProc);
                    
                    int throughput = 0;
                    int loadAverage = 0;
 
                    proc *fcfsHead = copy(list);
                    fcfs(fcfsHead, numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, throughput, loadAverage);
                    
                    break;
                }
            case 2:
                {
                    int *waitingTime = (int *)malloc(sizeof(int) * numProc);
                    int *turnAroundTime = (int *)malloc(sizeof(int) * numProc);
                    int *responseTime = (int *)malloc(sizeof(int) * numProc);
                    int *cpuUtilisation = (int *)malloc(sizeof(int) * numProc);
                    int *serviceTime = (int *)malloc(sizeof(int) * numProc);
                    
                    int throughput = 0;
                    int loadAverage = 0;
 
                    proc *sjfHead = copy(list);
                    sjf(sjfHead, numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, throughput, loadAverage);
                    break;
                }
            case 3:
                {
                    int *waitingTime = (int *)malloc(sizeof(int) * numProc);
                    int *turnAroundTime = (int *)malloc(sizeof(int) * numProc);
                    int *responseTime = (int *)malloc(sizeof(int) * numProc);
                    int *cpuUtilisation = (int *)malloc(sizeof(int) * numProc);
                    int *serviceTime = (int *)malloc(sizeof(int) * numProc);
                    
                    int throughput = 0;
                    int loadAverage = 0;
 
                    proc *srtfHead = copy(list);
                    srtf(srtfHead, numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, throughput, loadAverage);
                    break;
                }
            case 4:
                {
                    int *waitingTime = (int *)malloc(sizeof(int) * numProc);
                    int *turnAroundTime = (int *)malloc(sizeof(int) * numProc);
                    int *responseTime = (int *)malloc(sizeof(int) * numProc);
                    int *cpuUtilisation = (int *)malloc(sizeof(int) * numProc);
                    int *serviceTime = (int *)malloc(sizeof(int) * numProc);
                    
                    int throughput = 0;
                    int loadAverage = 0;
 
                    proc *pr1Head = copy(list);
                    priority1(pr1Head, numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, throughput, loadAverage);
                    break;
                }
            case 5:
                {
                    int *waitingTime = (int *)malloc(sizeof(int) * numProc);
                    int *turnAroundTime = (int *)malloc(sizeof(int) * numProc);
                    int *responseTime = (int *)malloc(sizeof(int) * numProc);
                    int *cpuUtilisation = (int *)malloc(sizeof(int) * numProc);
                    int *serviceTime = (int *)malloc(sizeof(int) * numProc);
                    
                    int throughput = 0;
                    int loadAverage = 0;
 
                    proc *pr2Head = copy(list);
                    priority2(pr2Head, numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, throughput, loadAverage);
                    break;
                }
            case 6:
                {
                    int *waitingTime = (int *)malloc(sizeof(int) * numProc);
                    int *turnAroundTime = (int *)malloc(sizeof(int) * numProc);
                    int *responseTime = (int *)malloc(sizeof(int) * numProc);
                    int *cpuUtilisation = (int *)malloc(sizeof(int) * numProc);
                    int *serviceTime = (int *)malloc(sizeof(int) * numProc);
                    
                    int throughput = 0;
                    int loadAverage = 0;
                    
                    proc *rrHead = copy(list);
                    printf("Enter Quanta size\n");
                    int quanta = 1;
                    scanf("%d", &quanta);
                    round_robin(rrHead, numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, throughput, loadAverage, quanta);
                    break;
                }
            case 7:
                {
                    int *waitingTime = (int *)malloc(sizeof(int) * numProc);
                    int *turnAroundTime = (int *)malloc(sizeof(int) * numProc);
                    int *responseTime = (int *)malloc(sizeof(int) * numProc);
                    int *cpuUtilisation = (int *)malloc(sizeof(int) * numProc);
                    int *serviceTime = (int *)malloc(sizeof(int) * numProc);
                    
                    int throughput = 0;
                    int loadAverage = 0;
 
                    proc *mLHead = copy(list);
                    m_level(mLHead, numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, throughput, loadAverage);
                    break;
                }
            case 8:
                {
                    int *waitingTime = (int *)malloc(sizeof(int) * numProc);
                    int *turnAroundTime = (int *)malloc(sizeof(int) * numProc);
                    int *responseTime = (int *)malloc(sizeof(int) * numProc);
                    int *cpuUtilisation = (int *)malloc(sizeof(int) * numProc);
                    int *serviceTime = (int *)malloc(sizeof(int) * numProc);
                    
                    int throughput = 0;
                    int loadAverage = 0;
 
                    proc *mLFHead = copy(list);
                    mlf_queue(mLFHead, numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, throughput, loadAverage);
                    break;
                }
            case 9:
                {
                    return 0;
                    break;
                }
            default:
                printf("Enter Correct option\n");
                break;
        }
    }
   
    return 0;
}
