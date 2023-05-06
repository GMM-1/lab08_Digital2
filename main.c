#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#define XTAL 16000000

uint32_t i = 0;

void delay(uint32_t msec);
void delay1ms(void);

int main(void)
{
   SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //reloj a 16MHZ
   SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //activamos el puerto F
   GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

	while(1)
	{
	    //apagamos todos los pines
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
	    delay(500);

	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 , 0 );
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 , GPIO_PIN_3 );
	    delay(1000);
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 , 0 );
	    delay(500);
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 , GPIO_PIN_3 );
	    delay(500);
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 , 0 );
	    delay(500);
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 , GPIO_PIN_3 );
	    delay(500);
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 , 0 );

	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
	            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_3 , GPIO_PIN_1 | GPIO_PIN_3  );
	            delay(500);

	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 , GPIO_PIN_1) ;
	    delay(1000);






	}
}

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

