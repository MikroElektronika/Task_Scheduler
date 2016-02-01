/*******************************************************************************
* Title                 :   Task Scheduler
* Filename              :   scheduler.c
* Author                :   RL
* Origin Date           :   08/20/2015
* Notes                 :   None
*******************************************************************************/
/*************** MODULE REVISION LOG ******************************************
*
*    Date    Software Version    Initials   Description 
*  08/21/15    1.0              RL          Initial
*
*******************************************************************************/
/** 
 * @file scheduler.c
 * @brief Scheduler that runs task on a regular basis 
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "scheduler.h"

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
/**
 * @struct task_control_t 
 * @brief holds all members that describe a task
 */
typedef struct
{
    uint8_t   id;               /**< Task ID */
    task_t    task;             /**< Pointer to the task */
    uint32_t  delay;            /**< Delay before execution */
    uint32_t  period;           /**< Time task was set to at time of adding */
    task_status_e task_status;  /**< Status of task */
} task_control_t;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
static volatile task_control_t task_list[MAX_TASKS]; /**< Array of tasks */
static uint8_t task_scheduler_running;               /**< // flag for enabling / disabling scheduler */
static float count_per_ms;                           /**< used to calculate delay based on user input */

/******************************************************************************
* Function Prototypes
*******************************************************************************/
static task_control_t* find_task( uint8_t id );

/******************************************************************************
* Function Definitions
*******************************************************************************/
/**
 * @brief Finds task in array
 * 
 * @param uint8_t id - ID of task to find
 * @return int - ID of task or -1 if not found
 */
static task_control_t* find_task( uint8_t id )
{
    int i;

    for( i = 0; i < MAX_TASKS; i++ )
    {
        if( task_list[i].id == id )
        {
            return &task_list[i];
        }
    }
    
    return 0;
}


/*
 * Initializes RR Scheduler
 */
void task_scheduler_init( uint16_t clock )
{
    task_scheduler_running = 0;    
    count_per_ms = 1.0f / ( float )clock;
    
    return;
}


/* 
 * Adds a new task to the task list
 */
uint8_t task_add( task_t task, uint32_t period )
{
    uint8_t task_id = 0;
    float time_calc = ( ( double )period ) * count_per_ms;
    
    if( time_calc < 1 ) time_calc = 1.0f;

    for( task_id = 0; task_id < MAX_TASKS; task_id++ )
    {
        if( task_list[task_id].task_status == TASK_EMPTY )
        {
            task_list[task_id].task_status = TASK_RUNNABLE;
            task_list[task_id].id          = task_id;
            task_list[task_id].task        = task;
            task_list[task_id].delay       = ( time_calc > ( floor( time_calc + 0x5f ) ) ) ?
                                           ceil( time_calc ) : floor( time_calc );
            task_list[task_id].period      = task_list[task_id].delay;

            return task_list[task_id].id;
        }
    }
    
    return TASK_ERROR;
}

/*
 * Remove task from task list
 */
void task_delete( uint8_t id )
{
    task_control_t* task = find_task( id );
    
    if( task == 0 )
        return;
    
    task->task = ( task_t ) 0x00;
    task->task_status = TASK_EMPTY;
}

/**
 * Retrieves the task status
 */
task_status_e task_get_status( uint8_t id )
{
    task_control_t* task = find_task( id );
    
    return ( task == 0 ) ? TASK_ERROR : task->task_status;
}

/*
 * Stops the task from running
 */
void task_stop( uint8_t id )
{
    task_control_t* task = find_task( id );
    
    if( task == 0 )
        return;
    
    task->task_status = TASK_STOPPED;
}

/*
 * Resumes the task
 */
void task_resume( uint8_t id )
{
    task_control_t* task = find_task( id );
    
    if( task == 0 )
        return;
    
    task->task_status = TASK_RUNNABLE;
}

/*
 * Starts the scheduler
 */
void task_scheduler_start()
{
    task_scheduler_running = 1;
}

/* 
 * Used in cases where you need to stop all tasks
 */
void task_scheduler_stop()
{
    task_scheduler_running = 0;
}


/*
 * Dispatches tasks when they are ready to run
 */
void task_dispatch()
{
    if( task_scheduler_running == 1 )
    {
        int i;

        for( i = 0; i < MAX_TASKS; i++ )
        {
            // check for a valid task ready to run
            if( ( task_list[i].delay == 0 ) && ( task_list[i].task_status == TASK_RUNNABLE ) )
            {
                task_list[i].task_status = TASK_RUNNING;  // task is now running
                ( *task_list[i].task )();                 // call the task
                task_list[i].delay = task_list[i].period; // reset the delay
                task_list[i].task_status = TASK_RUNNABLE; // task is runnable again
            }
        }
    }
}

/*
 * Gets the number of tasks in scheduler
 */
uint8_t task_get_count()
{
    uint8_t i, count = 0;
    
    for( i = 0; i < MAX_TASKS; i++ )
    {
        if( task_list[i].task_status == TASK_EMPTY )
            count++;
    }
    
    return count;
}

/*
 * Called in ISR to decrement time for each task.
 */
void task_scheduler_clock()
{
    if( task_scheduler_running )
    {
        int i;

        // cycle through available tasks
        for( i = 0; i < MAX_TASKS; i++ )
        {
            if( task_list[i].task_status == TASK_RUNNABLE )
            {
                if( task_list[i].delay > 0 )
                {
                    task_list[i].delay--;
                }
            }
        }
    }
}

/*************** END OF FUNCTIONS ***************************************************************************/