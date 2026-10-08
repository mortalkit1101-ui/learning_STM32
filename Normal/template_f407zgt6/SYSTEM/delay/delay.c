#include "delay.h"

static uint32_t ticks_per_us;

void delay_init(void)
{
    SystemCoreClockUpdate();
    ticks_per_us = SystemCoreClock / 1000000U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->LOAD = 0U;
    SysTick->VAL = 0U;
}

void delay_us(uint32_t us)
{
    uint32_t max_us;
    uint32_t chunk;

    if (ticks_per_us == 0U)
    {
        delay_init();
    }
    max_us = (SysTick_LOAD_RELOAD_Msk + 1U) / ticks_per_us;
    while (us != 0U)
    {
        chunk = (us > max_us) ? max_us : us;
        SysTick->LOAD = chunk * ticks_per_us - 1U;
        SysTick->VAL = 0U;
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0U)
        {
        }
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
        SysTick->VAL = 0U;
        us -= chunk;
    }
}

void delay_ms(uint32_t ms)
{
    while (ms != 0U)
    {
        delay_us(1000U);
        --ms;
    }
}
