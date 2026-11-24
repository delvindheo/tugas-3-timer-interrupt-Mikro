#include "stm32f4xx.h"

static void gpio_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;
    GPIOA->MODER = (GPIOA->MODER & ~(3U << 10U)) | (1U << 10U);
    GPIOA->OTYPER &= ~(1U << 5U);
    GPIOA->BSRR = (1U << 21U);
    GPIOC->MODER &= ~(3U << 26U);
    GPIOC->PUPDR = (GPIOC->PUPDR & ~(3U << 26U)) | (1U << 26U);
}

static void tim3_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->CR1 = 0U;
    TIM3->PSC = 15999U;
    TIM3->ARR = 99U; 
    TIM3->CNT = 0U;
    TIM3->EGR = TIM_EGR_UG;
    TIM3->SR = 0U;
    TIM3->DIER |= TIM_DIER_UIE;
    NVIC_ClearPendingIRQ(TIM3_IRQn);
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1 |= TIM_CR1_CEN;
}

int main(void)
{
    gpio_init(); tim3_init();
    while (1) { __WFI(); }
}

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;
        if ((GPIOC->IDR & (1U << 13U)) == 0U) GPIOA->ODR ^= (1U << 5U);
        else GPIOA->BSRR = (1U << 21U);
    }
}
//Delvin Dheo
//101032400211