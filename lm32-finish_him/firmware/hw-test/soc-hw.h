#ifndef SPIKEHW_H
#define SPIKEHW_H

#define PROMSTART 0x00000000
#define RAMSTART  0x00000800
#define RAMSIZE   0x400
#define RAMEND    (RAMSTART + RAMSIZE)

#define RAM_START 0x40000000
#define RAM_SIZE  0x04000000

#define FCPU      100000000

#define UART_RXBUFSIZE 32


/****************************************************************************
 * Types
 */
typedef unsigned int  uint32_t;    // 32 Bit
typedef signed   int   int32_t;    // 32 Bit

typedef unsigned char  uint8_t;    // 8 Bit
typedef signed   char   int8_t;    // 8 Bit

/****************************************************************************
 * Interrupt handling
 */
typedef void(*isr_ptr_t)(void);

void     irq_enable();
void     irq_disable();
void     irq_set_mask(uint32_t mask);
uint32_t irq_get_mak();

void     isr_init();
void     isr_register(int irq, isr_ptr_t isr);
void     isr_unregister(int irq);

/****************************************************************************
 * General Stuff
 */
void     halt();
void     jump(uint32_t addr);


/****************************************************************************
 * Timer
 */
#define TIMER_EN     0x08    // Enable Timer
#define TIMER_AR     0x04    // Auto-Reload
#define TIMER_IRQEN  0x02    // IRQ Enable
#define TIMER_TRIG   0x01    // Triggered (reset when writing to TCR)

typedef struct {
	volatile uint32_t tcr0;
	volatile uint32_t compare0;
	volatile uint32_t counter0;
	volatile uint32_t tcr1;
	volatile uint32_t compare1;
	volatile uint32_t counter1;
} timer_t;

void msleep(uint32_t msec);
void nsleep(uint32_t nsec);

void prueba();
void tic_init();


/***************************************************************************
 * UART0
 */
#define UART_DR   0x01                    // RX Data Ready
#define UART_ERR  0x02                    // RX Error
#define UART_BUSY 0x10                    // TX Busy

typedef struct {
   volatile uint32_t ucr;
   volatile uint32_t rxtx;
} uart_t;

void uart_init();
void uart_putchar(char c);
void uart_putstr(char *str);
char uart_getchar();

void uart_gen_putstr(char *str);
void uart_gen_putchar(char c);
void uart_one_putchar(char c);
char uart_one_getchar();

void init_wifi();
void wifi_putchar2(char a);
void wifi_putstr(char *a);
char wifi_getchar2();
int ok();


/***************************************************************************
 * SPI0
 */

#define SPI_START  0x01                    // spi Start
#define SPI_BUSY   0x02                    // spi busy
#define SPI_NEWDATA  0x04                    // spi newData

typedef struct{
	volatile uint32_t ucr;
	volatile uint32_t data_in;
	volatile uint32_t data_out; 
} spi_t;

void spi_start();
unsigned char spi_read(char reg);
void spi_write(char reg, char value);


/***************************************************************************
 * I2C0
 */
#define I2C_DR	 0x03			//RX Data Ready
#define I2C_BUSY 0x04			//I2C Busy
#define I2C_ERR  0x02			//RX Error
typedef struct {
   volatile uint32_t scr;
   volatile uint32_t sdat;
} i2c_t;
uint32_t i2c_read();
void i2c_write();


/***************************************************************************
 * TECLADO MATRICIAL
 */
	typedef struct {
		volatile uint32_t valor;//00
	} teclado_t;
	
	char leer_teclado(); //valor

/***************************************************************************
 * Pointer to actual components
 */
extern timer_t  *timer0;
extern uart_t   *uart0;
extern uart_t   *uart1;
extern spi_t   *spi0; 
extern uint32_t *sram0; 
extern teclado_t *teclado0;
extern i2c_t *i2c0;

#endif // SPIKEHW_H
