    void applicationControl(char *ModuleNameString,char *SwitchNumberString,char *AcknowledgementString,char *SwitchStatusString,
                char  *RedColorIntensityString,char *GreenColorIntensityString,char *BlueColorIntensityString,char *ChildLockString,char *FinalframebitString ){
   
        int IntegerSwitchNumber=0;
        int IntergerSwitchStatus = 0;
        int IntergerRedColorIntensity = 0;
        int IntegerGreenColorIntensity = 0;
        int IntegerBlueColorIntensity = 0;
        int IntegerChildLock = 0;
        int IntegerFinalframebit = 0;
        char charchildLock = '0';
        char charSwitchState = '0';
sendAcknowledgment("APP");
        
//while(*ModuleNameString != NULL)//convert string into buffer to concatenate the string>>>Not possible through string
//{
    int i=0;
    for(i=0;i<MODULENAMELENGTH;i++)
    {
        ModuleNameBuffer[i]=*ModuleNameString;
        *ModuleNameString++;
    }
//}
        //frame>>SW.
        strcpy(sendFinalBufferToGAteway,ModuleNameBuffer);//in firststep we need to ccreate a new buffer so copy is best option rather than concatenation
        strcat(sendFinalBufferToGAteway,".");//now add delimiter with all frames in ;last
        

//while(*SwitchNumberString != NULL)
//{
     i=0;
    for(i=0;i<SWITCHNUMBERLENGTH;i++)
    {
        SwitchNumberBuffer[i]=*SwitchNumberString;
        *SwitchNumberString++;
    }
//}
        //>>frame SW.01.
        strcat(sendFinalBufferToGAteway,SwitchNumberBuffer);
        strcat(sendFinalBufferToGAteway,".");
        
//while(*AcknowledgementString != NULL)
//{
//    i=0;
//    for(i=0;i<ACKNOWLEDGEMENTLENGTH;i++)
//    {
//        AcknowledgementBuffer[i]=*AcknowledgementString;
//        *AcknowledgementString++;
//    }
//}
        //frame SW.01.ACTACK
        strcat(sendFinalBufferToGAteway,"ACTACK");
        strcat(sendFinalBufferToGAteway,".");

//while(*SwitchStatusString != NULL)
//{
     i=0;
    for(i=0;i<SWITCHSTATUS;i++)
    {
        SwitchStatusBuffer[i]=*SwitchStatusString;
        *SwitchStatusString++;
    }
//}
        //frame SW.01.ACTACK.1
        strcat(sendFinalBufferToGAteway,SwitchStatusBuffer);
        strcat(sendFinalBufferToGAteway,".");
        
//while(*RedColorIntensityString != NULL)
//{
     i=0;
    for(i=0;i<REDCOLOINTENSITYLENGTH;i++)
    {
        RedColorIntensityBuffer[i]=*RedColorIntensityString;
        *RedColorIntensityString++;
    }
//}
        //frame SW.01.ACTACK.1.0
        strcat(sendFinalBufferToGAteway,RedColorIntensityBuffer);
        strcat(sendFinalBufferToGAteway,".");
        
//while(*GreenColorIntensityString != NULL)
//{
     i=0;
    for(i=0;i<GREENCOLORINTENSITYLENGHT;i++)
    {
        GreenColorIntensityBuffer[i]=*GreenColorIntensityString;
        *GreenColorIntensityString++;
    }
//}
        //frame SW.01.ACTACK.1.0.0
        strcat(sendFinalBufferToGAteway,GreenColorIntensityBuffer);
        strcat(sendFinalBufferToGAteway,".");
        
//while(*BlueColorIntensityBuffer != NULL)
//{
     i=0;
    for(i=0;i<REDCOLOINTENSITYLENGTH;i++)
    {
        BlueColorIntensityBuffer[i]=*BlueColorIntensityBuffer;
        *BlueColorIntensityString++;
    }
//}
        ////frame SW.01.ACTACK.1.0.0.0
        strcat(sendFinalBufferToGAteway,BlueColorIntensityBuffer);
        strcat(sendFinalBufferToGAteway,".");

//while(*ChildLockString != NULL)
//{
    i=0;
    for(i=0;i<CHILDLOCKLENGTH;i++)
    {
        ChildLockBuffer[i]=*ChildLockString;
        *ChildLockString++;
    }
//}
         ////frame SW.01.ACTACK.1.0.0.0.0
        strcat(sendFinalBufferToGAteway,ChildLockBuffer);
        strcat(sendFinalBufferToGAteway,".");
        
//while(*FinalframebitString != NULL)
//{
     i=0;
    for(i=0;i<FINALFRAMEBITLENGTH;i++)
    {
        FinalFramebitBuffer[i]=*FinalframebitString;
        *FinalframebitString++;
    }
//}
        //INPUT FRAME >> SW/,.01.ACT.1.0.0.0.0.1.|
        //>>Final frame be like>>>>SW/RGB.01.ACTACK .1.0.0.0.0.1.|
        
        strcat(sendFinalBufferToGAteway,FinalFramebitBuffer);
        strcat(sendFinalBufferToGAteway,".");
        strcat(sendFinalBufferToGAteway,"|");
        //sendAcknowledgment(sendFinalBufferToGAteway);
        

        
        
        IntegerSwitchNumber = atoi(SwitchNumberString);
        IntergerSwitchStatus = atoi(SwitchStatusString);
        IntergerRedColorIntensity = atoi(RedColorIntensityString);
        IntegerBlueColorIntensity = atoi(GreenColorIntensityString);
        IntegerGreenColorIntensity = atoi(GreenColorIntensityString);
        IntegerChildLock = atoi(ChildLockString);
        IntegerFinalframebit= atoi(FinalframebitString);
        
        charchildLock = IntegerChildLock + '0';
        charSwitchState = IntergerSwitchStatus + '0';
        
        sendAcknowledgment("STS");
        TX1REG = '1';
        
        parentalLockBuffer[IntegerSwitchNumber] = charchildLock;
        copyparentalLockBuffer[IntegerSwitchNumber] = parentalLockBuffer[IntegerSwitchNumber];
        if(IntegerFinalframebit == 1)
        {
            sendAcknowledgment(sendFinalBufferToGAteway);
        }
        
        switch(IntegerSwitchNumber)
        {
            
            case 1:
            {
                OUTPUT_RELAY1 = IntergerSwitchStatus;
                sendAcknowledgment("OK");
            }
                break;
            case 2:
                OUTPUT_RELAY2 = IntergerSwitchStatus;
                break;
            case 3:
                OUTPUT_RELAY3 = IntergerSwitchStatus;
                break;
            case 4:
                OUTPUT_RELAY4 = IntergerSwitchStatus;
                break;
            default:
                break;
            
        }
        
        
    }