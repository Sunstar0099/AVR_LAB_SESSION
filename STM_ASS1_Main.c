/*====================================================================
MAIN.C
====================================================================
*/

#include "stm32f407xx.h"
#include <stdint.h>

/*
 * Select the Assignment 1 question:
 *
 * 1 = Toggle PD12 every 500 ms
 * 2 = Toggle PD12-PD15 simultaneously
 * 3 = Forward LED sequence
 * 4 = Forward-reverse LED sequence
 * 5 = PA0 button controls PD12
 */
//====================================================================
//PROGRAM SELECTION
//====================================================================
//
//Question 1:
#define ASSIGNMENT_QUESTION    1U
//
//Question 2:
//#define ASSIGNMENT_QUESTION    2U
//
//Question 3:
//#define ASSIGNMENT_QUESTION    3U
//
//Question 4:
//#define ASSIGNMENT_QUESTION    4U
//
//Question 5:
//#define ASSIGNMENT_QUESTION    5U


#define LED_GREEN      (1U << 12)
#define LED_ORANGE     (1U << 13)
#define LED_RED        (1U << 14)
#define LED_BLUE       (1U << 15)

#define ALL_LEDS       (LED_GREEN | LED_ORANGE | LED_RED | LED_BLUE)
#define USER_BUTTON    (1U << 0)

static void SysTick_Init(void);
static void Delay_ms(uint32_t milliseconds);
static void LED_GPIO_Init(void);
static void Button_GPIO_Init(void);
static void LEDs_Off(void);

/* Configure SysTick to generate a 1 millisecond time base. */
static void SysTick_Init(void)
{
    SystemCoreClockUpdate();

    SysTick->LOAD = (SystemCoreClock / 1000U) - 1U;
    SysTick->VAL = 0U;

    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_ENABLE_Msk;
}

/* Register-level blocking delay. */
static void Delay_ms(uint32_t milliseconds)
{
    while (milliseconds > 0U)
    {
        while ((SysTick->CTRL & (1 << 16U)) == 0U)
        {
            /* Wait for one millisecond. */
        }

        milliseconds--;
    }
}

/* Configure PD12-PD15 as GPIO push-pull outputs. */
static void LED_GPIO_Init(void)
{
    /* Enable the GPIOD peripheral clock. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    /* Dummy read ensures that the peripheral clock is available. */
    (void)RCC->AHB1ENR;

    /*
     * Each GPIO pin has two MODER bits.
     * 00 = Input
     * 01 = General-purpose output
     */
    GPIOD->MODER &= ~(0xFFU << 24U);
    GPIOD->MODER |=  (0x55U << 24U);

    LEDs_Off();
}

/* Configure PA0 as a GPIO input for the User button. */
static void Button_GPIO_Init(void)
{
    /* Enable the GPIOA peripheral clock. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* Set PA0 to input mode. */
    GPIOA->MODER &= ~(3U << 0U);
}

/* Turn all four onboard LEDs OFF using the BSRR register. */
static void LEDs_Off(void)
{
    GPIOD->BSRR = (ALL_LEDS << 16U);
}

int main(void)
{
    LED_GPIO_Init();

#if ASSIGNMENT_QUESTION != 5U
    SysTick_Init();
#endif

#if ASSIGNMENT_QUESTION == 1U

    /*
     * QUESTION 1
     * Configure PD12 as an output and toggle the green LED
     * continuously with a 500 millisecond delay.
     */
    while (1)
    {
        GPIOD->ODR ^= LED_GREEN;
        Delay_ms(500U);
    }

#elif ASSIGNMENT_QUESTION == 2U

    /*
     * QUESTION 2
     * Switch PD12-PD15 ON and OFF simultaneously.
     */
    while (1)
    {
        GPIOD->BSRR = ALL_LEDS;
        Delay_ms(500U);

        GPIOD->BSRR = (ALL_LEDS << 16U);
        Delay_ms(500U);
    }

#elif ASSIGNMENT_QUESTION == 3U

    /*
     * QUESTION 3
     * Create the forward LED sequence:
     * PD12 -> PD13 -> PD14 -> PD15
     */
    while (1)
    {
        for (uint32_t pin = 12U; pin <= 15U; pin++)
        {
            LEDs_Off();
            GPIOD->BSRR = (1U << pin);
            Delay_ms(500U);
        }
    }

#elif ASSIGNMENT_QUESTION == 4U

    /*
     * QUESTION 4
     * Create the forward-reverse sequence:
     * PD12 -> PD13 -> PD14 -> PD15 -> PD14 -> PD13
     */
    while (1)
    {
        for (uint32_t pin = 12U; pin <= 15U; pin++)
        {
            LEDs_Off();
            GPIOD->BSRR = (1U << pin);
            Delay_ms(500U);
        }

        for (uint32_t pin = 14U; pin > 12U; pin--)
        {
            LEDs_Off();
            GPIOD->BSRR = (1U << pin);
            Delay_ms(500U);
        }
    }

#elif ASSIGNMENT_QUESTION == 5U

    /*
     * QUESTION 5
     * Keep PD12 ON while the PA0 User button is pressed.
     */
    Button_GPIO_Init();

    while (1)
    {
        if ((GPIOA->IDR & USER_BUTTON) != 0U)
        {
            GPIOD->BSRR = LED_GREEN;
        }
        else
        {
            GPIOD->BSRR = (LED_GREEN << 16U);
        }
    }

#else
    #error "ASSIGNMENT_QUESTION must be between 1 and 5"
#endif
}


