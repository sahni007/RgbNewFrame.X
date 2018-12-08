/*
 * File:   mainFileRgbNewframeFormate.c
 * Author: Alfaone
 *
 * Created on 8 December, 2018, 12:07 PM
 */


#include <xc.h>
#include<pic16f1526.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#pragma config LVP = OFF // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)


#define _XTAL_FREQ 16000000

// Pin MACROS
#define OUTPUT_RELAY1 PORTFbits.RF1
#define OUTPUT_RELAY2 PORTFbits.RF0
#define OUTPUT_RELAY3 PORTAbits.RA3
#define OUTPUT_RELAY4 PORTAbits.RA2


#define INPUTSWITCH1 PORTFbits.RF7
#define INPUTSWITCH2 PORTFbits.RF5
#define INPUTSWITCH3 PORTFbits.RF3
#define INPUTSWITCH4 PORTFbits.RF2


#define INPUT_SWITCH_DIR_1 TRISFbits.TRISF7
#define INPUT_SWITCH_DIR_2 TRISFbits.TRISF5
#define INPUT_SWITCH_DIR_3 TRISFbits.TRISF3
#define INPUT_SWITCH_DIR_4 TRISFbits.TRISF2


#define OUTPUT_RELAY_DIR_1 TRISFbits.TRISF0
#define OUTPUT_RELAY_DIR_2 TRISFbits.TRISF1
#define OUTPUT_RELAY_DIR_3 TRISAbits.TRISA3
#define OUTPUT_RELAY_DIR_4 TRISAbits.TRISA2

#define USART_1_TRANSMIT_OUTPUT_DIR TRISCbits.TRISC6
#define USART_1_RECIEVE_INPUT_DIR TRISCbits.TRISC7

#define RECIEVED_DATA_LENGTH (30+2)
#define TOTAL_NUMBER_OF_SWITCH (4+2)

#define TRUE 1
#define FALSE 0

#define DEBUG



        
/* DATA USED IN MANUAL END HERE*/

unsigned char ErrorNames[5]="####";

int mainReceivedDataPosition=0, mainDataReceived=FALSE;
unsigned char mainReceivedDataBuffer[RECIEVED_DATA_LENGTH]="#"; 
unsigned char copymainReceivedDataBuffer[RECIEVED_DATA_LENGTH]="#";
unsigned char tempReceivedDataBuffer[RECIEVED_DATA_LENGTH-8]="#";
unsigned char parentalLockBuffer[5]="00000";
unsigned char currentStateBuffer[(TOTAL_NUMBER_OF_SWITCH*4)+2]="#";

unsigned int M1;unsigned int M2;unsigned int M3;unsigned int M4;unsigned int M5;



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

void applianceControl(char switchMSB, char switchLSB, char switchSTATE, char dimmerSpeedMSB, char dimmerSpeedLSB, char parentalControl, char finalFrameState);




interrupt void isr(){
 
    // ************************************* UART INTERRUPT *********************************************** //
    if(RC1IF){        
        if(RC1STAbits.OERR){    // If over run error, then reset the receiver
            RC1STAbits.CREN = 0; // countinuous Recieve Disable
            RC1STAbits.CREN = 1; // countinuous Recieve Enable
            
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='O';      ErrorNames[3]='V';
            errorsISR(ErrorNames); 
        } 
        
        mainReceivedDataBuffer[mainReceivedDataPosition] = RC1REG;
        copymainReceivedDataBuffer[mainReceivedDataPosition] = mainReceivedDataBuffer[mainReceivedDataPosition];
        #ifdef DEBUG
        TX1REG = copymainReceivedDataBuffer[mainReceivedDataPosition];
        #endif
        if()
        {
            mainReceivedDataPosition++;
            if(mainReceivedDataPosition>15){
                mainDataReceived=TRUE;
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
    OUTPUT_DIMMER=1;
}
