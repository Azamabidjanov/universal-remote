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

uint8_t BUFFER[BLOCK_SIZE];
uint32_t NAME_BUFFER[BLOCK_SIZE/4];
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
uint32_t generate_Address(char key, uint32_t entry);
uint32_t count_Key(char key);
void byteToWord( uint8_t buffer[], uint32_t wordBuffer[] );
void byteToHalfWord( uint8_t buffer[], uint16_t halfWordBuffer[] );
void wordToByte( uint8_t buffer[], uint32_t wordBuffer[] );
void halfWordToByte( uint8_t buffer[], uint16_t halfWordBuffer[] );


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
    uint8_t     status;
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
    SDCARD_ReadBlock(0, BUFFER);
    byteToWord( BUFFER, STATUS_BUFFER );

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
                    printf("S: Test\r\n");
                    printf("X: LED Test\r\n");
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
                {
                    
                    printf("Print a key on the Keypad to print contents from SD Card\r\n");
                    uint8_t pressed = false;
                    char key;
                    
                    while( !pressed ){
                        for(uint8_t i = 0;i < BUTTON_ROWS; i++)
                        {
                            for(uint8_t j = 0;j < BUTTON_COLUMNS;j++)
                            {
                                if(PRESSED_BUTTONS[i][j] == TEST_BUTTONS[i][j])
                                {
                                    pressed = true;
                                    key = PRESSED_BUTTONS[i][j];
                                }
                            }
                        }
                    }
                    printf("Getting contents of %c key\r\n", key);
                    
                    uint32_t keyAddress = ((count_Key( key ) * 2) - 1)* 512;
                    SDCARD_ReadBlock(keyAddress, BUFFER); //Get the next code ready to send
                    byteToWord( BUFFER, INFO_BUFFER );    
                    
                    printf("Size of the INFO_BUFFER: %d\r\n", INFO_BUFFER[LENGTH]);
                    
                    for(uint8_t i = 0; i < INFO_BUFFER[LENGTH]; i++){
                        char buf[5];
                        uint32_t name_t = INFO_BUFFER[i+1];
                        buf[0] = name_t >> 24;
                        buf[1] = name_t >> 16;
                        buf[2] = name_t >> 8;
                        buf[3] = name_t;
                        buf[4] = '\0';
                        
                        printf("Command: %d  Name: %s\r\n", i, buf);
                    }
                    
                    
                    
                    keyAddress = ((count_Key( key ) * 2))* 512;
                    SDCARD_ReadBlock(keyAddress, BUFFER); //Get the next code ready to send
                    byteToWord( BUFFER, ADDRESS_BUFFER );    
                    
                    printf("Size of the ADDRESS_BUFFER: %d\r\n", ADDRESS_BUFFER[LENGTH]);
                    
                    for(uint8_t i = 0; i < ADDRESS_BUFFER[LENGTH]; i++){
                 
                        printf("Command: %d  Address: ", i);
                        printf("%04x", ADDRESS_BUFFER[i+1]>>16); printf(":");  printf("%04x",ADDRESS_BUFFER[i+1]&0X0000FFFF);    printf("\r\n");
                        
                    }
                            

                    printf("We have %d number of pulses\r\n",IR_SIGNAL_BUFFER[LENGTH]);
                    for(i = 1; i<IR_SIGNAL_BUFFER[LENGTH];i++){
                        printf("Duration of pulse %d is: %d us\r\n",i,IR_SIGNAL_BUFFER[i]);
                    } 
                    break;
                }
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
                    
                    printf("Entering Transmission Mode. Press any key on the terminal to quit..\r\n");

                    HEADLESS_RUNNING = true;
                    
//                    while(!EUSART1_DataReady);
//                    (void) EUSART1_Read();
                    
                    //TRANSMIT_SIGNAL = true;
                    
                    while( TRANSMIT_SIGNAL != true );
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
                 case 'W':
                 {
                    printf("Press the you want to override: \r\n");
                    
                    uint8_t pressed = false;
                    char key;
                    
                    while( !pressed ){
                        for(uint8_t i = 0;i < BUTTON_ROWS; i++)
                        {
                            for(uint8_t j = 0;j < BUTTON_COLUMNS;j++)
                            {
                                if(PRESSED_BUTTONS[i][j] == TEST_BUTTONS[i][j])
                                {
                                    pressed = true;
                                    key = PRESSED_BUTTONS[i][j];
                                }
                            }
                        }
                    }
                    printf("Overriding %c\r\n", key);
                    
                    STATUS_BUFFER[ count_Key(key) - 1 ] = 0;
                    INFO_BUFFER[ LENGTH ] = 0;
                    ADDRESS_BUFFER[ LENGTH ] = 0;
                    
                    
                    while( true ){
                        
                        printf("Capturing IR signal...\r\n");
                   
                    
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

                        printf("IR signal captured. Enter the name for the signal exactly 4 characters long:\r\n");
                        
                        uint8_t nameLength = 0;
                        char letter;
                        uint32_t name = 0;

                        while( nameLength < 4 ){
                            while(!EUSART1_DataReady);
                            letter =  EUSART1_Read();
                            printf("%c", letter);

                            nameLength++;
                            name = name | letter;
                            if( nameLength != 4 ){ name = name << 8; }
                        }
                        
                        uint32_t commandAddr = generate_Address( key, STATUS_BUFFER[ count_Key(key) - 1 ] );
                        halfWordToByte( BUFFER, IR_SIGNAL_BUFFER );
                        
                        SDCARD_WriteBlock(commandAddr, BUFFER);
                        while ((status = SDCARD_PollWriteComplete()) == WRITE_NOT_COMPLETE);
                        
                        printf("Writing IR_SIGNAL_BUFFER:\r\n");
                        printf("    Address:    ");
                        printf("%04x", commandAddr>>16); printf(":");  printf("%04x",commandAddr&0X0000FFFF);    printf("\r\n");
                        printf("    Status:     %02x\r\n",status);
                        
                        printf("\r\n ------------- \r\n");
                        
                        NAME_BUFFER[ STATUS_BUFFER[ count_Key(key) - 1 ] ] = name;
                        STATUS_BUFFER[ count_Key(key) - 1 ]++;
                                
                        printf("Command is saved. Continue capturing? Press (q) to quit\r\n");
                        while(!EUSART1_DataReady);
                        letter =  EUSART1_Read();
                        if( letter == 'q' ){break;}
                        // 1. We increment Status_Buffer at key's location
                        // 2. Store new command at the new address
                        // 3. Append to the list NAME_BUFFER (RAM) of "unique" commands (naming list)
                        // 4. Check if we are done
                        
                        // 1. Print all unique names with numbers
                        // 2. Ask the user to input the sequence which he wants to execute (Enter to skip)
                        // 3. Get the value at the list of each index -> feed into generate address -> append to ADDRESS_BUFFER -> Increment ADDRESS_BUFFER[0]
                        // 4. Append NAME_BUFFER[index] to the INFO_BUFFER -> Increment INFO_BUFFER[0]
                    }
                    for( uint8_t i = 0;  i < STATUS_BUFFER[ count_Key(key) - 1 ]; i++ ){
                        char buf[5];
                        uint32_t name_t = NAME_BUFFER[i];
                        buf[0] = name_t >> 24;
                        buf[1] = name_t >> 16;
                        buf[2] = name_t >> 8;
                        buf[3] = name_t;
                        buf[4] = '\0';
                        
                        printf("Command: %d  Name: %s\r\n", i, buf);
                    }
                    
                    printf("Enter a sequence of command numbers you would like to bind to the key press.\r\n");
                    printf("Separate command numbers with space and end the sequence by pressing enter.\r\n");
                    
                    uint8_t finished = false;
                    char charNumber;
                    uint8_t number = 0;
                    while( !finished ){
                           while(!EUSART1_DataReady);
                           charNumber =  EUSART1_Read();
                           printf("%c", charNumber);

                           if( charNumber == ' ' || charNumber == '\r' ){
                               
                               if( charNumber == '\r' ){finished = true;}
                               
                               if( number >= STATUS_BUFFER[ count_Key(key) - 1 ] ){ continue; }
                               else{
                                   INFO_BUFFER[ LENGTH ]++;
                                   ADDRESS_BUFFER[ LENGTH ]++;
                                   
                                   INFO_BUFFER[ INFO_BUFFER[ LENGTH ] ] = NAME_BUFFER[ number ];
                                   ADDRESS_BUFFER[ ADDRESS_BUFFER[ LENGTH ] ] = generate_Address( key, number );
                                   number = 0;
                               }
                           }else{
                               number = number * 10;
                               number += (charNumber - 48);
                           }
                       }
                    
                    uint32_t keyAddress = ((count_Key( key ) * 2) - 1)* 512;
                    
                    wordToByte( BUFFER, INFO_BUFFER );
                    SDCARD_WriteBlock(keyAddress, BUFFER);
                    while ((status = SDCARD_PollWriteComplete()) == WRITE_NOT_COMPLETE);

                    printf("Writing INFO_BUFFER:\r\n");
                    printf("    Address:    ");
                    printf("%04x", keyAddress>>16); printf(":");  printf("%04x",keyAddress&0X0000FFFF);    printf("\r\n");
                    printf("    Status:     %02x\r\n",status);
                    
                    keyAddress = (count_Key( key ) * 2) * 512;
                    wordToByte( BUFFER, ADDRESS_BUFFER );
                    SDCARD_WriteBlock(keyAddress, BUFFER);
                    while ((status = SDCARD_PollWriteComplete()) == WRITE_NOT_COMPLETE);

                    printf("Writing ADDRESS_BUFFER:\r\n");
                    printf("    Address:    ");
                    printf("%04x", keyAddress>>16); printf(":");  printf("%04x",keyAddress&0X0000FFFF);    printf("\r\n");
                    printf("    Status:     %02x\r\n",status);
                    
                    break;
                 }
                 
                case '+':
                    printf("LED ON\r\n");
                   EPWM2_LoadDutyValue(LED_ON); 
                   break;
               
               case '-':
                   printf("LED OFF\r\n");
                   EPWM2_LoadDutyValue(LED_OFF); 
                   break;
                 
                case 'S':
                {
                    uint8_t nameLength = 0;
                    char letter;
                    uint32_t word = 0;
                    
                    while( nameLength < 4 ){
                        while(!EUSART1_DataReady);
                        letter =  EUSART1_Read();
                        printf("%c", letter);
                        
                        nameLength++;
                        word = word | letter;
                        if( nameLength != 4 ){ word = word << 8; }
                    }
                    
                    uint16_t a = (uint16_t)(word >> 16);
                    uint16_t b = (uint16_t)((word << 16) >> 16);
                    
                    printf("%x%x ", a, b);
                    printf("\r\n");
                    
                    break;
                }     
//                case 'S':
//                {
//                    uint32_t testBuffer[BLOCK_SIZE/4];
//                    testBuffer[0] = 0x12345678;
//                    testBuffer[1] = 0xABCDEFBB;
//                    
//                    uint32_t loadBuffer[BLOCK_SIZE/4];
//                    
//                    uint16_t testBuff[BLOCK_SIZE/2];
//                    testBuff[0] = 0xDDCA;
//                    testBuff[1] = 0x7941;
//                    testBuff[2] = 0x2457;
//                    testBuff[3] = 0x764E;
//                    
//                    uint16_t loadBuff[BLOCK_SIZE/2];
//                    
//                    uint8_t testBuf[BLOCK_SIZE];
//                    testBuf[0] = 0x12;
//                    testBuf[1] = 0x34;
//                    testBuf[2] = 0x56;
//                    testBuf[3] = 0x78;
//                    testBuf[4] = 0xAB;
//                    testBuf[5] = 0xCD;
//                    testBuf[6] = 0xEF;
//                    testBuf[7] = 0xBB;
//                    uint8_t loadBuf[BLOCK_SIZE];
//                    
//                    wordToByte( loadBuf, testBuffer );
//                    for(uint8_t i = 0; i < 8; i++){
//                        printf("%x ", loadBuf[i]);
//                    }
//                    printf("\r\n");
//                    
//                    byteToWord( loadBuf, loadBuffer );
//                    for(uint8_t i = 0; i < 2; i++){
//                        uint16_t a = (uint16_t)(loadBuffer[i] >> 16);
//                        uint16_t b = (uint16_t)((loadBuffer[i] << 16) >> 16);
//                        printf("%x%x ", a, b);
//                    }
//                    printf("\r\n");
//                    
//                    halfWordToByte( loadBuf, testBuff );
//                    for(uint8_t i = 0; i < 8; i++){
//                        printf("%x ", loadBuf[i]);
//                    }
//                    printf("\r\n");
//                    
//                    byteToHalfWord( loadBuf, loadBuff );
//                    for(uint8_t i = 0; i < 4; i++){
//                        printf("%x ", loadBuff[i]);
//                    }
//                    printf("\r\n");
//                    break;
//                }
                    
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
// Function to translate uint8 buffer to uint32
// The first byte read would become the MSB
//----------------------------------------------
void byteToWord( uint8_t buffer[], uint32_t wordBuffer[] ){

    uint16_t j = 0;
    for(uint16_t i = 0; i < BLOCK_SIZE; i+=4){  
        uint32_t temp = 0;
        for(uint8_t k = 0; k < 4; k++){
            temp = temp | buffer[i+k];
            if(k != 3){
                temp = temp << 8;
            }
        }
        
        wordBuffer[j] = temp;
        j++;
    }
}


//----------------------------------------------
// Function to translate uint8 buffer to uint16
// The first byte read would become the MSB
//----------------------------------------------
void byteToHalfWord( uint8_t buffer[], uint16_t halfWordBuffer[] ){

    uint16_t j = 0;
    for(uint16_t i = 0; i < BLOCK_SIZE; i+=2){  
        uint16_t temp = 0;
        for(uint8_t k = 0; k < 2; k++){
            temp = temp | buffer[i+k];
            if(k != 1){
                temp = temp << 8;
            }
        }
        
        halfWordBuffer[j] = temp;
        j++;
    }
}


//----------------------------------------------
// Function to translate uint32 buffer to uint8
// The first byte read would become the MSB
//----------------------------------------------
void wordToByte( uint8_t buffer[], uint32_t wordBuffer[] ){

    uint16_t j = 4;
    for(uint16_t i = 0; i < BLOCK_SIZE/4; i++){  
        uint32_t temp = wordBuffer[i];
        for(uint8_t k = 0; k < 4; k++){
            buffer[ (j-1)-k ] =(uint8_t) (temp & 0xFF);
            if(k != 3){
                temp = temp >> 8;
            }
        }
        j+=4;
    }
}


//----------------------------------------------
// Function to translate uint16 buffer to uint8
// The first byte read would become the MSB
//----------------------------------------------
void halfWordToByte( uint8_t buffer[], uint16_t halfWordBuffer[] ){

    uint16_t j = 2;
    for(uint16_t i = 0; i < BLOCK_SIZE/2; i++){  
        uint16_t temp = halfWordBuffer[i];
        for(uint8_t k = 0; k < 2; k++){
            buffer[ (j-1)-k ] =(uint8_t) (temp & 0xFF);
            if(k != 1){
                temp = temp >> 8;
            }
        }
        j+=2;
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
            result = 12;
            break; 
            
        case '0':
            result = 10;
            break;    
        
        case '#':
            result = 11;
            break; 
            
        case '7':
            result = 7;
            break; 
        
        case '8':
            result = 8;
            break; 
            
        case '9':
            result = 9;
            break; 
        
        case '4':
            result = 4;
            break; 
            
        case '5':
            result = 5;
            break; 
            
        case '6':
            result = 6;
            break; 
            
        case '1':
            result = 1;
            break; 
            
        case '2':
            result = 2;
            break; 
            
        case '3':
            result = 3;
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
            //printf("transmit\r\n");
            if(index_signal & 0x0001)   //If the index is odd, we know the signal should be high
            {
                EPWM2_LoadDutyValue(LED_ON); 
            }
            else
            {
                EPWM2_LoadDutyValue(LED_OFF); 
            }
  
            TMR1_WriteTimer(0x10000 - micro_Seconds_to_TMR1_Counts(IR_SIGNAL_BUFFER[index_signal]));   //Set the timer for the pulse duration.
        }
        else if(index_address < ADDRESS_BUFFER[LENGTH]) //If we have more codes to send
        {
            index_address++;
            index_signal = 0;

            
            //TODO: find a way to delay the signal sending.
            
            SDCARD_ReadBlock(ADDRESS_BUFFER[index_address], BUFFER); //Get the next code ready to send
            byteToHalfWord( BUFFER, IR_SIGNAL_BUFFER );
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
            
            for(uint8_t j = 0; j < BUTTON_COLUMNS; j++)
            {
                if(escape_flag) break;
                
                if(PRESSED_BUTTONS[i][j] == TEST_BUTTONS[i][j])
                {
                    escape_flag = 1; //We found a button press so stop looking.
                  
                    SDCARD_ReadBlock(count_Key(TEST_BUTTONS[i][j])*BLOCK_SIZE*2, BUFFER); //Get the list of addresses where the codes are stored
                    byteToWord( BUFFER, ADDRESS_BUFFER );
                    
                    
                    SDCARD_ReadBlock(ADDRESS_BUFFER[1], BUFFER);  // Get the first code ready to transmit.
                    byteToHalfWord( BUFFER, IR_SIGNAL_BUFFER );
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