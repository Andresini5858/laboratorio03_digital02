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
#include "SPI.h" //librería SPI

#define _XTAL_FREQ 4000000 //Frecuencia 4MHz
//MASTER MASTER MASTER

unsigned char voltaje1; //Variable para guardar valor del voltaje del potenciómetro 1
unsigned char voltaje2; //Variable para guardar valor del voltaje del potenciómetro 2
unsigned char contador = 0; //Variable para guardar valor del voltaje del potenciómetro 2
int vol1; //Variable para guardar valor del voltaje del potenciómetro 1 mapeado
int vol2; //Variable para guardar valor del voltaje del potenciómetro 2 mapeado
unsigned int unidad1; //Variable para guardar unidades del voltaje del AN0
unsigned int decima1; //Variable para guardar decimas del voltaje del AN0
unsigned int centesima1; //Variable para guardar centesimas del voltaje del AN0
unsigned int unidad2; //Variable para guardar unidades del voltaje del AN1
unsigned int decima2; //Variable para guardar decimas del voltaje del AN1
unsigned int centesima2; //Variable para guardar centesimas del voltaje del AN0
char buffer[20]; //Arreglo para guardar valores de voltajes
char buffer1[20]; //Arreglo para guardar valor de contador

void setup(void); //Función de setup
int map(unsigned char value, int inputmin, int inputmax, int outmin, int outmax){ //función para mapear valores
    return ((value - inputmin)*(outmax-outmin)) / (inputmax-inputmin)+outmin;}

void main(void) {
    setup(); //Llamar al setup
    Lcd_Init(); //Iniciar pantalla LCD
    
    Lcd_Set_Cursor(1,7); //Cursor en (1,7)
    Lcd_Write_String("S2:"); //Escribir S2 en pantalla
    Lcd_Set_Cursor(1,1); //Cursor en (1,1)
    Lcd_Write_String("S1:"); //Escribir S1 en pantalla
    Lcd_Set_Cursor(1,14); //Cursor en (1,14)
    Lcd_Write_String("S3:"); //Escribir S3 en pantalla  
    while(1){ //Loop
        
        PORTCbits.RC2 = 0; //Iniciar comunicación con esclavo 1 
        __delay_ms(1); //delay de 1ms

        spiWrite(1); //Enviar un 1 al esclavo para solicitar datos del potenciómetro
        voltaje1 = spiRead(); // Leer valor de voltaje del potenciómetro del esclavo 1
         
        __delay_ms(1); //delay de 1ms
        PORTCbits.RC2 = 1; //Terminar comunicación con esclavo 1
        
        PORTCbits.RC2 = 0; //Iniciar comunicación con esclavo 1 
        __delay_ms(1); //delay de 1ms
        
        spiWrite(3); //Enviar un 1 al esclavo para solicitar datos del contador
        contador = spiRead(); //Leer datos del contador
        
        __delay_ms(1); //delay de 1ms
        PORTCbits.RC2 = 1; //Terminar comunicación con esclavo 1
        
        PORTDbits.RD5 = 0; //Iniciar comunicación con esclavo 2 
        __delay_ms(1); //delay de 1ms
        
        spiWrite(2); //Enviar un 2 al esclavo 2 para solicitar datos del potenciómetro
        voltaje2 = spiRead(); //Leer datos del contador
        
        __delay_ms(1); //delay de 1ms
        PORTDbits.RD5 = 1; //Terminar comunicación con esclavo 2 
        
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
        
        if (contador >= 0 && contador < 10){
            Lcd_Set_Cursor(2,16); //Cursor en (2,16)
            sprintf(buffer1, "%d" , contador); //convertir variable a una cadena de caracteres
            Lcd_Write_String(buffer1); //Mostrar cadena de caracteres en pantalla
            Lcd_Set_Cursor(2,14); //Cursor en (2,14)
            Lcd_Write_Char('0'); //Mostrar 0 en pantalla
            Lcd_Set_Cursor(2,15); //Cursor en (2,15)
            Lcd_Write_Char('0'); //Mostrar 0 en pantalla
        }
        else if (contador > 9 && contador < 100){
            Lcd_Set_Cursor(2,15); //Cursor en (2,15)
            sprintf(buffer1, "%d" , contador); //convertir variable a una cadena de caracteres
            Lcd_Write_String(buffer1); //Mostrar cadena de caracteres en pantalla
            Lcd_Set_Cursor(2,14); //Cursor en (2,14)
            Lcd_Write_Char('0'); //Mostrar 0 en pantalla
        }
        else {
            Lcd_Set_Cursor(2,14); //Cursor en (2,14)
            sprintf(buffer1, "%d" , contador); //convertir variable a una cadena de caracteres
            Lcd_Write_String(buffer1); //Mostrar cadena de caracteres en pantalla
        }
    }
}

void setup(void){
    ANSEL = 0; //Puertos como I/O digitales
    ANSELH = 0; //Puertos como I/O digitales
    
    TRISCbits.TRISC2 = 0; //PIN C2 como salida
    TRISDbits.TRISD5 = 0; //PIN D5 como salida
    TRISDbits.TRISD6 = 0; //PIN D6 como salida
    TRISDbits.TRISD7 = 0; //PIN D7 como salida
    TRISB = 0; //Puerto B como salida
    PORTB = 0; //Limpiar puerto B
    PORTD = 0; //Limpiar Puerto D
    PORTDbits.RD6 = 0; //Limpiar pin D6
    PORTDbits.RD7 = 0; //Limpiar pin D7
    PORTCbits.RC2 = 1; //Mantener apagado puerto de cominucación de esclavo 1
    PORTDbits.RD5 = 1; //Mantener apagado puerto de cominucación de esclavo 2
    
    OSCCONbits.IRCF2 = 1; //Oscilador interno a 4MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;
    OSCCONbits.SCS = 1; //Utilizar oscilador interno
    
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_END, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE); //Función Librería de Setup de comunicación de ISP
