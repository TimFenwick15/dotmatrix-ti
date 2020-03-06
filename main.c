#include "Library/F2802x_Device.h"
#include "Library/gpio.h"
#include "Library/F2802x_Examples.h"
#include "Library/flash.h"
#include "glyph.h"
#include "Library/pie.h"
#include "Library/timer.h"

//#define TIMER_PERIOD_1MS (50000) // 1ms interrupt to draw a line, 16 lines so 16ms per frame, 62.5fps
//#define TIMER_PERIOD_1MS (12500) // 1ms interrupt to draw a line, 16 lines so 16ms per frame, 62.5fps - 4* faster for depth
#define TIMER_PERIOD_1MS (5000) // 1ms interrupt to draw a line, 16 lines so 16ms per frame, 62.5fps - 4* faster for depth
#define WIDTH (64) // Display width in LEDs
#define ADDRESS_PORT_SHIFT (16) // The address is set using 4 contiguous GPIO, this shifts the 4 bit address to this location
#define I_MAX (16) // The number of display row addresses (each address maps to two rows, 1 and 17, 2 and 18 etc
#define DEPTH_MAX (3)

#define PIN_CLK ((Uint16)GPIO_Number_28)
#define PIN_OE  ((Uint16)GPIO_Number_29)
#define PIN_LAT ((Uint16)GPIO_Number_12)
#define PIN_A   ((Uint16)GPIO_Number_16)
#define PIN_B   ((Uint16)GPIO_Number_17)
#define PIN_C   ((Uint16)GPIO_Number_18)
#define PIN_D   ((Uint16)GPIO_Number_19)
#define PIN_R1  ((Uint16)GPIO_Number_0)
#define PIN_G1  ((Uint16)GPIO_Number_1)
#define PIN_B1  ((Uint16)GPIO_Number_2)
#define PIN_R2  ((Uint16)GPIO_Number_3)
#define PIN_G2  ((Uint16)GPIO_Number_4)
#define PIN_B2  ((Uint16)GPIO_Number_5)
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
static Uint16 depth = 0;

/**
 * main.c
 */
int main(void)
{
    Uint16 recalculateBackBuffer = 1;
    Uint16 swapBuffer = 0;
    //Uint16 buffer1[1024]; // We'd need 3* this for the depth information
    //Uint16 buffer2[1024];
    //Uint16 (*buffer)[1024];
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

    /*Uint16 loopVar = 0;
    for (loopVar = 0; loopVar < 32; loopVar++)
        _agumon.pixel[loopVar] = agumon[loopVar];

    _agumon.dimensions.x = 24;
    _agumon.dimensions.y = 8;
    _agumon.dimensions.width = 16;
    _agumon.dimensions.height = 16;
    _agumon.colour[0].R = 0b11;
    _agumon.colour[0].G = 0b11;
    _agumon.colour[0].B = 0b11;
    _agumon.colour[0].unused = 0;
    _agumon.colour[1].R = 0b00;
    _agumon.colour[1].G = 0b00;
    _agumon.colour[1].B = 0b00;
    _agumon.colour[1].unused = 0;
    _agumon.colour[2].R = 0b11;
    _agumon.colour[2].G = 0b11;
    _agumon.colour[2].B = 0b00;
    _agumon.colour[2].unused = 0;
    _agumon.colour[3].R = 0b00;
    _agumon.colour[3].G = 0b00;
    _agumon.colour[3].B = 0b00;
    _agumon.colour[3].unused = 0;*/

    for (;;) {
        if (recalculateBackBuffer)
        {
            //buffer = &buffer1;
            recalculateBackBuffer = 0;
            swapBuffer = 1;
        }
        if (swapBuffer)
        {
            swapBuffer = 0;

        }
    }
}

interrupt void drawScanLine(void)
{
    int16 j = 0;
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
        depth++;
        if (depth >= DEPTH_MAX)
        {
            depth = 0;
        }
    }

    // The glyphs are drawn with the left most bit in the highest bit position.
    // So we need to run this loop backwards so the glyph isn't reflected.
    for (j = WIDTH - 1; j >= 0; j--)
    {
        Uint16 colourPort = 0;
        Colour* upperGlyphColour = 0;
        Colour* lowerGlyphColour = 0;

        // First glyphs
        if (j > 48)
        {
            upperGlyphColour = &glyph_0_0_colour[ (glyph_0_0[i] >> ((j - 48) * 2)) & 0x03 ];
            lowerGlyphColour = &agumon_colour[ (agumon[i] >> ((j - 48) * 2)) & 0x03 ];
        }
        else if (j > 32)
        {
            upperGlyphColour = &glyph_1_0_colour[ (glyph_1_0[i] >> ((j - 32) * 2)) & 0x03 ];
            lowerGlyphColour = &glyph_1_1_colour[ (glyph_1_1[i] >> ((j - 32) * 2)) & 0x03 ];
        }
        else if (j > 16)
        {
            upperGlyphColour = &glyph_2_0_colour[ (glyph_2_0[i] >> ((j - 16) * 2)) & 0x03 ];
            lowerGlyphColour = &glyph_2_1_colour[ (glyph_2_1[i] >> ((j - 16) * 2)) & 0x03 ];
        }
        else if (j >= 0)
        {
            upperGlyphColour = &glyph_3_0_colour[ (glyph_3_0[i] >> (j * 2)) & 0x03 ];
            lowerGlyphColour = &glyph_3_1_colour[ (glyph_3_1[i] >> (j * 2)) & 0x03 ];
        }

        {
            Uint16 x = j;
            Uint16 y1 = i;
            Uint16 y2 = y1 + 16;
            if ((x >= _agumon.dimensions.x) && (x < (_agumon.dimensions.x + _agumon.dimensions.width)))
            {
                if ((y1 >= _agumon.dimensions.y) && (y1 < (_agumon.dimensions.y + _agumon.dimensions.height)))
                {
                    upperGlyphColour = &_agumon.colour[
                        (_agumon.pixel[y1 - _agumon.dimensions.y] >> ((x - _agumon.dimensions.x) * 2)) & 0x03
                    ];
                }
                if ((y2 >= _agumon.dimensions.y) && (y2 < (_agumon.dimensions.y + _agumon.dimensions.height)))
                {
                    lowerGlyphColour = &_agumon.colour[
                        (_agumon.pixel[y2 - _agumon.dimensions.y] >> ((x - _agumon.dimensions.x) * 2)) & 0x03
                    ];
                }
            }
        }
        if (upperGlyphColour->R > depth)
        {
            colourPort |= RED_1;
        }
        if (upperGlyphColour->G > depth)
        {
            colourPort |= GREEN_1;
        }
        if (upperGlyphColour->B > depth)
        {
            colourPort |= BLUE_1;
        }
        if (lowerGlyphColour->R > depth)
        {
            colourPort |= RED_2;
        }
        if (lowerGlyphColour->G > depth)
        {
            colourPort |= GREEN_2;
        }
        if (lowerGlyphColour->B > depth)
        {
            colourPort |= BLUE_2;
        }
        GpioDataRegs.GPASET.all |= colourPort;
        GpioDataRegs.GPACLEAR.all |= ~colourPort;
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
    setGpioAsOutput(PIN_CLK);
    setGpioAsOutput(PIN_OE);
    setGpioAsOutput(PIN_LAT);
    setGpioAsOutput(PIN_A);
    setGpioAsOutput(PIN_B);
    setGpioAsOutput(PIN_C);
    setGpioAsOutput(PIN_D);
    setGpioAsOutput(PIN_R1);
    setGpioAsOutput(PIN_G1);
    setGpioAsOutput(PIN_B1);
    setGpioAsOutput(PIN_R2);
    setGpioAsOutput(PIN_G2);
    setGpioAsOutput(PIN_B2);
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
