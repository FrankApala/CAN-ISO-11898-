/*
© [2025] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#define FCY 8000000UL  // Par exemple pour 40 MHz
#include <libpic30.h>
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/can/can1.h"
#include "mcc_generated_files/system/pins.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
/*
    Main application
*/

bool canInt= false;

static void PrintDemoFeaturesMessage(void)
{
    printf("\r\n");
    printf("*******************************************************\r\n");
    printf("dsPIC33CK Curiosity CAN 2.0 Demo\r\n");
    printf("*******************************************************\r\n");
    
    printf("* CAN-2.0 communication using the on-chip CAN peripheral in dsPIC33CK256MP508\r\n");
    printf("* Loopback CAN data frame if received  message ID is 0x64 or 0x65 from the bus\r\n");
    printf("* Green LED will blink if data message  with 0x64 or 0x65 is received and\r\n transmitted back successfully\r\n\r\n");

}

static uint8_t CAN_DlcToDataBytesGet(const enum CAN_DLC dlc)
{
    static const uint8_t dlcByteSize[] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U};
    return dlcByteSize[dlc];
}

static void printCanDatainHex(struct CAN_MSG_OBJ *rxCanMsg)
{
    uint8_t i=0;
    for(i=0;i<CAN_DlcToDataBytesGet(rxCanMsg->field.dlc);i++)
    {
        printf("0x%X ",rxCanMsg->data[i]);
    }
    printf("\r\n");
}

static void PrintCanMsgObjStruct(struct CAN_MSG_OBJ *rxCanMsg)
{
    printf("------------------------------------------------------------------\r\n");
    printf("[*] Msg ID: 0x%lX\r\n", rxCanMsg->msgId);
    printf("[*] Msg Data:");
    printCanDatainHex(rxCanMsg);
    printf("[*] DLC: 0x%X\r\n", CAN_DlcToDataBytesGet(rxCanMsg->field.dlc));
    printf("[*] IdType: %s\r\n", rxCanMsg->field.idType == CAN_FRAME_STD ? "CAN_FRAME_STD" : "CAN_FRAME_EXT");
    printf("[*] FrameType: %s\r\n", rxCanMsg->field.frameType == CAN_FRAME_DATA ? "CAN_FRAME_DATA" : "CAN_FRAME_RTR");
    printf("[*] FormateType: %s\r\n", rxCanMsg->field.formatType == CAN_2_0_FORMAT ? "CAN_2_0_FORMAT" : "CAN_FD_FORMAT");
    printf("------------------------------------------------------------------\r\n");
}


int main(){
    
    SYSTEM_Initialize();
    
    __builtin_enable_interrupts(); 
    
    PrintDemoFeaturesMessage();

  uint8_t txData[8] = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};

  struct CAN_MSG_OBJ canMsg, rxMsg;

canMsg.msgId=0x55;
canMsg.field.idType=CAN_FRAME_STD;
canMsg.field.dlc=DLC_8;
canMsg.field.frameType=CAN_FRAME_DATA;
canMsg.field.brs=CAN_NON_BRS_MODE;
canMsg.field.formatType= CAN_2_0_FORMAT;

canMsg.data=txData;

while(1){
    
/*CAN_Driver.Transmit(CAN1_TXQ, &canMsg);
 printf("******** FRAME SENT*********\r\n");
PrintCanMsgObjStruct(&canMsg);
            // Optional: handle error (e.g., FIFO full, BRS error)
       

        __delay_ms(10000);  // Delay between messages*/
        
 /*     while (!CAN_Driver.Receive(&rxMsg));
 
    // === STEP 5: Process Received Message ===
    if (rxMsg.msgId == 0x65 && rxMsg.field.dlc == 8)
    {
        // Message received successfully ? do something
         LED_GREEN_Toggle();
         PrintCanMsgObjStruct(&rxMsg);
        // Example: toggle a pin or compare data
    }*/
        
        
    if (canInt){
  if (CAN1_Receive(&rxMsg))  // This also calls UINC to update the FIFO pointer
    {
     PrintCanMsgObjStruct(&rxMsg);
    }
  C1INTHbits.RXIE = 1;
IEC1bits.C1RXIE = 1;
C1FIFOCON1Lbits.TFNRFNIE = 1;
}
        
}


}


void __attribute__((__interrupt__, no_auto_psv)) _C1RXInterrupt(void)
{
    canInt = true;

    // Clear RX interrupt flags
    C1INTLbits.RXIF = 0;
    IFS1bits.C1RXIF = 0;

    // Optionally disable interrupts if processing will take time
    C1INTHbits.RXIE = 0;
    IEC1bits.C1RXIE = 0;
    C1FIFOCON1Lbits.TFNRFNIE = 0;
}
