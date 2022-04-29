#include <xc.h>
#pragma config FOSC = INTOSC, WDTE = OFF, PWRTE = ON, MCLRE = OFF, CP = OFF
#pragma config CPD = OFF, BOREN = ON, CLKOUTEN = OFF, IESO = OFF, FCMEN = OFF
#pragma config WRT = OFF, PLLEN = OFF, STVREN = ON, BORV = HI, LVP = OFF
#define REnc_def 8000000

unsigned char HD,SendFlag;
unsigned long reData, sendData;
char rL = 0; 
signed char DB[] = {0,-1,1,0,  1,0,0,-1,  -1,0,0,1,  0,1,-1,0}; 

void __interrupt() OnInterrupt(void)
{ 
    if (IOCIF)
    { 
        rL = (rL << 2) | (PORTB >> 4 & 0x3);   
        reData += DB[rL & 0x0F];
        IOCBF = 0;
    }
    if (RCIF)
    {
        if(RCREG == '\n') SendFlag = 1;   
        else if(RCREG == 'r') reData = 8000000;
        RCIF = 0;
    }
}


void SendUART_Raw(unsigned char c)
{ 
    while(!TRMT); 
    TXREG = c;
}


void SendUART(unsigned char c)
{
    SendUART_Raw("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-"[c]);  
}
  

void SendAD()
{ 
    _delay(500);
    ADCON0bits.GO = 1;
    asm("nop");
    while (ADCON0bits.GO_nDONE); 
    HD = ((((ADRESH & 0x03) << 8) + ADRESL) >> 2) & 0x0FF;
    SendUART(HD >> 4 & 0xF);
    SendUART(HD & 0xF);
}
 

void main()
{
    OSCCON = 0xF0;
    reData = 8000000;
    ANSELA = 0x09;
    ANSELB = 0x00;
    TRISA = 0xFF;
    TRISB = 0xFA;
    PORTA = PORTB =  0x00; 
    APFCON0= 0x00;  
    APFCON1= 0x00; 
    BAUDCON= 0x08;
    SPBRGH = 0x00;
    SPBRGL = 0x44;
    TXSTA = 0x24;
    RCSTA = 0x90;    
    FVRCON = 0x00;
    ADCON1 = 0xd0;   
    IOCBN = 0x30;
    IOCBP = 0x30;
    IOCBF = 0;
    IOCIE = 1;
    RCIF = 0; 
    RCIE = PEIE = GIE = 1; 
    LATBbits.LATB0 = 1;
    
    for (;;)
    {
        while(!SendFlag);
        //handles Data-----------
        ADCON0 = 0x01; SendAD(); //handle X  
        ADCON0 = 0x0D; SendAD(); //handle Y 
        //Rotation ------------------------------
        sendData = reData;
        SendUART(sendData >> 18 & 0x3F);
        SendUART(sendData >> 12 & 0x3F);
        SendUART(sendData >>  6 & 0x3F);
        SendUART(sendData       & 0x3F);
        //End -----------------------------------
        SendUART_Raw('\n');
        SendFlag = 0;
    }
}
