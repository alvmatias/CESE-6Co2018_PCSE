/*==================[inclusions]=============================================*/
#include "lcd.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
/**
* @var static lcd_t lcd
* @brief Variable global privada de configuracion del lcd.
*/
static lcd_t lcd;
/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void lcdInit(uint8_t i2cAddress){
	/* Inicializar estructura de datos local */
	lcd.i2cAddress = i2cAddress;
	lcd.cursorMode = LCD_CURSOR_OFF;
	lcd.displayMode = LCD_DISPLAY_ON;
	lcd.backlightMode = LCD_BACKLIGHT_ON;
	lcd.size = LCD_SIZE_8X5;
	/* Configuracion I2C Port */
	i2cConfig( I2C0, LCD_CLOCK_RATE_10KHZ );

	/* Configuracion LCD */
	lcdSendByte(LCD_COMMAND_TYPE, 0x02);     /* Command 0x02 set 4 bit mode for start   */
	lcdSendByte(LCD_COMMAND_TYPE, 0x20);     /* Command 0x20 set 4 bit mode and 1 lines */

	lcdSendByte(LCD_COMMAND_TYPE, 0x28);     /* Command 0x28 set 4 bit mode and 2 lines */

	lcdSendByte(LCD_COMMAND_TYPE, 0x0C);     /* Command 0x0c set display on, cursor off */
	lcdSendByte(LCD_COMMAND_TYPE, 0x06);     /* Command 0x06 set default entry mode set */
	lcdSendByte(LCD_COMMAND_TYPE, 0x80);     /* Command 0x80 set default character type */ 

}

void lcdSendByte(lcdDataType_t dataType, uint8_t data){
	uint8_t dataHigh;     		/** dataHigh : Parte alta del dato a enviar */
	uint8_t dataLow;			/** dataLow : Parte baja del dato a enviar */
	uint8_t dataNibble[4];		/** dataNibble[4] : Nibbles de datos a enviar via I2C */

	/* Separar el dato a enviar en parte alta y parte baja */
	dataHigh = data & 0xf0;
	dataLow = (data << 4) & 0xf0;

	/* Formatear el paquete a enviar */
	if(dataType){	
		dataNibble[0] = dataHigh | (1 << LCD_I2C_EN) | (1 << LCD_I2C_RS) | lcd.backlightMode;    
		dataNibble[1] = dataHigh | (1 << LCD_I2C_RS) | lcd.backlightMode;      
		dataNibble[2] = dataLow  | (1 << LCD_I2C_EN) | (1 << LCD_I2C_RS) | lcd.backlightMode;    
		dataNibble[3] = dataLow  | (1 << LCD_I2C_RS) | lcd.backlightMode;
	}
	else{
		dataNibble[0] = dataHigh | (1 << LCD_I2C_EN) | lcd.backlightMode;    
		dataNibble[1] = dataHigh | lcd.backlightMode;      
		dataNibble[2] = dataLow  | (1 << LCD_I2C_EN) | lcd.backlightMode;    
		dataNibble[3] = dataLow  | lcd.backlightMode;   
	}

	/* Enviar por I2C */
	i2cWrite(I2C0, lcd.i2cAddress, (uint8_t *)dataNibble, 4, FALSE);      
   
}

void lcdWriteChar(char c){
	switch (c){
		case '\f' :	
			lcdSendByte(LCD_COMMAND_TYPE,1);
			delayInaccurate(5);
		break;
		default:
			lcdSendByte(LCD_DATA_TYPE,c);
		break;
	}
}

void lcdWriteString (char *c){
	while (*c){
	  lcdWriteChar(*c);
	  c++;
	}
}

void lcdSetPosXY(uint8_t x, lcdLine_t y){
  uint8_t address;	/** address : Direccion de la linea a donde escribir el dato */
  if (y==1) 
  	address = LCD_SECOND_LINE_ADDRESS; 
  else if(y==2)
  	address = LCD_THIRD_LINE_ADDRESS;
  else if(y==3)
  	address = LCD_FOURTH_LINE_ADDRESS;
  else{
  	address=LCD_FIRST_LINE_ADDRESS;
  }
  address += x;
  lcdSendByte(LCD_COMMAND_TYPE,0x80|address);
  delayInaccurateUs(100);
}

void lcdSetDisplayMode(lcdDisplayMode_t mode){
	lcd.displayMode = mode;
	lcdSendByte(LCD_COMMAND_TYPE, 0x08 | lcd.cursorMode | lcd.displayMode);
}

void lcdSetCursorMode(lcdCursorMode_t mode){
	lcd.cursorMode = mode;
	lcdSendByte(LCD_COMMAND_TYPE, 0x08 | lcd.cursorMode | lcd.displayMode);
}

void lcdSetCursorBackLightMode(lcdBacklightMode_t mode){
	lcd.backlightMode = mode;
	lcdSendByte(LCD_COMMAND_TYPE, 0x00);
}

void lcdClearLine(lcdLine_t line){	
	lcdSetPosXY(LCD_COMMAND_TYPE,line);
	lcdWriteString("                    ");
}

/*==================[end of file]============================================*/