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
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC18F26K22
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.05 and above
        MPLAB 	          :  MPLAB X 5.20	
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

// get/set COLUMN_2 aliases
#define COLUMN_2_TRIS                 TRISAbits.TRISA0
#define COLUMN_2_LAT                  LATAbits.LATA0
#define COLUMN_2_PORT                 PORTAbits.RA0
#define COLUMN_2_ANS                  ANSELAbits.ANSA0
#define COLUMN_2_SetHigh()            do { LATAbits.LATA0 = 1; } while(0)
#define COLUMN_2_SetLow()             do { LATAbits.LATA0 = 0; } while(0)
#define COLUMN_2_Toggle()             do { LATAbits.LATA0 = ~LATAbits.LATA0; } while(0)
#define COLUMN_2_GetValue()           PORTAbits.RA0
#define COLUMN_2_SetDigitalInput()    do { TRISAbits.TRISA0 = 1; } while(0)
#define COLUMN_2_SetDigitalOutput()   do { TRISAbits.TRISA0 = 0; } while(0)
#define COLUMN_2_SetAnalogMode()      do { ANSELAbits.ANSA0 = 1; } while(0)
#define COLUMN_2_SetDigitalMode()     do { ANSELAbits.ANSA0 = 0; } while(0)

// get/set ROW_3 aliases
#define ROW_3_TRIS                 TRISAbits.TRISA1
#define ROW_3_LAT                  LATAbits.LATA1
#define ROW_3_PORT                 PORTAbits.RA1
#define ROW_3_ANS                  ANSELAbits.ANSA1
#define ROW_3_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define ROW_3_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define ROW_3_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define ROW_3_GetValue()           PORTAbits.RA1
#define ROW_3_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define ROW_3_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define ROW_3_SetAnalogMode()      do { ANSELAbits.ANSA1 = 1; } while(0)
#define ROW_3_SetDigitalMode()     do { ANSELAbits.ANSA1 = 0; } while(0)

// get/set ROW_4 aliases
#define ROW_4_TRIS                 TRISAbits.TRISA4
#define ROW_4_LAT                  LATAbits.LATA4
#define ROW_4_PORT                 PORTAbits.RA4
#define ROW_4_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define ROW_4_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define ROW_4_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define ROW_4_GetValue()           PORTAbits.RA4
#define ROW_4_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define ROW_4_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)

// get/set COLUMN_3 aliases
#define COLUMN_3_TRIS                 TRISAbits.TRISA6
#define COLUMN_3_LAT                  LATAbits.LATA6
#define COLUMN_3_PORT                 PORTAbits.RA6
#define COLUMN_3_SetHigh()            do { LATAbits.LATA6 = 1; } while(0)
#define COLUMN_3_SetLow()             do { LATAbits.LATA6 = 0; } while(0)
#define COLUMN_3_Toggle()             do { LATAbits.LATA6 = ~LATAbits.LATA6; } while(0)
#define COLUMN_3_GetValue()           PORTAbits.RA6
#define COLUMN_3_SetDigitalInput()    do { TRISAbits.TRISA6 = 1; } while(0)
#define COLUMN_3_SetDigitalOutput()   do { TRISAbits.TRISA6 = 0; } while(0)

// get/set SCK2 aliases
#define SCK2_TRIS                 TRISBbits.TRISB1
#define SCK2_LAT                  LATBbits.LATB1
#define SCK2_PORT                 PORTBbits.RB1
#define SCK2_WPU                  WPUBbits.WPUB1
#define SCK2_ANS                  ANSELBbits.ANSB1
#define SCK2_SetHigh()            do { LATBbits.LATB1 = 1; } while(0)
#define SCK2_SetLow()             do { LATBbits.LATB1 = 0; } while(0)
#define SCK2_Toggle()             do { LATBbits.LATB1 = ~LATBbits.LATB1; } while(0)
#define SCK2_GetValue()           PORTBbits.RB1
#define SCK2_SetDigitalInput()    do { TRISBbits.TRISB1 = 1; } while(0)
#define SCK2_SetDigitalOutput()   do { TRISBbits.TRISB1 = 0; } while(0)
#define SCK2_SetPullup()          do { WPUBbits.WPUB1 = 1; } while(0)
#define SCK2_ResetPullup()        do { WPUBbits.WPUB1 = 0; } while(0)
#define SCK2_SetAnalogMode()      do { ANSELBbits.ANSB1 = 1; } while(0)
#define SCK2_SetDigitalMode()     do { ANSELBbits.ANSB1 = 0; } while(0)

// get/set SDI2 aliases
#define SDI2_TRIS                 TRISBbits.TRISB2
#define SDI2_LAT                  LATBbits.LATB2
#define SDI2_PORT                 PORTBbits.RB2
#define SDI2_WPU                  WPUBbits.WPUB2
#define SDI2_ANS                  ANSELBbits.ANSB2
#define SDI2_SetHigh()            do { LATBbits.LATB2 = 1; } while(0)
#define SDI2_SetLow()             do { LATBbits.LATB2 = 0; } while(0)
#define SDI2_Toggle()             do { LATBbits.LATB2 = ~LATBbits.LATB2; } while(0)
#define SDI2_GetValue()           PORTBbits.RB2
#define SDI2_SetDigitalInput()    do { TRISBbits.TRISB2 = 1; } while(0)
#define SDI2_SetDigitalOutput()   do { TRISBbits.TRISB2 = 0; } while(0)
#define SDI2_SetPullup()          do { WPUBbits.WPUB2 = 1; } while(0)
#define SDI2_ResetPullup()        do { WPUBbits.WPUB2 = 0; } while(0)
#define SDI2_SetAnalogMode()      do { ANSELBbits.ANSB2 = 1; } while(0)
#define SDI2_SetDigitalMode()     do { ANSELBbits.ANSB2 = 0; } while(0)

// get/set SDO2 aliases
#define SDO2_TRIS                 TRISBbits.TRISB3
#define SDO2_LAT                  LATBbits.LATB3
#define SDO2_PORT                 PORTBbits.RB3
#define SDO2_WPU                  WPUBbits.WPUB3
#define SDO2_ANS                  ANSELBbits.ANSB3
#define SDO2_SetHigh()            do { LATBbits.LATB3 = 1; } while(0)
#define SDO2_SetLow()             do { LATBbits.LATB3 = 0; } while(0)
#define SDO2_Toggle()             do { LATBbits.LATB3 = ~LATBbits.LATB3; } while(0)
#define SDO2_GetValue()           PORTBbits.RB3
#define SDO2_SetDigitalInput()    do { TRISBbits.TRISB3 = 1; } while(0)
#define SDO2_SetDigitalOutput()   do { TRISBbits.TRISB3 = 0; } while(0)
#define SDO2_SetPullup()          do { WPUBbits.WPUB3 = 1; } while(0)
#define SDO2_ResetPullup()        do { WPUBbits.WPUB3 = 0; } while(0)
#define SDO2_SetAnalogMode()      do { ANSELBbits.ANSB3 = 1; } while(0)
#define SDO2_SetDigitalMode()     do { ANSELBbits.ANSB3 = 0; } while(0)

// get/set CS aliases
#define CS_TRIS                 TRISBbits.TRISB4
#define CS_LAT                  LATBbits.LATB4
#define CS_PORT                 PORTBbits.RB4
#define CS_WPU                  WPUBbits.WPUB4
#define CS_ANS                  ANSELBbits.ANSB4
#define CS_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define CS_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define CS_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define CS_GetValue()           PORTBbits.RB4
#define CS_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define CS_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define CS_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define CS_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define CS_SetAnalogMode()      do { ANSELBbits.ANSB4 = 1; } while(0)
#define CS_SetDigitalMode()     do { ANSELBbits.ANSB4 = 0; } while(0)

// get/set IR_TX aliases
#define IR_TX_TRIS                 TRISCbits.TRISC1
#define IR_TX_LAT                  LATCbits.LATC1
#define IR_TX_PORT                 PORTCbits.RC1
#define IR_TX_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define IR_TX_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define IR_TX_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define IR_TX_GetValue()           PORTCbits.RC1
#define IR_TX_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define IR_TX_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)

// get/set COLUMN_0 aliases
#define COLUMN_0_TRIS                 TRISCbits.TRISC2
#define COLUMN_0_LAT                  LATCbits.LATC2
#define COLUMN_0_PORT                 PORTCbits.RC2
#define COLUMN_0_ANS                  ANSELCbits.ANSC2
#define COLUMN_0_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define COLUMN_0_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define COLUMN_0_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define COLUMN_0_GetValue()           PORTCbits.RC2
#define COLUMN_0_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define COLUMN_0_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define COLUMN_0_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define COLUMN_0_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set ROW_2 aliases
#define ROW_2_TRIS                 TRISCbits.TRISC3
#define ROW_2_LAT                  LATCbits.LATC3
#define ROW_2_PORT                 PORTCbits.RC3
#define ROW_2_ANS                  ANSELCbits.ANSC3
#define ROW_2_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define ROW_2_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define ROW_2_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define ROW_2_GetValue()           PORTCbits.RC3
#define ROW_2_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define ROW_2_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define ROW_2_SetAnalogMode()      do { ANSELCbits.ANSC3 = 1; } while(0)
#define ROW_2_SetDigitalMode()     do { ANSELCbits.ANSC3 = 0; } while(0)

// get/set IR_RX aliases
#define IR_RX_TRIS                 TRISCbits.TRISC4
#define IR_RX_LAT                  LATCbits.LATC4
#define IR_RX_PORT                 PORTCbits.RC4
#define IR_RX_ANS                  ANSELCbits.ANSC4
#define IR_RX_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define IR_RX_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define IR_RX_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define IR_RX_GetValue()           PORTCbits.RC4
#define IR_RX_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define IR_RX_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define IR_RX_SetAnalogMode()      do { ANSELCbits.ANSC4 = 1; } while(0)
#define IR_RX_SetDigitalMode()     do { ANSELCbits.ANSC4 = 0; } while(0)

// get/set ROW_1 aliases
#define ROW_1_TRIS                 TRISCbits.TRISC5
#define ROW_1_LAT                  LATCbits.LATC5
#define ROW_1_PORT                 PORTCbits.RC5
#define ROW_1_ANS                  ANSELCbits.ANSC5
#define ROW_1_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define ROW_1_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define ROW_1_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define ROW_1_GetValue()           PORTCbits.RC5
#define ROW_1_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define ROW_1_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define ROW_1_SetAnalogMode()      do { ANSELCbits.ANSC5 = 1; } while(0)
#define ROW_1_SetDigitalMode()     do { ANSELCbits.ANSC5 = 0; } while(0)

// get/set RC6 procedures
#define RC6_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define RC6_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define RC6_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define RC6_GetValue()              PORTCbits.RC6
#define RC6_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define RC6_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define RC6_SetAnalogMode()         do { ANSELCbits.ANSC6 = 1; } while(0)
#define RC6_SetDigitalMode()        do { ANSELCbits.ANSC6 = 0; } while(0)

// get/set RC7 procedures
#define RC7_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define RC7_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define RC7_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define RC7_GetValue()              PORTCbits.RC7
#define RC7_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define RC7_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define RC7_SetAnalogMode()         do { ANSELCbits.ANSC7 = 1; } while(0)
#define RC7_SetDigitalMode()        do { ANSELCbits.ANSC7 = 0; } while(0)

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