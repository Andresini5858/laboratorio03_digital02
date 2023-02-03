/*
 * File:   main.c
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
#include "LCD.h" //librería LCD
#include "SPI.h"

#define _XTAL_FREQ 4000000 //Frecuencia 4MHz
//MASTER MASTER MASTER

unsigned char voltaje1;
unsigned char voltaje2;
int vol1;
int vol2;
unsigned int unidad1; //Variable para guardar unidades del voltaje del AN0
unsigned int decima1; //Variable para guardar decimas del voltaje del AN0
unsigned int centesima1; //Variable para guardar centesimas del voltaje del AN0
unsigned int unidad2; //Variable para guardar unidades del voltaje del AN1
unsigned int decima2; //Variable para guardar decimas del voltaje del AN1
unsigned int centesima2; //Variable para guardar centesimas del voltaje del AN0
char buffer[20];

void setup(void);
int map(unsigned char value, int inputmin, int inputmax, int outmin, int outmax){ //función para mapear valores
    return ((value - inputmin)*(outmax-outmin)) / (inputmax-inputmin)+outmin;}

void main(void) {
    setup();
    Lcd_Init(); //Iniciar pantalla LCD
    
    Lcd_Set_Cursor(1,7); //Cursor en (1,7)
    Lcd_Write_String("S2:"); //Escribir S2 en pantalla
    Lcd_Set_Cursor(1,1); //Cursor en (1,1)
    Lcd_Write_String("S1:"); //Escribir S1 en pantalla
    Lcd_Set_Cursor(1,14); //Cursor en (1,14)
    Lcd_Write_String("S3:"); //Escribir S3 en pantalla
    __delay_ms(1000);
    while(1){
        PORTCbits.RC2 = 0;
        spiWrite(1);
        voltaje1 = spiRead();
        __delay_ms(1);
        PORTCbits.RC2 = 1;
        
        PORTDbits.RD5 = 0;
        spiWrite(2);
        voltaje2 = spiRead();
        __delay_ms(1);
        PORTDbits.RD5 = 1;
        
        vol1 = map(voltaje1, 0, 255, 0, 100); //mapear valor del voltaje de 0 a 100
        unidad1 = (vol1*5)/100; //Separar las unidades del valor del voltaje
        decima1 = ((vol1*5)/10)%10; //Separar las decimas del valor del voltaje
        centesima1 = (vol1*5)%10; //Separar las centesimas del valor del voltaje
        Lcd_Set_Cursor(2,1); //Cursor en (1,7)
        sprintf(buffer, "%d.%d%dV" , unidad1 , decima1 , centesima1 ); //convertir variable a una cadena de caracteres
        Lcd_Write_String(buffer); //Mostrar cadena de caracteres en pantalla
        
                
        vol2 = map(voltaje2, 0, 255, 0, 100); //mapear valor del voltaje de 0 a 100
        unidad2 = (vol2*5)/100; //Separar las unidades del valor del voltaje
        decima2 = ((vol2*5)/10)%10; //Separar las decimas del valor del voltaje
        centesima2 = (vol2*5)%10; //Separar las centesimas del valor del voltaje
        Lcd_Set_Cursor(2,7); //Cursor en (1,7)
        sprintf(buffer, "%d.%d%dV" , unidad2 , decima2 , centesima2 ); //convertir variable a una cadena de caracteres
        Lcd_Write_String(buffer); //Mostrar cadena de caracteres en pantalla
    }
}

void setup(void){
    ANSEL = 0; //Puertos como I/O digitales
    ANSELH = 0; //Puertos como I/O digitales
    
    TRISCbits.TRISC2 = 0;
    TRISDbits.TRISD5 = 0;
    TRISB = 0;
    TRISD = 0;
    PORTB = 0;
    PORTD = 0;
    PORTCbits.RC2 = 1;
    PORTDbits.RD5 = 1;
    
    OSCCONbits.IRCF2 = 1; //Oscilador interno a 4MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;
    OSCCONbits.SCS = 1; //Utilizar oscilador interno
    
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}
