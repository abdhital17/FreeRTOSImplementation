/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 * main() creates all the demo application tasks, then starts the scheduler.
 * The web documentation provides more details of the standard demo application
 * tasks, which provide no particular functionality but do provide a good
 * example of how to use the FreeRTOS API.
 *
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Check" task - This only executes every five seconds but has a high priority
 * to ensure it gets processor time.  Its main function is to check that all the
 * standard demo tasks are still operational.  While no errors have been
 * discovered the check task will print out "OK" and the current simulated tick
 * time.  If an error is discovered in the execution of a task then the check
 * task will print out an appropriate error message.
 *
 */


/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "FreeRTOS.h"		/* RTOS firmware */
#include "task.h"			/* Task */
#include "timers.h"
#include "queue.h"
#include "crc.h"
/* Examples */

/* --------------------------------------------- */

#define NUM_TIMERS 1
#define MAX_DATA_IN_A_PACKAGE 10
TimerHandle_t xTimers[NUM_TIMERS];

void Producer_Task(void*);
void Consumer_Task(void*);

QueueHandle_t messageQueue;

struct _data_package
{
    char headers[4];
    char data[10];
    uint16_t CRC;
}data_package;

void vApplicationIdleHook(void);

void ProducerCallBack(TimerHandle_t xTimer)
{
    printf("Producer CallBack 50ms\n");
    struct _data_package temp;
    
    temp.headers[0] = 0xAA;
    temp.headers[1] = 0xBB;
    temp.headers[2] = 0xCC;
    temp.headers[3] = 0xC;

    for (int i =0; i<10; i++)
    {
        temp.data[i] = 65 + rand()%25 + 1;
    }
    
    uint16_t crc = 0;

    crc =  getCRC(temp.data, 10);
    temp.CRC = crc;

    printf("Sending data: ");
    for(int i =0; i<10; i++)
    {
        printf("%c", temp.data[i]);
    }
    printf("\n");
    
    xQueueSendFromISR(messageQueue, (void*) &temp, NULL);
}

int main ( void )
{
	/* Creating Two Task Same Priorities and Delay*/
//	xTaskCreate( vTask1, "Task 1", 1000, NULL, 1, NULL );
//	xTaskCreate( vTask2, "Task 2", 1000, NULL, 1, NULL );
	/* Creating Two Task Same Priorities and DelayUntil*/
    messageQueue = xQueueCreate(10, sizeof(data_package));

#if 0
    xTaskCreate( Producer_Task, "Producer", 1000, NULL, 1, NULL );
#else
    xTimers[0] = xTimerCreate("Producer", pdMS_TO_TICKS(50), pdTRUE, (void*) 0, ProducerCallBack);
    if (xTimers[0] == NULL)
    {
        printf("Error in creation of Producer Timer\n");
    }

    if (xTimerStart(xTimers[0], 0) != pdPASS)
    {
        printf("The timer could not be set into the Active state.");
    }
#endif
    xTaskCreate( Consumer_Task, "Consumer", 1000, NULL, 1, NULL );
	vTaskStartScheduler();
	return 0;
}

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
 	taskENTER_CRITICAL();
	{
        printf("[ASSERT] %s:%lu\n", pcFileName, ulLine);
        fflush(stdout);
	}
	taskEXIT_CRITICAL();
	exit(-1);
}



void Producer_Task(void* parameter)
{
	TickType_t xLastWaketime = xTaskGetTickCount();
    while(1){
        printf("Producer Task 250ms\n");
		vTaskDelayUntil(&xLastWaketime, pdMS_TO_TICKS(250));
    }
}
void Consumer_Task(void* parameter)
{
	TickType_t xLastWaketime = xTaskGetTickCount();
    struct _data_package temp;
    int data = 0;
    uint16_t crc = 0;

    while(1){
        if (xQueueReceive(messageQueue, &(temp), portMAX_DELAY) == pdPASS)
        {
            printf("Consumer Task\n");
            printf("receiving data: ");
            for(int i =0; i<10; i++)
            {
                printf("%c", temp.data[i]);
            }
            
            crc =  getCRC(temp.data, 10);
            
            printf("\n");

            if((uint16_t)temp.CRC == crc)
            {
                printf("The received CRC matches with the calculated CRC!\n");
                printf("Received crc: %u, Calculated crc: %u\n\n", temp.CRC, crc);
            }
        }
    }
}

/* CH3_TASKMANAGEMENT ends */


void vApplicationIdleHook(void)
{
//	printf("Idle\r\n");
}
/*-----------------------------------------------------------*/
