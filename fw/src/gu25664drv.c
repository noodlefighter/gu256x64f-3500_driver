/**
 * @file
 * @brief 
 *     Driver for itron GU256X64F-3500
 *
 *     适配说明
 *          当前驱动适配到stm32f103, 管脚分配如下
 *              DAT0-7          GPIA0-7
 *              WR_N            GPIA8
 *              CD              GPIA9
 *              RD_N            GPIA10
 *              CS_N            GPIA11
 *              BL_N            悬空
 *
 * @internal
 * @par Modification history
 * - 1.00 2017-12-25     noodlefighter, first implementation
 * @endinternal
 */

#include "gu25664drv.h"

#include <stdint.h>


/* adapter */
#include "stm32f10x_gpio.h"

#define PIN_WRN         GPIO_Pin_8
#define PIN_CD          GPIO_Pin_9
#define PIN_RDN         GPIO_Pin_10
#define PIN_CSN         GPIO_Pin_11

static inline
void _pin_set (uint16_t pin, int value)
{
    if (value) {
        #if 0
        GPIO_SetBits(GPIOA, pin);
        #else
        GPIOA->BSRR = pin;
        #endif
    } else {
        #if 0
        GPIO_ResetBits(GPIOA, pin);
        #else
        GPIOA->BRR = pin;
        #endif
    }
}

static inline
void _data_write (uint8_t data)
{
    #if 0
    GPIO_Write(GPIOA, (uint16_t) ((GPIO_ReadOutputData(GPIOA) & 0xff00) | data));
    #else
    GPIOA->ODR = (uint16_t) ((GPIOA->ODR & 0xff00) | data);
    #endif
}

static
void _gpio_init (void)
{
    GPIO_InitTypeDef init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  //Enable GPIOA

    init.GPIO_Mode     = GPIO_Mode_Out_PP;
    init.GPIO_Pin      = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3  |
                         GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7  |
                         PIN_WRN    | PIN_CD     | PIN_CSN    | PIN_RDN;
    init.GPIO_Speed    = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &init);

    _pin_set(PIN_CSN, 1);
    _pin_set(PIN_WRN, 1);
    _pin_set(PIN_RDN, 1);
    _pin_set(PIN_CD, 1);
}

static inline
void _delay_100ns (void)
{
    int i = 1;
    while (i--);
}
/* adapter */

static uint8_t display_buf[256*64/8];

static void _send_cmd (uint8_t dat)
{
    _pin_set(PIN_CD, 1);
    _pin_set(PIN_CSN, 0);
    _pin_set(PIN_WRN, 0);
    _pin_set(PIN_RDN, 1);
    _delay_100ns();
    _data_write(dat);
    _delay_100ns();
    _pin_set(PIN_WRN, 1);
    _delay_100ns();
    _pin_set(PIN_CSN, 1);
    _pin_set(PIN_WRN, 1);
    _pin_set(PIN_RDN, 1);
    _pin_set(PIN_CD, 1);
}

static void _send_dat (uint8_t dat)
{
    _pin_set(PIN_CD, 0);
    _pin_set(PIN_CSN, 0);
    _pin_set(PIN_WRN, 0);
    _pin_set(PIN_RDN, 1);
    _delay_100ns();
    _data_write(dat);
    _delay_100ns();
    _pin_set(PIN_WRN, 1);
    _delay_100ns();
    _pin_set(PIN_CSN, 1);
    _pin_set(PIN_WRN, 1);
    _pin_set(PIN_RDN, 1);
    _pin_set(PIN_CD, 1);
}

void gu25664drv_draw_point (int x, int y, int value)
{
    uint16_t block_num = (uint16_t) ((x << 3) + (y >> 3));      // block_num = 8*x + y/8
    uint8_t  bit_idx   = (uint8_t) (7 - y + ((y >> 3) << 3));   // bit_idx   = 7 - y + y/8*8

    if (value) {
        display_buf[block_num] |= (uint8_t) (1 << bit_idx);
    } else {
        display_buf[block_num] &= (uint8_t) ~(1 << bit_idx);
    }

    _send_cmd(0x0a);//Command of lower adrs of disp start of Screen #1
    _send_dat(0x00);//Lower adrs of disp start of Screen #1
    _send_cmd(0x0b);//Command of upper adrs of disp start of Screen #1
    _send_dat(0x00);//upper adrs of disp start of Screen #1

    _send_cmd(0x0c);//Command of lower adrs of disp start of Screen #2
    _send_dat(0x00);//Lower adrs of disp start of Screen #2
    _send_cmd(0x0d);//Command of upper adrs of disp start of Screen #2
    _send_dat(0x00);//upper adrs of disp start of Screen #2

    _send_cmd(0x0e);//Command of Lower adrs of cursor of Screen #1   #2
    _send_dat((uint8_t) (block_num & 0x00ff));//Lower adrs of cursor of Screen #1   #2
    _send_cmd(0x0f);//Command of upper adrs of cursor of Screen #1   #2
    _send_dat((uint8_t) ((block_num & 0xff00) >> 8));//upper adrs of cursor of Screen #1   #2

    _send_cmd(0x08);//Command : Data write mode
    _send_dat(display_buf[block_num]);
}

void gu25664drv_clear (void)
{
    int i;

    _send_cmd(0x01);//Screen #1 is ON;
    _send_cmd(0x04);//Cursor increments automatically;
    _send_cmd(0x10);//OR display of  Screen #1   #2. OR = 0x10; EX-OR = 0x11; AND = 0x12.

    _send_cmd(0x0a);//Command of lower adrs of disp start of Screen #1
    _send_dat(0x00);//Lower adrs of disp start of Screen #1
    _send_cmd(0x0b);//Command of upper adrs of disp start of Screen #1
    _send_dat(0x00);//upper adrs of disp start of Screen #1

    _send_cmd(0x0c);//Command of lower adrs of disp start of Screen #2
    _send_dat(0x00);//Lower adrs of disp start of Screen #2
    _send_cmd(0x0d);//Command of upper adrs of disp start of Screen #2
    _send_dat(0x00);//upper adrs of disp start of Screen #2

    _send_cmd(0x0e);//Command of Lower adrs of cursor of Screen #1   #2
    _send_dat(0x00);//Lower adrs of cursor of Screen #1   #2
    _send_cmd(0x0f);//Command of upper adrs of cursor of Screen #1   #2
    _send_dat(0x00);//upper adrs of cursor of Screen #1   #2

    /* clear */
    _send_cmd(0x08);//Command : Data write mode
    for(i=0;i<2048;i++)
    {
        _send_dat(0x00);
    }

    /* clean buf */
    memset(&display_buf, 0, sizeof(display_buf));
}

int gu25664drv_init (void)
{
    _gpio_init();
    gu25664drv_clear();
}
