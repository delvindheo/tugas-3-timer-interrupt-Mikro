#include "stm32f4xx.h"

static void gpio_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    GPIOA->MODER = (GPIOA->MODER & ~(3U << 10U)) | (1U << 10U);
    GPIOB->MODER &= ~((3U << 0U) | (3U << 2U));
    GPIOB->MODER |=  ((1U << 0U) | (1U << 2U));
    GPIOA->OTYPER &= ~(1U << 5U);
    GPIOB->OTYPER &= ~((1U << 0U) | (1U << 1U));
    GPIOA->BSRR = (1U << 21U);
    GPIOB->BSRR = (1U << 16U) | (1U << 17U);
}

static void tim_base_init(TIM_TypeDef *t, uint32_t psc, uint32_t arr)
{
    t->CR1 = 0U;
    t->PSC = psc;
    t->ARR = arr;
    t->CNT = 0U;
    t->EGR = TIM_EGR_UG;
    t->SR = 0U;
    t->DIER |= TIM_DIER_UIE;
}

int main(void)
{
    gpio_init();
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN;
    tim_base_init(TIM2, 15999U, 499U);
    tim_base_init(TIM3, 15999U, 249U);
    tim_base_init(TIM4, 15999U, 99U);
    NVIC_ClearPendingIRQ(TIM2_IRQn); NVIC_ClearPendingIRQ(TIM3_IRQn); NVIC_ClearPendingIRQ(TIM4_IRQn);
    NVIC_EnableIRQ(TIM2_IRQn); NVIC_EnableIRQ(TIM3_IRQn); NVIC_EnableIRQ(TIM4_IRQn);
    TIM2->CR1 |= TIM_CR1_CEN; TIM3->CR1 |= TIM_CR1_CEN; TIM4->CR1 |= TIM_CR1_CEN;
    while (1) { __WFI(); }
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF) { TIM2->SR &= ~TIM_SR_UIF; GPIOA->ODR ^= (1U << 5U); }
}
void TIM3_IRQHandler(void)
{
    if (TIM3->SR & TIM_SR_UIF) { TIM3->SR &= ~TIM_SR_UIF; GPIOB->ODR ^= (1U << 0U); }
}
void TIM4_IRQHandler(void)
{
    if (TIM4->SR & TIM_SR_UIF) { TIM4->SR &= ~TIM_SR_UIF; GPIOB->ODR ^= (1U << 1U); }
}
// Delvin Dheo
// 101032400211