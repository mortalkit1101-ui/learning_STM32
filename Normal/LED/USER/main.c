#include "stm32f4xx.h"
#include "delay.h"

int main(void)
{
    /* Startup calls SystemInit(); delay_init() also refreshes SystemCoreClock. */
    delay_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    while (1)
    {
        /* Initialize peripherals above and add application code here. */
    }
}
