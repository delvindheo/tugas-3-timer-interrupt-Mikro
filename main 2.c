#include "stm32f4xx.h"

static volatile uint8_t btn_flag = 0U;

static void gpio_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;
    GPIOA->MODER = (GPIOA->MODER & ~(3U << 10U)) | (1U << 10U);
    GPIOA->OTYPER &= ~(1U << 5U);
    GPIOA->BSRR = (1U << 21U); 
    GPIOC->MODER &= ~(3U << 26U); 
    GPIOC->PUPDR = (GPIOC->PUPDR & ~(3U << 26U)) | (1U << 26U); 
}

static void tim5_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
    TIM5->CR1 = 0U;
    TIM5->PSC = 15999U;
    TIM5->ARR = 19U;
    TIM5->CNT = 0U;
    TIM5->EGR = TIM_EGR_UG;
    TIM5->SR = 0U;
    TIM5->DIER |= TIM_DIER_UIE;
    TIM5->CR1 |= TIM_CR1_OPM;
}

static void exti13_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[3] = (SYSCFG->EXTICR[3] & ~(0xFU << 4U)) | (0x2U << 4U);
    EXTI->IMR &= ~(1U << 13U);
    EXTI->RTSR &= ~(1U << 13U);
    EXTI->FTSR |= (1U << 13U);
    EXTI->PR = (1U << 13U);
    NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
    NVIC_ClearPendingIRQ(TIM5_IRQn);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    NVIC_EnableIRQ(TIM5_IRQn);
    EXTI->IMR |= (1U << 13U);
}

int main(void)
{
    gpio_init(); tim5_init(); exti13_init();
    while (1) {
        if (btn_flag) { btn_flag = 0U; GPIOA->ODR ^= (1U << 5U); }
        __WFI();
    }
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR & (1U << 13U)) {
        EXTI->PR = (1U << 13U);
        EXTI->IMR &= ~(1U << 13U);
        TIM5->CNT = 0U; TIM5->SR = 0U;
        TIM5->CR1 |= TIM_CR1_CEN;
    }
}

void TIM5_IRQHandler(void)
{
    if (TIM5->SR & TIM_SR_UIF) {
        TIM5->SR &= ~TIM_SR_UIF;
        if ((GPIOC->IDR & (1U << 13U)) == 0U) btn_flag = 1U;
        EXTI->PR = (1U << 13U);
        EXTI->IMR |= (1U << 13U);
    }
}
 //Delvin Dheo
 //101032400211