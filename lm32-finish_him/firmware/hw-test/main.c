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
#define SIZEBUFF 10


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
	spi_write( TReloadRegH, 0x0 );
	spi_write( TxAutoReg, 0x40 );    //100%ASK
	spi_write( ModeReg, 0x3D );

	//AntennaON
	setBit( TxControlReg, 0x03 );
	
	//FINISH INIT 
}

char comparestr(char uno, char dos, char tres, char cuatro, char cinco, char seis, char siete, char ocho, char nueve, char diez){
	char uno_0 = '2';
	char dos_0 = '5';
	char tres_0 = '5';
	char cuatro_0 = '2';
	char cinco_0 = '2';
	char seis_0 = '6';
	char siete_0 = '5';
	char ocho_0 = '0';
	char nueve_0 = '0';
	char diez_0 = '1';

	char uno_1 = 'B';
	char dos_1 = '0';
	char tres_1 = '6';
	char cuatro_1 = '0';
	char cinco_1 = '1';
	char seis_1 = 'F';
	char siete_1 = '2';
	char ocho_1 = 'B';
	char nueve_1 = 'E';
	char diez_1 = '4';

	char uno_2 = '8';
	char dos_2 = 'B';
	char tres_2 = '4';
	char cuatro_2 = 'C';
	char cinco_2 = '3';
	char seis_2 = '9';
	char siete_2 = '4';
	char ocho_2 = '0';
	char nueve_2 = 'B';
	char diez_2 = 'E';

	char uno_3 = 'F';
	char dos_3 = 'B';
	char tres_3 = '7';
	char cuatro_3 = '4';
	char cinco_3 = '3';
	char seis_3 = '9';
	char siete_3 = '4';
	char ocho_3 = '0';
	char nueve_3 = 'F';
	char diez_3 = '6';

	char uno_4 = '4';
	char dos_4 = 'B';
	char tres_4 = 'D';
	char cuatro_4 = '2';
	char cinco_4 = '3';
	char seis_4 = 'A';
	char siete_4 = '4';
	char ocho_4 = '0';
	char nueve_4 = 'E';
	char diez_4 = '2';



	if((uno == uno_0) && (dos == dos_0) &&(tres == tres_0) &&(cuatro == cuatro_0) &&(cinco == cinco_0) &&(seis == seis_0) &&(siete == siete_0) &&(ocho == ocho_0) &&(nueve == nueve_0) &&(diez == diez_0)){
	return 'a';
	}

	if((uno == uno_1) && (dos == dos_1) &&(tres == tres_1) &&(cuatro == cuatro_1) &&(cinco == cinco_1) &&(seis == seis_1) &&(siete == siete_1) &&(ocho == ocho_1) &&(nueve == nueve_1) &&(diez == diez_1)){
	return 'b';
	}

	if((uno == uno_2) && (dos == dos_2) &&(tres == tres_2) &&(cuatro == cuatro_2) &&(cinco == cinco_2) &&(seis == seis_2) &&(siete == siete_2) &&(ocho == ocho_2) &&(nueve == nueve_2) &&(diez == diez_2)){
	return 'c';
	}

	if((uno == uno_3) && (dos == dos_3) &&(tres == tres_3) &&(cuatro == cuatro_3) &&(cinco == cinco_3) &&(seis == seis_3) &&(siete == siete_3) &&(ocho == ocho_3) &&(nueve == nueve_3) &&(diez == diez_3)){
	return 'd';
	}

	if((uno == uno_4) && (dos == dos_4) &&(tres == tres_4) &&(cuatro == cuatro_4) &&(cinco == cinco_4) &&(seis == seis_4) &&(siete == siete_4) &&(ocho == ocho_4) &&(nueve == nueve_4) &&(diez == diez_4)){
	return 'e';
	}
	return 'f';

}

char* bienvenido(char a){
	if(a == 'a'){
		return "**Bienvenido admin**";
	}
	if(a == 'b'){
		return "*Bienvenido  admin2*";
	}
	if(a == 'c'){
		return "*Bienvenido  Daniel*";
	}
	if(a == 'd'){
		return "*Bienvenido Joaquin*";
	}
	if(a == 'e'){
		return "***Bienvenido Jan***";
	}
	return "**Intente de nuevo**";
}
/*
int ya_almorzo(char a, char *p){
        if(a == 'c'){
                if(p[0] == '1'){
                        return 1;
                }else{
                        return 0;
                }
        }
        if(a == 'd'){
                if(p[1] == '1'){
                        return 1;
                }else{
                        return 0;
                }
        }
        if(a == 'e'){
                if(p[2] == '1'){
                        return 1;
                }else{
                        return 0;
                }
        }
        
}
*/

char loop(char *p)
{
	char uno;
	char dos;
	char tres;
	char cuatro;
	char cinco;
	char seis;
	char siete;
	char ocho;
	char nueve;
	char diez;
        int jj;

	//Begin Testing
        for(jj=0; jj<16; jj++){
		writeStringlcd("*");		
	        msleep(100);
        }
	
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
	

	
	n = spi_read(FIFOLevelReg); //leer cantidad de datos

	msleep(100);
	char i=0;
	
	while (i<n)
	{
		lec = spi_read(FIFODataReg);
		msleep(100);

		msleep(100);
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

		msleep(100);		
		i=i+1;
	}
	
	
	//FIN TO_CARD
	//FIN REQUEST
	//FIN IS_CARD

	//READ_CARD_SERIAL

	//ANTICOLL
	spi_write( BitFramingReg, 0x00 );
	clearBit( Status2Reg, 0x08 );   
	
	
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

		n =spi_read( ComIrqReg );
		if(n & waitIrq){

		flag = 0;
		}
		if(n & 0x01){

		flag=0;
		}

		msleep(1);
	}
	
	flag = 1;

	clearBit( BitFramingReg, 0x80 );
	//char lec;
	
	
	n = spi_read(FIFOLevelReg); //leer cantidad de datos


		msleep(200);

	i=0;
	
	while (i<n)
	{
		lec = spi_read(FIFODataReg);
//		uart_putstr(" Dato leido en el primer ciclo de FIFO: ");
		msleep(100);
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


		msleep(10);

		if(i==0){
		uno = secondDigit;
		dos = firstDigit;
		}
		if(i==1){
		tres = secondDigit;
		cuatro = firstDigit;
		}
		if(i==2){
		cinco = secondDigit;
		seis = firstDigit;
		}
		if(i==3){
		siete = secondDigit;
		ocho = firstDigit;
		}
		if(i==4){
		nueve = secondDigit;
		diez = firstDigit;
		}
		msleep(10);

		i=i+1;
	}
	
	
	char result = comparestr(uno, dos, tres, cuatro, cinco, seis, siete, ocho, nueve, diez);
	return 	result;
}



char dato_valido() {
	char principio;
        principio = 0x00;
	while(principio == 0x00  || (principio != 0x01 && principio != 0x02 && principio != 0x03)){
		principio = leer_teclado();
		msleep(100);
	}
	return principio;
}

char readyOk(){
	char ok;
        ok = 0x00;
	while(ok == 0x00  || (ok != 0x01)){
		ok = leer_teclado();
		msleep(100);
	}
	return ok;
}

char dia_actual() {
        char dia = 0x00;
	while(dia == 0x00  || (dia != 0x01 && dia != 0x02 && dia != 0x03 && dia != 0x04 && dia != 0x5)){
		dia = leer_teclado();
		msleep(100);
	}
	return dia;
}

int check() {
	char check = 0x00;
	while(check != 0x01 && check != 0x0F){
		check = leer_teclado();
		msleep(100);
	}
        if(check == 0x01){
                return 1;
        }
        return 0;
}



/**********************************************************************************************************************
*********************************************I2C***********************************************************************
**********************************************************************************************************************/

//---------------------------------------------------------------------------
// LCD Functions
//---------------------------------------------------------------------------

void writeCharlcd (char letter) 
{
  char highnib;
  char lownib;
  highnib = letter & 0xF0;
  lownib  = letter & 0x0F;

     i2c_write(0x27, 0x00, highnib|0b00001001);
     i2c_write(0x27, 0x00, highnib|0b00001101);
     i2c_write(0x27, 0x00, highnib|0b00001001);  

     i2c_write(0x27, 0x00, (lownib<<4)|0b00001001);
     i2c_write(0x27, 0x00, (lownib<<4)|0b00001101);
     i2c_write(0x27, 0x00, (lownib<<4)|0b00001001);
}




void writeCommandlcd (char command) 
{
  char highnib;
  char lownib;
  highnib = command&0xF0;
  lownib = command&0x0F;

     i2c_write(0x27, 0x00, highnib|0b00001000);
     i2c_write(0x27, 0x00, highnib|0b00001100);
     i2c_write(0x27, 0x00, highnib|0b00001000);  

     i2c_write(0x27, 0x00, (lownib<<4)|0b00001000);
     i2c_write(0x27, 0x00, (lownib<<4)|0b00001100);
     i2c_write(0x27, 0x00, (lownib<<4)|0b00001000);
}

void writeStringlcd (char *str) {
	char *c = str;
	while(*c) {
		writeCharlcd(*c);
		c++;
	}
}

// LCD Commands------------------------------------------

// LCD_I2C CONFIG
// DB7 DB6 DB5 DB4 CTRST EN RW RS

void clearDisplay() 
{
   writeCommandlcd(0b00000001);
   msleep(1);
}

void returnHome()
{
   writeCommandlcd(0b00000010);
   msleep(2);
}

// I/D = 1, S=0
void entryModeSet2()
{  
  
   writeCommandlcd(0b00000110);
   msleep(1);
}

// I/D = 1, S=1
void entryModeSet()
{  
   writeCommandlcd(0b00000111);
   msleep(1);
}


// I/D = 0, S=0
void entryModeSet3()
{  
   writeCommandlcd(0b00000100);
   msleep(1);
}

// I/D = 0, S=1
void entryModeSet4()
{  
   writeCommandlcd(0b00000101);
   msleep(1);
}


void displayOff()
{  
   writeCommandlcd(0b00001000);
   msleep(1);
}

// D=1, C=1, B=1
void displayOn()
{  

   writeCommandlcd(0b00001111);
   msleep(1);
}


// S/C = 0, R/L = 1
void cursorShiftRight()
{  
   writeCommandlcd(0b00010100);
   msleep(1);
}



// S/C = 0, R/L = 0
void cursorShiftLeft()
{  
   writeCommandlcd(0b00010000);
   msleep(1);
}


// S/C = 1, R/L = 1
void displayShiftRight()
{   

   writeCommandlcd(0b00011100);
   msleep(1);
}


// S/C = 1, R/L = 0
void displayShiftLeft()
{  
   writeCommandlcd(0b00011000);
   msleep(1);
}



// D/L = 0, N = 1, F = 0
//4-Bit mode, 2 lines, 5x8 dots
void functionSet()
{  

   writeCommandlcd(0b00101000);
   msleep(1);
}


void lcdInit ()
{  //1
   msleep(50);
   i2c_write(0x27, 0x00, 0b00111000);
   i2c_write(0x27, 0x00, 0b00111100);
   i2c_write(0x27, 0x00, 0b00111000);
   msleep(5);
   
   //2
   i2c_write(0x27, 0x00, 0b00111000);
   i2c_write(0x27, 0x00, 0b00111100);
   i2c_write(0x27, 0x00, 0b00111000);
   msleep(5);
   //3
   i2c_write(0x27, 0x00, 0b00111000);
   i2c_write(0x27, 0x00, 0b00111100);
   i2c_write(0x27, 0x00, 0b00111000);
   msleep(1);
   //5
   i2c_write(0x27, 0x00, 0b00101000);
   i2c_write(0x27, 0x00, 0b00101100);
   i2c_write(0x27, 0x00, 0b00101000);
   msleep(1);
   //6
   i2c_write(0x27, 0x00, 0b00101000);
   i2c_write(0x27, 0x00, 0b00101100);
   i2c_write(0x27, 0x00, 0b00101000);
   msleep(1);
   //7
   i2c_write(0x27, 0x00, 0b10001000);
   i2c_write(0x27, 0x00, 0b10001100);
   i2c_write(0x27, 0x00, 0b10001000);
   msleep(1);
   //8
   i2c_write(0x27, 0x00, 0b00001000);
   i2c_write(0x27, 0x00, 0b00001100);
   i2c_write(0x27, 0x00, 0b00001000);
   msleep(1);
   //9
   i2c_write(0x27, 0x00, 0b10001000);
   i2c_write(0x27, 0x00, 0b10001100);
   i2c_write(0X27, 0x00, 0b10001000);
   msleep(1);
   //10
   i2c_write(0x27, 0x00, 0b00001000);
   i2c_write(0x27, 0x00, 0b00001100);
   i2c_write(0x27, 0x00, 0b00001000);
   msleep(2);
   //11
   i2c_write(0x27, 0x00, 0b11111000);
   i2c_write(0x27, 0x00, 0b11111100);
   i2c_write(0x27, 0x00, 0b11111000);
   msleep(1);

   //12
   i2c_write(0x27, 0x00, 0b00001000);
   i2c_write(0x27, 0x00, 0b00001100);
   i2c_write(0x27, 0x00, 0b00001000);
   msleep(1);
   //13
   i2c_write(0x27, 0x00, 0b01101000);
   i2c_write(0x27, 0x00, 0b01101100);
   i2c_write(0x27, 0x00, 0b01101000);
   msleep(2);

}

char* seleccion(char *a, char*b, char *c){
	char principio;
	char *selection;
		clearDisplay();
		writeStringlcd("Digite:             ");
		writeStringlcd("1 ");
		writeStringlcd(a);
                msleep(1200);      
 		clearDisplay();
		writeStringlcd("Digite:             ");
		writeStringlcd("2 ");
		writeStringlcd(b);
                msleep(1200);
 		clearDisplay();
		writeStringlcd("Digite:             ");
		writeStringlcd("3 ");
		writeStringlcd(c);
                msleep(1200);  
		principio = dato_valido();
                msleep(100);


		if(principio == 0x01){
			selection = a;
		};
		if(principio == 0x02){
			selection = b;
		};
		if(principio == 0x03){
			selection = c;
		}
		return selection;

}

int main()
{	
	init_wifi();
	lcdInit();
	clearDisplay();
/*
while(1){	
	wifi_putstr("HOLA");
}
*/


	char principio;
	int checkw=1;
        char dia;
	char ready;

	char *menu_1;
	char *menu_2;
	char *menu_3 = "Sin principio";
        char *jugo_1;
        char *jugo_2;
        char *jugo_3 = "sin jugo";
        char *carne_1;
        char *carne_2;
        char *carne_3 = "sin carne";
        char *entrada_1 = "sopa del dia";
        char *entrada_2 = "fruta" ; 
        char *entrada_3 = "sin entrada";
	char *menu_final;
	char *jugo_final;
	char *carne_final;
	char *entrada_final;
	
	writeStringlcd("Que dia es hoy?");
        dia = dia_actual();
        msleep(100);
        if(dia == 0x01){
                menu_1 = "Frijoles";
                menu_2 = "lentejas";
                jugo_1 = "mora";
                jugo_2 = "fresa";
                carne_1 = "pescado";
                carne_2 = "pollo";
        }
        if(dia == 0x02){
                menu_1 = "ahuyama";
                menu_2 = "arvejas";
                jugo_1 = "mango";
                jugo_2 = "lulo";
                carne_1 = "albondigas";
                carne_2 = "pollo";
        }
        if(dia == 0x03){
                menu_1 = "pasta";
                menu_2 = "garbanzo";
                jugo_1 = "maracuya";
                jugo_2 = "limonada";
                carne_1 = "sobrebarriga";
                carne_2 = "bisteck";
        }
        if(dia == 0x04){
                menu_1 = "acelgas";
                menu_2 = "lentejas";
                jugo_1 = "guayaba";
                jugo_2 = "banano";
                carne_1 = "gulash";
                carne_2 = "pescado";
        }
        if(dia == 0x05){
                menu_1 = "wok";
                menu_2 = "pasta";
                jugo_1 = "uva";
                jugo_2 = "pera";
                carne_1 = "pollo";
                carne_2 = "pescado";
        }



        char almorzo[2];

        char p[3] = "000";
	char user;
	initRfidReader();
	char n;	
	
	clearDisplay();
	writeStringlcd("*****Presione 1*****");
        msleep(100);
	ready = readyOk();
        msleep(100);
	
	while(1)
	{
		if(ready == 0x01)
		{

			clearDisplay();
			writeStringlcd("Acerque su tarjeta  ");
			user=loop(p);

			if(user == 'c' || user == 'd' || user == 'e'){
				checkw = 0;
			}
  
			clearDisplay();
			writeStringlcd(bienvenido(user));
			msleep(2000);

		}
			principio = 0x00;
	
	while(checkw == 0){
		menu_final = seleccion(menu_1, menu_2, menu_3);
		jugo_final = seleccion(jugo_1, jugo_2, jugo_3);
		carne_final = seleccion(carne_1, carne_2, carne_3);
		entrada_final = seleccion(entrada_1, entrada_2, entrada_3);
		
		clearDisplay();
		writeStringlcd("Usted eligio:       ");
		writeStringlcd(menu_final);
		msleep(1500);
		clearDisplay();
		writeStringlcd("Usted eligio:       ");
		writeStringlcd(jugo_final);
		msleep(1500);
		clearDisplay();
		writeStringlcd("Usted eligio:       ");
		writeStringlcd(carne_final);
		msleep(1500);
		clearDisplay();
		writeStringlcd("Usted eligio:       ");
		writeStringlcd(entrada_final);
		msleep(1500);
		clearDisplay();
		writeStringlcd("Si es correcto 1    ");
		writeStringlcd("De lo contrario *   ");
	        msleep(100);
		checkw = check();
	        msleep(100);
		
	}

		clearDisplay();
		writeStringlcd("Gracias por utilizar");
		writeStringlcd("****TU ALMUERZO*****");
		msleep(2000);
		checkw = 1;

}

}

