/** 
* @file  lcd.h
* @brief Archivo de cabecera del archivo fuente lcd.c.
* @brief Manejo de LCD 20*4 via I2C 
* @note  Copyright 2018 - Ing Matias Alvarez.
*/
#ifndef LCD_H_
#define LCD_H_
/*==================[inclusions]=============================================*/
#include "sapi.h"
/*==================[macros]=================================================*/
/**
* @def LCD_CLOCK_RATE_10KHZ
* @brief Reloj utilizado por el bus I2C.
*/
#define LCD_CLOCK_RATE_10KHZ 10000
/*==================[typedef]================================================*/
/** 
* @enum  lcdPin_t
* @brief Indice del pin del lcd
*/
typedef enum{
	LCD_I2C_RS = 0, /**< RS Pin */
	LCD_I2C_RW = 1, /**< RW Pin */
	LCD_I2C_EN = 2, /**< EN Pin */
	LCD_I2C_BL = 3  /**< BL Pin */
} lcdPin_t;

/** 
* @enum  lcdLine_t
* @brief Linea del lcd
*/
typedef enum{
	LCD_FIRST_LINE = 0, 	/**< Primera Linea */
	LCD_SECOND_LINE = 1,	/**< Segunda Linea */
	LCD_THIRD_LINE = 2, 	/**< Tercera Linea */
	LCD_FOURTH_LINE = 3		/**< Cuarta Linea */
} lcdLine_t;

/** 
* @enum  lcdLineAddress_t
* @brief Direccion de cada linea del lcd
*/
typedef enum{
	LCD_FIRST_LINE_ADDRESS = 0x00,  	/**< Direccion de la Primera Linea */
	LCD_SECOND_LINE_ADDRESS = 0x40, 	/**< Direccion de la Segunda Linea */
	LCD_THIRD_LINE_ADDRESS = 0x14, 		/**< Direccion de la Tercera Linea */
	LCD_FOURTH_LINE_ADDRESS = 0x54		/**< Direccion de la Cuarta Linea */
} lcdLineAddress_t;

/** 
* @enum  lcdMargin_t
* @brief Margen de linea del lcd
*/
typedef enum{
	LCD_LEFT_MARGIN = 0, 	/**< Margen Izquierdo */
	LCD_CENTRE = 9, 		/**< Centrado */
	LCD_RIGHT_MARGIN = 19	/**< Margen Derecho */
} lcdMargin_t;

/** 
* @enum  lcdCursorMode_t
* @brief Modo del cursor del lcd
*/
typedef enum{
	LCD_CURSOR_OFF = 0, 	/**< Cursor Apagado */
	LCD_BLINK_ON = 1, 		/**< Cursor Encendido */
	LCD_CURSOR_ON = 2		/**< Cursor Titilando */
} lcdCursorMode_t;

/** 
* @enum  lcdDisplayMode_t
* @brief Modo del display del lcd
*/
typedef enum{
	LCD_DISPLAY_OFF = 0,	/**< Display Apagado */
	LCD_DISPLAY_ON = 4		/**< Display Encendido */
} lcdDisplayMode_t;

/** 
* @enum  lcdBacklightMode_t
* @brief Modo de la luz de fondo del lcd
*/
typedef enum{
	LCD_BACKLIGHT_OFF = 0,  /**< Luz de Fondo Apagada */
	LCD_BACKLIGHT_ON = 8	/**< Luz de Fondo Encendida */
} lcdBacklightMode_t;

/** 
* @enum  lcdSize_t
* @brief Tamaño de la matriz de puntos del lcd
*/
typedef enum{
	LCD_SIZE_8X5 = 0, 		/**< Tamaño de matriz de puntos de 8x5 */
	LCD_SIZE_10X5 = 4		/**< Tamaño de matriz de puntos de 10x5 */
} lcdSize_t;

/** 
* @enum  lcdRow_t
* @brief Cantidad de filas del lcd
*/
typedef enum{
	LCD_1_ROW = 0, 		/**< 1 Columna */
	LCD_2_ROWS = 8		/**< 2 Columnas */
} lcdRow_t;

/** 
* @enum  lcdDataType_t
* @brief Tipo de dato enviado
*/
typedef enum{
	LCD_COMMAND_TYPE = 0,	/**< Comando */
	LCD_DATA_TYPE = 1		/**< Data */
} lcdDataType_t;

/** 
* @struct  lcd_t
* @brief Modo del cursor del lcd
*/
typedef struct{
   lcdBacklightMode_t backlightMode;	/**< Modo de Luz de Fondo */
   lcdCursorMode_t cursorMode;			/**< Modo de Cursor */
   lcdDisplayMode_t displayMode; 		/**< Modo de Display */
   lcdSize_t size;						/**< Tamaño de la matriz de punto */
   uint8_t i2cAddress;					/**< Direccion de I2C */
}lcd_t;
/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
/** 
 * @fn      lcdInit(uint8_t i2cAddress)
 * @param   i2cAddress : Direccion I2C.
 * @return  Nada
 */
void lcdInit(uint8_t i2cAddress);

/** 
 * @fn      lcdSendByte(lcdDataType_t dataType, uint8_t data)
 * @param   dataType : Tipo de Dato a enviar. 
 * @param   data : Dato a enviar.
 * @return  Nada
 */
void lcdSendByte(lcdDataType_t dataType, uint8_t data);

/** 
 * @fn      lcdWriteChar(char c)
 * @param   c : Caracter a escribir.
 * @return  Nada
 */
void lcdWriteChar(char c);

/** 
 * @fn      lcdWriteString(char *c)
 * @param   c : Puntero a la cadena de caracteres a escribir.
 * @return  Nada
 */
void lcdWriteString(char *c);

/** 
 * @fn      lcdSetPosXY(uint8_t x, lcdLine_t y)
 * @param   x : Posicion izquierda a derecha donde posicionar el cursor
 * @param   y : Linea donde posicionar el cursor
 * @return  Nada
 */
void lcdSetPosXY(uint8_t x, lcdLine_t y);

/** 
 * @fn      lcdSetDisplayMode(lcdDisplayMode_t mode)
 * @param   mode : Modo de Display a setear.
 * @return  Nada
 */
void lcdSetDisplayMode(lcdDisplayMode_t mode);

/** 
 * @fn      lcdSetCursorMode(lcdCursorMode_t mode)
 * @param   mode : Modo de Cursor a setear.
 * @return  Nada
 */
void lcdSetCursorMode(lcdCursorMode_t mode);

/** 
 * @fn      lcdSetCursorBackLightMode(lcdBacklightMode_t mode)
 * @param   mode : Modo de Luz de Fondo a setear.
 * @return  Nada
 */
void lcdSetCursorBackLightMode(lcdBacklightMode_t mode);

/** 
 * @fn      lcdClearLine(lcdLine_t line)
 * @param   line : Linea a Limpiar.
 * @return  Nada
 */
void lcdClearLine(lcdLine_t line);

/*==================[end of file]============================================*/

#endif /* LCD_H_ */
