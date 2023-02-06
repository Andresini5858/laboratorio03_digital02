/*
 * File:   main_slave_1.c
 * Author: Andres Lemus
 *
 * Created on February 1, 2023, 10:15 PM
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
#include "SPI.h" //Libería SPI

#define _XTAL_FREQ 4000000 //Frecuencia 4MHz
//SLAVE1 SLAVE1 SLAVE1

unsigned char var; //Variable para decidir que dato enviar
unsigned char voltaje1 = 0; //Variable para guardar voltaje del potenciómetro
unsigned char contador = 0; //Variable para guardar contador
unsigned char bandera = 0; //Variable para antirrebote

void setup(void); //Función de configuración
void setupADC(void); //Función de configuración del ADC

void main(void) {
    setup(); //Llamar a configuración
    setupADC(); //Llamar a configuración del ADC
    while(1){ //LOOP
        ADCON0bits.GO = 1; //Iniciar conversión ADC   
    }
}

void __interrupt() isr(void){ //Interrupciones
    if (PIR1bits.ADIF == 1){ //Revisar si es interrupción del ADC
        if (ADCON0bits.CHS == 0b0000){ //Verificar el canal AN0
            voltaje1 = ADRESH; //Guardar valor de ADRESH
            ADCON0bits.CHS = 0b0000; //Canal AN0
        }
        PIR1bits.ADIF = 0; //Limpiar bandera
    }
    if (PIR1bits.SSPIF == 1){ //Revisar si es interrupción del SPI
        var = spiRead(); //Leer variable para decidir que dato enviar
        if (var == 1){ //Si se recibe 1 enviar contador 
            spiWrite(contador); //Enviar contador
        }
        else if (var == 3){ //Si se recibe 3 enviar voltaje
            spiWrite(voltaje1); //Enviar valor del potenciómetro
        }
        PIR1bits.SSPIF = 0; //Limpiar bandera 
    }
    if (INTCONbits.RBIF == 1){ //Revisar si es interrupción del puerto B
        INTCONbits.RBIF = 0; //Limpiar bandera
        if (PORTBbits.RB7 == 0){ //Revisar si se presionó boton de RB7
            bandera = 1; //bandera en 1
        }
        if (PORTBbits.RB7 == 1 && bandera == 1){ //Revisar si se soltó el botón
            contador++; //Aumentar contador
            bandera = 0; //Limpiar bandera
        }
        if (PORTBbits.RB6 == 0){ //Revisar si se presionó boton de RB6
            bandera = 2; //Bandera en 2
        }
        if (PORTBbits.RB6 == 1 && bandera == 2){ //Revisar si ya se soltó el botón
            contador--; //Decrementar contador
            bandera = 0; //bandera en 0
        }
    }
}

void setup(void){
    ANSELbits.ANS0 = 1; //Canal AN0 como entrada analógica
    ANSELbits.ANS4 = 0; //Pin A5 como pin digital
    ANSELH = 0; //Puertos como I/O digitales
    
    TRISAbits.TRISA5 = 1; //Puerto A5 como entrada
    TRISBbits.TRISB7 = 1; //Puerto B6 como entrada
    TRISBbits.TRISB6 = 1; //Puerto B7 como entrada
    TRISD = 0; //Puerto D como salida
    
    PORTB = 0; //Limpiar Puerto B
    PORTD = 0; //Limpiar Puerto D
    
    OPTION_REGbits.nRBPU = 0; //Pull-ups activos
    IOCBbits.IOCB7 = 1; //Activar interrupt-onchange del pin B7
    IOCBbits.IOCB6 = 1; //Activar interrupt-onchange del pin B6
    
    WPUBbits.WPUB7 = 1; //Activar pull-up del pin B7
    WPUBbits.WPUB6 = 1; //Activar pull-up del pin B6
    
    INTCONbits.GIE = 1; //Activar interrupciones globales
    INTCONbits.PEIE = 1; //Activar interrupciones periféricas
    INTCONbits.RBIE = 1; //Activar interrupción del puerto B
    INTCONbits.RBIF = 0; //Limpiar bandera
    PIE1bits.ADIE = 1; // Habiliar interrupcion del conversor ADC
    PIR1bits.SSPIF = 0; //Limpiar bandera de interrupción del SPI
    PIE1bits.SSPIE = 1; //Activar bandera del SPI
    
    OSCCONbits.IRCF2 = 1; //Oscilador interno a 4MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;
    OSCCONbits.SCS = 1; //Utilizar oscilador interno
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_END, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE); //Función de configuración del SPI
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