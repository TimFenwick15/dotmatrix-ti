#include "Library/F2802x_Device.h"
#include "Library/gpio.h"
#include "Library/F2802x_Examples.h"
#include "Library/clk.h"

#define REG_CONTROL // regs, over the GPIO handle stuff

// With REG_CONTROL defined, I get no screen flicker, but lots of errors
// Without, I get no errors, but some flicker. So the GPIO handle pointer control stuff must be slower than direct control

#define HIGH (1)
#define LOW (0)

#define ROWS (32)
#define ROW_ADDRESSES (ROWS / 2)
#define WIDTH (64)

#define ADDRESS_PORT_SHIFT (16)

#define CLK ((Uint16)GPIO_Number_28)
#define OE  ((Uint16)GPIO_Number_29)
#define LAT ((Uint16)GPIO_Number_34)
#define A   ((Uint16)GPIO_Number_16)
#define B   ((Uint16)GPIO_Number_17)
#define C   ((Uint16)GPIO_Number_18)
#define D   ((Uint16)GPIO_Number_19)
#define R1  ((Uint16)GPIO_Number_0)
#define G1  ((Uint16)GPIO_Number_1)
#define B1  ((Uint16)GPIO_Number_2)
#define R2  ((Uint16)GPIO_Number_3)
#define G2  ((Uint16)GPIO_Number_4)
#define B2  ((Uint16)GPIO_Number_5)

#ifdef REG_CONTROL
#define CLEAR_CLK (GpioDataRegs.GPACLEAR.bit.GPIO28 = 1) // J1-3 CLK
#define CLEAR_OE  (GpioDataRegs.GPACLEAR.bit.GPIO29 = 1) // J1-4 OE
#define CLEAR_LAT (GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1) // J1-5 LAT // Pulled up via S1-1
#define CLEAR_A   (GpioDataRegs.GPACLEAR.bit.GPIO16 = 1) // J6-7 A
#define CLEAR_B   (GpioDataRegs.GPACLEAR.bit.GPIO17 = 1) // J6-8 B   // shared with GPIO33
#define CLEAR_C   (GpioDataRegs.GPACLEAR.bit.GPIO18 = 1) // J1-7 C   // shared with GPIO32
#define CLEAR_D   (GpioDataRegs.GPACLEAR.bit.GPIO19 = 1) // J2-2 D
#define CLEAR_R1  (GpioDataRegs.GPACLEAR.bit.GPIO0 = 1)  // J6-1 R1
#define CLEAR_G1  (GpioDataRegs.GPACLEAR.bit.GPIO1 = 1)  // J6-2 G1
#define CLEAR_B1  (GpioDataRegs.GPACLEAR.bit.GPIO2 = 1)  // J6-3 B1
#define CLEAR_R2  (GpioDataRegs.GPACLEAR.bit.GPIO3 = 1)  // J6-4 R2
#define CLEAR_G2  (GpioDataRegs.GPACLEAR.bit.GPIO4 = 1)  // J6-5 G2
#define CLEAR_B2  (GpioDataRegs.GPACLEAR.bit.GPIO5 = 1)  // J6-6 B2
#define SET_CLK   (GpioDataRegs.GPASET.bit.GPIO28 = 1)
#define SET_OE    (GpioDataRegs.GPASET.bit.GPIO29 = 1)
#define SET_LAT   (GpioDataRegs.GPBSET.bit.GPIO34 = 1)
#define SET_A     (GpioDataRegs.GPASET.bit.GPIO16 = 1)
#define SET_B     (GpioDataRegs.GPASET.bit.GPIO17 = 1)
#define SET_C     (GpioDataRegs.GPASET.bit.GPIO18 = 1)
#define SET_D     (GpioDataRegs.GPASET.bit.GPIO19 = 1)
#define SET_R1    (GpioDataRegs.GPASET.bit.GPIO0 = 1)
#define SET_G1    (GpioDataRegs.GPASET.bit.GPIO1 = 1)
#define SET_B1    (GpioDataRegs.GPASET.bit.GPIO2 = 1)
#define SET_R2    (GpioDataRegs.GPASET.bit.GPIO3 = 1)
#define SET_G2    (GpioDataRegs.GPASET.bit.GPIO4 = 1)
#define SET_B2    (GpioDataRegs.GPASET.bit.GPIO5 = 1)
#else
#define CLEAR_CLK (GPIO_setLow(myGpio, GPIO_Number_28))
#define CLEAR_OE  (GPIO_setLow(myGpio, GPIO_Number_29))
#define CLEAR_LAT (GPIO_setLow(myGpio, GPIO_Number_34))
#define CLEAR_A   (GPIO_setLow(myGpio, GPIO_Number_16))
#define CLEAR_B   (GPIO_setLow(myGpio, GPIO_Number_17))
#define CLEAR_C   (GPIO_setLow(myGpio, GPIO_Number_18))
#define CLEAR_D   (GPIO_setLow(myGpio, GPIO_Number_19))
#define CLEAR_R1  (GPIO_setLow(myGpio, GPIO_Number_0))
#define CLEAR_G1  (GPIO_setLow(myGpio, GPIO_Number_1))
#define CLEAR_B1  (GPIO_setLow(myGpio, GPIO_Number_2))
#define CLEAR_R2  (GPIO_setLow(myGpio, GPIO_Number_3))
#define CLEAR_G2  (GPIO_setLow(myGpio, GPIO_Number_4))
#define CLEAR_B2  (GPIO_setLow(myGpio, GPIO_Number_5))
#define SET_CLK   (GPIO_setHigh(myGpio, GPIO_Number_28))
#define SET_OE    (GPIO_setHigh(myGpio, GPIO_Number_29))
#define SET_LAT   (GPIO_setHigh(myGpio, GPIO_Number_34))
#define SET_A     (GPIO_setHigh(myGpio, GPIO_Number_16))
#define SET_B     (GPIO_setHigh(myGpio, GPIO_Number_17))
#define SET_C     (GPIO_setHigh(myGpio, GPIO_Number_18))
#define SET_D     (GPIO_setHigh(myGpio, GPIO_Number_19))
#define SET_R1    (GPIO_setHigh(myGpio, GPIO_Number_0))
#define SET_G1    (GPIO_setHigh(myGpio, GPIO_Number_1))
#define SET_B1    (GPIO_setHigh(myGpio, GPIO_Number_2))
#define SET_R2    (GPIO_setHigh(myGpio, GPIO_Number_3))
#define SET_G2    (GPIO_setHigh(myGpio, GPIO_Number_4))
#define SET_B2    (GPIO_setHigh(myGpio, GPIO_Number_5))
#endif
#define ABOUT_1S_OF_BIG_NUMBER ((Uint32)1000000) // count ~ 1us, 1s = 1,000,000 counts
#define ABOUT_10MS_OF_BIG_NUMBER ((Uint32)10000) // count ~ 1us, 10ms = 10,000 counts
#define ABOUT_1US_OF_BIG_NUMBER ((Uint32)1)      // count ~ 1us, 1us = 1 count

void initTimer(void);
void initGPIO(void);
void setGpioAsOutput(Uint16 gpioNumber);
void setGpioHigh(Uint16 pin);
void setGpioLow(Uint16 pin);
void delayMicroseconds(Uint32 delayTime);

GPIO_Handle myGpio;
Uint32 i = 0;
#define I_MAX (16)

// 20:12 first char
const Uint16 glyph_0_0[ROW_ADDRESSES] = {
    0b0111000000000010,
    0b0000111110000100,
    0b0000000010001000,
    0b0000000010000000,
    0b0000000010000000,
    0b0111111110000000,
    0b0001000010000111,
    0b0001000010000100,
    0b0001000010000100,
    0b0001000001000100,
    0b0001000001000100,
    0b0001000000100100,
    0b0001000000000100,
    0b0000000000011010,
    0b0111111111100001,
    0b0000000000000000
};

// 20:12 second char
const Uint16 glyph_1_0[ROW_ADDRESSES] = {
    0b0000000000000010,
    0b0000000000000100,
    0b0011111111101000,
    0b0000001000000000,
    0b0000001000000001,
    0b0000001000000010,
    0b0000001000000100,
    0b0000001000000000,
    0b0000001000000000,
    0b0000001000000100,
    0b0000001000000100,
    0b0000001000000010,
    0b0000001000000010,
    0b0111111111110001,
    0b0000000000000001,
    0b0000000000000000
};

// 20:12 third char
const Uint16 glyph_2_0[ROW_ADDRESSES] = {
    0b0000000000000100,
    0b0011111111111110,
    0b0000100100100101,
    0b0000100100100100,
    0b0111111111111111,
    0b0000100100100100,
    0b0000100100100100,
    0b0011111111111110,
    0b0000100000001000,
    0b0011111101111100,
    0b0000100101000010,
    0b0000100100100101,
    0b0111111110011000,
    0b0000100000001100,
    0b0000100000000011,
    0b0000000000000000
};

// 20:12 fourth char
const Uint16 glyph_3_0[ROW_ADDRESSES] = {
    0b0000000000000000,
    0b0001111111111100,
    0b0000100000000000,
    0b0000011000000000,
    0b0000000110000000,
    0b0000000010000000,
    0b0000000010000000,
    0b0111111111111111,
    0b0000000010000000,
    0b0000000010000000,
    0b0000000010000000,
    0b0000000010000000,
    0b0000000010000000,
    0b0000000010000000,
    0b0000000011100000,
    0b0000000000000000
};

// 20:12 blank
const Uint16 glyph_0_1[ROW_ADDRESSES] = {
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000
};

// 20:12 twenty
const Uint16 glyph_1_1[ROW_ADDRESSES] = {
    0b0000000000000000,
    0b0000110000001100,
    0b0001001000010010,
    0b0010000100100001,
    0b0010000100100001,
    0b0010000100100000,
    0b0010000100010000,
    0b0010000100010000,
    0b0010000100001000,
    0b0010000100000100,
    0b0010000100000100,
    0b0010000100000010,
    0b0001001000000001,
    0b0000110000111111,
    0b0000000000000000,
    0b0000000000000000
};

// 20:12 colon
const Uint16 glyph_2_1[ROW_ADDRESSES] = {
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000011000000,
    0b0000000011000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000011000000,
    0b0000000011000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000
};

// 20:12 twelve
const Uint16 glyph_3_1[ROW_ADDRESSES] = {
    0b0000000000000000,
    0b0000110000001000,
    0b0001001000001110,
    0b0010000100001000,
    0b0010000100001000,
    0b0010000000001000,
    0b0001000000001000,
    0b0001000000001000,
    0b0000100000001000,
    0b0000010000001000,
    0b0000010000001000,
    0b0000001000001000,
    0b0000000100001000,
    0b0011111100001000,
    0b0000000000000000,
    0b0000000000000000
};

/**
 * main.c
 */
int main(void)
{
    //Uint32 time = 0;
    // Think we need this to be able to run from flash, not during JTAG debugging
    Uint16 j = 0;

#ifdef _FLASH
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
#endif

    initGPIO();
    initTimer();

    for (;;) {
        //time = CpuTimer1Regs.TIM.all;

        SET_OE;
        SET_LAT;

        GpioDataRegs.GPASET.all |= (i << ADDRESS_PORT_SHIFT);
        GpioDataRegs.GPACLEAR.all |= ~(i << ADDRESS_PORT_SHIFT);

        CLEAR_OE;
        CLEAR_LAT;

        i++;
        if (i >= I_MAX)
        {
            i = 0;
        }

        for (j = 0; j < WIDTH; j++)
        {
            // First glyph
            if (j < 16)
            {
                if (((glyph_0_0[i] >> j) & 0x01) == 0x01)
                {
                    SET_G1;
                }
                else
                {
                    CLEAR_G1;
                }
                if (((glyph_0_1[i] >> j) & 0x01) == 0x01)
                {
                    SET_R2;
                }
                else
                {
                    CLEAR_R2;
                }
            }

            // Second glyphs
            else if (j < 32)
            {
                if (((glyph_1_0[i] >> (j - 16)) & 0x01) == 0x01)
                {
                    SET_G1;
                }
                else
                {
                    CLEAR_G1;
                }
                if (((glyph_1_1[i] >> (j - 16)) & 0x01) == 0x01)
                {
                    SET_R2;
                }
                else
                {
                    CLEAR_R2;
                }
                if (((glyph_1_0[i] >> (j - 16)) & 0x01) == 0x01)
                {
                    SET_G1;
                }
                else
                {
                    CLEAR_G1;
                }
            }

            // Third glyphs
            else if (j < 48)
            {
                if (((glyph_2_0[i] >> (j - 32)) & 0x01) == 0x01)
                {
                    SET_G1;
                }
                else
                {
                    CLEAR_G1;
                }
                if (((glyph_2_1[i] >> (j - 32)) & 0x01) == 0x01)
                {
                    SET_R2;
                }
                else
                {
                    CLEAR_R2;
                }
            }
            // Fourth glyphs
            else if (j < 64)
            {
                if (((glyph_3_0[i] >> (j - 48)) & 0x01) == 0x01)
                {
                    SET_G1;
                }
                else
                {
                    CLEAR_G1;
                }
                if (((glyph_3_1[i] >> (j - 48)) & 0x01) == 0x01)
                {
                    SET_R2;
                }
                else
                {
                    CLEAR_R2;
                }
            }
            SET_CLK;
            CLEAR_CLK;
        }
        //delayMicroseconds(100);
        //while ((time - CpuTimer1Regs.TIM.all) < ABOUT_1US_OF_BIG_NUMBER) {};
    }
}

void delayMicroseconds(Uint32 delayTimeUS)
{
    Uint32 startTime = CpuTimer1Regs.TIM.all;
    while ((startTime - CpuTimer1Regs.TIM.all) < delayTimeUS) {};
}

void setGpioAsOutput(Uint16 gpioNumber)
{
    GPIO_setMode(myGpio, (GPIO_Number_e) gpioNumber, GPIO_0_Mode_GeneralPurpose);
    GPIO_setDirection(myGpio, (GPIO_Number_e) gpioNumber, GPIO_Direction_Output);
    GPIO_setPullUp(myGpio, (GPIO_Number_e) gpioNumber, GPIO_PullUp_Disable);
}

void initGPIO(void)
{
    myGpio = GPIO_init((void *)GPIO_BASE_ADDR, sizeof(GPIO_Obj));
    setGpioAsOutput(CLK);
    setGpioAsOutput(OE);
    setGpioAsOutput(LAT);
    setGpioAsOutput(A);
    setGpioAsOutput(B);
    setGpioAsOutput(C);
    setGpioAsOutput(D);
    setGpioAsOutput(R1);
    setGpioAsOutput(G1);
    setGpioAsOutput(B1);
    setGpioAsOutput(R2);
    setGpioAsOutput(G2);
    setGpioAsOutput(B2);
    //setGpioAsOutput(GPIO_Number_12); // J2-3
    //setGpioAsOutput(GPIO_Number_6);  // J2-8
    //setGpioAsOutput(GPIO_Number_7);  // J2-9
    CLEAR_CLK;
    SET_OE;
    CLEAR_LAT;
    CLEAR_A;
    CLEAR_B;
    CLEAR_C;
    CLEAR_D;
    CLEAR_R1;
    CLEAR_G1;
    CLEAR_B1;
    CLEAR_R2;
    CLEAR_G2;
    CLEAR_B2;
}

void initTimer(void)
{
    CpuTimer1Regs.TCR.bit.FREE      = 1;                /* Set CpuTimer1 to free run mode   */
    CpuTimer1Regs.TCR.bit.SOFT      = 1;

    CpuTimer1Regs.PRD.all           = 0xFFFFFFFF;       /* Initialize CpuTimer1 period to maximum   */

    /* Initialize pre-scale counter to 0x7A, 1/ (150MHz / 122) = 814nS period   */
    CpuTimer1Regs.TPR.bit.TDDR      = 0x3C;//79;             /* Set Timer1 divide down low 2 bytes with 0x79 = 0x7A - 1  */
    CpuTimer1Regs.TPRH.bit.TDDRH    = 0x00;             /* Set Timer1 divide down high 2 bytes with 0x00            */

    CpuTimer1Regs.TCR.bit.TSS       = 1;                /* Stop CpuTimer1                       */

    CpuTimer1Regs.TCR.bit.TRB       = 1;                /* Reload CpuTimer1 counter register    */

    CpuTimer1.InterruptCount        = 0;                /* Reset CpuTimer1 interrupt counter    */

    CpuTimer1Regs.TCR.bit.TIF       = 1;                /* Clear CpuTimer1 interrupt flag       */

    CpuTimer1Regs.TCR.bit.TIE       = 0;                /* Disable CpuTimer1 interrupt          */
    CpuTimer1Regs.TIM.all           = 0xFFFFFFFF;

    CpuTimer1Regs.TCR.bit.TSS       = 0;                /* Start CpuTimer1                      */
}
