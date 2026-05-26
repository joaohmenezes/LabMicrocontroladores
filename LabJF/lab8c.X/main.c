//atividade 1
// Configuration Bits em formato C (Pragma)
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (MCLR)
#pragma config CP = OFF         // Code Protection bit (disabled)
#pragma config CPD = OFF        // Data Code Protection bit (disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (disabled)
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable (off)

// Inclusão de bibliotecas C para XC8
#include <xc.h>
#define _XTAL_FREQ 8000000 // Cristal de 8 MHz

void main(void) {
    // Config pinos
    ANSEL = 0;  // Desabilita funções analógicas
    ANSELH = 0; 
    TRISDbits.TRISD0 = 0; // Configura RD0 como saída
    PORTDbits.RD0 = 0;    // Inicializa com o led apagado
    
    // Config OPTION_REG para o Timer0
    // Prescaler 256 (PS=111), Clock Interno (T0CS=0), Atribuído ao Timer0 (PSA=0)
    OPTION_REG = 0b11010111; 
    
    // variáveis e Timer
    TMR0 = 0; // Inicializa o registrador TMR0 com 0
    int overflows = 0; 
    
    // Polling
    while(1) {
        if (INTCONbits.T0IF) { // Checa a flag de estouro do Timer0
            INTCONbits.T0IF = 0; // Limpa a flag imediatamente
            overflows++;         // Conta o estouro
            
            // 15 overflows * 32,768 ms = ~500 ms
            if (overflows >= 15) { 
                PORTDbits.RD0 = ~PORTDbits.RD0; // Alterne o LED
                overflows = 0; // Zera o contador
            }
        }
    }
}

//atividade 2
#pragma config FOSC = HS        
#pragma config WDTE = OFF       
#pragma config PWRTE = ON       
#pragma config MCLRE = ON       
#pragma config CP = OFF         
#pragma config CPD = OFF        
#pragma config BOREN = ON       
#pragma config IESO = ON        
#pragma config FCMEN = ON       
#pragma config LVP = OFF        
#pragma config BOR4V = BOR40V   
#pragma config WRT = OFF        

#include <xc.h>
#define _XTAL_FREQ 8000000 

// conta os estouros na interrupcao
int overflows = 0;

// Isr
void __interrupt() ISR(void) { 
    if (PIR1bits.TMR1IF) {           // Checa se a interrupção foi gerada pelo Timer1 
        PIR1bits.TMR1IF = 0;         // Limpa a flag de estouro 
        
        // Recarrega o Timer1 com o valor inicial 0x0BDC para mais 250 ms 
        TMR1H = 0x0B; 
        TMR1L = 0xDC;
        
        overflows++;                 // Incrementa o contador 
        if (overflows >= 4) {        // Ao atingir 4 estouros 1 seg
            PORTDbits.RD0 = ~PORTDbits.RD0; // Alterne o estado do LED em RD0 
            overflows = 0;           // Zera o contador para o próximo ciclo
        }
    }
}

void main(void) {
    //Configr pinos
    ANSEL = 0;  // Desabilita func analogicas
    ANSELH = 0; 
    TRISDbits.TRISD0 = 0; // RD0 como saída 
    PORTDbits.RD0 = 0;    // Inicializa o LED apagado
    
    // Config o registrador T1CON do Timer1 
    // Prescaler 1-8 T1CKPS=1), Clock interno (TMR1CS=0), Timer1 ON (TMR1ON=1) 
    T1CON = 0b00110001; 
    
    // Carrega valor inicial no Timer1 (0x0BDC para 250ms) 
    TMR1H = 0x0B;
    TMR1L = 0xDC;
    
    // Configurar Interrupções 
    PIR1bits.TMR1IF = 0; // Limpa a flag por segurança antes de habilitar 
    PIE1bits.TMR1IE = 1; // Habilita interrupção específica do Timer1 
    INTCONbits.PEIE = 1; // Habilita interrupções de periféricos 
    INTCONbits.GIE = 1;  // Habilita a chave geral de interrupções globais 
    
    while(1) {
    }
}


//atividade 3
#pragma config FOSC = HS        
#pragma config WDTE = OFF       
#pragma config PWRTE = ON       
#pragma config MCLRE = ON       
#pragma config CP = OFF         
#pragma config CPD = OFF        
#pragma config BOREN = ON       
#pragma config IESO = ON        
#pragma config FCMEN = ON       
#pragma config LVP = OFF        
#pragma config BOR4V = BOR40V   
#pragma config WRT = OFF        

#include <xc.h>
#define _XTAL_FREQ 8000000 

void main(void) {
    ANSEL = 0;  // Desabilita funções analógicas
    ANSELH = 0; 
    TRISDbits.TRISD0 = 0; // Configura RD0 como saída
    PORTDbits.RD0 = 0;    // Inicializa o LED apagado
    
    // Configura RC0 como entrada digital para ler o botão externo
    TRISCbits.TRISC0 = 1; 
    // Configurar o registrador T1CON para Contador Externo
    // Clock Externo no pino T1CKI (TMR1CS=1), Síncrono (T1SYNC=0), Prescaler 1:1 (T1CKPS=00), Timer1 ON (TMR1ON=1)
    T1CON = 0b00000011; 
    
    // Carregar valor inicial no Timer1 (0xFFFC)
    // Isso faz com que o Timer estoure após exatamente 4 pulsos no botão
    TMR1H = 0xFF; 
    TMR1L = 0xFC;
    
    // Limpa a flag por garantia antes de entrar no loop
    PIR1bits.TMR1IF = 0;
    
    // Polling 
    while(1) {
        if (PIR1bits.TMR1IF) { // Checa o estouro
            PIR1bits.TMR1IF = 0; // Limpa a flag 
            
            PORTDbits.RD0 = ~PORTDbits.RD0; // Alterne o estado do LED em RD0 
            // Recarrega o Timer1 para a próxima contagem de 4 apertos 
            TMR1H = 0xFF; 
            TMR1L = 0xFC;
        }
    }
}