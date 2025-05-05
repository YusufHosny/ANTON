/**
 * Generated Pins header File
 * 
 * @file pins.h
 * 
 * @defgroup  pinsdriver Pins Driver
 * 
 * @brief This is generated driver header for pins. 
 *        This header file provides APIs for all pins selected in the GUI.
 *
 * @version Driver Version  3.1.1
*/

/*
© [2025] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#ifndef PINS_H
#define PINS_H

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set RB1 aliases
#define SCK_TRIS                 TRISBbits.TRISB1
#define SCK_LAT                  LATBbits.LATB1
#define SCK_PORT                 PORTBbits.RB1
#define SCK_WPU                  WPUBbits.WPUB1
#define SCK_OD                   ODCONBbits.ODCB1
#define SCK_ANS                  ANSELBbits.ANSELB1
#define SCK_SetHigh()            do { LATBbits.LATB1 = 1; } while(0)
#define SCK_SetLow()             do { LATBbits.LATB1 = 0; } while(0)
#define SCK_Toggle()             do { LATBbits.LATB1 = ~LATBbits.LATB1; } while(0)
#define SCK_GetValue()           PORTBbits.RB1
#define SCK_SetDigitalInput()    do { TRISBbits.TRISB1 = 1; } while(0)
#define SCK_SetDigitalOutput()   do { TRISBbits.TRISB1 = 0; } while(0)
#define SCK_SetPullup()          do { WPUBbits.WPUB1 = 1; } while(0)
#define SCK_ResetPullup()        do { WPUBbits.WPUB1 = 0; } while(0)
#define SCK_SetPushPull()        do { ODCONBbits.ODCB1 = 0; } while(0)
#define SCK_SetOpenDrain()       do { ODCONBbits.ODCB1 = 1; } while(0)
#define SCK_SetAnalogMode()      do { ANSELBbits.ANSELB1 = 1; } while(0)
#define SCK_SetDigitalMode()     do { ANSELBbits.ANSELB1 = 0; } while(0)

// get/set RB2 aliases
#define MISO_TRIS                 TRISBbits.TRISB2
#define MISO_LAT                  LATBbits.LATB2
#define MISO_PORT                 PORTBbits.RB2
#define MISO_WPU                  WPUBbits.WPUB2
#define MISO_OD                   ODCONBbits.ODCB2
#define MISO_ANS                  ANSELBbits.ANSELB2
#define MISO_SetHigh()            do { LATBbits.LATB2 = 1; } while(0)
#define MISO_SetLow()             do { LATBbits.LATB2 = 0; } while(0)
#define MISO_Toggle()             do { LATBbits.LATB2 = ~LATBbits.LATB2; } while(0)
#define MISO_GetValue()           PORTBbits.RB2
#define MISO_SetDigitalInput()    do { TRISBbits.TRISB2 = 1; } while(0)
#define MISO_SetDigitalOutput()   do { TRISBbits.TRISB2 = 0; } while(0)
#define MISO_SetPullup()          do { WPUBbits.WPUB2 = 1; } while(0)
#define MISO_ResetPullup()        do { WPUBbits.WPUB2 = 0; } while(0)
#define MISO_SetPushPull()        do { ODCONBbits.ODCB2 = 0; } while(0)
#define MISO_SetOpenDrain()       do { ODCONBbits.ODCB2 = 1; } while(0)
#define MISO_SetAnalogMode()      do { ANSELBbits.ANSELB2 = 1; } while(0)
#define MISO_SetDigitalMode()     do { ANSELBbits.ANSELB2 = 0; } while(0)

// get/set RC4 aliases
#define nRF24_CE_TRIS                 TRISCbits.TRISC4
#define nRF24_CE_LAT                  LATCbits.LATC4
#define nRF24_CE_PORT                 PORTCbits.RC4
#define nRF24_CE_WPU                  WPUCbits.WPUC4
#define nRF24_CE_OD                   ODCONCbits.ODCC4
#define nRF24_CE_ANS                  ANSELCbits.ANSELC4
#define nRF24_CE_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define nRF24_CE_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define nRF24_CE_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define nRF24_CE_GetValue()           PORTCbits.RC4
#define nRF24_CE_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define nRF24_CE_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define nRF24_CE_SetPullup()          do { WPUCbits.WPUC4 = 1; } while(0)
#define nRF24_CE_ResetPullup()        do { WPUCbits.WPUC4 = 0; } while(0)
#define nRF24_CE_SetPushPull()        do { ODCONCbits.ODCC4 = 0; } while(0)
#define nRF24_CE_SetOpenDrain()       do { ODCONCbits.ODCC4 = 1; } while(0)
#define nRF24_CE_SetAnalogMode()      do { ANSELCbits.ANSELC4 = 1; } while(0)
#define nRF24_CE_SetDigitalMode()     do { ANSELCbits.ANSELC4 = 0; } while(0)

// get/set RC5 aliases
#define nRF24_CSN_TRIS                 TRISCbits.TRISC5
#define nRF24_CSN_LAT                  LATCbits.LATC5
#define nRF24_CSN_PORT                 PORTCbits.RC5
#define nRF24_CSN_WPU                  WPUCbits.WPUC5
#define nRF24_CSN_OD                   ODCONCbits.ODCC5
#define nRF24_CSN_ANS                  ANSELCbits.ANSELC5
#define nRF24_CSN_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define nRF24_CSN_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define nRF24_CSN_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define nRF24_CSN_GetValue()           PORTCbits.RC5
#define nRF24_CSN_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define nRF24_CSN_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define nRF24_CSN_SetPullup()          do { WPUCbits.WPUC5 = 1; } while(0)
#define nRF24_CSN_ResetPullup()        do { WPUCbits.WPUC5 = 0; } while(0)
#define nRF24_CSN_SetPushPull()        do { ODCONCbits.ODCC5 = 0; } while(0)
#define nRF24_CSN_SetOpenDrain()       do { ODCONCbits.ODCC5 = 1; } while(0)
#define nRF24_CSN_SetAnalogMode()      do { ANSELCbits.ANSELC5 = 1; } while(0)
#define nRF24_CSN_SetDigitalMode()     do { ANSELCbits.ANSELC5 = 0; } while(0)

// get/set RC6 aliases
#define MOSI_TRIS                 TRISCbits.TRISC6
#define MOSI_LAT                  LATCbits.LATC6
#define MOSI_PORT                 PORTCbits.RC6
#define MOSI_WPU                  WPUCbits.WPUC6
#define MOSI_OD                   ODCONCbits.ODCC6
#define MOSI_ANS                  ANSELCbits.ANSELC6
#define MOSI_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define MOSI_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define MOSI_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define MOSI_GetValue()           PORTCbits.RC6
#define MOSI_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define MOSI_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define MOSI_SetPullup()          do { WPUCbits.WPUC6 = 1; } while(0)
#define MOSI_ResetPullup()        do { WPUCbits.WPUC6 = 0; } while(0)
#define MOSI_SetPushPull()        do { ODCONCbits.ODCC6 = 0; } while(0)
#define MOSI_SetOpenDrain()       do { ODCONCbits.ODCC6 = 1; } while(0)
#define MOSI_SetAnalogMode()      do { ANSELCbits.ANSELC6 = 1; } while(0)
#define MOSI_SetDigitalMode()     do { ANSELCbits.ANSELC6 = 0; } while(0)

// get/set RD3 aliases
#define racketOut_TRIS                 TRISDbits.TRISD3
#define racketOut_LAT                  LATDbits.LATD3
#define racketOut_PORT                 PORTDbits.RD3
#define racketOut_WPU                  WPUDbits.WPUD3
#define racketOut_OD                   ODCONDbits.ODCD3
#define racketOut_ANS                  ANSELDbits.ANSELD3
#define racketOut_SetHigh()            do { LATDbits.LATD3 = 1; } while(0)
#define racketOut_SetLow()             do { LATDbits.LATD3 = 0; } while(0)
#define racketOut_Toggle()             do { LATDbits.LATD3 = ~LATDbits.LATD3; } while(0)
#define racketOut_GetValue()           PORTDbits.RD3
#define racketOut_SetDigitalInput()    do { TRISDbits.TRISD3 = 1; } while(0)
#define racketOut_SetDigitalOutput()   do { TRISDbits.TRISD3 = 0; } while(0)
#define racketOut_SetPullup()          do { WPUDbits.WPUD3 = 1; } while(0)
#define racketOut_ResetPullup()        do { WPUDbits.WPUD3 = 0; } while(0)
#define racketOut_SetPushPull()        do { ODCONDbits.ODCD3 = 0; } while(0)
#define racketOut_SetOpenDrain()       do { ODCONDbits.ODCD3 = 1; } while(0)
#define racketOut_SetAnalogMode()      do { ANSELDbits.ANSELD3 = 1; } while(0)
#define racketOut_SetDigitalMode()     do { ANSELDbits.ANSELD3 = 0; } while(0)

// get/set RF0 aliases
#define servoOut_TRIS                 TRISFbits.TRISF0
#define servoOut_LAT                  LATFbits.LATF0
#define servoOut_PORT                 PORTFbits.RF0
#define servoOut_WPU                  WPUFbits.WPUF0
#define servoOut_OD                   ODCONFbits.ODCF0
#define servoOut_ANS                  ANSELFbits.ANSELF0
#define servoOut_SetHigh()            do { LATFbits.LATF0 = 1; } while(0)
#define servoOut_SetLow()             do { LATFbits.LATF0 = 0; } while(0)
#define servoOut_Toggle()             do { LATFbits.LATF0 = ~LATFbits.LATF0; } while(0)
#define servoOut_GetValue()           PORTFbits.RF0
#define servoOut_SetDigitalInput()    do { TRISFbits.TRISF0 = 1; } while(0)
#define servoOut_SetDigitalOutput()   do { TRISFbits.TRISF0 = 0; } while(0)
#define servoOut_SetPullup()          do { WPUFbits.WPUF0 = 1; } while(0)
#define servoOut_ResetPullup()        do { WPUFbits.WPUF0 = 0; } while(0)
#define servoOut_SetPushPull()        do { ODCONFbits.ODCF0 = 0; } while(0)
#define servoOut_SetOpenDrain()       do { ODCONFbits.ODCF0 = 1; } while(0)
#define servoOut_SetAnalogMode()      do { ANSELFbits.ANSELF0 = 1; } while(0)
#define servoOut_SetDigitalMode()     do { ANSELFbits.ANSELF0 = 0; } while(0)

/**
 * @ingroup  pinsdriver
 * @brief GPIO and peripheral I/O initialization
 * @param none
 * @return none
 */
void PIN_MANAGER_Initialize (void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt on Change Handling routine
 * @param none
 * @return none
 */
void PIN_MANAGER_IOC(void);


#endif // PINS_H
/**
 End of File
*/