#include "stm32f10x.h"

void Delay_ms(uint32_t ms)
{
    volatile uint32_t i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 8000; j++);
}

int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    uint8_t blink = 0;

    // 开时钟：GPIOC + PWR + BKP
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
    PWR->CR |= PWR_CR_DBP;      // 解锁备份寄存器

    // PC13 推挽输出
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 上电或按复位时，翻转状态；否则沿用上次状态
    if (RCC->CSR & (RCC_CSR_PINRSTF | RCC_CSR_PORRSTF))
        blink = (BKP->DR1 == 0) ? 1 : 0;
    else
        blink = BKP->DR1;

    BKP->DR1 = blink;           // 保存状态，复位不丢
    RCC->CSR |= RCC_CSR_RMVF;   // 清复位标志

    while (1)
    {
        if (blink)              // 闪烁
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
            Delay_ms(500);
            GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
            Delay_ms(500);
        }
        else                    // 常亮
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
        }
    }
}