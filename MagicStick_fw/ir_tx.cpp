/*
 * ir_tx.cpp
 *
 *  Created on: 24 мар. 2019 г.
 *      Author: Kreyl
 */

#include "ir_tx.h"
#include "kl_lib.h"
#include "board.h"
#include "hal.h"
#include "uart.h"
#include "shell.h"

#define IRLED_DMA_MODE  \
    STM32_DMA_CR_CHSEL(DAC_DMA_CHNL) | \
    DMA_PRIORITY_HIGH | \
    STM32_DMA_CR_MSIZE_BYTE | \
    STM32_DMA_CR_PSIZE_BYTE | \
    STM32_DMA_CR_MINC        | \
    STM32_DMA_CR_DIR_M2P     | \
    STM32_DMA_CR_CIRC

static const stm32_dma_stream_t *PDmaTx;
static Timer_t SamplingTmr{IR_SAMPLING_TMR};
#define TX_BUF_SZ   8
static uint8_t TxBuf[TX_BUF_SZ] = {0, 99, 0, 99, 0, 99, 0, 99};

void IrTRx_Init() {
    PinSetupAnalog(IR_LVL_PIN);
    PinSetupAnalog(IR_CARRIER_PIN); // Not used
    rccEnableDAC1(FALSE);
    // Enable DAC, enable DMA, TIM7 TRGO evt as trigger, trigger enable
    DAC->CR = DAC_CR_DMAEN1 | (0b010UL << 3) | DAC_CR_TEN1;
    DAC->CR |= DAC_CR_EN1;
    // ==== DMA ====
    PDmaTx = dmaStreamAlloc(DAC_DMA, IRQ_PRIO_MEDIUM, nullptr, nullptr);
    dmaStreamSetPeripheral(PDmaTx, &DAC->DHR8R1);
    dmaStreamSetMode      (PDmaTx, IRLED_DMA_MODE);
    dmaStreamSetMemory0   (PDmaTx, TxBuf);
    dmaStreamSetTransactionSize(PDmaTx, TX_BUF_SZ);
    dmaStreamEnable(PDmaTx);
    // ==== Sampling timer ====
    SamplingTmr.Init();
    SamplingTmr.SetUpdateFrequencyChangingTopValue(SAMPLING_FREQ_HZ);
    SamplingTmr.SelectMasterMode(mmUpdate);
    SamplingTmr.Enable();
}


