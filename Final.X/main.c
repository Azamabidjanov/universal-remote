//--------------------------------------------------------------------
// Name:            Azam Abidjanov & Connor Stutsman-Plunkett
// Date:            Fall  2019
// Purp:            Final Project
//
// Assisted:        The entire class of EENG 383
// Assisted by:     Microchips 18F26K22 Tech Docs 
//-
//- Academic Integrity Statement: I certify that, while others may have
//- assisted me in brain storming, debugging and validating this program,
//- the program itself is my own work. I understand that submitting code
//- which is the work of other individuals is a violation of the course
//- Academic Integrity Policy and may result in a zero credit for the
//- assignment, or course failure and a report to the Academic Dishonesty
//- Board. I also understand that if I knowingly give my original work to
//- another individual that it could also result in a zero credit for the
//- assignment, or course failure and a report to the Academic Dishonesty
//- Board.
//------------------------------------------------------------------------

#include "mcc_generated_files/mcc.h"


//Definitions
#define INTERRUPT_GlobalInterruptEnable() (INTCONbits.GIE = 1)          //Do not touch these, for some reason dispite them being in interrupt_manager.h the compiler
#define INTERRUPT_PeripheralInterruptEnable() (INTCONbits.PEIE = 1)     //refuses to run without them. Also tried including interrupt_manager.h, didn't work.


#define BLOCK_SIZE      512


//Global Variables
uint16_t IR_SIGNAL_BUFFER[BLOCK_SIZE/2]; //Composition: length, first high pulse duration, first low pulse duration, second high pulse duration...


//Functions
uint16_t pulse_Duration_In_Micro_Seconds(uint16_t pulse_Start, uint16_t pulse_End);

//Flags
uint8_t TRANSMIT_SIGNAL = 0;

//ISR Declarations
void ECCP1_Rising_Edge_Detected(void);
void ECCP3_Falling_Edge_Detected(void);
void my_TMR1_ISR(void);

//----------------------------------------------
// Main "function"
//----------------------------------------------
void main(void)
{
    //Variables
    
    // Set timer one to run for one full cycle. MUST BE DONE
    // BEFORE enabling interrupts, otherwise that while loop becomes an
    // infinite loop.  Doing this to give EUSART1's baud rate generator time
    // to stabelize - this will make the splash screen looks better
    //TMR1_WriteTimer(0x0000);
    //PIR1bits.TMR1IF= 0; 
    //while(PIR1bits.TMR1IF == 0);
    
    // Initialize the device
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

   
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    //TODO: Set 

    while(true)
    {
        // Add your application code
    }
}

//----------------------------------------------
// Function to find the length of an IR pulse.
// Finds the difference in timer one counts.
// 1 timer one count = 500ns.
// Multiplies by to achieve answer.
//----------------------------------------------
uint16_t pulse_Duration_In_Micro_Seconds(uint16_t pulse_Start, uint16_t pulse_End)
{
    uint16_t result = pulse_End - pulse_Start;
    result = result >> 1;
    return result;
}

//----------------------------------------------
// ECCP1 ISR
// Records timer 1 count when a rising edge is 
// detected on the input pin RC2
// Note: RC2 will need to be jumpered to RC4
//----------------------------------------------
void ECCP1_Rising_Edge_Detected()
{
    //TODO: Record timer one value at the point of the rising edge, store in a global variable.
    PIR1bits.CCP1IF = 0; //Clear interrupt flag, see technical docs page 176.
}

//----------------------------------------------
// ECCP3 ISR
// Records timer 1 count when a falling edge is 
// detected on the input pin RC2
// Note: RB5 will need to be jumpered to RC4
//----------------------------------------------
void ECCP3_Falling_Edge_Detected()
{
    //TODO: Record timer one value at the point of the falling edge, store in a global variable..
    
    PIR4bits.CCP3IF = 0; //Clear interrupt flag, see technical docs page 176. 
}

//----------------------------------------------
// Timer 1 ISR
// Primarily used for transmitting IR codes.
//----------------------------------------------
void my_TMR1_ISR()
{
    PIR1bits.TMR1IF= 0; 
}


/**
 End of File
*/