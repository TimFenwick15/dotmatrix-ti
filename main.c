#include "Library/F2802x_Device.h"
#include "Library/gpio.h"
#include "Library/F2802x_Examples.h"
#include "Library/flash.h"
#include "glyph.h"
#include "Library/pie.h"
#include "Library/timer.h"
#include "Library/pll.h"
#include "Library/clk.h"
#include <string.h> // memset

#define TIMER_PERIOD_350US (35000) // 350us interrupt to draw a line, 16 lines = 5.6ms, two buffers so 10.2ms, ~80fps
                                   // As slow as I can make it without visible jitter
#define WIDTH (64)                 // Display width in LEDs
#define ADDRESS_PORT_SHIFT (16)    // The address is set using 4 contiguous GPIO, this shifts the 4 bit address to this location
#define ROW_ADDRESS_MAX (16)       // The number of display row addresses (each address maps to two rows, 1 and 17, 2 and 18 etc
#define DEPTH_MAX (2)              // The number of buffers we can store. Means a colour can be on (100%), on every other frame (50%), or off (0%)
#define SHIFT_TO_LOWER_DATA (0)    // A pixel is 6bit, so we store one pixel in the first 8-bits, and another in the next 8bits.
                                   // Shift to extract lower pixel data from the buffer
#define SHIFT_TO_UPPER_DATA (8)    // Shift to extract upper pixel data from the buffer
#define ABOUT_500MS_OF_BIG_NUMBER ((Uint32)1000 * (Uint32)500)
#define ABOUT_200MS_OF_BIG_NUMBER ((Uint32)1000 * (Uint32)200)

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
void writeBuffer(Uint16 d);
void initTimer(void);
interrupt void drawScanLine(void);

static GPIO_Handle myGpio; // initGPIO needs access to this
static PIE_Handle myPie; // drawScanLine needs access to this
static volatile Uint32 currentRow = 0;
static volatile Uint16 depth = 0;
static Uint16 lockDepth = 0;
static Uint16 buffer[DEPTH_MAX][WIDTH * ROW_ADDRESS_MAX / 2]; // The draw buffer. We can fit 2 pixels in a Uint16.
/*static Uint32 writeTimes[10];
static Uint32 drawTimes[10];
static Uint16 writeTimesStored = 0;
static Uint16 drawTimesStored = 0;*/

/**
 * main.c
 */
int main(void)
{
    Uint32 initTime;
    Uint16 loopVar;
    Uint16 recalculateBuffer = 0;

    PLL_Handle myPll = PLL_init((void *)PLL_BASE_ADDR, sizeof(PLL_Obj));
    CLK_Handle myClk = CLK_init((void *)CLK_BASE_ADDR, sizeof(CLK_Obj));
    TIMER_Handle myTimer = TIMER_init((void *)TIMER0_BASE_ADDR, sizeof(TIMER_Obj));
    CPU_Handle myCpu = CPU_init((void *)NULL, sizeof(CPU_Obj));
    FLASH_Handle myFlash = FLASH_init((void *)FLASH_BASE_ADDR, sizeof(FLASH_Obj));
    myPie = PIE_init((void *)PIE_BASE_ADDR, sizeof(PIE_Obj));

    CLK_enableAdcClock(myClk);// Added these, does it help?
    (*Device_cal)();
    CLK_setOscSrc(myClk, CLK_OscSrc_Internal);

    // Init on chip clock thing
    PLL_setup(myPll, PLL_Multiplier_12, PLL_DivideSelect_ClkIn_by_2);
    //InitPll(DSP28_PLLCR, DSP28_DIVSEL);

    CLK_setClkOutPreScaler(myClk, CLK_ClkOutPreScaler_SysClkOut_by_1);


    // Disable the PIE and all interrupts
    PIE_disable(myPie);
    PIE_disableAllInts(myPie);
    CPU_disableGlobalInts(myCpu);
    CPU_clearIntFlags(myCpu);

    // Copy ramfuncs to RAM
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);

    FLASH_setup(myFlash); // Added this, eg made it sound epic

    // Set up interrupts
    PIE_setDefaultIntVectorTable(myPie);
    PIE_enable(myPie);
    PIE_registerPieIntHandler(myPie, PIE_GroupNumber_1, PIE_SubGroupNumber_7, (intVec_t)&drawScanLine);


    TIMER_stop(myTimer);
    TIMER_setPeriod(myTimer, TIMER_PERIOD_350US);
    TIMER_setPreScaler(myTimer, 0);
    TIMER_reload(myTimer);
    TIMER_setEmulationMode(myTimer, TIMER_EmulationMode_StopAfterNextDecrement);
    TIMER_enableInt(myTimer);
    TIMER_start(myTimer);

    initGPIO();
    initTimer();

    for (loopVar = 0; loopVar < DEPTH_MAX; loopVar++)
    {
        writeBuffer(loopVar);
    }

    // Enable CPU INT1 which is connected to CPU-Timer 0:
    CPU_enableInt(myCpu, CPU_IntNumber_1);

    // Enable TINT0 in the PIE: Group 1 interrupt 7
    PIE_enableTimer0Int(myPie);

    // Enable global Interrupts and higher priority real-time debug events
    CPU_enableGlobalInts(myCpu);
    CPU_enableDebugInt(myCpu);

    initTime = CpuTimer1Regs.TIM.all;

    for (;;)
    {
        if ((initTime - CpuTimer1Regs.TIM.all) > (ABOUT_200MS_OF_BIG_NUMBER))
        {
            initTime = CpuTimer1Regs.TIM.all;
            if (red_front_still.dimensions.y > 0)
            {
                red_front_still.dimensions.y -= 1;
                recalculateBuffer = 1;
            }
        }

        if (recalculateBuffer)
        {
    //writeTimes[writeTimesStored++] = CpuTimer1Regs.TIM.all;
            lockDepth = 1;          // Don't allow the draw code to swap buffers
            writeBuffer(1 - depth); // This knows about DEPTH_MAX implicitly. Write over the inactive buffer
            depth = 1 - depth;      // swap to the newly written buffer
            writeBuffer(1 - depth); // write the inactive buffer
            lockDepth = 0;          // Allow the draw code to swap buffers again
    //writeTimes[writeTimesStored++] = CpuTimer1Regs.TIM.all;
            recalculateBuffer = 0;
        }
    }
}

interrupt void drawScanLine(void)
{
    SET_OE;
    SET_LAT;

    GpioDataRegs.GPASET.all |= (currentRow << ADDRESS_PORT_SHIFT);
    GpioDataRegs.GPACLEAR.all |= ~(currentRow << ADDRESS_PORT_SHIFT);

    CLEAR_OE;
    CLEAR_LAT;

    currentRow++;
    if (currentRow >= ROW_ADDRESS_MAX)
    {
        currentRow = 0;
        if (0 == lockDepth)
        {
            depth++;
            if (depth >= DEPTH_MAX)
            {
                depth = 0;
            }
        }
    }

    Uint16 x = 0;
    Uint16 colourPort = 0;
    const Uint16 indexOffset = currentRow * WIDTH / 2; // This would not work for a odd number of pixels in a row

    // Loop unrolling, idea taken from Adafruits Arduino code for a dot matrix display
#define even \
        colourPort = buffer[depth][x++ + indexOffset];\
        GpioDataRegs.GPASET.all |= colourPort & 0x3F;\
        GpioDataRegs.GPACLEAR.all |= (~colourPort) & 0x3F;\
        SET_CLK;\
        CLEAR_CLK;

#define odd \
        colourPort = (colourPort >> SHIFT_TO_UPPER_DATA) & 0x3F;\
        GpioDataRegs.GPASET.all |= colourPort;\
        GpioDataRegs.GPACLEAR.all |= (~colourPort) & 0x3F;\
        SET_CLK;\
        CLEAR_CLK;

    even odd even odd even odd even odd
    even odd even odd even odd even odd
    even odd even odd even odd even odd
    even odd even odd even odd even odd
    even odd even odd even odd even odd
    even odd even odd even odd even odd
    even odd even odd even odd even odd
    even odd even odd even odd even odd

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

void writeBuffer(Uint16 d)
{
    Uint16 index;
    Uint16 colourPort;
    Colour* colour;
    memset(buffer[d], 0, sizeof buffer[d]);

    Uint16 row, column;
    for (row = 0; row < 16; row++)
    {
        for (column = 0; column < 16; column++) // this draws the sprite backwards. Solve this later
        {
            colour = &red_front_still.colour[(red_front_still.pixel[row] >> (column * 2)) & 0x03];
            if (
                (0 == colour->R) &&
                (0 == colour->G) &&
                (0 == colour->B) &&
                (1 == red_front_still.transparency)
            )
            {
                // Do not draw, this is a transparent pixel
            }
            else
            {
                colourPort = 0;
                if ((red_front_still.dimensions.y + row) < 16)
                {
                    index = (red_front_still.dimensions.x + column + (red_front_still.dimensions.y + row) * WIDTH); // pixel index
                    if (colour->R > d)
                    {
                        colourPort |= RED_1;
                    }
                    if (colour->G > d)
                    {
                        colourPort |= GREEN_1;
                    }
                    if (colour->B > d)
                    {
                        colourPort |= BLUE_1;
                    }
                }
                else
                {
                    index = (red_front_still.dimensions.x + column + (red_front_still.dimensions.y + row - 16) * WIDTH); // pixel index
                    if (colour->R > d)
                    {
                        colourPort |= RED_2;
                    }
                    if (colour->G > d)
                    {
                        colourPort |= GREEN_2;
                    }
                    if (colour->B > d)
                    {
                        colourPort |= BLUE_2;
                    }
                }
                if ((index & 0x1) == 0)
                {
                    buffer[d][index >> 1] = colourPort;
                }
                else
                {
                    buffer[d][index >> 1] |= (colourPort << 8);
                }
            }
        }
    }
}

void initTimer(void)
{
    CpuTimer1Regs.TCR.bit.FREE = 1; // Free run mode
    CpuTimer1Regs.TCR.bit.SOFT = 1;
    CpuTimer1Regs.PRD.all = 0xFFFFFFFF; // Initialize CpuTimer1
    CpuTimer1Regs.TPR.bit.TDDR = 0x3C; // 60MHz / 3C = 1us I think
    CpuTimer1Regs.TPRH.bit.TDDRH = 0x00;
    CpuTimer1Regs.TCR.bit.TSS = 1; // Stop CpuTimer1
    CpuTimer1Regs.TCR.bit.TRB = 1; // Reload CpuTimer1 counter register
    CpuTimer1.InterruptCount = 0; // Reset CpuTimer1 interrupt counter
    CpuTimer1Regs.TCR.bit.TIF = 1; // Clear CpuTimer1 interrupt flag
    CpuTimer1Regs.TCR.bit.TIE = 0; // Disable CpuTimer1 interrupt
    CpuTimer1Regs.TIM.all = 0xFFFFFFFF;
    CpuTimer1Regs.TCR.bit.TSS = 0; // Start CpuTimer1
}
