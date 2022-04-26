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
    
    /*これに後で置き換える!!
    data.N = PORTAbits.RA5;
    if(data.N != data.L)
    { 
          rot += data.N ^ PORTAbits.RA4 ? -1 : 1;
          data.L = data.N;
    }
     */
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
 

unsigned char ReadAD()
{ 
    _delay(400);
    ADCON0bits.GO = 1;
    asm("nop");
    while (ADCON0bits.GO_nDONE);
    return ADRESL;
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
    ADCON1 = 0x50;   
    IOCBN = 0x30;
    IOCBP = 0x30;
    IOCBF = 0;
    IOCIE = 1;
    RCIF = 0;
    RCIE = 1;
    PEIE = 1;
    GIE = 1; 
    LATBbits.LATB0 = 1;
    
    for (;;)
    {
        while(!SendFlag);
        
        //handle X-----------
        ADCON0 = 0x01;  
        HD = ReadAD();
        SendUART(HD >> 4);
        SendUART(HD & 0xF);
        
        //handle Y------------
        ADCON0 = 0x0D; 
        HD = ReadAD();
        SendUART(HD >> 4);
        SendUART(HD & 0xF);
        
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
