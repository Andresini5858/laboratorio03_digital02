/*
 * File:   main.c
 * Author: Andres
 *
 * Created on February 3, 2023, 1:53 AM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "SPI.h"

#define _XTAL_FREQ 4000000 //Frecuencia 4MHz
//SLAVE2 SLAVE2 SLAVE2

unsigned char voltaje1;
int var;

void setup(void);
void setupADC(void);
int map(int value, int inputmin, int inputmax, int outmin, int outmax){ //función para mapear valores
    return ((value - inputmin)*(outmax-outmin)) / (inputmax-inputmin)+outmin;}

void main(void) {
    setup();
    setupADC();
    while(1){
        ADCON0bits.GO = 1;    
    }
}

void __interrupt() isr(void){ //
    if (PIR1bits.ADIF == 1){
        
        if (ADCON0bits.CHS == 0b0000){
            voltaje1 = ADRESH;
            ADCON0bits.CHS = 0b0000;
        }
        PIR1bits.ADIF = 0;
    }
    if (PIR1bits.SSPIF == 1){
        var = spiRead();
        if (var == 2){
            spiWrite(voltaje1);
            PIR1bits.SSPIF = 0;
        }
    }
}

void setup(void){
    ANSELbits.ANS0 = 1;
    ANSELbits.ANS4 = 0;
    ANSELH = 0;
    
    TRISAbits.TRISA5 = 1;
    TRISB = 0;
    TRISD = 0;
    
    PORTB = 0;
    PORTD = 0;
    
    INTCONbits.GIE = 1; //Activar interrupciones globales
    INTCONbits.PEIE = 1; //Activar interrupciones periféricas
    PIE1bits.ADIE = 1; // Habiliar interrupcion del conversor ADC
    PIR1bits.SSPIF = 0;
    PIE1bits.SSPIE = 1;
    
    OSCCONbits.IRCF2 = 1; //Oscilador interno a 4MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;
    OSCCONbits.SCS = 1; //Utilizar oscilador interno
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}

void setupADC(void){
    ADCON0bits.ADCS1 = 0; // Fosc/8        
    ADCON0bits.ADCS0 = 1; //       
    
    ADCON1bits.VCFG1 = 0; // Referencia VSS (0 Volts)
    ADCON1bits.VCFG0 = 0; // Referencia VDD (5 Volts)
    
    ADCON1bits.ADFM = 0;  // Justificado hacia izquierda
    
    ADCON0bits.CHS3 = 0; // Canal AN0
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS0 = 0;        
    
    ADCON0bits.ADON = 1; // Habilitamos el ADC
    __delay_us(100); //delay de 100 us
}

