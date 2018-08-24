/** 
* @file  wifi_esp8266.h
* @brief Archivo de cabecera del archivo fuente wifi_esp8266.c.
* @brief Manejo de Wifi ESP8266 
* @note  Copyright 2018 - Ing Matias Alvarez.
*/
#ifndef _WIFI_ESP8266_H_
#define _WIFI_ESP8266_H_

/*==================[inclusions]=============================================*/
#include "sapi.h"
#include <string.h>
/*==================[macros]=================================================*/
/**
* @def AT
* @brief Comando AT del modulo Wifi ESP8266.
* @brief Utilizado para ver que el modulo este correctamente funcionando.
* @note Respuesta esperada : 'OK'.
*/
#define AT 			"AT\r\n"

/**
* @def ATRST
* @brief Comando AT del modulo Wifi ESP8266.
* @brief Utilizado para resetear el modulo.
* @note Respuesta esperada : 'OK'.
*/
#define ATRST 		"AT+RST\r\n"

/**
* @def ATCWJAP
* @brief Comando AT+CWJAP del modulo Wifi ESP8266.
* @brief Utilizado para conectar el modulo a la red especificada.
* @note Respuesta esperada : 'WIFI CONNECTED' y 'WIFI GOT IP'.
*/
#define ATCWJAP		"AT+CWJAP=\""

/**
* @def ATCWMODE
* @brief Comando AT+CWMODE del modulo Wifi ESP8266.
* @brief Utilizado para establecer el modo de wifi(STATION o SOFT AP).
* @note Respuesta esperada : 'OK'.
*/
#define ATCWMODE	"AT+CWMODE="

/**
* @def ATCIPMUX
* @brief Comando AT+CIPMUX del modulo Wifi ESP8266.
* @brief Utilizado para establecer cuantas conexion TCP/UDP puede soportar al mismo tiempo.
* @note Respuesta esperada : 'OK'.
*/
#define ATCIPMUX	"AT+CIPMUX="
/**
* @def ATCIPSTART
* @brief Comando AT+CIPSTART del modulo Wifi ESP8266.
* @brief Utilizado para iniciar una conexion TCP/UDP con el servidor especificado.
* @note Respuesta esperada : 'OK'.
*/
#define ATCIPSTART	"AT+CIPSTART="

/**
* @def ATCIPCLOSE
* @brief Comando AT+CIPCLOSE del modulo Wifi ESP8266.
* @brief Utilizado para finalizar una conexion TCP/UDP.
* @note Respuesta esperada : 'OK'.
*/
#define ATCIPCLOSE	"AT+CIPCLOSE"

/**
* @def ATCIPSEND
* @brief Comando AT+CIPSEND del modulo Wifi ESP8266.
* @brief Utilizado para enviar data via TCP/UDP.
* @note Respuesta esperada : 'OK'.
*/
#define ATCIPSEND	"AT+CIPSEND="

/**
* @def ESP8266_DELAY
* @brief Maxima cantidad de tiempo en milisegundos que se espera la respuesta por parte del modulo Wifi.
*/
#define ESP8266_DELAY 					5000

/**
* @def WIFI_NAME_LENGTH
* @brief Maxima cantidad de caracteres posibles para el nombre de red a la cual conectar el modulo Wifi.
*/
#define WIFI_NAME_LENGTH 				30

/**
* @def WIFI_PASSWORD_LENGTH
* @brief Maxima cantidad de caracteres posibles para la contraseña de red a la cual conectar el modulo Wifi.
*/
#define WIFI_PASSWORD_LENGTH 			30

/**
* @def RESPONSE_TO_WAIT_LENGTH
* @brief Maxima cantidad de caracteres de la respuesta que se espera por parte del modulo Wifi.
*/
#define RESPONSE_TO_WAIT_LENGTH 		15
/*==================[typedef]================================================*/
/**
* @enum esp8266Error_t
* @brief Definicion de los estados posibles de funcionamiento del modulo Wifi ESP8266.
*/
typedef enum{
	ESP8266_NOT_WORKING = 0, 		/**< Modulo Wifi ESP8266 No Funcionando */
	ESP8266_WORKING = 1				/**< Modulo Wifi ESP8266 Funcionando */
} esp8266Error_t;

/**
* @enum wifiMode_t
* @brief Definicion de los modos de Wifi posibles para el modulo Wifi ESP8266.
* @note Se pueden usar en conjunto. Por ejemplo : STATION_MODE | ACCESS_POINT_MODE.
*/
typedef enum{
	STATION_MODE = 1, 				/**< Modulo Wifi en modo STATION */
	ACCESS_POINT_MODE = 2			/**< Modulo Wifi en modo SOFT AP */
} wifiMode_t; 

/**
* @enum TCPUDPConnectionType_t
* @brief Definicion de la cantidad de conexiones TCP/UDP simultaneas que puede soportar el modulo Wifi ESP8266.
*/
typedef enum{
	SINGLE_CONNECTION = 0, 			/**< Modulo Wifi con 1 sola conexion */
	MULTIPLE_CONNECTION = 1			/**< Modulo Wifi con hasta 4 conexiones */
} TCPUDPConnectionType_t;

/**
* @struct esp8266_t
* @brief Estructura de datos del modulo Wifi ESP8266.
*/
typedef struct{
	uartMap_t				wifiUart;				/**< UART a utilizar por el modulo Wifi */
	wifiMode_t 				wifiMode;				/**< Modo a utilizar por el modulo Wifi */
	TCPUDPConnectionType_t  TCPUDPConnectionType;	/**< Tipo de conexion a utilizar por el modulo Wifi */

	char wifiName[WIFI_NAME_LENGTH];				/**< Nombre de la red a la cual se conecta el modulo Wifi */
	char wifiPassword [WIFI_PASSWORD_LENGTH];		/**< Contraseña de la red a la cual se conecta el modulo Wifi */

	char responseToWait [RESPONSE_TO_WAIT_LENGTH];	/**< Respuesta a esperar por parte del modulo Wifi */
}esp8266_t;
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
* @fn esp8266Error_t wifiInit(uartMap_t wifiUart, wifiMode_t wifiMode, TCPUDPConnectionType_t TCPUDPConnectionType,
						 char* wifiName, char* wifiPassword)
* @brief Inicializacion y configuracion del modulo Wifi.
* @param wifiUart 				: UART a utilizar por el modulo Wifi.
* @param wifiMode 				: Modo a utilizar por el modulo Wifi.
* @param TCPUDPConnectionType 	: Tipo de conexion a utilizar por el modulo Wifi.
* @param wifiName 				: Nombre de la red a la cual se conecta el modulo Wifi.
* @param wifiPassword			: Contraseña de la red a la cual se conecta el modulo Wifi.
* @return ESP8266_NOT_WORKING Si la inicializacion NO fue correcta, ESP8266_WORKING caso contrario.
*/
esp8266Error_t wifiInit(uartMap_t wifiUart, wifiMode_t wifiMode, TCPUDPConnectionType_t TCPUDPConnectionType,
						 char* wifiName, char* wifiPassword);


/**
* @fn esp8266Error_t wifiSendTCPIPData(char *data, char* transportLayer, char* TCPUDPIp, int32_t TCPUDPPort, uint8_t TCPUDPConnectionId)
* @brief Envio de data via TCP/UDP.
* @param data 					: Puntero al buffer que almacena los datos a enviar.
* @param transportLayer			: Capa de transporte a utilizar para enviar los datos.
* @param TCPUDPIp 				: IP del servidor al cual enviar los datos. 
* @param TCPUDPPort 			: Puerto TCP/UDP del servidor con el cual establecer la conexion.
* @param TCPUDPConnectionId		: Interfaz TCP/UDP a utilizar(0 - 4).
* @return ESP8266_NOT_WORKING Si la inicializacion NO fue correcta, ESP8266_WORKING caso contrario.
* @note La interfaz TCP/UDP puede ser distinto de 0 solo si TCPUDPConnectionType fue seteado como MULTIPLE_CONNECTION.
*/
esp8266Error_t wifiSendTCPIPData(char *data, char* transportLayer, char* TCPUDPIp, int32_t TCPUDPPort, uint8_t TCPUDPConnectionId);

/**
* @fn char* getWifiName()
* @brief Obtencion del nombre de la red a la cual se conecta el modulo Wifi.
* @return El nombre de la red.
*/
char* getWifiName();

/**
* @fn char* getWifiPassword()
* @brief Obtencion de la contraseña de la red a la cual se conecta el modulo Wifi.
* @return La contraseña de la red.
*/
char* getWifiPassword();

/*==================[end of file]============================================*/
#endif /* #ifndef _WIFI_ESP8266_H_ */
