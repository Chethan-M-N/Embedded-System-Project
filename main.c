#include "lcd.h"
#include "driverlib.h"

volatile unsigned int counter = 0;
unsigned long counter1 = 0;



//In running time to vary the battery backup memory variable to track status through LPM3.5
volatile unsigned char *S1buttonDebounce = &BAKMEM2_L; //S1 button debounce flag
volatile unsigned char *S2buttonDebounce = &BAKMEM2_H; //S2 button debounce flag

int main(void)
{
    WDT_A_hold(WDT_A_BASE); //Stop Watchdog timer

    initGPIO(); //Initialize pins
    LCD_init(); //Initialize LCD
    LCD_displayNumber(counter1);

    *S1buttonDebounce = *S2buttonDebounce = 0; // S1 S2 Debounce
    TA1CCR0 = 1000-1; //Set the PWM Period in the Timer A1 Capture/Compare 0 register to 1000us.
    TA1CTL = TASSEL__ACLK | MC__UP | TACLR;//ACLK, up mode, clear TAR
    __bis_SR_register(GIE + LPM3_bits); // Enter LPM3 mode, Enable interrupts globally
    return 0;
}



#pragma vector=PORT1_VECTOR
__interrupt void pushbutton_ISR(void)
{
    switch (__even_in_range(P1IV, P1IV_P1IFG7))
    {
    case P1IV_NONE:
        break;  // None

    case P1IV_P1IFG0:
        __no_operation();
        break;  // Pin 0

    case P1IV_P1IFG1:
        __no_operation();
        break;  // Pin 1

    case P1IV_P1IFG2:   // Pin 2 (button 1)
        if((*S1buttonDebounce) == 0)
        {
            *S1buttonDebounce = 1; //High to low transition
            counter = 0;
        }
        Switch1();
        break;

    case P1IV_P1IFG3:
            __no_operation();
            break;  // Pin 3

    case P1IV_P1IFG4:
            __no_operation();
            break;  // Pin 4

    case P1IV_P1IFG5:
            __no_operation();
            break;  // Pin 5

    case P1IV_P1IFG6:
            __no_operation();
            break;  // Pin 6

    case P1IV_P1IFG7:
            __no_operation();
           break;   // Pin 7

    default:
       _never_executed();
    }
}


#pragma vector=PORT2_VECTOR
__interrupt void pushbutton_ISR2(void)
{
    switch (__even_in_range(P2IV, P2IV_P2IFG7))
    {
    case P2IV_NONE:
        break;  // None

    case P2IV_P2IFG0:
        __no_operation();
        break;  // Pin 0

    case P2IV_P2IFG1:
        __no_operation();
        break;  // Pin 1

    case P2IV_P2IFG2:
       __no_operation();
        break;  // Pin 2

    case P2IV_P2IFG3:
        __no_operation();
        break;  // Pin 3

    case P2IV_P2IFG4:
        __no_operation();
        break;  // Pin 4

    case P2IV_P2IFG5:
        __no_operation();
        break;  // Pin 5

    case P2IV_P2IFG6:   // Pin 6 (button 2)
        if((*S2buttonDebounce) == 0)
               {
                   *S2buttonDebounce = 1;   // First high to low transition
                   counter = 0;
               }
            Switch2();
            break;

    case P2IV_P2IFG7:
            __no_operation();
           break;   // Pin 7

    default:
       _never_executed();
    }
}


void initGPIO(void)
{
    // Set pin P1.0 to output direction and turn LED off
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // Red LED (LED1)
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0); // Green LED (LED2)
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0);

    PMM_unlockLPM5();  //unlocking all the pins because it will freeze

    // Set P1.2 as input with pull-up resistor (for push button S1)
    // Configure interrupt on low to high transition and then clear flag
    // and enable the interrupt
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN2);
    GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN2, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN2);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN2);

    // Set P2.6 as input with pull-up resistor (for push button S2)
    // Configure interrupt on low to high transition and then clear flag
    // and enable the interrupt
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_selectInterruptEdge(GPIO_PORT_P2, GPIO_PIN6, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    displayScrollText("PRESS S1 TO INCREASE AND S2 TO DECREASE THE CAR SPEED");
}



void Switch1(void)
{
    LCD_displayNumber(counter1);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

    if(counter1 == 3)
    {
        displayScrollText("MAX LIMIT REACHED");
        return;
    }
    counter1 = counter1 + 1;

        TA1CCR1 = counter1 * 100;// CCR1 PWM duty cycle.
                                //It is half the time, which translates to 50% duty cycle
        TA1CTL = TASSEL_2 | MC_1; //ACLK, UPMODE, start timer
        TA1CCTL1 = OUTMOD_7; // CCR1 reset/set

    P1DIR |= BIT7;  // Setting P1.7 as PWM output
    P1SEL0 |= BIT7; // P1.7 output PWM

    displayScrollText("INCREASING SPEED LEVEL");
    LCD_displayNumber(TA1CCR1/100);

    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN0);
}

void Switch2(void)
{
    LCD_displayNumber(counter1);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN0);

    if(counter1 == 0)
    {
        displayScrollText("LOW LIMIT REACHED");
        return;
    }
    counter1 = counter1 - 1;

        TA1CCR1 = counter1 * 100;//PWM DUITY CYCLE
        TA1CTL = TASSEL_2 | MC_1;
        TA1CCTL1 = OUTMOD_7;

    P1DIR |= BIT7; // Setting P1.7 as PWM output
    P1SEL0 |= BIT7;

    displayScrollText("DECREASING SPEED LEVEL");
    LCD_displayNumber(TA1CCR1/100);

    GPIO_clearInterrupt(GPIO_PORT_P2, GPIO_PIN6);
}


