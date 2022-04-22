#include <xc.h>
#pragma config FOSC = INTOSC, WDTE = OFF, PWRTE = ON, MCLRE = OFF, CP = OFF
#pragma config CPD = OFF, BOREN = ON, CLKOUTEN = OFF, IESO = OFF, FCMEN = OFF
#pragma config WRT = OFF, PLLEN = OFF, STVREN = ON, BORV = HI, LVP = OFF
#define REnc_def 8000000
#define X 0
#define P 1
#define M -1 
 
typedef union 
{
    struct 
    {
        unsigned SendFlag : 1;
        unsigned RENC_LDAT : 2; 
    };
} DATA;
 
char RENC_DATASHEET[4][4] =//[nA << 1 + nB][]
{
    {0,P,M,X},
    {M,0,X,P},
    {P,X,0,M},
    {X,M,P,0}, 
};

unsigned char HD;
unsigned short reData, sendData;
DATA data;


void SendUART_Raw(unsigned char c)
{ 
    while(!TRMT); 
    TXREG = c;
}

void SendUART(unsigned char c) 
{ 
    SendUART_Raw("0123456789ABCDEF"[c]);  
}


void SendUART64(unsigned char c)
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



void __interrupt() OnInterrupt(void)
{
    if (IOCIF)
    { 
        reData += RENC_DATASHEET[data.RENC_LDAT][PORTB >> 4 & 0x3];
        data.RENC_LDAT = PORTB >> 4 & 0x3;
    }
    
    if (RCIF)
    {
        if(RCREG == '\n') data.SendFlag = 1;   
        else if(RCREG == 'r') reData = 8000000;
        RCIF = 0;
    }
}



void main()
{
    OSCCON = 0xF0;
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
    reData = 8000000;
    data.RENC_LDAT = PORTB >> 4 & 0x3;
    LATBbits.LATB0 = 1;
    for (;;)
    {
        while(!data.SendFlag);
        
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
        SendUART64(sendData >> 18);
        SendUART64(sendData >> 12 & 0x3F);
        SendUART64(sendData >> 6 & 0x3F);
        SendUART64(sendData & 0x3F);
        
        //End -----------------------------------
        SendUART_Raw('\n');
        data.SendFlag = 0;
    }
    
}

///test_Serial/testAD/testRenc