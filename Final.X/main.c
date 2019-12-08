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

/*
 * Understanding the SD card structure:
 * 
 * Address: 0
 * This address block has the number of codes stored for each key. This can be
 * converted to the address of the last stored code thusly: 
 * ([NUMBER ASSOCIATED WITH THE KEY] + [NUMBER OF CODES STORED FOR THIS BUTTON]*12 + 24)*[BLOCK_SIZE]
 * 
 * Address: 1*512 - 24*512
 * These store the codes associaded with each key and a corrisponding array
 * that has 4 character human readable codes associated with the signals
 * 
 * Example:
 * Address: 1*512 -> [v+LG, v+SB, v+SS]
 * Address: 2*512 -> [ 25*512, 49*512, 73*512]
 * 
 * 
 * Address: 25*512+
 * These store the actual codes for the keys, spaced 12 blocks apart, such that
 * the first code for the first key will be stored just before the first code
 * for the second key and so on.
 */



//Libraries
#include "mcc_generated_files/mcc.h"
#include "sdCard.h"

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
/*Quote from INLAB 6: the IR LED will blink at 38KHz. When the IR decoder "sees" this, it's output will go to logic 0.*/
#define IR_RX_LED_HIGH      0
#define IR_RX_LED_LOW       1

//Global Variables
uint16_t IR_SIGNAL_BUFFER[BLOCK_SIZE/2]; //Composition: length, first high pulse duration, first low pulse duration, second high pulse duration...
uint32_t ADDRESS_BUFFER[BLOCK_SIZE/4];   //Composition: length, address of first command, address of second command...
uint32_t STATUS_BUFFER[BLOCK_SIZE/4];    //Composition: Current location to store commands for 1st button, current location to store commands for 2st button...
uint32_t INFO_BUFFER[BLOCK_SIZE/4];      //Composition: length, 4 character numonics for the codes
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
void poll_Keypad();
uint32_t generate_Address(char key, uint16_t entry);
uint32_t count_Key(char key);

//Flags
uint8_t TRANSMIT_SIGNAL = 0;
uint8_t INPUT_SIGNAL_AQUIRED = 0;
uint8_t INPUT_SIGNAL_COMPLETE = 1;
uint8_t RECORD_IR_SIGNAL = 0;
uint8_t noChangeCount = 0;  
uint8_t HEADLESS_RUNNING = 1;

//ISR Declarations
void my_TMR0_ISR(void);
void my_TMR1_ISR(void);
void my_TMR3_ISR(void);
//----------------------------------------------
// Main "function"
//----------------------------------------------
void main(void)
{
    //Variables
    char cmd;
    uint16_t i;
    uint8_t data_prev = 255;
    uint8_t data_cur = 255;
    
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
    TMR3_SetInterruptHandler(my_TMR3_ISR);
    
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    //Turn off LED
    EPWM2_LoadDutyValue(LED_OFF); 
    printf("Reset complete.\r\n");
    //TODO: Output board configuration.
    
    //Get the SD Card ready
    SDCARD_Initialize(true);
    
    //Disable headless running (temporary)
    HEADLESS_RUNNING = 0;
    
    //Get the information for where to put new codes
    SDCARD_ReadBlock(0 ,STATUS_BUFFER);

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
                    printf("P: Print the contents of IR Buffer\r\n");
                    printf("T: Transmit IR buffer.\r\n");
                    printf("K: Test the keypad.\r\n");
                    //TODO: Make a reset option to full reset the remote
                    //TODO: Make a way to edit remote commands.
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
            
                case 'P':
                    printf("We have %d number of pulses\r\n",IR_SIGNAL_BUFFER[LENGTH]);

                    for(i = 1; i<IR_SIGNAL_BUFFER[LENGTH];i++){
                        printf("Duration of pulse %d is: %d us\r\n",i,IR_SIGNAL_BUFFER[i]);
                    } 
                    break;
                //--------------------------------------------
                //Record IR input
                //--------------------------------------------
                case 'R':
                    printf("Press any key to start recording IR signal.\r\n");
                    
                    //Wait for user input
                    while(!EUSART1_DataReady);
                    (void) EUSART1_Read();
                    
                    data_cur = 255;
                    data_prev = 255;
                    INPUT_SIGNAL_AQUIRED = 0;   //Make sure we only start storing values once we have a full pulse.
                    IR_SIGNAL_BUFFER[0] = 1;
                    
                    noChangeCount = 0;
                    while(noChangeCount < 20 && IR_SIGNAL_BUFFER[LENGTH] < (BLOCK_SIZE/2 - 1) ){
                    
                        data_cur = IR_RX_GetValue();
                        //printf("data: %d\r\n", data_cur);
                        if( data_prev == 255 ){ // We got our first sample
                            data_prev = data_cur;
                            continue;
                        }
                            
                        if( data_prev != data_cur ){
                            noChangeCount = 0;
                            if( data_cur == IR_RX_LED_LOW  ){
                                PULSE_FALLING = TMR3_ReadTimer();
                                if( INPUT_SIGNAL_AQUIRED == true ){
                                    IR_SIGNAL_BUFFER[IR_SIGNAL_BUFFER[LENGTH]] = pulse_Duration_In_Micro_Seconds(PULSE_RISING, PULSE_FALLING); //Add the captured pulse length to the command.
                                    IR_SIGNAL_BUFFER[LENGTH]++;  //Increment elements
                                }else{
                                    INPUT_SIGNAL_AQUIRED = true;
                                }
                            }else{
                                PULSE_RISING = TMR3_ReadTimer();
                                if( INPUT_SIGNAL_AQUIRED == true ){
                                    IR_SIGNAL_BUFFER[IR_SIGNAL_BUFFER[LENGTH]] = pulse_Duration_In_Micro_Seconds(PULSE_FALLING, PULSE_RISING); //Add the captured pulse length to the command.
                                    IR_SIGNAL_BUFFER[LENGTH]++;  //Increment elements
                                }else{
                                    INPUT_SIGNAL_AQUIRED = true;
                                }
                            }
                        }
                        data_prev = data_cur;
                    }
                    
                    //TODO: Save code to SD card in the correct place
                    
                    //TODO: Add something to request a 4 character pnumonic to associate with the code
                    
                    //TODO: Add logic to ask the user to put in additional codes
                    
                    printf("IR signal captured and loaded into buffer.\r\n");
                    
                    break;
                   
                //--------------------------------------------
                //Send IR buffer
                //--------------------------------------------
                case 'T':
                    //TODO: change how this works to work with the SD CARD.
                    
                    printf("Press any key to start transmitting IR signal.\r\n");


                    while(!EUSART1_DataReady);
                    (void) EUSART1_Read();
                    
                    TRANSMIT_SIGNAL = true;
                    
                    while( TRANSMIT_SIGNAL == true );
                    
                    printf("Sending complete.\n\r");
   
                    
                    break;
                    
                //--------------------------------------------
                //Test Keypad
                //-------------------------------------------- 
                case 'K':
                    printf("Watching for keypad presses, press any terminal key to stop.\r\n");

                    while(!EUSART1_DataReady)
                    {
                        
                        for(uint8_t i = 0;i < BUTTON_ROWS; i++)
                        {
                            for(uint8_t j = 0;j < BUTTON_COLUMNS;j++)
                            {
                                if(PRESSED_BUTTONS[i][j] == TEST_BUTTONS[i][j])
                                {
                                    printf("%c\r\n", PRESSED_BUTTONS[i][j]);
                                    while(PRESSED_BUTTONS[i][j] == TEST_BUTTONS[i][j]);
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
// Function to transition the key to a number
//----------------------------------------------
uint32_t count_Key(char key)
{
    uint32_t result = 0;
    
    switch(key)
    {
        case '*':
            result = 1;
            break; 
            
        case '0':
            result = 2;
            break;    
        
        case '#':
            result = 3;
            break; 
            
        case '7':
            result = 4;
            break; 
        
        case '8':
            result = 5;
            break; 
            
        case '9':
            result = 6;
            break; 
        
        case '4':
            result = 7;
            break; 
            
        case '5':
            result = 8;
            break; 
            
        case '6':
            result = 9;
            break; 
            
        case '1':
            result = 10;
            break; 
            
        case '2':
            result = 11;
            break; 
            
        case '3':
            result = 12;
            break; 
             
    }

    
    return result;
}


//----------------------------------------------
// Function to map an SD card address to a
// a keypad input
//----------------------------------------------
uint32_t generate_Address(char key, uint32_t entry)
{
    uint32_t result = 0;
    
    result = ((count_Key(key)) + 12*(entry) + 24)*BLOCK_SIZE;
    
    
    return result;
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
    
  
    
    if(COLUMN_0_GetValue() == 1)    
    {
        PRESSED_BUTTONS[0][0] = '1';
    }
    
    if(COLUMN_0_GetValue() == 1)    
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
  
    if(COLUMN_0_GetValue() == 1)    
    {
        PRESSED_BUTTONS[1][0] = '4';
    }
    
    if(COLUMN_0_GetValue() == 1)    
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
  
    if(COLUMN_0_GetValue() == 1)    
    {
        PRESSED_BUTTONS[2][0] = '7';
    }
    
    if(COLUMN_0_GetValue() == 1)    
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
    
    if(COLUMN_0_GetValue() == 1)    
    {
        PRESSED_BUTTONS[3][0] = '*';
    }
    
    if(COLUMN_0_GetValue() == 1)    
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
    static uint16_t index_signal = 0;   //Index for the IR buffer
    static uint16_t index_address = 1;   //Index for the IR code list
    
    //IR transmission logic
    if(TRANSMIT_SIGNAL == true) //If we're transmitting
    {
        index_signal++;
        
        //TODO: Add logic to handle pause and stop bits in the ISR
        
        if(index_signal < IR_SIGNAL_BUFFER[LENGTH])    //If we still have signal left to send, send it
        {
            if(index & 0x0001)   //If the index is odd, we know the signal should be high
            {
                EPWM2_LoadDutyValue(LED_ON); 
            }
            else
            {
                EPWM2_LoadDutyValue(LED_OFF); 
            }
  
            TMR1_WriteTimer(0x10000 - micro_Seconds_to_TMR1_Counts(IR_SIGNAL_BUFFER[index]));   //Set the timer for the pulse duration.
        }
        else if((index_address + 1) < ADDRESS_BUFFER[LENGTH]) //If we have more codes to send
        {
            index_address++;
            
            //TODO: find a way to delay the signal sending.
            
            SDCARD_ReadBlock(ADDRESS_BUFFER[index_address], IR_SIGNAL_BUFFER); //Get the next code ready to send
        }
        else    //else, stop sending
        {
            TRANSMIT_SIGNAL = false;
            index_signal = 0;
            index_address = 1;
        }
    }
    else
    {
        index_signal = 0;
    }
    
    PIR1bits.TMR1IF= 0; 
}

//----------------------------------------------
// Timer 0 ISR
// Primarily used for polling the keypad.
//----------------------------------------------
void my_TMR0_ISR()
{
    uint8_t escape_flag = 0;
    
    poll_Keypad();
    
    if(HEADLESS_RUNNING)
    {
        //Check if any buttons were pressed, and send the stored command if they did.
        for(uint8_t i = 0;i < BUTTON_ROWS; i++)
        {
            if(escape_flag) break;
            
            for(uint8_t j = 0;j < BUTTON_COLUMNS;j++)
            {
                if(escape_flag) break;
                
                if(PRESSED_BUTTONS[i][j] == TEST_BUTTONS[i][j])
                {
                    escape_flag = 1; //We found a button press so stop looking.
                  
                    SDCARD_ReadBlock(count_Key(TEST_BUTTONS[i][j])*BLOCK_SIZE*2, ADDRESS_BUFFER); //Get the list of addresses where the codes are stored
                    
                    SDCARD_ReadBlock(ADDRESS_BUFFER[1], IR_SIGNAL_BUFFER);  // Get the first code ready to transmit.
                    
                    TRANSMIT_SIGNAL = true;
                    
                    
                }
            }
        }
    }
    
    escape_flag = 0;
    
    TMR0_WriteTimer(0xFFFF - TMR0_1_MS);
    
    INTCONbits.TMR0IF= 0;
}

void my_TMR3_ISR(){
    
    if( INPUT_SIGNAL_AQUIRED == true){
        noChangeCount++;
    }
    TMR3_WriteTimer(0x0000);
    PIR2bits.TMR3IF = 0;
}

/**
 End of File
*/