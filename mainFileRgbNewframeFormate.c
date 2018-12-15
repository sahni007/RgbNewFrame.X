/* 
 * File:   varun_4_1.c
 * Author: VARUNS SAHNI
 *
 * Created on 8 April, 2018, 8:40 PM
 * this is working code of 4 on off switches
 */
//INPUT FRAME: SW/RGB.<device_number>.ACT.<status>.<REedColorIntensity>.<greencolorintensity>.<bluecolorintensity>.<chilLock>.<finalframebit>.|
//example: SW.01.ACT.255.255.255.0.1.|
//OUTPUT FRAME: SW/RGB.<device_number>.ACTACK.<status>.<REedColorIntensity>.<greencolorintensity>.<bluecolorintensity>.<chilLock>.<finalframebit>.|
//example: SW.01.ACTACK.255.255.255.0.1.|
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include<pic16f1526.h>
#include<string.h>
#include<ctype.h>
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection (HS Oscillator, High-speed crystal/resonator connected between OSC1 and OSC2 pins)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable bit (VCAP pin function disabled)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will not cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


// Since we have used 16 MHz crystal
#define _XTAL_FREQ 16000000  

// Pin MACROS
#define OUTPUT_RELAY1 PORTFbits.RF1
#define OUTPUT_RELAY2 PORTFbits.RF0
#define OUTPUT_RELAY3 PORTAbits.RA3
#define OUTPUT_RELAY4 PORTAbits.RA2

#define OUTPUT_RELAY_DIR_1 TRISFbits.TRISF1
#define OUTPUT_RELAY_DIR_2 TRISFbits.TRISF0
#define OUTPUT_RELAY_DIR_3 TRISAbits.TRISA3
#define OUTPUT_RELAY_DIR_4 TRISAbits.TRISA2

#define INPUTSWITCH1 PORTFbits.RF7
#define INPUTSWITCH2 PORTFbits.RF5
#define INPUTSWITCH3 PORTFbits.RF3
#define INPUTSWITCH4 PORTFbits.RF2


#define INPUT_SWITCH_DIR_1 TRISFbits.TRISF7
#define INPUT_SWITCH_DIR_2 TRISFbits.TRISF5
#define INPUT_SWITCH_DIR_3 TRISFbits.TRISF3
#define INPUT_SWITCH_DIR_4 TRISFbits.TRISF2



// direction of PWM OUTPUT to MOC3021

/*
 * Extra Periferals Direction and PORT
 */
//#define ZCD_CCP9_DIR TRISEbits.TRISE3
// USART Directions
#define USART_1_TRANSMIT_OUTPUT_DIR TRISCbits.TRISC6
#define USART_1_RECIEVE_INPUT_DIR TRISCbits.TRISC7

#define RECIEVED_DATA_LENGTH (16+2)
#define TOTAL_NUMBER_OF_SWITCH (5+2)

#define TRUE 1
#define FALSE 0




// Conditional compilation
#define DEBUG
#define MODULENAMELENGTH 3
#define SWITCHNUMBERLENGTH 2
#define ACKNOWLEDGEMENTLENGTH 6
#define SWITCHSTATUS 1
#define REDCOLOINTENSITYLENGTH 3
#define GREENCOLORINTENSITYLENGHT 3
#define BLUECOLOINTENSITYLENGHT 3
#define CHILDLOCKLENGTH 1
#define FINALFRAMEBITLENGTH 1

#define FINALFRAMELENGTH ((3+2+6+1+3+3+3+1+1)*2)

unsigned char ModuleNameBuffer[MODULENAMELENGTH]="#";
unsigned char SwitchNumberBuffer[SWITCHNUMBERLENGTH]="#";
unsigned char AcknowledgementBuffer[ACKNOWLEDGEMENTLENGTH]="#";
unsigned char SwitchStatusBuffer[SWITCHSTATUS]="#";
unsigned char RedColorIntensityBuffer[REDCOLOINTENSITYLENGTH]="#";
unsigned char GreenColorIntensityBuffer[GREENCOLORINTENSITYLENGHT]="#";
unsigned char BlueColorIntensityBuffer[BLUECOLOINTENSITYLENGHT]="#";
unsigned char ChildLockBuffer[CHILDLOCKLENGTH]="#";
unsigned char FinalFramebitBuffer[FINALFRAMEBITLENGTH]="#";

unsigned char sendFinalBufferToGAteway[FINALFRAMELENGTH];

// ALL error Definitions
/* 
 * #define WRONG_DATA_RECIEVED_ERROR_CODE ERRX
 * #define RECIVING_OVERRUN_ERROR EROV
 * #define RECEIVING_DATA_LOST_IN_MAIN ERLS
 */
/* DATA USED IN MANUAL  STARTS HERE*/
unsigned int M1;unsigned int M2;unsigned int M3;unsigned int M4;unsigned int M5;


#define ON 1
#define OFF 0
#define CHAR_OFF '0'
#define CHAR_ON '1'
        
/* DATA USED IN MANUAL END HERE*/

unsigned char ErrorNames[5]="####";

int mainReceivedDataPosition=0, mainDataReceived=FALSE;
unsigned char mainReceivedDataBuffer[RECIEVED_DATA_LENGTH]="#"; 
unsigned char copymainReceivedDataBuffer[RECIEVED_DATA_LENGTH]="#";
unsigned char tempReceivedDataBuffer[RECIEVED_DATA_LENGTH-8]="#";
unsigned char parentalLockBuffer[5]="0000";
unsigned char copyparentalLockBuffer[5]="0000";
char *ConvertmaindataReceiveIntoString = NULL;
char *ModuleName = NULL;
char *token=NULL;
int partCounter=0;
char *StoreModuleName = NULL;

char *ModuleNameString=NULL;
char *SwitchNumberString = NULL;
char *AcknowledgementString =NULL;
char *SwitchStatusString = NULL;
char *RedColorIntensityString = NULL;
char *GreenColorIntensityString = NULL;
char *BlueColorIntensityString = NULL;
char *ChildLockString = NULL;
char *FinalframebitString = NULL;


unsigned int M1;unsigned int M2;unsigned int M3;unsigned int M4;unsigned int M5;

int start_PWM_Generation_in_ISR_FLAG=FALSE;
char levelofDimmer_MSB='0',levelofDimmer_LSB='0';

void errorsISR(char* errNum);
void errorsMain(char* errNum);
void sendAcknowledgment(char* currentStateBuffer);

void clearAllPorts();
void pinINIT_extra();
void GPIO_pin_Initialize();
void peripheralsEnable();
void AllInterruptEnable();
void EUSART_Initialize();

void TMR3_Initialize();
void TMR1_Initialize();
void CCP9_Initialize();
void allPeripheralInit();

void copyReceivedDataBuffer();

    void applicationControl(char *ModuleNameString,char *SwitchNumberString,char *AcknowledgementString,char *SwitchStatusString,
                char  *RedColorIntensityString,char *GreenColorIntensityString,char *BlueColorIntensityString,char *ChildLockString,char *FinalframebitString );
#include "include.h"
   
interrupt void isr(){
 
    // ************************************* UART INTERRUPT *********************************************** //
    if(RC1IF){        
        if(RC1STAbits.OERR){    // If over run error, then reset the receiver
            RC1STAbits.CREN = 0; // countinuous Recieve Disable
            RC1STAbits.CREN = 1; // countinuous Recieve Enable           
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='O';      ErrorNames[3]='V';
            errorsISR(ErrorNames); 
        } 
        
        mainReceivedDataBuffer[mainReceivedDataPosition]=RC1REG;
       copymainReceivedDataBuffer[mainReceivedDataPosition] = mainReceivedDataBuffer[mainReceivedDataPosition];
        #ifdef DEBUG
        TX1REG=copymainReceivedDataBuffer[mainReceivedDataPosition];
        #endif
        if(copymainReceivedDataBuffer[0] == 'R'){
            
            mainReceivedDataPosition++;
            if(mainReceivedDataPosition > 30){
                mainDataReceived=TRUE;   
                 ErrorNames[0]='O';      ErrorNames[1]='K';     
            errorsISR(ErrorNames); 
                mainReceivedDataPosition=0;                
                RC1IF=0;                
            }
        }
        else{
            RC1STAbits.CREN = 0; // countinuous Recieve Disable
            RC1STAbits.CREN = 1; // countinuous Recieve Enable
            mainReceivedDataPosition=0; // Reinitiate buffer counter
            
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='R';      ErrorNames[3]='X';
            errorsISR(ErrorNames);            
        }
    }// End of RC1IF 

}



/*
 * Alfaone Main code starts here
 * For 4 switches 1 Dimmer
 */
int main() {
    __delay_ms(2000);
        M1=ON;    M2=ON;    M3=ON;    M4=ON;    M5=ON;
        OUTPUT_RELAY1 = OFF;  OUTPUT_RELAY2 = OFF;    OUTPUT_RELAY3 = OFF;  OUTPUT_RELAY4 = OFF;

            GPIO_pin_Initialize();
            allPeripheralInit();
    
    while(1){       
        if(mainDataReceived == TRUE){
               ErrorNames[0]='M';      ErrorNames[1]='A';      ErrorNames[2]='I';      ErrorNames[3]='N';
                errorsMain(ErrorNames);
            mainDataReceived=FALSE;       
            //ConvertmaindataReceiveIntoString = copymainReceivedDataBuffer;
//            int i=0;
//            for(i=0;i<30;i++)
//            {
//               TX1REG = copymainReceivedDataBuffer[i];__delay_ms(1);
//            }
            
           // sendAcknowledgment(ConvertmaindataReceiveIntoString);
//            token = strtok(ConvertmaindataReceiveIntoString,".");
//            ModuleName = token;
           // errorsMain("MOD:");
           //  errorsMain(ModuleName);
//              if((strcmp(StoreModuleName,"RGB") == 0) || (strcmp(StoreModuleName,"SW") == 0)){  
//                 // errorsMain("KILO");OK
//               while(token != NULL)
//                {
//                    partCounter++;
//                    token = strtok(NULL,".");
//                    
//                    switch(partCounter)
//                    {
//                        case 1:
//                        {
//                            
//                            ModuleNameString = token;
//                            errorsMain(ModuleNameString);
//                            break;
//                        }
//                        case 2:
//                        {
//                            
//                            SwitchNumberString = token;
//                            errorsMain(SwitchNumberString);
//                            break;
//                        }
//                        case 3:
//                        {
//                            AcknowledgementString = token;
//                            errorsMain(AcknowledgementString);
//                            break;
//                        }
//                        case 4:
//                        {
//                            SwitchStatusString = token;
//                            errorsMain(SwitchStatusString);
//                            break;
//                        }
//                        case 5:
//                        {
//                            RedColorIntensityString = token;
//                            errorsMain(RedColorIntensityString);
//                            break;
//                        }
//                        case 6:
//                        {
//                            GreenColorIntensityString = token;
//                            errorsMain(GreenColorIntensityString);
//                            break;
//                        }   
//                        case 7:
//                        {
//                            BlueColorIntensityString = token;
//                            errorsMain(BlueColorIntensityString);
//                            break;
//                        }
//                        case 8:
//                        {
//                            ChildLockString = token;
//                            errorsMain(ChildLockString);
//                            break;
//                        }
//                        case 9:
//                        {
//                            FinalframebitString =token;
//                            errorsMain(FinalframebitString);
//                            break;
//                        }
//                        default:
//                            break;
//                         
//                            
//                    }
//                } 
            if(copymainReceivedDataBuffer[0] == 'R' && copymainReceivedDataBuffer[1] == 'G' && copymainReceivedDataBuffer[2] == 'B' && copymainReceivedDataBuffer[7] == 'A'
                    && copymainReceivedDataBuffer[8] == 'C' && copymainReceivedDataBuffer[9] == 'T' && copymainReceivedDataBuffer[27] == '1' && copymainReceivedDataBuffer[29] == '|'){
                errorsMain("OK");
                 applicationControl(ModuleNameString,SwitchNumberString,AcknowledgementString,SwitchStatusString,
                 RedColorIntensityString,GreenColorIntensityString,BlueColorIntensityString,ChildLockString,FinalframebitString ); 
            }   // End of all buffer data check
            else
            {
//                ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='L';      ErrorNames[3]='S';
//                errorsMain(ErrorNames);
               // errorsMain("ERLS");
                  errorsMain("FRLS");
                RC1STAbits.SPEN=0;  // Serial port disabled 
                RC1STAbits.CREN = 0; // countinuous Recieve Disable                
                for(int dataBufferCounter = 0; dataBufferCounter < 29; dataBufferCounter++)
                {
                    mainReceivedDataBuffer[dataBufferCounter] = '#'; // clean received data buffer
                }
                RC1STAbits.CREN = 1; // countinuous Recieve Enable
                RC1STAbits.SPEN=1;  // Serial port enabled (configures RXx/DTx and TXx/CKx pins as serial port pins)
            }
      

            
        } // End of mainDataReceived condition
        /******************** MANUAL RESPONE STARTS HERE************ */
        
        //check switch one status
        //off condition
       int man = 1;
        if(parentalLockBuffer[1] == CHAR_OFF  && INPUTSWITCH1 == OFF && M1 == OFF)
        {
            if(man == 1)
            {
            sendAcknowledgment("SW.01.ACTACK.0.0.0.0.0.1.|");
            //sendAcknowledgment(sendFinalBufferToGAteway);
            OUTPUT_RELAY1=OFF;
            }
            man=0;
            M1=1;
            
        }
        //on condition
        if(parentalLockBuffer[1] == CHAR_OFF && INPUTSWITCH1 == ON && M1 == ON)
        {
            //TX1REG='C';
            if(man==1)
            {
            sendAcknowledgment("SW.01.ACTACK.1.0.0.0.0.1.|");
            OUTPUT_RELAY1=ON;
            }
            man=0;
            M1=0;
        }
        
       // //check switch second status 
        //off condition
        if(parentalLockBuffer[2] == CHAR_OFF && INPUTSWITCH2 == OFF && M2 == OFF)
        {
            if(man==1)
            {
            sendAcknowledgment("SW.01.ACTACK.2.0.0.0.0.1.|");
            OUTPUT_RELAY2=OFF;
            }
            man=0;
            M2=1;
        }
        //on condtion
        if(parentalLockBuffer[2] == CHAR_OFF && INPUTSWITCH2 == ON && M2 == ON)
        {
            if(man==1)
            {
           sendAcknowledgment("SW.02.ACTACK.0.0.0.0.0.1.|");
            OUTPUT_RELAY2=ON;
            }
            man=0;
            M2=0;
        }
        
        
       // //check switch third status 
        //off condition
        if(parentalLockBuffer[3] == CHAR_OFF && INPUTSWITCH3 == OFF && M3 == OFF)
        {
            if(man == 1)
            {
             sendAcknowledgment("SW.03.ACTACK.0.0.0.0.0.1.|");
            OUTPUT_RELAY3=OFF;
            }
            man=0;
            M3=1;
          
        }
        //on condtion
        if(parentalLockBuffer[3] == CHAR_OFF && INPUTSWITCH3 == ON && M3 == ON)
        {
            if(man==1)
            {
            sendAcknowledgment("SW.03.ACTACK.1.0.0.0.0.1.|");
            OUTPUT_RELAY3=ON;
            }
            man=0;
            M3=0;
            
        }
        
       // //check switch fourth status 
        //off condition
        if(parentalLockBuffer[4] == CHAR_OFF && INPUTSWITCH4 == OFF && M4 == OFF)
        {
            if(man==1)
            {
            sendAcknowledgment("SW.04.ACTACK.0.0.0.0.0.1.|");
            OUTPUT_RELAY4=OFF;
            }
            man=0;
            M4=1;
            
        }
        //on condtion
        if(parentalLockBuffer[4] == CHAR_OFF && INPUTSWITCH4 == ON && M4 == ON)
        {
            if(man==1)
            {
           sendAcknowledgment("SW.04.ACTACK.1.0.0.0.0.1.|");
            OUTPUT_RELAY4=ON;
            }
            man=0;
            M4=0;
           
        }

    }    
}

/*
 * All input output pin initialization
 */
void GPIO_pin_Initialize(){
    clearAllPorts();
    pinINIT_extra();
    INPUT_SWITCH_DIR_1 = 1;
    INPUT_SWITCH_DIR_2 = 1;
    INPUT_SWITCH_DIR_3 = 1;
    INPUT_SWITCH_DIR_4 = 1;
   
    
    OUTPUT_RELAY_DIR_1 = 0;
    OUTPUT_RELAY_DIR_2 = 0;
    OUTPUT_RELAY_DIR_3 = 0;
    OUTPUT_RELAY_DIR_4 = 0;
   
    
    // peripherals directions
    
    // USART DIRECTIONS
    USART_1_TRANSMIT_OUTPUT_DIR = 0;
    USART_1_RECIEVE_INPUT_DIR = 1;
    
    clearAllPorts();
}

/*
 * ALL Peripheral Initialization
 */
void allPeripheralInit(){
    EUSART_Initialize();

}

/*
 * USART Control Registers initialization
 */
void EUSART_Initialize(){
    PIE1bits.RC1IE = 0;
    PIE1bits.TX1IE = 0;

    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE enabled; ABDEN disabled;
    BAUD1CON = 0x0A;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled;
    RC1STA = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave;
    TX1STA = 0x24;

    // Baud Rate = 9600; SP1BRGL 12;
    //SPBRGL = 0x0C;
    //SPBRGL = 0x19;                  // SP1BRGL is 25 (hex value=0x19) for 9600 baud on 16 MHz crystal frequency
    SP1BRGL = 0xA0;                  // SYNC =0 ; BRGH = 1 ; BRG16=1;
    // Baud Rate = 9600; SP1BRGH 1;
    SP1BRGH = 0x01;

    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;

    // enable receive interrupt
    PIE1bits.RC1IE = 1;                    // handled into INTERRUPT_Initialize()

    // Transmit Enabled
    TX1STAbits.TXEN = 1;

    // Serial Port Enabled
    RC1STAbits.SPEN = 1;
}



void errorsISR(char* errNum){
    int Tx_count=0;
  	while(Tx_count!=4)
 	{ 
        while (!TX1STAbits.TRMT);
 		TX1REG = *errNum;
 		*errNum++;
        Tx_count++;
 	}
}
void errorsMain(char* errNum){
   int Tx_count=0;
  	while(Tx_count != FINALFRAMELENGTH)
 	{ 
        while (!TX1STAbits.TRMT);
 		TX1REG = *errNum;
 		*errNum++;
        Tx_count++;
 	}
}
void sendAcknowledgment(char* currentStateBuffer){
  int Tx_count=0;
  	while(Tx_count != FINALFRAMELENGTH)
 	{ 
        while (!TX1STAbits.TRMT);
 		TX1REG = *currentStateBuffer;
 		*currentStateBuffer++;
        Tx_count++;
 	}
}

/*
 * AANALOG and PULL up REGISTERS related initialization
 */
void pinINIT_extra(){
    ANSELG=0x00;    WPUG = 0;
    
    ANSELF=0x00;
    
    ANSELE=0x00;    WPUE=0x00;
    
    ANSELD=0x00;    WPUD=0x00;
    
    ANSELB=0x00;    WPUB=0x00;
    
    ANSELA=0x00;     
} 

/*
 * always clear all the ports before initialization
 */
void clearAllPorts(){
    OUTPUT_RELAY1=0;
    OUTPUT_RELAY2=0;
    OUTPUT_RELAY3=0;
    OUTPUT_RELAY4=0;
    
}