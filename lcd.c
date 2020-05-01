#include "lcd.h"
#include "driverlib.h"

// Memory locations for LCD characters
const unsigned int location[NUM_CHAR] =
{
        pos1,   // Digit A1
        pos2,   // Digit A2
        pos3,   // Digit A3
        pos4,   // Digit A4
        pos5,   // Digit A5
        pos6    // Digit A6
};

// LCD memory map for numeric digits
const char digit[10][2] =
{
        {0xFC, 0x28},  /* "0" LCD segments a+b+c+d+e+f+k+q */
        {0x60, 0x20},  /* "1" */
        {0xDB, 0x00},  /* "2" */
        {0xF3, 0x00},  /* "3" */
        {0x67, 0x00},  /* "4" */
        {0xB7, 0x00},  /* "5" */
        {0xBF, 0x00},  /* "6" */
        {0xE4, 0x00},  /* "7" */
        {0xFF, 0x00},  /* "8" */
        {0xF7, 0x00}   /* "9" */
};

// LCD memory map for uppercase letters
const char alphabetBig[26][2] =
{
        {0xEF, 0x00},  /* "A" LCD segments a+b+c+e+f+g+m */
        {0xF1, 0x50},  /* "B" */
        {0x9C, 0x00},  /* "C" */
        {0xF0, 0x50},  /* "D" */
        {0x9F, 0x00},  /* "E" */
        {0x8F, 0x00},  /* "F" */
        {0xBD, 0x00},  /* "G" */
        {0x6F, 0x00},  /* "H" */
        {0x90, 0x50},  /* "I" */
        {0x78, 0x00},  /* "J" */
        {0x0E, 0x22},  /* "K" */
        {0x1C, 0x00},  /* "L" */
        {0x6C, 0xA0},  /* "M" */
        {0x6C, 0x82},  /* "N" */
        {0xFC, 0x00},  /* "O" */
        {0xCF, 0x00},  /* "P" */
        {0xFC, 0x02},  /* "Q" */
        {0xCF, 0x02},  /* "R" */
        {0xB7, 0x00},  /* "S" */
        {0x80, 0x50},  /* "T" */
        {0x7C, 0x00},  /* "U" */
        {0x0C, 0x28},  /* "V" */
        {0x6C, 0x0A},  /* "W" */
        {0x00, 0xAA},  /* "X" */
        {0x00, 0xB0},  /* "Y" */
        {0x90, 0x28}   /* "Z" */
};

void LCD_init()
{
    // Turn off LCD
    LCD_E_off(LCD_E_BASE);

    // L0~L26 & L36~L39 pins selected
    LCD_E_setPinAsLCDFunctionEx( LCD_E_BASE,         // Gives range of pins to LCD controller
            LCD_E_SEGMENT_LINE_0,                    // Starting Lx pin of range
            LCD_E_SEGMENT_LINE_26                    // Ending pin of range
    );

    LCD_E_setPinAsLCDFunctionEx( LCD_E_BASE,         // Gives range of pins to LCD controller
            LCD_E_SEGMENT_LINE_36,                   // Starting Lx pin of range
            LCD_E_SEGMENT_LINE_39                    // Ending pin of range
    );

    LCD_E_initParam initParams = LCD_E_INIT_PARAM;
    initParams.clockDivider = LCD_E_CLOCKDIVIDER_3;
    initParams.muxRate = LCD_E_4_MUX;
    initParams.segments = LCD_E_SEGMENTS_ENABLED;

    // Init LCD as 4-mux mode
    LCD_E_init(LCD_E_BASE, &initParams);

    // LCD Operation - Mode 3, internal 3.02v, charge pump 256Hz
    LCD_E_setVLCDSource(LCD_E_BASE, LCD_E_INTERNAL_REFERENCE_VOLTAGE, LCD_E_EXTERNAL_SUPPLY_VOLTAGE);
    LCD_E_setVLCDVoltage(LCD_E_BASE, LCD_E_REFERENCE_VOLTAGE_2_96V);

    LCD_E_enableChargePump(LCD_E_BASE);
    LCD_E_setChargePumpFreq(LCD_E_BASE, LCD_E_CHARGEPUMP_FREQ_16);

    // Clear LCD memory
    LCD_E_clearAllMemory(LCD_E_BASE);
    LCD_E_clearAllBlinkingMemory(LCD_E_BASE);

    // Configure COMs and SEGs
    // L0 = COM0, L1 = COM1, L2 = COM2, L3 = COM3
    LCD_E_setPinAsCOM(LCD_E_BASE, LCD_E_SEGMENT_LINE_0, LCD_E_MEMORY_COM0);
    LCD_E_setPinAsCOM(LCD_E_BASE, LCD_E_SEGMENT_LINE_1, LCD_E_MEMORY_COM1);
    LCD_E_setPinAsCOM(LCD_E_BASE, LCD_E_SEGMENT_LINE_2, LCD_E_MEMORY_COM2);
    LCD_E_setPinAsCOM(LCD_E_BASE, LCD_E_SEGMENT_LINE_3, LCD_E_MEMORY_COM3);

    // Select to display main LCD memory
    LCD_E_selectDisplayMemory(LCD_E_BASE, LCD_E_DISPLAYSOURCE_MEMORY);

    // Turn blinking features off
    LCD_E_setBlinkingControl(LCD_E_BASE, LCD_E_BLINK_FREQ_CLOCK_PRESCALAR_64, LCD_E_BLINK_MODE_DISABLED);

    // Turn on LCD
    LCD_E_on(LCD_E_BASE);
}

/*
 * Displays input character at given LCD digit/position
 * Only spaces, numeric digits, and uppercase letters are accepted characters
 */
void LCD_showChar(char c, int position)
{
    if (c == ' ')
    {
        // Display space
        LCD_E_setMemory(LCD_E_BASE, location[position], 0);
        LCD_E_setMemory(LCD_E_BASE, location[position] + 1, 0);
    }
    else if (c >= '0' && c <= '9')
    {
        // Display digit
        LCD_E_setMemory(LCD_E_BASE, location[position], digit[c-48][0]);
        LCD_E_setMemory(LCD_E_BASE, location[position] + 1, digit[c-48][1]);
    }
    else if (c >= 'A' && c <= 'Z')
    {
        // Display alphabet
        LCD_E_setMemory(LCD_E_BASE, location[position], alphabetBig[c-65][0] );
        LCD_E_setMemory(LCD_E_BASE, location[position] + 1, alphabetBig[c-65][1] );
    }
    else
    {
        // Turn all segments on if character is not a space, digit, or uppercase letter
        LCD_E_setMemory(LCD_E_BASE, location[position], 0xFF);
        LCD_E_setMemory(LCD_E_BASE, location[position] + 1, 0xFF);
    }
}

void LCD_displayNumber(unsigned long val)
{
    unsigned long div[6];
    unsigned long mod[6];
    unsigned long err;

    err = val / 1000000;
    if (err > 0)
    {
        LCD_showChar(' ', 0);
        LCD_showChar('E', 1);
        LCD_showChar('R', 2);
        LCD_showChar('R', 3);
        LCD_showChar('O', 4);
        LCD_showChar('R', 5);
    }
    else
    {
        div[0] = val / 100000;
        mod[0] = val % 100000;
        div[1] = mod[0] / 10000;
        mod[1] = mod[0] % 10000;
        div[2] = mod[1] / 1000;
        mod[2] = mod[1] % 1000;
        div[3] = mod[2] / 100 ;
        mod[3] = mod[2] % 100 ;
        div[4] = mod[3] / 10 ;
        div[5] = mod[3] % 10 ;

        int i = 0;
        int LeadingZeros = 0;

        for ( i = 0; i < (NUM_CHAR - 1); i++ )
        {
            if ( ( div[i] == 0 ) && ( !LeadingZeros ) )
            {
                LCD_showChar( ' ', i );
            }
            else
            {
                LCD_showChar( div[i] + '0', i );
                LeadingZeros++;
            }
        }

        i = NUM_CHAR - 1;
        LCD_showChar( div[i] + '0', i );
    }
}

/*
 * Scrolls input string across LCD screen from left to right
 */
void displayScrollText(char *msg)
{
    int length = strlen(msg);
    int i;
    int s = 5;
    char buffer[6] = "      ";
    for (i=0; i<length+7; i++)
    {
        int t;
        for (t=0; t<6; t++)
            buffer[t] = ' ';
        int j;
        for (j=0; j<length; j++)
        {
            if (((s+j) >= 0) && ((s+j) < 6))
                buffer[s+j] = msg[j];
        }
        s--;

        LCD_showChar(buffer[0], 0);
        LCD_showChar(buffer[1], 1);
        LCD_showChar(buffer[2], 2);
        LCD_showChar(buffer[3], 3);
        LCD_showChar(buffer[4], 4);
        LCD_showChar(buffer[5], 5);

        __delay_cycles(200000);
    }
}
