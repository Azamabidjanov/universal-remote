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

// get/set ROW_1 aliases
#define ROW_1_TRIS                 TRISAbits.TRISA1
#define ROW_1_LAT                  LATAbits.LATA1
#define ROW_1_PORT                 PORTAbits.RA1
#define ROW_1_ANS                  ANSELAbits.ANSA1
#define ROW_1_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define ROW_1_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define ROW_1_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define ROW_1_GetValue()           PORTAbits.RA1
#define ROW_1_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define ROW_1_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define ROW_1_SetAnalogMode()      do { ANSELAbits.ANSA1 = 1; } while(0)
#define ROW_1_SetDigitalMode()     do { ANSELAbits.ANSA1 = 0; } while(0)

// get/set COLUMN_1 aliases
#define COLUMN_1_TRIS                 TRISAbits.TRISA2
#define COLUMN_1_LAT                  LATAbits.LATA2
#define COLUMN_1_PORT                 PORTAbits.RA2
#define COLUMN_1_ANS                  ANSELAbits.ANSA2
#define COLUMN_1_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define COLUMN_1_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define COLUMN_1_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define COLUMN_1_GetValue()           PORTAbits.RA2
#define COLUMN_1_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define COLUMN_1_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define COLUMN_1_SetAnalogMode()      do { ANSELAbits.ANSA2 = 1; } while(0)
#define COLUMN_1_SetDigitalMode()     do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set ROW_4 aliases
#define ROW_4_TRIS                 TRISAbits.TRISA3
#define ROW_4_LAT                  LATAbits.LATA3
#define ROW_4_PORT                 PORTAbits.RA3
#define ROW_4_ANS                  ANSELAbits.ANSA3
#define ROW_4_SetHigh()            do { LATAbits.LATA3 = 1; } while(0)
#define ROW_4_SetLow()             do { LATAbits.LATA3 = 0; } while(0)
#define ROW_4_Toggle()             do { LATAbits.LATA3 = ~LATAbits.LATA3; } while(0)
#define ROW_4_GetValue()           PORTAbits.RA3
#define ROW_4_SetDigitalInput()    do { TRISAbits.TRISA3 = 1; } while(0)
#define ROW_4_SetDigitalOutput()   do { TRISAbits.TRISA3 = 0; } while(0)
#define ROW_4_SetAnalogMode()      do { ANSELAbits.ANSA3 = 1; } while(0)
#define ROW_4_SetDigitalMode()     do { ANSELAbits.ANSA3 = 0; } while(0)

// get/set COLUMN_3 aliases
#define COLUMN_3_TRIS                 TRISAbits.TRISA4
#define COLUMN_3_LAT                  LATAbits.LATA4
#define COLUMN_3_PORT                 PORTAbits.RA4
#define COLUMN_3_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define COLUMN_3_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define COLUMN_3_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define COLUMN_3_GetValue()           PORTAbits.RA4
#define COLUMN_3_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define COLUMN_3_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)

// get/set ROW_3 aliases
#define ROW_3_TRIS                 TRISAbits.TRISA5
#define ROW_3_LAT                  LATAbits.LATA5
#define ROW_3_PORT                 PORTAbits.RA5
#define ROW_3_ANS                  ANSELAbits.ANSA5
#define ROW_3_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define ROW_3_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define ROW_3_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define ROW_3_GetValue()           PORTAbits.RA5
#define ROW_3_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define ROW_3_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define ROW_3_SetAnalogMode()      do { ANSELAbits.ANSA5 = 1; } while(0)
#define ROW_3_SetDigitalMode()     do { ANSELAbits.ANSA5 = 0; } while(0)

// get/set ROW_2 aliases
#define ROW_2_TRIS                 TRISAbits.TRISA6
#define ROW_2_LAT                  LATAbits.LATA6
#define ROW_2_PORT                 PORTAbits.RA6
#define ROW_2_SetHigh()            do { LATAbits.LATA6 = 1; } while(0)
#define ROW_2_SetLow()             do { LATAbits.LATA6 = 0; } while(0)
#define ROW_2_Toggle()             do { LATAbits.LATA6 = ~LATAbits.LATA6; } while(0)
#define ROW_2_GetValue()           PORTAbits.RA6
#define ROW_2_SetDigitalInput()    do { TRISAbits.TRISA6 = 1; } while(0)
#define ROW_2_SetDigitalOutput()   do { TRISAbits.TRISA6 = 0; } while(0)

// get/set RB5 procedures
#define RB5_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define RB5_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define RB5_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define RB5_GetValue()              PORTBbits.RB5
#define RB5_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define RB5_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define RB5_SetPullup()             do { WPUBbits.WPUB5 = 1; } while(0)
#define RB5_ResetPullup()           do { WPUBbits.WPUB5 = 0; } while(0)
#define RB5_SetAnalogMode()         do { ANSELBbits.ANSB5 = 1; } while(0)
#define RB5_SetDigitalMode()        do { ANSELBbits.ANSB5 = 0; } while(0)

// get/set RC2 procedures
#define RC2_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define RC2_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define RC2_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define RC2_GetValue()              PORTCbits.RC2
#define RC2_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define RC2_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define RC2_SetAnalogMode()         do { ANSELCbits.ANSC2 = 1; } while(0)
#define RC2_SetDigitalMode()        do { ANSELCbits.ANSC2 = 0; } while(0)

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