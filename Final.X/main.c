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
#define STOP_BIT        0x0000
#define PAUSE_BIT       0xFFFF
#define BLOCK_SIZE      512
#define LED_ON          25 
#define LED_OFF         0
#define LENGTH          0
#define BUTTON_ROWS     4
#define BUTTON_COLUMNS  3
#define TMR0_1_MS       1600

//Global Variables
uint16_t IR_SIGNAL_BUFFER[BLOCK_SIZE/2]; //Composition: length, first high pulse duration, first low pulse duration, second high pulse duration...
uint16_t PULSE_RISING = 0;
uint16_t PULSE_FALLING = 0;
char PRESSED_BUTTONS[BUTTON_ROWS][BUTTON_COLUMNS] = 
{
    {'\0', '\0', '\0'}, 
    {'\0', '\0', '\0'}, 
    {'\0', '\0', '\0'}, 
    {'\0', '\0', '\0'}
}; 
const char TEST_BUTTONS[BUTTON_ROWS][BUTTON_COLUMNS] = 
{
    {'1', '2', '3'}, 
    {'4', '5', '6'}, 
    {'7', '8', '9'}, 
    {'*', '0', '#'}
}; 

//Functions
uint16_t pulse_Duration_In_Micro_Seconds(uint16_t pulse_Start, uint16_t pulse_End);
uint16_t micro_Seconds_to_TMR1_Counts(uint16_t input);
uint8_t input_Signal_Complete();
void poll_Keypad();

//Flags
uint8_t TRANSMIT_SIGNAL = 0;
uint8_t INPUT_SIGNAL_AQUIRED = 0;
uint8_t INPUT_SIGNAL_COMPLETE = 1;
uint8_t RECORD_IR_SIGNAL = 0;

//ISR Declarations
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
    
    // Initialize the device
    SYSTEM_Initialize();

    // Set timer one to run for one full cycle. MUST BE DONE
    // BEFORE enabling interrupts, otherwise that while loop becomes an
    // infinite loop.  Doing this to give EUSART1's baud rate generator time
    // to stabilize - this will make the splash screen looks better
    TMR1_WriteTimer(0x0000);
    PIR1bits.TMR1IF= 0; 
    while(PIR1bits.TMR1IF == 0);
    
    //Set custom ISRs
    //TODO: Find a way to set custom ISRs for ECCP1 and ECCP3
    
    TMR0_SetInterruptHandler(my_TMR0_ISR);
    TMR1_SetInterruptHandler(my_TMR1_ISR);
    
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    //Turn off LED
    EPWM2_LoadDutyValue(LED_OFF); 
    printf("Reset complete.\r\n");
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
                    printf("R: Record IR transmission into buffer\r\n");
                    printf("T: Transmit IR buffer.\r\n");
                    printf("K: Test the keypad.\r\n");
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
                //Send IR buffer
                //--------------------------------------------
                case 'T':
                    printf("Press any key to start transmitting IR signal.\r\n");

                    while(!EUSART1_DataReady);
                    (void) EUSART1_Read();
                    
                    printf("Sending complete.");
                    
                    break;
                    
                //--------------------------------------------
                //Test Keypad
                //-------------------------------------------- 
                case 'K':
                    printf("Watching for keypad presses, press any terminal key to stop.\r\n");

                    while(!EUSART1_DataReady)
                    {
                        
                        for(uint8_t i = 0;i < BUTTON_ROWS;i++)
                        {
                            for(uint8_t j = 0;j < BUTTON_COLUMNS;j++)
                            {
                                if(PRESSED_BUTTONS[i][j] == TEST_BUTTONS[i][j])
                                {
                                    printf("%c\r\n", PRESSED_BUTTONS[i][j]);
                                }
                            }
                        }
                        
                    }
                    
                    
                    (void) EUSART1_Read();
                    
                    printf("Stopped watching.\r\n");
                    
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
// Function to determine which keys are pressed
// returns a two dimensional array of characters
// that will match the corresponding cell in
// "TEST_BUTTONS" if that button is pressed,
//  '\0' otherwise.
//----------------------------------------------
void poll_Keypad()
{
    
    for(uint8_t i = 0;i < BUTTON_ROWS;i++)
    {
        for(uint8_t j = 0;j < BUTTON_COLUMNS;j++)
        {
               PRESSED_BUTTONS[i][j] = '\0';
        }
    }
    
    //COLUMN_1_SetDigitalInput();
    //COLUMN_2_SetDigitalInput();
    //COLUMN_3_SetDigitalInput();
    //ROW_1_SetDigitalOutput();
    //ROW_2_SetDigitalOutput();
    //ROW_3_SetDigitalOutput();
    //ROW_4_SetDigitalOutput();
    
    ROW_1_SetLow();
    ROW_2_SetLow();
    ROW_3_SetLow();
    ROW_4_SetLow();
    
    ROW_1_SetHigh();
    
  
    if(COLUMN_1_GetValue() == 1)    
    {
        PRESSED_BUTTONS[0][0] = '1';
    }
    
    if(COLUMN_2_GetValue() == 1)    
    {
        PRESSED_BUTTONS[0][1] = '2';
    }
    
    
    if(COLUMN_3_GetValue() == 1)    
    {
        PRESSED_BUTTONS[0][2] = '3';
    }
    
    ROW_1_SetLow();
    
    ROW_2_SetHigh();
  
    if(COLUMN_1_GetValue() == 1)    
    {
        PRESSED_BUTTONS[1][0] = '4';
    }
    
    if(COLUMN_2_GetValue() == 1)    
    {
        PRESSED_BUTTONS[1][1] = '5';
    }
    
    if(COLUMN_3_GetValue() == 1)    
    {
        PRESSED_BUTTONS[1][2] = '6';
    }
    
    ROW_2_SetLow();
    
    ROW_3_SetHigh();
  
    if(COLUMN_1_GetValue() == 1)    
    {
        PRESSED_BUTTONS[2][0] = '7';
    }
    
    if(COLUMN_2_GetValue() == 1)    
    {
        PRESSED_BUTTONS[2][1] = '8';
    }
    
    if(COLUMN_3_GetValue() == 1)    
    {
        PRESSED_BUTTONS[2][2] = '9';
    }
    
    ROW_3_SetLow();
    
    ROW_4_SetHigh();
    
    if(COLUMN_1_GetValue() == 1)    
    {
        PRESSED_BUTTONS[3][0] = '*';
    }
    
    if(COLUMN_2_GetValue() == 1)    
    {
        PRESSED_BUTTONS[3][1] = '0';
    }
    
    if(COLUMN_3_GetValue() == 1)    
    {
        PRESSED_BUTTONS[3][2] = '#';
    }
    
    ROW_4_SetLow();
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
// Function to convert microseconds to TMR1
// counts... it multiplies by 2, but I can't
// be fucked to remember that.
//----------------------------------------------
uint16_t micro_Seconds_to_TMR1_Counts(uint16_t input)
{
    return input << 1;
}


//----------------------------------------------
// Timer 1 ISR
// Primarily used for transmitting IR codes
// and determining IR pulse length.
//----------------------------------------------
void my_TMR1_ISR()
{
    static uint16_t index = 0;   //Index for the IR buffer
    
    if(input_Signal_Complete() == true && RECORD_IR_SIGNAL == true)// Reset flags if recording is complete
    {
        INPUT_SIGNAL_COMPLETE = 1;
        RECORD_IR_SIGNAL = 0;
        INPUT_SIGNAL_AQUIRED = 0;
    }
    
    //IR transmission logic
    if(TRANSMIT_SIGNAL == true) //If we're transmitting
    {
        index++;
        
        //TODO: Add logic to handle pause and stop bits in the ISR
        
        if(index < IR_SIGNAL_BUFFER[LENGTH])    //If we still have signal left to send, send it
        {
            if(((index >> 1) & 0b0001))   //If the index is odd, we know the signal should be high
            {
                EPWM2_LoadDutyValue(LED_OFF); 
            }
            else
            {
                EPWM2_LoadDutyValue(LED_ON); 
            }
  
            TMR1_WriteTimer(0x10000 - micro_Seconds_to_TMR1_Counts(IR_SIGNAL_BUFFER[index]));   //Set the timer for the pulse duration.
        }
        else    //else, stop sending
        {
            TRANSMIT_SIGNAL = false;
        }
    }
    else
    {
        index = 0;
    }
    
    PIR1bits.TMR1IF= 0; 
}

//----------------------------------------------
// Timer 0 ISR
// Primarily used for polling the keypad.
//----------------------------------------------
void my_TMR0_ISR()
{
    poll_Keypad();
    
    TMR0_WriteTimer(0xFFFF - TMR0_1_MS);
    
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