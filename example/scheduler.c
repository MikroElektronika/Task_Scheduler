/*******************************************************************************
* Title                 :   Scheduler Example
* Filename              :   scheduler.c
* Author                :   RBL
* Origin Date           :   08/23/2015
* Notes                 :   None
*******************************************************************************/
/*************** MODULE REVISION LOG ******************************************
*
*    Date    Software Version    Initials   Description 
*  08/23/15         .1              RBL      Module Created.
*
*******************************************************************************/
/** 
 *  @file scheduler.c
 *  @brief Example of using the task scheduler. 
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

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
void say_hello( void );
void init_timer2( void );

/******************************************************************************
* Function Definitions
*******************************************************************************/
void say_hello()
{
    UART1_Write_Text( "Hello" );
}

//Timer2 Prescaler :575; Preload = 62499; Actual Interrupt Time = 500 ms
void init_timer2()
{
    RCC_APB1ENR.TIM2EN = 1;
    TIM2_CR1.CEN = 0;
    TIM2_PSC = 575;
    TIM2_ARR = 62499;
    NVIC_IntEnable(IVT_INT_TIM2);
    TIM2_DIER.UIE = 1;
    TIM2_CR1.CEN = 1;
}


void main() 
{
    /* Initialize task scheduler by informing it how often the 
       clock interrupts */
    task_scheduler_init( 500 ); 
    init_timer2();
    
    task_add( say_hello, SCH_SECONDS_1 );
    EnableInterrupts();
    task_scheduler_start();

    while( 1 )
    {
        task_dispatch();
    }
}

void timer2_interrupt() iv IVT_INT_TIM2
{
    TIM2_SR.UIF = 0;
    task_scheduler_clock();
}
/*************** END OF FUNCTIONS ***************************************************************************/


