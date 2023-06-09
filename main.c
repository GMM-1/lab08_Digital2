#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#define XTAL 16000000

//*************************************************************************************
//*******************************variables del programa********************************
//*************************************************************************************
uint32_t i = 0;
uint32_t ui32SysClock = 0;
uint32_t ui32LoadValue = 0;
volatile char receivedChar;

//*************************************************************************************
//***************************prototipos de funciones***********************************
//*************************************************************************************
void delay(uint32_t msec); //funcion para crear delay en milisegundos
void delay1ms(void); //funcion de delay de 1 milisegundo
void pinset(void); //configuracion de pines
void tmr0set(void); //configuracion del TMR0
void UARTset(void);//configuracion del UART

//*************************************************************************************
//**************************Interrupciones*********************************************
//*************************************************************************************
void Timer0IntHandler(void)
{
    // Limpiar la bandera de interrupci�n del Timer 0
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Alternar el estado del pin PB1
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, ~GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1) & GPIO_PIN_1);
}

void UART0IntHandler(void)
{
    // Limpiar la bandera de interrupci�n
    UARTIntClear(UART0_BASE, UART_INT_RX);

    // Leer el car�cter recibido
    receivedChar = (char)UARTCharGetNonBlocking(UART0_BASE);

    // Habilitar/deshabilitar el toggle de los LEDs seg�n el car�cter recibido
    switch (receivedChar)
    {
        case 'r':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, ~GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) & GPIO_PIN_1);
            delay(500);
            break;
        case 'g':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, ~GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3) & GPIO_PIN_3);
            delay(500);
            break;
        case 'b':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, ~GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) & GPIO_PIN_2);
            delay(500);
            break;
    }
}

//*************************************************************************************
//***************************codigo principal******************************************
//*************************************************************************************
int main(void)
{
   SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //reloj a 40Mhz
   ui32SysClock = SysCtlClockGet(); //variable para guardar valor del reloj

   pinset(); //configuracion de los GPIOs
   tmr0set(); //configuracion del TMR0
   UARTset(); // configuracion del UART

	while(1)
	{}
}

//*************************************************************************************
//*****************************funciones***********************************************
//*************************************************************************************
void delay(uint32_t msec)
{
    for (i = 0; i < msec; i++)
    {
        delay1ms();
    }
}
void delay1ms(void)
{
    SysTickDisable();
    SysTickPeriodSet(16000-1);
    SysTickEnable();
    while ((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT)== 0);
    }

void pinset(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //activamos el puerto F
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //colocamos como salidas los pines RGB del led
    // Pin1 = red                  Pin2 = blue                Pin3 = green

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //activamos el puerto B
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_1); //colocamos el pin PB1 como salida

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //habilitamos puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //habilitamos puerto B

    //inicializamos los puertos en 0
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0);
}

void tmr0set(void)
{
    // Habilitar el perif�rico Timer 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)){}

    // Configurar el Timer 0 en modo peri�dico de 32 bits
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    // Establecer el tiempo de carga (1 s)
    ui32LoadValue = (ui32SysClock / 2) - 1;
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32LoadValue);

    // Habilitar las interrupciones del Timer 0
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Configurar y habilitar las interrupciones del Timer 0 en el NVIC
    IntEnable(INT_TIMER0A);
    IntMasterEnable();

    // Habilitar el Timer 0
    TimerEnable(TIMER0_BASE, TIMER_A);
}

void UARTset(void)
{
    //habilitamos el puerto del UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //habilitamos modulo UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //configuracion de Tx y Rx
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //configuracion del UART
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Habilitar las interrupciones de UART0
    UARTIntEnable(UART0_BASE, UART_INT_RX);
    IntEnable(INT_UART0);

    // Habilitar el m�dulo UART0
    UARTEnable(UART0_BASE);
}
