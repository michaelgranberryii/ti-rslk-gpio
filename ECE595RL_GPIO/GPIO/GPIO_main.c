/**
 * @file GPIO_main.c
 * @brief Main source code for the GPIO program.
 *
 * This file contains the main entry point and function definitions for the GPIO program.
 * It interfaces with the following:
 *  - User buttons and LEDs of the TI MSP432 LaunchPad
 *  - PMOD SWT (4 Slide Switches)
 *  - PMOD 8LD (8 LEDs)
 *
 * To verify the pinout of the user buttons and LEDs, refer to the MSP432P401R SimpleLink Microcontroller LaunchPad Development Kit User's Guide
 * Link: https://docs.rs-online.com/3934/A700000006811369.pdf
 *
 * For more information regarding the PMODs used in this lab, visit the following links:
 *  - PMOD SWT: https://digilent.com/reference/pmod/pmodswt/reference-manual
 *  - PMOD 8LD: https://digilent.com/reference/pmod/pmod8ld/reference-manual
 *
 *
 * @note The user buttons, located at P1.1 and P1.4, are configured with negative logic
 * as the default setting. When the buttons are pressed, they connect to GND. Refer to the
 * schematic found in the MSP432P401R LaunchPad User's Guide.
 *
 * @author Aaron Nanas
 */

#include <stdint.h>
#include "msp.h"
#include "../inc/Clock.h"

// Constant definitions for the built-in red LED
const uint8_t RED_LED_OFF           =   0x00;
const uint8_t RED_LED_ON            =   0x01;

// Constant definitions for the RGB LED colors
const uint8_t RGB_LED_OFF           =   0x00;
const uint8_t RGB_LED_RED           =   0x01;
const uint8_t RGB_LED_GREEN         =   0x02;
const uint8_t RGB_LED_YELLOW        =   0x03;
const uint8_t RGB_LED_BLUE          =   0x04;
const uint8_t RGB_LED_PINK          =   0x05;
const uint8_t RGB_LED_SKY_BLUE      =   0x06;
const uint8_t RGB_LED_WHITE         =   0x07;

// Constant definitions for the PMOD 8LD module
const uint8_t PMOD_8LD_ALL_OFF      =   0x00;
const uint8_t PMOD_8LD_ALL_ON       =   0xFF;
const uint8_t PMOD_8LD_0_3_ON       =   0x0F;
const uint8_t PMOD_8LD_4_7_ON       =   0xF0;
const uint8_t PMOD_8LD_0_2_4_6_ON   =   0x55;
const uint8_t PMOD_8LD_1_3_5_7_ON   =   0xAA;


/**
 * @brief The LED1_Init function initializes the built-in red LED (P1.0).
 *
 * This function initializes the built-in red LED located at pin P1.0
 * and configures it as a GPIO pin. It sets the direction of the pin as output.
 *
 * @param None
 *
 * @return None
 */
void LED1_Init()
{
    P1->SEL0 &= ~0x01;
    P1->SEL1 &= ~0x01;
    P1->DIR |= 0x01;
}

/**
 * @brief The LED1_Output function sets the output of the built-in red LED and returns the status.
 *
 * This function sets the output of the built-in red LED based on the value of the input, led_value.
 * A bitwise AND operation (& 0xFE) is performed to mask the first bit (LSB) of the output register
 * to preserve the state of other pins connected to Port 1 while keeping the LED pin unaffected.
 * Then, a bitwise OR operation is performed with led_value to set the LED pin to the desired state
 * specified by led_value.
 *
 * @param led_value An 8-bit unsigned integer that determines the output of the built-in red LED. To turn off
 *                  the LED, set led_value to 0. Otherwise, setting led_value to 1 turns on the LED.
 *
 * @return Indicates the status of the LED.
 *         - 0: LED Off
 *         - 1: LED On
 */
uint8_t LED1_Output(uint8_t led_value)
{
    P1->OUT = (P1->OUT & 0xFE) | led_value;
    return ((P1->OUT != 0) ? 1 : 0);
}

/**
 * @brief The LED2_Init function initializes the RGB LED (P2.0 - P2.2).
 *
 * This function initializes the following RGB LED, configures the pins as GPIO pins with high drive strength,
 * and sets the direction of the pins as output. The RGB LED is off by default upon initialization.
 *  - RGBLED_RED      (P2.0)
 *  - RGBLED_GREEN    (P2.1)
 *  - RGBLED_BLUE     (P2.2)
 *
 * @param None
 *
 * @return None
 */
void LED2_Init()
{
    P2->SEL0 &= ~0x07;
    P2->SEL1 &= ~0x07;
    P2->DS |= 0x07;
    P2->DIR |= 0x07;
    P2->OUT &= ~0x07;
}

/**
 * @brief The LED2_Output function sets the output of the RGB LED and returns the status.
 *
 * This function sets the output of the RGB LED based on the value of the input, led_value.
 * A bitwise AND operation (& 0xF8) is performed to mask the lower three bits of the output register
 * to preserve the state of other pins connected to Port 2 while keeping the RGB LED pin unaffected.
 * Then, a bitwise OR operation is performed with led_value to set the RGB LED pin to the desired state
 * specified by led_value.
 *
 * @param led_value An 8-bit unsigned integer that determines the output of the RGB LED. To turn off
 *                  the RGB LED, set led_value to 0. The following values determine the color of the RGB LED:
 *
 *  Color       LED(s)   led_value
 *  Off         ---         0x00
 *  Red         R--         0x01
 *  Green       -G-         0x02
 *  Yellow      RG-         0x03
 *  Blue        --B         0x04
 *  Pink        R-B         0x05
 *  Sky Blue    -GB         0x06
 *  White       RGB         0x07
 *
 * @return Indicates the status of the RGB LED.
 *          - 0: RGB LED Off
 *          - 1: RGB LED On
 */
uint8_t LED2_Output(uint8_t led_value)
{
    P2->OUT = (P2->OUT & 0xF8) | led_value;
    return ((P2->OUT != 0) ? 1 : 0);
}

/**
 * @brief The Buttons_Init function initializes the user buttons (P1.1 and P1.4).
 *
 * This function initializes the user buttons located at pins P1.1 and P1.4.
 * It configures the pins as GPIO input pins with pull-up resistors enabled.
 *
 * @param None
 *
 * @return None
 */
void Buttons_Init()
{
    P1->SEL0 &= ~0x12;
    P1->SEL1 &= ~0x12;
    P1->DIR &= ~0x12;
    P1->REN |= 0x12;
    P1->OUT |= 0x12;
}

/**
 * @brief The Get_Buttons_Status reads the status of the user buttons (P1.1 and P1.4) and returns it.
 *
 * This function reads the status of the user buttons located at pins P1.1 and P1.4.
 * It indicates whether or not the buttons are pressed and returns the status.
 * A bitwise AND operation (& 0x12) is performed to mask the unused bits in the input register.
 *
 * @param None
 *
 * @return Indicates the status of the buttons.
 *  - 0x00: Button 1 and Button 2 are pressed
 *  - 0x10: Button 1 is pressed while Button 2 is not pressed
 *  - 0x02: Button 1 is not pressed while Button 2 is pressed
 *  - 0x12: Button 1 and Button 2 are not pressed
 */
uint8_t Get_Buttons_Status()
{
    uint8_t button_status = (P1->IN & 0x12);
    return button_status;
}

/**
 * @brief The PMOD_8LD_Init function initializes the pins (P9.0 - P9.7) used by the Digilent PMOD 8LD module.
 *
 * This function initializes the pins, P9.0 through P9.7, which will be used to drive the eight
 * LEDs on the Digilent PMOD 8LD module. It configures the pins as GPIO output pins with high drive strength.
 *
 * The following connections must be made:
 *  - PMOD LED0   <-->  MSP432 LaunchPad Pin P9.0
 *  - PMOD LED1   <-->  MSP432 LaunchPad Pin P9.1
 *  - PMOD LED2   <-->  MSP432 LaunchPad Pin P9.2
 *  - PMOD LED3   <-->  MSP432 LaunchPad Pin P9.3
 *  - PMOD Pin 5  <-->  MSP432 LaunchPad GND
 *  - PMOD Pin 6  <-->  MSP432 LaunchPad VCC (3.3V)
 *  - PMOD LED4   <-->  MSP432 LaunchPad Pin P9.4
 *  - PMOD LED5   <-->  MSP432 LaunchPad Pin P9.5
 *  - PMOD LED6   <-->  MSP432 LaunchPad Pin P9.6
 *  - PMOD LED7   <-->  MSP432 LaunchPad Pin P9.7
 *  - PMOD Pin 11 <-->  MSP432 LaunchPad GND
 *  - PMOD Pin 12 <-->  MSP432 LaunchPad VCC (3.3V)
 *
 * @param None
 *
 * @return None
 */
void PMOD_8LD_Init()
{
    P9->SEL0 &= ~0xFF;
    P9->SEL1 &= ~0xFF;
    P9->DS |= 0xFF;
    P9->DIR |= 0xFF;
    P9->OUT &= ~0xFF;
}

/**
 * @brief The PMOD_8LD_Output function sets the output of the eight LEDs on the PMOD 8LD module.
 *
 * This function sets the output value of the PMOD 8LD module by writing the provided led_value to the
 * corresponding output pins. It then reads back the actual value written to the PMOD 8LD module and returns it.
 *
 * @param led_value An 8-bit unsigned integer representing the desired output value for the PMOD 8LD module.
 *
 * @return The output value written to the PMOD 8LD module.
 *         0: LED Off
 *         1: LED On
 */
uint8_t PMOD_8LD_Output(uint8_t led_value)
{
    P9->OUT = led_value;
    uint8_t PMOD_8LD_value = P9->OUT;
    return PMOD_8LD_value;
}

/**
 * @brief The PMOD_SWT_Init function initializes the pins (P10.0 - P10.3) used by the Digilent PMOD SWT module.
 *
 * This function initializes the pins, P10.0 through P10.3, which will be used to receive the inputs
 * from the four slide switches on the PMOD SWT module. It configures the pins as GPIO input pins.
 *
 * The following connections must be made:
 *  - PMOD SWT1   <-->  MSP432 LaunchPad Pin P10.0
 *  - PMOD SWT2   <-->  MSP432 LaunchPad Pin P10.1
 *  - PMOD SWT3   <-->  MSP432 LaunchPad Pin P10.2
 *  - PMOD SWT4   <-->  MSP432 LaunchPad Pin P10.3
 *  - PMOD Pin 5  <-->  MSP432 LaunchPad GND
 *  - PMOD Pin 6  <-->  MSP432 LaunchPad VCC (3.3V)
 *
 * @param None
 *
 * @return None
 */
void PMOD_SWT_Init()
{
    P10->SEL0 &= ~0xF;
    P10->SEL1 &= ~0xF;
    P10->DIR &= ~0xF;
}

/**
 * @brief The PMOD_SWT_Status function gets the input values of the PMOD SWT.
 *
 * This function reads the values of the four slide switches on the PMOD SWT
 * from the P10 IN register. A bitwise AND operation (& 0xF) is performed to mask
 * the upper four bits of the input register, keeping only the lower four bits
 * that correspond to the switch positions.
 *
 * @param None
 *
 * @return Indicates the status of the switches.
 *
 *  SWT1    SWT2    SWT3    SWT4    switch_status
 *  ----    ----    ----    ----    -------------
 *  OFF     OFF     OFF     OFF         0x00
 *  ON      OFF     OFF     OFF         0x01
 *  OFF     ON      OFF     OFF         0x02
 *  ON      ON      OFF     OFF         0x03
 *  OFF     OFF     ON      OFF         0x04
 *  ON      OFF     ON      OFF         0x05
 *  OFF     ON      ON      OFF         0x06
 *  ON      ON      ON      OFF         0x07
 *  OFF     OFF     OFF     ON          0x08
 *  ON      OFF     OFF     ON          0x09
 *  OFF     ON      OFF     ON          0x0A
 *  ON      ON      OFF     ON          0x0B
 *  OFF     OFF     ON      ON          0x0C
 *  ON      OFF     ON      ON          0x0D
 *  OFF     ON      ON      ON          0x0E
 *  ON      ON      ON      ON          0x0F
 *
 */
uint8_t PMOD_SWT_Status()
{
    uint8_t switch_status = P10->IN & 0xF;
    return switch_status;
}

/**
 * @brief The LED_Pattern_1 function sets the output of the user LEDs and the 8 PMOD LEDs based on the status of the user buttons.
 *
 * This function sets the output of both the built-in red LED (P1.0), the RGB LED (P2.0 - P2.2), and the 8 PMOD LEDs (P9.0 - P9.7) based on
 * the status of the Button 1 (P1.1) and Button 2 (P1.4).
 *
 * @param button_status An 8-bit unsigned integer that indicates the status of Button 1 and Button 2.
 *
 *  button_status      LED 1 Color          RGB LED Color               PMOD 8 LED
 *  -------------      -----------        -----------------             --------------
 *      0x00            1 Hz Flash          1 Hz Blue FLash                 All Off
 *      0x10               On                    Off                     0, 2, 4, 6 ON
 *      0x02               Off                   Pink                    1, 3, 5, 7 ON
 *      0x12               Off                   Green                      All On
 *
 *
 * @return None
 */
void LED_Pattern_1(uint8_t button_status)
{
    switch(button_status)
    {
        // Button 1 and Button 2 are pressed
        case 0x00:
        {
            PMOD_8LD_Output(PMOD_8LD_ALL_OFF);
            LED1_Output(RED_LED_ON);
            LED2_Output(RGB_LED_BLUE);
            Clock_Delay1ms(500);
            LED1_Output(RED_LED_OFF);
            LED2_Output(RGB_LED_OFF);
            Clock_Delay1ms(500);
            break;
        }

        // Button 1 is pressed
        // Button 2 is not pressed
        case 0x10:
        {
            LED1_Output(RED_LED_ON);
            LED2_Output(RGB_LED_OFF);
            PMOD_8LD_Output(PMOD_8LD_0_2_4_6_ON);
            break;
        }

        // Button 1 is not pressed
        // Button 2 is pressed
        case 0x02:
        {
            LED1_Output(RED_LED_OFF);
            LED2_Output(RGB_LED_PINK);
            PMOD_8LD_Output(PMOD_8LD_1_3_5_7_ON);
            break;
        }

        // Button 1 and Button 2 are not pressed
        case 0x12:
        {
            LED1_Output(RED_LED_OFF);
            LED2_Output(RGB_LED_GREEN);
            PMOD_8LD_Output(PMOD_8LD_ALL_ON);
            break;
        }
    }
}

/**
 * @brief The LED_Pattern_2 function controls the user LEDs and the eight LEDs on the PMOD 8LD module.
 *
 * This function turns on LED1 with a red color, sets the RGB LED to display a red color,
 * and then initiates a binary counter pattern on the PMOD 8LD module. The counter starts from 0
 * and increments up to 255 (0xFF) with a delay of 100 ms between each count. The sequence stops if
 * a specific switch status is detected or if led_count has reached 0xFF.
 *
 *
 * @param None
 *
 * @return None
 */
void LED_Pattern_2()
{
    LED1_Output(RED_LED_ON);
    LED2_Output(RGB_LED_RED);

    for (uint8_t led_count = 0; led_count <= 0xFF; led_count++)
    {
        PMOD_8LD_Output(led_count);
        Clock_Delay1ms(100);
        uint8_t switch_status = PMOD_SWT_Status();
        if (switch_status != 0x01)
        {
            break;
        }
    }
}

/**
 * @brief The LED_Pattern_3 function controls the user LEDs and the eight LEDs on the PMOD 8LD module.
 *
 * This function turns off LED1, sets the RGB LED to display a blue color,
 * and then initiates a binary counter pattern on the PMOD 8LD module. The counter starts from 255 (0xFF)
 * and decrements down to 0 with a delay of 100 ms between each count. The sequence runs infinitely until another switch
 * status is detected.
 *
 *
 * @param None
 *
 * @return None
 */
void LED_Pattern_3()
{
    LED1_Output(RED_LED_OFF);
    LED2_Output(RGB_LED_BLUE);

    for (uint8_t led_count = 0xFF; led_count >= 0; led_count--) // infinite loop but the uint8_t variable led_count will roll back to 0xFF after reaching 0
    {
        PMOD_8LD_Output(led_count);
        Clock_Delay1ms(100);
        uint8_t switch_status = PMOD_SWT_Status();
        if (switch_status != 0x02)
        {
            break;
        }
    }
}

/**
 * @brief The LED_Pattern_4 function controls the user LEDs and the eight LEDs on the PMOD 8LD module.
 *
 * This function toggles the red LED1, sets the RGB LED to display a green color and toggle it,
 * and toggles all the LEDs on the PMOD 8LD module. All toggling is done at a rate of 1 Hz.
 *
 *
 * @param None
 *
 * @return None
 */
void LED_Pattern_4()
{
    LED1_Output(RED_LED_ON);
    LED2_Output(RGB_LED_GREEN);
    PMOD_8LD_Output(PMOD_8LD_ALL_ON);
    Clock_Delay1ms(500);
    LED1_Output(RED_LED_OFF);
    LED2_Output(RGB_LED_OFF);
    PMOD_8LD_Output(PMOD_8LD_ALL_OFF);
    Clock_Delay1ms(500);
}

/**
 * @brief The LED_Pattern_5 function controls the user LEDs and the eight LEDs on the PMOD 8LD module.
 *
 * This function turns off LED1, turns off the RGB LED,
 * and displays a ring counter pattern on the PMOD 8LD module. The counter starts from 255 (0xFF)
 * and decrements down to 0 with a delay of 100 ms between each count. The sequence runs for 7 iterations or until another switch
 * status is detected.
 *
 *
 * @param None
 *
 * @return None
 */void LED_Pattern_5()
{
    LED1_Output(RED_LED_OFF);
    LED2_Output(RGB_LED_OFF);
    uint8_t shift_bit = 0x1;
    for (uint8_t led_count = 0x0; led_count <= 0x7; led_count++)
    {
        PMOD_8LD_Output(shift_bit);
        Clock_Delay1ms(500);
        shift_bit = shift_bit << 1;
        uint8_t switch_status = PMOD_SWT_Status();
        if (switch_status != 0x08)
        {
            break;
        }
    }
}

/**
 * @brief The LED_Controller function selects and executes an appropriate LED pattern based on button and switch statuses.
 *
 * This function determines the LED pattern to execute based on the given button status and switch status.
 * It calls different pattern functions based on the switch status value.
 *
 * @param button_status An 8-bit unsigned integer representing the status of the user buttons. This value is used to determine
 *                      the LED pattern in some cases.
 * @param switch_status An 8-bit unsigned integer representing the status of the switches on the PMOD SWT. This value is used
 *                      to select the LED pattern to execute.
 *
 * @return None
 */
void LED_Controller(uint8_t button_status, uint8_t switch_status)
{
    switch(switch_status)
    {
        case 0x00:
        {
            LED_Pattern_1(button_status);
        }
        break;

        case 0x01:
        {
            LED_Pattern_2();
        }
        break;
        case 0x02:
        {
            LED_Pattern_3();
        }
        break;
        case 0x04:
        {
            LED_Pattern_4();
        }
        break;
        case 0x08:
        {
            LED_Pattern_5();
        }
        break;
        default:
        {
            LED_Pattern_1(button_status);
        }
    }
}

int main(void)
{
    // Initialize the 48 MHz Clock
    Clock_Init48MHz();

    // Initialize the built-in red LED and the RGB LEDs
    LED1_Init();
    LED2_Init();

    // Initialize the user buttons
    Buttons_Init();

    // Initialize the PMOD 8LD module
    PMOD_8LD_Init();

    // Initialize the PMOD SWT module
    PMOD_SWT_Init();

    while(1)
    {
        uint8_t button_status = Get_Buttons_Status();
        uint8_t switch_status = PMOD_SWT_Status();
        LED_Controller(button_status, switch_status);
        //Clock_Delay1ms(100);
    }
}
