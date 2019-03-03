#pragma once

#include <inttypes.h>

// ==== General ====
#define APP_NAME            "MagicStickLumos"

#define SIMPLESENSORS_ENABLED   TRUE

// MCU type as defined in the ST header.
#define STM32L151xB

// Freq of external crystal if any. Leave it here even if not used.
#define CRYSTAL_FREQ_HZ     12000000

#define SYS_TIM_CLK         (Clk.APB1FreqHz)
#define ADC_REQUIRED        FALSE
#define BUTTONS_ENABLED     TRUE

#if 1 // ========================== GPIO =======================================
// PortMinTim_t: GPIO, Pin, Tim, TimChnl, invInverted, omPushPull, TopValue

// Buttons
#define BTN1_PIN        GPIOA, 0

// UART
#define UART_GPIO       GPIOB
#define UART_TX_PIN     6
#define UART_RX_PIN     7

// Light
#define LUMOS_CTRL      { GPIOA, 6, TIM3, 1, invNotInverted, omPushPull, 255 }
#define LED_B_PIN       { GPIOA, 8, TIM1, 1, invNotInverted, omPushPull, 255 }
#define LED_R_PIN       { GPIOA, 9, TIM1, 2, invNotInverted, omPushPull, 255 }
#define LED_G_PIN       { GPIOA, 10, TIM1, 3, invNotInverted, omPushPull, 255 }

// Button
#define BTN_PIN         GPIOA, 0, pudPullDown

#endif // GPIO

#if 1 // ========================== USART ======================================
#define CMD_UART        USART1
#define UART_TXBUF_SZ   256
#endif

#if ADC_REQUIRED // ======================= Inner ADC ==========================
// Clock divider: clock is generated from the APB2
#define ADC_CLK_DIVIDER     adcDiv2

// ADC channels
//#define BAT_CHNL          1

#define ADC_VREFINT_CHNL    17  // All 4xx, F072 and L151 devices. Do not change.
#define ADC_CHANNELS        { ADC_VREFINT_CHNL }
#define ADC_CHANNEL_CNT     1   // Do not use countof(AdcChannels) as preprocessor does not know what is countof => cannot check
#define ADC_SAMPLE_TIME     ast96Cycles
#define ADC_SAMPLE_CNT      8   // How many times to measure every channel

#define ADC_SEQ_LEN         (ADC_SAMPLE_CNT * ADC_CHANNEL_CNT)

#endif

#if 1 // =========================== DMA =======================================
#define STM32_DMA_REQUIRED  TRUE
// ==== Uart ====
#define UART_DMA_TX     STM32_DMA1_STREAM4
#define UART_DMA_RX     STM32_DMA1_STREAM5
#define UART_DMA_CHNL   0   // Dummy

#if I2C1_ENABLED // ==== I2C ====
#define I2C1_DMA_TX     STM32_DMA1_STREAM6
#define I2C1_DMA_RX     STM32_DMA1_STREAM7
#define I2C1_DMA_CHNL   0   // Dummy
#endif

#if ADC_REQUIRED
#define ADC_DMA         STM32_DMA1_STREAM1
#define ADC_DMA_MODE    STM32_DMA_CR_CHSEL(0) |   /* dummy */ \
                        DMA_PRIORITY_LOW | \
                        STM32_DMA_CR_MSIZE_HWORD | \
                        STM32_DMA_CR_PSIZE_HWORD | \
                        STM32_DMA_CR_MINC |       /* Memory pointer increase */ \
                        STM32_DMA_CR_DIR_P2M |    /* Direction is peripheral to memory */ \
                        STM32_DMA_CR_TCIE         /* Enable Transmission Complete IRQ */
#endif // ADC

#endif // DMA
