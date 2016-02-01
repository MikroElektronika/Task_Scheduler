> ![MikroE](http://www.mikroe.com/img/designs/beta/logo_small.png)
> #Task Scheduler#
> ##By [MikroElektronika](http://www.mikroe.com)
---

## Installation
[Package manager](http://www.mikroe.com/package-manager/) required to install the package to your IDE.  

###Example
```
#include "scheduler.h"

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
    task_scheduler_init( 500 );
    InitTimer2();
    
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
```
