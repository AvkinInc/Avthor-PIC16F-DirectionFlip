/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC16F1454
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.36 and above
        MPLAB 	          :  MPLAB X 6.00	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set MS3 aliases
#define MS3_TRIS                 TRISAbits.TRISA4
#define MS3_LAT                  LATAbits.LATA4
#define MS3_PORT                 PORTAbits.RA4
#define MS3_WPU                  WPUAbits.WPUA4
#define MS3_ANS                  ANSELAbits.ANSA4
#define MS3_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define MS3_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define MS3_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define MS3_GetValue()           PORTAbits.RA4
#define MS3_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define MS3_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define MS3_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define MS3_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define MS3_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define MS3_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set MS1 aliases
#define MS1_TRIS                 TRISAbits.TRISA5
#define MS1_LAT                  LATAbits.LATA5
#define MS1_PORT                 PORTAbits.RA5
#define MS1_WPU                  WPUAbits.WPUA5
#define MS1_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define MS1_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define MS1_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define MS1_GetValue()           PORTAbits.RA5
#define MS1_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define MS1_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define MS1_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define MS1_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)

// get/set DIR aliases
#define DIR_TRIS                 TRISCbits.TRISC0
#define DIR_LAT                  LATCbits.LATC0
#define DIR_PORT                 PORTCbits.RC0
#define DIR_ANS                  ANSELCbits.ANSC0
#define DIR_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define DIR_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define DIR_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define DIR_GetValue()           PORTCbits.RC0
#define DIR_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define DIR_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define DIR_SetAnalogMode()      do { ANSELCbits.ANSC0 = 1; } while(0)
#define DIR_SetDigitalMode()     do { ANSELCbits.ANSC0 = 0; } while(0)

// get/set STEP aliases
#define STEP_TRIS                 TRISCbits.TRISC1
#define STEP_LAT                  LATCbits.LATC1
#define STEP_PORT                 PORTCbits.RC1
#define STEP_ANS                  ANSELCbits.ANSC1
#define STEP_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define STEP_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define STEP_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define STEP_GetValue()           PORTCbits.RC1
#define STEP_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define STEP_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define STEP_SetAnalogMode()      do { ANSELCbits.ANSC1 = 1; } while(0)
#define STEP_SetDigitalMode()     do { ANSELCbits.ANSC1 = 0; } while(0)

// get/set SLEEP aliases
#define SLEEP_TRIS                 TRISCbits.TRISC2
#define SLEEP_LAT                  LATCbits.LATC2
#define SLEEP_PORT                 PORTCbits.RC2
#define SLEEP_ANS                  ANSELCbits.ANSC2
#define SLEEP_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define SLEEP_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define SLEEP_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define SLEEP_GetValue()           PORTCbits.RC2
#define SLEEP_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define SLEEP_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define SLEEP_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define SLEEP_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set MS2 aliases
#define MS2_TRIS                 TRISCbits.TRISC3
#define MS2_LAT                  LATCbits.LATC3
#define MS2_PORT                 PORTCbits.RC3
#define MS2_ANS                  ANSELCbits.ANSC3
#define MS2_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define MS2_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define MS2_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define MS2_GetValue()           PORTCbits.RC3
#define MS2_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define MS2_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define MS2_SetAnalogMode()      do { ANSELCbits.ANSC3 = 1; } while(0)
#define MS2_SetDigitalMode()     do { ANSELCbits.ANSC3 = 0; } while(0)

// get/set RC4 procedures
#define RC4_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define RC4_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define RC4_GetValue()              PORTCbits.RC4
#define RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)

// get/set RC5 procedures
#define RC5_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define RC5_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define RC5_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define RC5_GetValue()              PORTCbits.RC5
#define RC5_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define RC5_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/