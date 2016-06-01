#include "soc-hw.h"

// Pa que servirá esto:??

uart_t  *uart0  = (uart_t *)   0x20000000;
timer_t *timer0 = (timer_t *)  0x30000000;
gpio_t  *gpio0  = (gpio_t *)   0x80000000; // Esta dirección no funciona
uart_t  *uart1  = (uart_t *)   0x70000000;
//uart_t  *uart1  = (uart_t *)   0x20000000;
spi_t   *spi0   = (spi_t *)    0x50000000;
teclado_t   *teclado0   = (teclado_t *)    0x60000000;
i2c_t   *i2c0   = (i2c_t *)    0x40000000;

isr_ptr_t isr_table[32];
/*
void prueba()
{
	   uart0->rxtx=30;
	   timer0->tcr0 = 0xAA;
	   //gpio0->ctrl=0x55;
	   spi0->rxtx=1;
	   spi0->nop1=2;
	   spi0->cs=3;
	   spi0->nop2=5;
	   spi0->divisor=4;
	   i2c0->rxtx=5;
	   i2c0->divisor=5;

}
*/
void tic_isr();
/***************************************************************************
 * IRQ handling
 */
void isr_null()
{
}

void irq_handler(uint32_t pending)
{
	int i;

	for(i=0; i<32; i++) {
		if (pending & 0x01) (*isr_table[i])();
		pending >>= 1;
	}
}

void isr_init()
{
	int i;
	for(i=0; i<32; i++)
		isr_table[i] = &isr_null;
}

void isr_register(int irq, isr_ptr_t isr)
{
	isr_table[irq] = isr;
}

void isr_unregister(int irq)
{
	isr_table[irq] = &isr_null;
}

/***************************************************************************
 * TIMER Functions
 */
void msleep(uint32_t msec)
{
	uint32_t tcr;

	// Use timer0.1
	timer0->compare1 = (FCPU/1000)*msec;
	timer0->counter1 = 0;
	timer0->tcr1 = TIMER_EN;

	do {
		//halt();
 		tcr = timer0->tcr1;
 	} while ( ! (tcr & TIMER_TRIG) );
}

void nsleep(uint32_t nsec)
{
	uint32_t tcr;

	// Use timer0.1
	timer0->compare1 = (FCPU/1000000)*nsec;
	timer0->counter1 = 0;
	timer0->tcr1 = TIMER_EN;

	do {
		//halt();
 		tcr = timer0->tcr1;
 	} while ( ! (tcr & TIMER_TRIG) );
}


uint32_t tic_msec;

void tic_isr()
{
	tic_msec++;
	timer0->tcr0     = TIMER_EN | TIMER_AR | TIMER_IRQEN;
}

void tic_init()
{
	tic_msec = 0;

	// Setup timer0.0
	timer0->compare0 = (FCPU/10000);
	timer0->counter0 = 0;
	timer0->tcr0     = TIMER_EN | TIMER_AR | TIMER_IRQEN;

	isr_register(1, &tic_isr);
}

/***************************************************************************
 * WIFI ESP8266 Functions
 */

void init_wifi(){ //configurar el modulo como estación con puerto 80

	uart_gen_putstr("AT+RST\r\n");
	msleep(200);
	int c = 0;
	while(c==0){
		uart_gen_putstr("AT+CIPMUX=1\r\n");
		c = ok();
	}
	msleep(10);
	uart_gen_putstr("hast");
	c = 0;
	while (c==0){
		uart_gen_putstr("AT+CIPSERVER=1,80\r\n");
		c = ok();
	}
}

void wifi_putchar2(char a){
	int c = 0; 	  
	while(c == 0){
		uart_gen_putstr("AT+CIPSEND=0,1\r\n");
		msleep(10);
		uart_gen_putchar(a);
		c = ok();
		msleep(100);
	}
	c = 0; 	  
	while(c == 0){
		uart_gen_putstr("AT+CIPCLOSE=0\r\n");
		c = ok();
	}
}
																																																																																																																																																																																																																																																																																																																			
char wifi_getchar2(){
	char c='\n';
	int i=0;
	for(i=0;i<20;i++){
		c = uart_one_getchar();
		if (c ==':'){
			c = uart_one_getchar();
			return c;
		}
	}
	return '\n';
}

int ok(){
	int i=0;
	char a;
	for(i=0;i<30;i++){
		a = uart_one_getchar();
		if(a=='K'){
			return 1;
		}
	}
	return 0;

}

/***************************************************************************
 * UART Functions
 */
void uart_init()
{
	//uart0->ier = 0x00;  // Interrupt Enable Register
	//uart0->lcr = 0x03;  // Line Control Register:    8N1
	//uart0->mcr = 0x00;  // Modem Control Register

	// Setup Divisor register (Fclk / Baud)
	//uart0->div = (FCPU/(57600*16));
}

void uart_putstr(char *str)
{
	char *c = str;
	while(*c) {
		uart_putchar(*c);
		c++;
	}
}

char uart_getchar()
{   
	while (! (uart0->ucr & UART_DR)) ;
	return uart0->rxtx;
}

void uart_putchar(char c)
{
	while (uart0->ucr & UART_BUSY) ;
	uart0->rxtx = c;
}


//*************************** 
//Esto lo modifico para el moódulo WIFI
//***************************


void uart_gen_putstr(char *str)
{
	char *c = str;
	while(*c) {
		uart_gen_putchar(*c);
		c++;
	}
}

void uart_gen_putchar(char c)
{
	 uart_putchar(c);
	 uart_one_putchar(c);
}

void uart_one_putchar(char c)
{
	while (uart1->ucr & UART_BUSY) ;  // Me toca poner las variables para el UART1 ojo!!!
	uart1->rxtx = c;
}


char uart_one_getchar()
{   
	while (! (uart1->ucr & UART_DR)) ;   // Me toca poner las variables para el UART1 ojo!!!
	return uart1->rxtx;
}



/***************************************************************************
 * GPIO Functions
 */

	char gpio_get_in()
	{
	return gpio0->gpio_in;
	}//gpio_in

	void gpio_set_o(char g)
	{
	gpio0->gpio_o=g;
	}//gpio_o

	char gpio_get_dir()
	{
	return gpio0->gpio_dir;
	}//gpio_dir
	
	void gpio_set_dir(char g)
	{
	gpio0->gpio_dir=g;
	}//gpio_dir
/***************************************************************************
 * SPI Functions
 */
//These are the specific funtions for the RC522 rfid reader card
void spi_start()
{
	spi0->ucr=0x1;
	spi0->ucr=0x0;
}
unsigned char spi_read (char reg)
{
	//reg= reg<<1;
	uint32_t regAddress = (0x80 | (reg & 0x7E));
	regAddress = regAddress << 8;
	while(spi0->ucr & SPI_BUSY);
	spi0->data_in= regAddress;
	spi_start();
	while(spi0->ucr & SPI_BUSY);
	
	return spi0->data_out;	
}
void spi_write (char reg, char value)
{
	//reg=reg<<1;
	uint32_t regAddress=(reg & 0x7E);
	regAddress = regAddress << 8;
	uint32_t data0 = regAddress | value;
	while(spi0->ucr & SPI_BUSY);
	spi0->data_in=data0;
	spi_start();
}
/***************************************************************************
 * TECLADO MATRICIAL
 */

char leer_teclado()     // creo que el retorno siempre es char.
{
	return teclado0 -> valor;  // Este es el valor de value en la lógica de teclado, que mando desde el modulo teclado.v
}




/***************************************************************************
 * NFC		 ARCHIVO .c  Joaquín Jiménez, based on code Dr. Leong
 ***************************************************************************/

/*** RFID rfid(SS_PIN,RST_PIN); **/
/*
void RFID(int chipSelectPin, int NRSTPD){	

	_chipSelectPin = chipSelectPin;		
//	pinMode(_chipSelectPin,OUTPUT);		//Ya cuadré esto por GPIO y UCF
//	digitalWrite(_chipSelectPin, LOW);	//Ya lo cuadré con la siguiente fucnión:
	digitalWrite(0x00);
//	pinMode(NRSTPD,OUTPUT);		//
//	digitalWrite(NRSTPD, HIGH);		//Error, este es el reset, siempre está en alto, ya lo cuadré con la función de arriba
	_NRSTPD = NRSTPD;
}
*/


/*** Función digitalWrite que reemplaza la de arduino **/
/*
void digitalWrite(char a){ // VArible que mantiene el reset en alto, y la variable CS la setea dependiendo el char a.
	gpio_set_dir(0x03); // 0000 0011 Cuadro de una vez el chip select, y el reset que siempre debe estar en alto, pero en un inicio no lo voy a conectar...
	
	if(a == 0x01){
		gpio_set_o(0X03);	// 0000 0011	
		
	}
	if(a == 0x00){
		gpio_set_o(0X02);	//0000 0010
	}
				
}
*/





/*	void spi_init(char a)
	void spi_putchar(char a)
	char spi_getchar(){
	return spi0->nop1;
	}
*/
