//*****************************************************************************
/*
 *  Electrónica Digital 2
 *  Steven Josué Castillo Lou - 17169
 *  LABORATORIO 9: Interrupciones --> TMRs & UART TivaC
 *  Fecha: 18/05/2020
 *  Sección: 12
 *
 */
//*****************************************************************************
//******************** Se importan las Librerias ******************************

#include <stdint.h>                 //Librerías Principales
#include <stdbool.h>

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"

#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"    //Librería Interrupciones
#include "driverlib/gpio.h"
#include "driverlib/timer.h"        //Librería TMR
#include "driverlib/systick.h"
#include "driverlib/uart.h"         //Librería UART


//*****************************************************************************

#define XTAL 16000000    // Se define el Cristal Interno del Sistema

unsigned char dato;
uint8_t rojo = 0, azul = 0, verde = 0, flag = 0;

// ----------------------------------------------------------------------------
void Timer0Config(void);
void InitUART(void);
int main(void)
{
    // Clock de 40MHz --> SYSCTL_SYSDIV_5
    SysCtlClockSet(
            SYSCTL_SYSDIV_5 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN  | SYSCTL_XTAL_16MHZ);

    // Enable the GPIO port that is used for the on-board LED (PUERTO F).
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Los pines de las leds como output
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    // Configuracion del TIMER 0
    Timer0Config();
    // Configuracion de UART 0
    InitUART();
    // ---------------------------------------------------------

    // Loop forever.
    while(1)
    {

    }
}
void Timer0Config(void){
    //---------------- TIMER 0 CONFIG -----------------------
    // Enable the Timer0 peripheral
    //
    uint32_t ui32Period;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    // Wait for the Timer0 module to be ready.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }
    // Confuguro el timer full width periodico
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    ui32Period = ( SysCtlClockGet() ) / 2;
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);

    // Configuro las interrupciones
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();

    // Enable the timers.
    TimerEnable(TIMER0_BASE, TIMER_A);
}

void InitUART(void){

    /*Enable the GPIO Port A*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /*Enable the peripheral UART Module 0*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);


    /* Make the UART pins be peripheral controlled. */
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Sets the configuration of a UART. */
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    IntEnable(INT_UART0);
    UARTEnable(UART0_BASE);

    UARTCharPut(UART0_BASE,'H');
    UARTCharPut(UART0_BASE,'O');
    UARTCharPut(UART0_BASE,'L');
    UARTCharPut(UART0_BASE,'A');
    UARTCharPut(UART0_BASE,' ');
    UARTCharPut(UART0_BASE,'S');
    UARTCharPut(UART0_BASE,'e');
    UARTCharPut(UART0_BASE,'l');
    UARTCharPut(UART0_BASE,'e');
    UARTCharPut(UART0_BASE,'c');
    UARTCharPut(UART0_BASE,'t');
    UARTCharPut(UART0_BASE,' ');
    UARTCharPut(UART0_BASE,'L');
    UARTCharPut(UART0_BASE,'E');
    UARTCharPut(UART0_BASE,'D');

}

void Timer0IntHandler(void){
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    if (flag == 1)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
        flag = 0;
    }
    else
    {
        flag = 1;

        //----------------------------------------------------------------------------
        if(rojo){       // Se enciende el led rojo
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
            }
        else            // Se apaga el led rojo si se vuelve a mandar la instruccion
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
        }

        //------------------------ LED AZUL ----------------------------------------
        if(azul){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
            }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
        }

        //------------------------ LED VERDE ---------------------------------------
        if(verde){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
            }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
        }


    }
}

void UARTIntHandler(void){
    // Limpio la bandera de interrupcion
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);
    // Guardo lo que recibe en el pueto serial
    dato = UARTCharGet(UART0_BASE);

    if (dato == 'R'){
        rojo = ~rojo;
    }
    else if (dato == 'B'){
        azul = ~azul;
    }
    else if (dato == 'G'){
        verde = ~verde;
    }
}
