/*
  * STMicroelectronics copyright and license:
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#include "stm32f4xx.h"

#if !defined(STM32F40_41xxx)
#error This template requires STM32F40_41xxx (STM32F407ZGT6).
#endif
#if HSE_VALUE != 8000000
#error Explorer V3 requires an 8 MHz HSE crystal.
#endif

uint32_t SystemCoreClock = 168000000U;

static void SetSysClock(void)
{
    uint32_t timeout = HSE_STARTUP_TIMEOUT;

    RCC->CR |= RCC_CR_HSEON;
    while (((RCC->CR & RCC_CR_HSERDY) == 0U) && (timeout > 0U))
    {
        --timeout;
    }
    if ((RCC->CR & RCC_CR_HSERDY) == 0U)
    {
        /* HSE failure: keep the reset HSI clock (16 MHz). */
        RCC->CR &= ~RCC_CR_HSEON;
        return;
    }

    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    (void)RCC->APB1ENR;
    PWR->CR |= PWR_CR_VOS;
    /* HCLK 168 MHz, APB1 42 MHz, APB2 84 MHz. */
    RCC->CFGR = RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2;
    /* HSE 8 MHz / M=8 * N=336 / P=2; Q=7 gives 48 MHz. */
    RCC->PLLCFGR = 8U | (336U << 6) | RCC_PLLCFGR_PLLSRC_HSE | (7U << 24);
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0U)
    {
    }

    /* Five wait states at 168 MHz with the board's 3.3 V supply. */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;
    while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_5WS)
    {
    }
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    {
    }
}

void SystemInit(void)
{
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= (3UL << 20) | (3UL << 22);
#endif
    RCC->CR |= RCC_CR_HSION;
    RCC->CFGR = 0U;
    RCC->CR &= 0xFEF6FFFFU;
    RCC->PLLCFGR = 0x24003010U;
    RCC->CR &= ~RCC_CR_HSEBYP;
    RCC->CIR = 0U;
    SetSysClock();
    SCB->VTOR = FLASH_BASE;
    /* Called before C runtime initialization: do not write initialized globals here. */
}

void SystemCoreClockUpdate(void)
{
    static const uint8_t ahb_shift[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
    uint32_t source;
    uint32_t pll_m;
    uint32_t pll_n;
    uint32_t pll_p;

    switch (RCC->CFGR & RCC_CFGR_SWS)
    {
    case RCC_CFGR_SWS_HSE:
        SystemCoreClock = HSE_VALUE;
        break;
    case RCC_CFGR_SWS_PLL:
        source = ((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC_HSE) != 0U) ? HSE_VALUE : HSI_VALUE;
        pll_m = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
        pll_n = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6;
        pll_p = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >> 16) + 1U) * 2U;
        SystemCoreClock = ((source / pll_m) * pll_n) / pll_p;
        break;
    default:
        SystemCoreClock = HSI_VALUE;
        break;
    }
    SystemCoreClock >>= ahb_shift[(RCC->CFGR & RCC_CFGR_HPRE) >> 4];
}
