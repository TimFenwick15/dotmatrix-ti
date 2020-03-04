#include "Library/F2802x_Device.h"
#include "Library/gpio.h"
#include "Library/F2802x_Examples.h"
#include "Library/flash.h"
#include "glyph.h"
#include "Library/pie.h"
#include "Library/timer.h"

#define TIMER_PERIOD_1MS (50000) // 1ms interrupt to draw a line, 16 lines so 16ms per frame, 62.5fps
#define WIDTH (64) // Display width in LEDs
#define ADDRESS_PORT_SHIFT (16) // The address is set using 4 contiguous GPIO, this shifts the 4 bit address to this location
#define I_MAX (16) // The number of display row addresses (each address maps to two rows, 1 and 17, 2 and 18 etc

#define CLK ((Uint16)GPIO_Number_28)
#define OE  ((Uint16)GPIO_Number_29)
#define LAT ((Uint16)GPIO_Number_12)
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
#define CLEAR_CLK (GpioDataRegs.GPACLEAR.bit.GPIO28 = 1) // J1-3
#define CLEAR_OE  (GpioDataRegs.GPACLEAR.bit.GPIO29 = 1) // J1-4
#define CLEAR_LAT (GpioDataRegs.GPACLEAR.bit.GPIO12 = 1) // J1-5 Pulled up via S1-1 is up - was 34 J1-5, go to 12 J2-3
#define CLEAR_A   (GpioDataRegs.GPACLEAR.bit.GPIO16 = 1) // J6-7
#define CLEAR_B   (GpioDataRegs.GPACLEAR.bit.GPIO17 = 1) // J6-8 Shared with GPIO33
#define CLEAR_C   (GpioDataRegs.GPACLEAR.bit.GPIO18 = 1) // J1-7 Shared with GPIO32
#define CLEAR_D   (GpioDataRegs.GPACLEAR.bit.GPIO19 = 1) // J2-2
#define CLEAR_R1  (GpioDataRegs.GPACLEAR.bit.GPIO0 = 1)  // J6-1
#define CLEAR_G1  (GpioDataRegs.GPACLEAR.bit.GPIO1 = 1)  // J6-2
#define CLEAR_B1  (GpioDataRegs.GPACLEAR.bit.GPIO2 = 1)  // J6-3
#define CLEAR_R2  (GpioDataRegs.GPACLEAR.bit.GPIO3 = 1)  // J6-4
#define CLEAR_G2  (GpioDataRegs.GPACLEAR.bit.GPIO4 = 1)  // J6-5
#define CLEAR_B2  (GpioDataRegs.GPACLEAR.bit.GPIO5 = 1)  // J6-6
#define SET_CLK   (GpioDataRegs.GPASET.bit.GPIO28 = 1)
#define SET_OE    (GpioDataRegs.GPASET.bit.GPIO29 = 1)
#define SET_LAT   (GpioDataRegs.GPASET.bit.GPIO12 = 1)
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

void initGPIO(void);
void setGpioAsOutput(Uint16 gpioNumber);
interrupt void drawScanLine(void);

static GPIO_Handle myGpio; // initGPIO needs access to this
static PIE_Handle myPie; // drawScanLine needs access to this
static Uint32 i = 0;

/**
 * main.c
 */
int main(void)
{
    TIMER_Handle myTimer = TIMER_init((void *)TIMER0_BASE_ADDR, sizeof(TIMER_Obj));
    CPU_Handle myCpu = CPU_init((void *)NULL, sizeof(CPU_Obj));
    myPie = PIE_init((void *)PIE_BASE_ADDR, sizeof(PIE_Obj));

    // Init on chip clock thing
    InitPll(DSP28_PLLCR, DSP28_DIVSEL);

    // Disable the PIE and all interrupts
    PIE_disable(myPie);
    PIE_disableAllInts(myPie);
    CPU_disableGlobalInts(myCpu);
    CPU_clearIntFlags(myCpu);

    // Copy ramfuncs to RAM
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);

    // Set up interrupts
    PIE_setDefaultIntVectorTable(myPie);
    PIE_enable(myPie);
    PIE_registerPieIntHandler(myPie, PIE_GroupNumber_1, PIE_SubGroupNumber_7, (intVec_t)&drawScanLine);

    // Set up the interrupt timer. Example code said 500ms = 50 * 500000, I don't know how they work that out
    TIMER_stop(myTimer);
    TIMER_setPeriod(myTimer, TIMER_PERIOD_1MS);
    TIMER_setPreScaler(myTimer, 0);
    TIMER_reload(myTimer);
    TIMER_setEmulationMode(myTimer, TIMER_EmulationMode_StopAfterNextDecrement);
    TIMER_enableInt(myTimer);
    TIMER_start(myTimer);

    initGPIO();

    // Enable CPU INT1 which is connected to CPU-Timer 0:
    CPU_enableInt(myCpu, CPU_IntNumber_1);

    // Enable TINT0 in the PIE: Group 1 interrupt 7
    PIE_enableTimer0Int(myPie);

    // Enable global Interrupts and higher priority real-time debug events
    CPU_enableGlobalInts(myCpu);
    CPU_enableDebugInt(myCpu);

    for (;;) {

    }
}

interrupt void drawScanLine(void)
{
    Uint16 j = 0;
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

    // Re-arm the interrupt
    PIE_clearInt(myPie, PIE_GroupNumber_1);
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
