/**
 * 
 */

#include "soc-hw.h"

//MF522 Command word
#define PCD_IDLE              0x00               //NO action; Cancel the current command
#define PCD_AUTHENT           0x0E               //Authentication Key
#define PCD_RECEIVE           0x08               //Receive Data
#define PCD_TRANSMIT          0x04               //Transmit data
#define PCD_TRANSCEIVE        0x0C               //Transmit and receive data,
#define PCD_RESETPHASE        0x0F               //Reset
#define PCD_CALCCRC           0x03               //CRC Calculate

  // Page 0: Command and status
#define    CommandReg       0x01 << 1  // starts and stops command execution
#define    ComIEnReg        0x02 << 1  // enable and disable interrupt request control bits
#define    DivIEnReg        0x03 << 1  // enable and disable interrupt request control bits
#define    ComIrqReg        0x04 << 1  // interrupt request bits
#define    DivIrqReg        0x05 << 1  // interrupt request bits
#define    ErrorReg         0x06 << 1  // error bits showing the error status of the last command executed 
#define    Status1Reg         0x07 << 1  // communication status bits
#define    Status2Reg         0x08 << 1  // receiver and transmitter status bits
#define    FIFODataReg        0x09 << 1  // input and output of 64 byte FIFO buffer
#define    FIFOLevelReg       0x0A << 1  // number of bytes stored in the FIFO buffer
#define    WaterLevelReg      0x0B << 1  // level for FIFO underflow and overflow warning
#define    ControlReg         0x0C << 1  // miscellaneous control registers
#define    BitFramingReg      0x0D << 1  // adjustments for bit-oriented frames
#define    CollReg          0x0E << 1  // bit position of the first bit-collision detected on the RF interface
    //              0x0F      // reserved for future use
    
    // Page 1: Command
    //              0x10      // reserved for future use
#define    ModeReg          0x11 << 1  // defines general modes for transmitting and receiving 
#define    TxModeReg        0x12 << 1  // defines transmission data rate and framing
#define    RxModeReg        0x13 << 1  // defines reception data rate and framing
#define    TxControlReg     0x14 << 1  // controls the logical behavior of the antenna driver pins TX1 and TX2
#define    TxAutoReg         0x15 << 1  // controls the setting of the transmission modulation
#define    TxSelReg         0x16 << 1  // selects the internal sources for the antenna driver
#define    RxSelReg         0x17 << 1  // selects internal receiver settings
#define    RxThresholdReg   0x18 << 1  // selects thresholds for the bit decoder
#define    DemodReg         0x19 << 1  // defines demodulator settings
    //              0x1A      // reserved for future use
    //              0x1B      // reserved for future use
#define    MfTxReg          0x1C << 1  // controls some MIFARE communication transmit parameters
#define    MfRxReg          0x1D << 1  // controls some MIFARE communication receive parameters
    //              0x1E      // reserved for future use
#define    SerialSpeedReg   0x1F << 1  // selects the speed of the serial UART interface
    
    // Page 2: Configuration
    //              0x20      // reserved for future use
#define    CRCResultRegH    0x21 << 1  // shows the MSB and LSB values of the CRC calculation
#define    CRCResultRegL    0x22 << 1
    //              0x23      // reserved for future use
#define    ModWidthReg      0x24 << 1  // controls the ModWidth setting?
    //              0x25      // reserved for future use
#define    RFCfgReg         0x26 << 1  // configures the receiver gain
#define    GsNReg           0x27 << 1  // selects the conductance of the antenna driver pins TX1 and TX2 for modulation 
#define    CWGsPReg         0x28 << 1  // defines the conductance of the p-driver output during periods of no modulation
#define    ModGsPReg        0x29 << 1  // defines the conductance of the p-driver output during periods of modulation
#define    TModeReg         0x2A << 1  // defines settings for the internal timer
#define    TPrescalerReg    0x2B << 1  // the lower 8 bits of the TPrescaler value. The 4 high bits are in TModeReg.
#define    TReloadRegH      0x2C << 1  // defines the 16-bit timer reload value
#define    TReloadRegL      0x2D << 1
#define    TCounterValueRegH    0x2E << 1  // shows the 16-bit timer value
#define    TCounterValueRegL    0x2F << 1

void setBit(char reg, char mask)
{
	char tmp;
	tmp = spi_read(reg);
	spi_write(reg, tmp | mask);   
}

void clearBit(char reg, char mask)
{
	char tmp;
	tmp = spi_read(reg);
	spi_write(reg, tmp & (~mask));
}

void initRfidReader()
{
	//Make Soft Reset
	spi_write(CommandReg, PCD_RESETPHASE);
	while (spi_read(CommandReg) & (1<<4));
        //Wait until the PCD finish reseting
	

	spi_write(TModeReg, 0x8D );      //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
	spi_write( TPrescalerReg, 0x3E );//TModeReg[3..0] + TPrescalerReg
	spi_write( TReloadRegL, 0x30 );           
	spi_write( TReloadRegH, 0x0 );   //WWWWWWWWWARNING: colocar ambo ceros o revisar que hace el registro
	spi_write( TxAutoReg, 0x40 );    //100%ASK
	spi_write( ModeReg, 0x3D );

	//AntennaON
	setBit( TxControlReg, 0x03 );
	
	//FINISH INIT 
}

void loop()
{
	//Begin Testing
	uart_putstr("Iniciando prueba....");
	uart_putstr("\n\r");
	msleep(3000);

	uart_putstr("Estoy bien");
	//IS_CARD
	//REQUEST

	spi_write( BitFramingReg, 0x07 );

	//TO_CARD
	char irqEn = 0x77;
	//Serial.println(irqEn, HEX);
	spi_write( ComIEnReg, irqEn | 0x80 );
	clearBit( ComIrqReg, 0x80 );
	setBit( FIFOLevelReg, 0x80 );
	spi_write( CommandReg, PCD_IDLE );
	spi_write( FIFODataReg, 0x26 );   //Escribiendo
	spi_write( CommandReg, PCD_TRANSCEIVE );
	setBit( BitFramingReg, 0x80 );

	//25ms revisando esto:
	char n;
	char waitIrq = 0x30;


	char flag = 1;
	while(flag)
	{
		//Serial.println("Entro a verificar");
		n =spi_read( ComIrqReg );
		if(n & waitIrq)
		{
			//Serial.println("ha cambiado algo");
			flag = 0;
		}
		if(n & 0x01)
		{
			//Serial.println("TIMEOUT, nada en 25ms");
			flag=0;
		}
		msleep(1);

	}
	
	flag=1;

	clearBit ( BitFramingReg, 0x80 );
	char lec;
	lec = spi_read( ErrorReg);
	
	/*if( !( lec & 0x1B) )
	{
		//Serial.println(lec, HEX);
		//Serial.println("status=MI_OK, todo va bien");
		if ( n & irqEn & 0x01 )     // WARNING: Parece ser otra lectura de un posible error, pero pues
			//Serial.println("status=NOTAGERR, NO todo va bien");
	}*/
	
	n = spi_read(FIFOLevelReg); //leer cantidad de datos
	
	uart_putstr("la cantidad de datos en el primer ciclo,n a leer es:  ");
	uart_putchar(n+48);
	uart_putstr("\n\r");
	uart_putstr("-------");
	uart_putstr("\n\r");
	
	char i=0;
	
	while (i<n)
	{
		lec = spi_read(FIFODataReg);
		uart_putstr(" Dato leido en el primer ciclo de FIFO: ");
		char firstDigit  = lec & 0x0f;
		if (firstDigit <= 9) 
			firstDigit=firstDigit+48;
		else
			firstDigit=firstDigit+55;
		
		char secondDigit = (lec & 0xf0)>>4;
		if (secondDigit <= 9) 
			secondDigit=secondDigit+48;
		else
			secondDigit=secondDigit+55;

		uart_putchar(secondDigit);
		uart_putchar(firstDigit);
		uart_putstr("\n\r");
		
		i=i+1;
	}
	
	/*
	uart_putstr("------- \n");
	char validBits;
	validBits = Rd(ControlReg) & 0x07; //Ver nro de bits validos
	Serial.print("Bits validos");Serial.print(validBits, BIN);
	Serial.println();
	*/
	
	//FIN TO_CARD
	//FIN REQUEST
	//FIN IS_CARD

	//READ_CARD_SERIAL

	//ANTICOLL
	spi_write( BitFramingReg, 0x00 );
	clearBit( Status2Reg, 0x08 );     //WARGNING: no presente en anterior ejemplo del cuaderno, tal vez puede ser omitido
	
	
	//TO_CARD

	spi_write( ComIEnReg, irqEn | 0x80 );
	clearBit( ComIrqReg, 0x80 );
	setBit( FIFOLevelReg, 0x80 );
	spi_write( CommandReg, PCD_IDLE );
	spi_write( FIFODataReg, 0x93 ); //EScribiendo PICC_ANTICOLL
	spi_write( FIFODataReg, 0x20 ); //EScribiendo NVB
	spi_write( CommandReg, PCD_TRANSCEIVE );
	setBit( BitFramingReg, 0x80 );
	
	while(flag)
	{
		//Serial.println("Entro a verificar");
		n =spi_read( ComIrqReg );
		if(n & waitIrq){
		//Serial.println("ha cambiado algo");
		flag = 0;
		}
		if(n & 0x01){
		//Serial.println("TIMEOUT, nada en 25ms");
		flag=0;
		}

		msleep(1);
	}
	
	flag = 1;

	clearBit( BitFramingReg, 0x80 );
	//char lec;
	/*lec = spi_read( ErrorReg);
	if( !( lec & 0x1B) ){
		Serial.println(lec, HEX);
		Serial.println("status=MI_OK, todo va bien");

		if ( n & irqEn & 0x01 )// WWWWWWWWARNING: Parece ser otra lectura de un posible error, pero pues
			Serial.println("status=NOTAGERR, NO todo va bien");
	}*/
	
	n = spi_read(FIFOLevelReg); //leer cantidad de datos
	
	uart_putstr("la cantidad de datos n a leer es:  ");
	uart_putchar(n+48);
	uart_putstr("\n\r");
	uart_putstr("-------");
	uart_putstr("\n\r");	

	i=0;
	
	while (i<n)
	{
		lec = spi_read(FIFODataReg);
		uart_putstr(" Dato leido en el primer ciclo de FIFO: ");
		char firstDigit  = lec & 0x0f;
		if (firstDigit <= 9) 
			firstDigit=firstDigit+48;
		else
			firstDigit=firstDigit+55;
		
		char secondDigit = (lec & 0xf0)>>4;
		if (secondDigit <= 9) 
			secondDigit=secondDigit+48;
		else
			secondDigit=secondDigit+55;

		uart_putchar(secondDigit);
		uart_putchar(firstDigit);
		uart_putstr("\n\r");
		
		i=i+1;
	}
	
	/*
	Serial.print("la cantidad de datos n a leer es: ");
	Serial.print(n, BIN); Serial.println("----");
	for(int i=0 ;i < n; i++){   //WWWWWWWWWARNING: Tener en cuenta para la FPGA, colocar un if(n == 0) o algo asi, para evitar que lea inecesariamente si no hay datos que leer
	lec = Rd(FIFODataReg);
	Serial.print(" Dato leido de FIFO (UID?): ");Serial.println(lec, HEX);
	}
	Serial.println("----");
	//byte validBits;
	validBits = Rd(ControlReg) & 0x07; //Ver nro de bits validos
	Serial.print("Bits validos");Serial.print(validBits, BIN);
	Serial.println();
	*/
}

int main()
{
	initRfidReader();
	char n;	

	while(1)
	{
		n=uart_getchar();
		if(n=='s')
		{
			uart_putstr("Valid Command, begin loop");
			uart_putstr("\n\r");
			loop();
		}
		else
		{
			uart_putstr("Invalid command");
			uart_putstr("\n\r");
		}
		
	}


}


//RFID restart command	
    /*
	char address=0x01;
	char value  =0x0f;

	char dataRead = spi_read(address);

	//uart_putchar(dataRead);
	
	spi_write(address,value);
	
	dataRead = spi_read(address);
	uart_putchar(dataRead+0x01);
	
	while (spi_read(address) & (1<<4)) {
    // PCD still restarting - unlikely after waiting 50ms, but better safe than sorry.
	}
	dataRead = spi_read(address);
	uart_putchar(dataRead+0x01);
	*/
	

//SPI trial
/* 
	uint32_t dataToSend = 0xAACC;
	
	spi_write(dataToSend);
	
	char dataRead = spi_read();

	uart_putchar(dataRead);
*/


