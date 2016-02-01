/****************************************************************************
* Title                 :   Task Scheduler   
* Filename              :   scheduler.h
* Author                :   RL
* Origin Date           :   08/20/2015
* Notes                 :   None
*****************************************************************************/
/**************************CHANGE LIST **************************************
*
*    Date    Software Version    Initials   Description 
*  08/21/15    1.0              RL          Initial version
*
*****************************************************************************/
/**
 * @file scheduler.h
 *
 * @brief Simple round robin task scheduler
 *
 * @author Richard Lowe
 * @copyright GNU Public License
 *
 *
 * @date 08/11/2014
 *
 * @version 1.0 - Initial
 *
 * @details Needs a timer with interrupt.  Call in ISR is task_scheduler_clock.
 * Existing tasks are repeatedly called in desired ms.
 *
 *
 * @note
 * Test configuration:
 *   MCU:             STM32107VC
 *   Dev.Board:       EasyMxPRO v7
 *   Oscillator:      72.Mhz
 *   Ext. Modules:    
 *   SW:              ARM 4.5.2
 * 
 * 
 * @code
 *  void main()
 *  {
 *      initTimer();  // Initialize timer
 *
 *      task_scheduler_init( 1000 );
 *      task_add( task1, SCH_SECONDS_1 );
 *      task_add( task2, SCH_SECONDS_5 );
 *
 *      task_scheduler_start();
 *
 *      while(1)
 *      {
 *          task_dispatch();
 *      }
 *   }
 *   ... Call task_scheduler_clock() in ISR
 *
 *  @endcode
 */

/**
 * @mainpage
 * 
 * <p>Micro controllers have no operating system therefore it is only capable of doing one thing at a time.
 * We can however, schedule events and make it appear that it can do multiple things.</p>  
 * 
 * <p>The concept is simple,
 * we create a list of functions to be ran and give each one a period of timeout.  The timer on the MCU will
 * decrement this count and when the count is 0, the function is ran then reset back to its' old value.</p>
 * 
 * <p>More can be found at: <a href="www.libstock.com">Libstock.com</a></p>
 * 
 */
#ifndef SCHEDULER_H
#define SCHEDULER_H

/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>

/******************************************************************************
* Preprocessor Constants
*******************************************************************************/
#define MAX_TASKS 7
#define SCH_SECONDS_1   1000
#define SCH_SECONDS_5   5000
#define SCH_SECONDS_10  10000
#define SCH_SECONDS_15  15000
#define SCH_SECONDS_30  30000
#define SCH_MINUTES_1   SCH_SECONDS_1 * 60
#define SCH_MINUTES_15  SCH_MINUTES_1 * 15
#define SCH_MINUTES_30  SCH_MINUTES_15 * 2
#define SCH_HOURS_1     SCH_MINUTES_30 * 2
#define SCH_HOURS_12    SCH_HOURS_1 * 12
#define SCH_DAY_1       SCH_HOURS_12 * 2

/******************************************************************************
* Configuration Constants
*******************************************************************************/


/******************************************************************************
* Macros
*******************************************************************************/


        
/******************************************************************************
* Typedefs
*******************************************************************************/
typedef void ( *task_t )( void ); /**< function type ran in scheduler */

/**
 * @enum Status of tasks in scheduler
 *
 */
typedef enum
{
    TASK_EMPTY = 0,
    TASK_STOPPED,       /**< Task is stopped           */
    TASK_RUNNABLE,      /**< Task is ready to be ran   */
    TASK_RUNNING,       /**< Task is currently running */
    TASK_ERROR = 99     /**< Error has occurred        */
} task_status_e;

/******************************************************************************
* Variables
*******************************************************************************/


/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Initializes RR Scheduler
 *
 * Initialization only requires a clock parameter.  Clock represents the
 * time that expires between calls to task_scheduler_clock().
 *
 * @param uint16_t clock - number of ms scheduler_timer_clock will be called
 *
 * @code
 *    task_scheduler_init( 1000 ); // 1000 ms between calls
 * @endcode
 *
 * @note
 *   Sets all tasks to NULL
 */
void task_scheduler_init( uint16_t clock );


/** 
 * @brief Adds a new task to the task list
 * 
 * Scans through the list and places the new task where
 * it finds free slot.
 *
 * @param task_t task - function to be added
 * @param uint32_t period - time in ms when to be repeated 
 * 
 * @return Task ID
 * 
 * @code
 *     int flasherID;
 *     flasherID = task_add( heart_beat, 1000 );
 * @endcode
 */
uint8_t task_add( task_t task, uint32_t period );


/**
 * @brief Remove task from task list
 * @details STOPPED is equivalent to removing a task
 * 
 * @param uint8_t id - ID of task to be removed
 */
void task_delete( uint8_t id );


/**
 *  @brief Get Task Status
 *
 *  @pre Scheduler must be initialized first
 *
 *  @param unsigned char id - ID of task
 *
 *  @return enum taskStatus_e
 *    @retval TASK_RUNNABLE
 *    @retval TASK_RUNNING
 *    @retval TASK_STOPPED
 *    @retval TASK_ERROR
 */
task_status_e task_get_status( uint8_t id );


/**
 * @brief Stops the task from running
 *
 * @param uint8_t id - id of task
 *
 */
void task_stop( uint8_t id );


/**
 * @brief Resumes the task from a running task
 *
 * @param uint8_t id - id of task
 *
 */
void task_resume( uint8_t id );


/**
 * @brief Starts the task scheduler
 *
 * This function is required to be called before the 
 * while loop.  Without this function, if your initialization 
 * was long, tasks would try to be ran prematurely.
 * 
 * @code
 *      task_scheduler_init( 500 );
 *      initialization_code();
 *      task_scheduler_start();
 *      
 *      while( 1 )
 *      {
 *          ...
 *          task_dispatch();
 *      }
 * @endcode
 */
void task_scheduler_start ( void );


/**
 * @brief Stops the task scheduler
 *
 * Used in cases where you need to stop all tasks
 *
 */
void task_scheduler_stop ( void );


/**
 * @brief Dispatches tasks when they are ready to run
 * 
 * Called in while loop.  If function is due to run will call it.
 * 
 * @code
 *      while( 1 )
 *      {
 *          task_dispatch();
 *      {
 * @endcode
 */
void task_dispatch( void );


/**
 * @brief Gets the number of tasks in scheduler
 * 
 * @return uint8_t - number of tasks
 */
uint8_t task_get_count( void );


/**
 *  @brief Called in ISR to decrement time for each task.
 *
 *  @pre Clock needs to be initialized
 *
 */
void task_scheduler_clock( void );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* SCHEDULER_H */

/*** End of File **************************************************************/