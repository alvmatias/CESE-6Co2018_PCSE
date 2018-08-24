/** 
* @file  gprs.h
* @brief Archivo de cabecera del archivo fuente gprs.c.
* @brief Manejo del modulo gprs SIM900
* @note  Copyright 2018 - Ing Matias Alvarez.
*/
#ifndef _GPRS_H_
#define _GPRS_H_

/*==================[inclusions]=============================================*/
#include "sapi.h"
#include <string.h>
/*==================[macros]=================================================*/
/**
* @def charToInt(c)
* @brief Conversion de caracter a digito.
* @note Se espera que el caracter sea un numero del 0 al 9.
*/
#define charToInt(c) (uint8_t)(c-'0')

/**
* @def GPRS_DELAY
* @brief Maxima cantidad de tiempo en milisegundos que se espera la respuesta por parte del modulo GPRS.
*/
#define GPRS_DELAY     			5000

/**
* @def MIN_SIGNAL_IN_DBM
* @brief Menor Valor posible de la señal del modulo GPRS.
* @note Esta definido como un valor positivo, pero en realidad es un valor negativo.
*/
#define MIN_SIGNAL_IN_DBM	 	109
	
/**
* @def AT
* @brief Comando AT del modulo GPRS SIM900.
* @brief Utilizado para ver que el modulo este correctamete conectado.
* @note Respuesta esperada : 'OK'.
*/
#define AT 						"AT\r\n"  		

/**
* @def ATCMGS
* @brief Comando AT+CMGS del modulo GPRS SIM900.
* @brief Utilizado para enviar SMS a traves del modulo GPRS.
* @note Respuesta esperada : ''>'' Para enviar el SMS y 'OK' luego de enviado.
*/ 	
#define ATCMGS					"AT+CMGS=\""

/**
* @def ATCMGF
* @brief Comando AT+CMGF del modulo GPRS SIM900.
* @brief Utilizado para setear el modo de texto.
* @note El modo de texto debe ser SIEMPRE seteado antes de enviar un SMS.
* @note Respuesta esperada : 'OK'.
*/	
#define ATCMGF					"AT+CMGF=1\r\n" 

/**
* @def ATCSQ
* @brief Comando AT+CSQ del modulo GPRS SIM900
* @brief Utilizado para obtener la señal del modulo GPRS.
* @note Respuesta esperada : ':'' y ','. Entre ambas respuestas el modulo envia el valor de la señal.
*/
#define ATCSQ					"AT+CSQ\r\n"	


/**
* @def GPRS_END_OF_SMS
* @brief Delimitador de fin de SMS del modulo GPRS SIM900.
*/
#define GPRS_END_OF_SMS 		0x1A

/**
* @def RESPONSE_TO_WAIT_LENGTH
* @brief Maxima cantidad de caracteres de la respuesta que se espera por parte del modulo GPRS.
*/
#define RESPONSE_TO_WAIT_LENGTH 15
/*==================[typedef]================================================*/
/**
* @enum gprsError_t
* @brief Definicion de los estados posibles de funcioamiento del GPRS.
*/
typedef enum{
	GPRS_NOT_WORKING = 0,  			/**< GPRS No Funcionando */
	GPRS_WORKING = 1,				/**< GPRS Funcionando */
} gprsError_t;

/**
* @struct gprs_t
* @brief Estructura de datos del modulo GPRS.
*/
typedef struct{
	uartMap_t 	gprsUart;										/**< UART a utilizar para enviar los comandos al modulo GPRS */
	char		responseToWait 	[RESPONSE_TO_WAIT_LENGTH];		/**< Respuesta a esperar despues de cada comando enviado al modulo GPRS */
} gprs_t;
/*==================[external data declaration]==============================*/


/*==================[external functions declaration]=========================*/
/**
* @fn gprsError_t gprsInit(uartMap_t gprsUart)
* @brief Inicializacion del modulo GPRS.
* @param gprsUart : UART a setear en la estructura de datos del modulo GPRS para enviar los comandos.
* @return GPRS_NOT_WORKING si no fue posible configurar el GPRS, GPRS_WORKING caso contrario.
*/
gprsError_t gprsInit(uartMap_t gprsUart);

/**
* @fn gprsError_t gprsSendSMS(char *sms, char *cellNumber)
* @brief Envio de SMS a traves del modulo GPRS.
* @param SMS 		: Puntero al buffer que almacena el mensaje a enviar.
* @param cellNumber : Puntero al buffer que contiene el numero de celular al cual enviar el sms.
* @return GPRS_WORKING si el sms se envio correctamente, GPRS_NOT_WORKING caso contrario.
*/
gprsError_t gprsSendSMS(char *sms, char *cellNumber);


/**
* @fn uint8_t gprsGetSignal()
* @brief Obtencion de la señal del modulo GPRS.
* @return Valor de la señal en dbm.
* @note El valor devuelto es positivo cuando en realidad deberia ser negativo. Agregar el signo al trabajar con el valor.
* @note Interactua con el hardware
* @note Getter.
* @note
	2		-109dbm		Marginal
	3		-107dbm		Marginal
	4		-105dbm		Marginal
	5		-103dbm		Marginal
	6		-101dbm		Marginal
	7		-99dbm		Marginal
	8		-97dbm		Marginal
	9		-95dbm		Marginal
	10		-93dbm		OK
	11		-91dbm		OK
	12		-89dbm		OK
	13		-87dbm		OK
	14		-85dbm		OK
	15		-83dbm		Good
	16		-81dbm		Good
	17		-79dbm		Good
	18		-77dbm		Good
	19		-75dbm		Good
	20		-73dbm		Excellent
	21		-71dbm		Excellent
	22		-69dbm		Excellent
	23		-67dbm		Excellent
	24		-65dbm		Excellent
	25		-63dbm		Excellent
	26		-61dbm		Excellent
	27		-59dbm		Excellent
	28		-57dbm		Excellent
	29		-55dbm		Excellent
	30		-53dbm		Excellent
*/
uint8_t gprsGetSignal(); 
/*==================[end of file]============================================*/
#endif /* #ifndef _GPRS_H_ */
