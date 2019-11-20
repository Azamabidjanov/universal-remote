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


#define BLOCK_SIZE      512


//Global Variables
uint16_t IR_SIGNAL_BUFFER[BLOCK_SIZE/2]; //Composition: length, first high pulse duration, first low pulse duration, second high pulse duration...
uint16_t PULSE_RISING = 0;
uint16_t PULSE_FALLING = 0;


//Functions
uint16_t pulse_Duration_In_Micro_Seconds(uint16_t pulse_Start, uint16_t pulse_End);
uint8_t input_Signal_Complete();

//Flags
uint8_t TRANSMIT_SIGNAL = 0;
uint8_t INPUT_SIGNAL_AQUIRED = 0;
uint8_t INPUT_SIGNAL_COMPLETE = 1;
uint8_t RECORD_IR_SIGNAL = 0;

//ISR Declarations
void ECCP1_Rising_Edge_Detected(void);
void ECCP3_Falling_Edge_Detected(void);
void my_TMR0_ISR(void);
void my_TMR1_ISR(void);

//----------------------------------------------
// Main "function"
//----------------------------------------------
void main(void)
{
    //Variables
    char cmd;
    uint8_t i;
    
    // Set timer one to run for one full cycle. MUST BE DONE
    // BEFORE enabling interrupts, otherwise that while loop becomes an
    // infinite loop.  Doing this to give EUSART1's baud rate generator time
    // to stabelize - this will make the splash screen looks better
    TMR1_WriteTimer(0x0000);
    PIR1bits.TMR1IF= 0; 
    while(PIR1bits.TMR1IF == 0);
    
    // Initialize the device
    SYSTEM_Initialize();

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    //TODO: Output board configuration.

    while(true)
    {
        if (EUSART1_DataReady) // wait for incoming data on USART
        {			
            cmd = EUSART1_Read();
            
            switch(cmd)
            {
                //--------------------------------------------
                // Reply with help menu
                //--------------------------------------------
                case '?':
                    printf("\r\n-------------------------------------------------\r\n");
                    printf("?: help menu\r\n");
                    printf("o: k\r\n");
                    printf("Z: Reset processor\r\n");                     
                    printf("z: Clear the terminal\r\n"); 
                    printf("R: Record IR transmission\r\n");
                    //TODO: Print additional menu options once finished.
                    printf("\r\n-------------------------------------------------\r\n");
                    break;
                    
                //--------------------------------------------
        		// Reply with "k", used for PC to PIC test
            	//--------------------------------------------
                case 'o':
                    printf("o:	ok\r\n");
                    break;
                    
                //--------------------------------------------
                // Reset the processor after clearing the terminal
                //--------------------------------------------                      
                case 'Z':
                    for (i=0; i<40; i++) printf("\n");
                    RESET();                    
                    break;

                //--------------------------------------------
                // Clear the terminal
                //--------------------------------------------                      
                case 'z':
                    for (i=0; i<40; i++) printf("\n");                            
                    break; 
            
                //--------------------------------------------
                //Record IR input
                //--------------------------------------------
                case 'R':
                    printf("Press any key to start recording IR signal.\r\n");
                    
                    //Wait for user input
                    while(!EUSART1_DataReady);
                    (void) EUSART1_Read();
                    
                    INPUT_SIGNAL_AQUIRED = 0;   //Make sure we only start storing values once we have a full pulse.
                    
                    IR_SIGNAL_BUFFER[0] = 1;    //Set the number of included elements in the IR string to 0.
                    
                    INPUT_SIGNAL_COMPLETE = 0;  //Set the flag to say we aren't done recording.
                    
                    RECORD_IR_SIGNAL = 1;       //Tell the capture ISRs to stop ignoring input
                    
                    
                    while(INPUT_SIGNAL_COMPLETE == false);
                    
                    printf("IR signal captured and loaded into buffer.\r\n");
                    
                    break;
                    
                //--------------------------------------------
                // If something unknown is hit, tell user
                //--------------------------------------------
                default:
                    printf("Unknown key %c\r\n",cmd);
                    break;
            }
        }
        
        
        
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
    PULSE_RISING = TMR1_ReadTimer();
    if(INPUT_SIGNAL_AQUIRED)    //If we've already seen a full pulse, get the duration.
    {
        IR_SIGNAL_BUFFER[IR_SIGNAL_BUFFER[0]] = pulse_Duration_In_Micro_Seconds(PULSE_FALLING, PULSE_RISING); //Add the captured pulse length to the command.
        IR_SIGNAL_BUFFER[0]++;  //Increment elements
    }
    else    //Else, the next edge will complete the pulse.
    {
        INPUT_SIGNAL_AQUIRED = true;
    }
    
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
    PULSE_RISING = TMR1_ReadTimer();
    if(INPUT_SIGNAL_AQUIRED)    //If we've already seen a full pulse, get the duration.
    {
        IR_SIGNAL_BUFFER[IR_SIGNAL_BUFFER[0]] = pulse_Duration_In_Micro_Seconds(PULSE_RISING, PULSE_FALLING); //Add the captured pulse length to the command.
        IR_SIGNAL_BUFFER[0]++;  //Increment elements
    }
    else    //Real IR signals don't start high, so if we wind up here it's garbage.
    {
        
    }
    
    PIR4bits.CCP3IF = 0; //Clear interrupt flag, see technical docs page 176. 
}

//----------------------------------------------
// Timer 1 ISR
// Primarily used for transmitting IR codes
// and determining IR pulse length.
//----------------------------------------------
void my_TMR1_ISR()
{
    if(input_Signal_Complete() == true && RECORD_IR_SIGNAL == true)// Reset flags if recording is complete
    {
        INPUT_SIGNAL_COMPLETE = 1;
        RECORD_IR_SIGNAL = 0;
        INPUT_SIGNAL_AQUIRED = 0;
    }
    
    //TODO: Add flags and logic to transmit IR codes
    
    PIR1bits.TMR1IF= 0; 
}

//----------------------------------------------
// Timer 0 ISR
// Primarily used for polling the keypad.
//----------------------------------------------
void my_TRM0_ISR()
{
    //TODO: Poll the keypad for presses every millisecond.
    
    INTCONbits.TMR0IF= 0;
}

//----------------------------------------------
// Timer 1 ISR helper function
// Checks to see if any new pulses have been
// recorded for a while and stops the recording
// process if they haven't.
//----------------------------------------------
uint8_t input_Signal_Complete()
{
    static uint16_t past_values[4] = {0,0,0,0};
    static uint8_t checks = 0;
    
    //Cycle the values of the array
    past_values[0] = IR_SIGNAL_BUFFER[0];
    past_values[1] = past_values[0];
    past_values[2] = past_values[1];
    past_values[3] = past_values[2];
    
    if(INPUT_SIGNAL_AQUIRED == false) //Reset for a new signal, if we haven't got our first pulse yet
    {
        checks = 0;
        past_values[0] = 0;
        past_values[1] = 0;
        past_values[2] = 0;
        past_values[3] = 0;
        return false;
    }
    else if(checks == 4) //Enough samples collected to make a judgment
    {
        if(past_values[0] == past_values[3]) //If no new pulses collected, assume it's done.
        {
            return true;
        }
    }
    else //Need more samples
    {
        checks++;
        return false;
    }
}

/**
 End of File
*/