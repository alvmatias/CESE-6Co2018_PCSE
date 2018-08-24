/*==================[inclusions]=============================================*/
#include "wifi_esp8266.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
/**
* @var static esp8266_t esp8266Wifi
* @brief Datos internos de la configuracion del modulo Wifi.
* @note static
*/
static esp8266_t esp8266Wifi;

/**
* @var static const char responseCWJAP1[]
* @brief Buffer interno con la primer respuesta esperada al comando CW+JAP del modulo Wifi.
* @note static const
*/
static const char responseCWJAP1[] = "WIFI CONNECTED";

/**
* @var static const char responseCWJAP1[]
* @brief Buffer interno con la segunda respuesta esperada al comando CW+JAP del modulo Wifi.
* @note static const
*/
static const char responseCWJAP2[] = "WIFI GOT IP";

/**
* @var static const char responseOK[]
* @brief Buffer interno con la respuesta esperada a ciertos comandos del modulo Wifi.
* @note static const
*/
static const char responseOK[]  = "OK";
/*==================[internal functions declaration]=========================*/

/*==================[internal functions definition]==========================*/
/**
* @fn static esp8266Error_t getResponse()
* @brief Obtencion de la respuesta a un comando por parte del modulo Wifi
* @return ESP8266_WORKING si la respuesta recibida es la esperada, ESP8266_NOT_WORKING caso contrario.
* @note static
*/
static esp8266Error_t getResponse(){
    uint8_t index = 0;										/** index : Indice actual dentro del buffer de la respuesta esperada */
    uint8_t byteReceived;									/** byteReceived : Byte recibido */
    esp8266Error_t moduleResponse = ESP8266_NOT_WORKING;	/** moduleResponse : Estado de funcionamiento del modulo Wifi */
    delay_t esp8266Delay;									/** esp8266Delay : Tiempo de espera de la respuesta por parte del modulo Wifi */
    /* Inicializacion del delay */
    delayInit(&esp8266Delay, ESP8266_DELAY);
    /* Mientras el delay no expire y no se reciba la respuesta esperada */
    while (!delayRead(&esp8266Delay) && ESP8266_NOT_WORKING == moduleResponse){
		/* Si se recibio un caracter por la UART del Wifi */
		if(uartReadByte(esp8266Wifi.wifiUart, &byteReceived)){
			/* Si el caracter recibido coincide con el caracter esperado */
			if (byteReceived == esp8266Wifi.responseToWait[index]){
				/* Se aumenta en uno el indice para obtener el siguiente caracter esperado */
				index++;
				/* Si se llego al final de la respuesta esperada es porque se recibio la respuesta esperada de forma correcta */
				if (esp8266Wifi.responseToWait[index] == '\0'){
					/* Se setea el valor de retorno como ESP8266 en funcionamiento */
					moduleResponse = ESP8266_WORKING;
				}
			} 
			else {/* Si el caracter recibido no coincide con el caracter esperado */
            	/* Se reinicia el indice para comenzar de nuevo */
				index = 0;
			}
		}
    }
    /* Se retorna ESP8266_NOT_WORKING si no se encontro la respuesta esperada y ESP8266_WORKING en caso contrario */
    return moduleResponse;
}

/*==================[external functions definition]==========================*/
esp8266Error_t wifiInit(uartMap_t wifiUart, wifiMode_t wifiMode, TCPUDPConnectionType_t TCPUDPConnectionType, 
						char* wifiName, char* wifiPassword){

	esp8266Error_t error;		/** error : Estado de funcionamiento del modulo Wifi */
	/* Se setea la UART a utilizar por el Wifi */
	esp8266Wifi.wifiUart = wifiUart;
	uartInit(wifiUart, 115200);
	/* Copiar al buffer de respuesta esperada la respuesta OK */
	strcpy(esp8266Wifi.responseToWait, responseOK);
	/* Enviar el comando AT */
	uartWriteString(esp8266Wifi.wifiUart, AT);
	/* Obtener el estado de funcionamiento del modulo Wifi de acuerdo a si se recibio o no la respuesta esperada */
	error = getResponse();
	/* Si el modulo Wifi funciona correctamente */
	if(ESP8266_WORKING == error){
		/* Enviar el comando AT+RST */
		uartWriteString(esp8266Wifi.wifiUart, ATRST);
		/* Obtener el estado de funcionamiento del modulo Wifi de acuerdo a si se recibio o no la respuesta esperada */
		error = getResponse();
		/* Si el modulo Wifi funciona correctamente */
		if(ESP8266_WORKING == error){
			/* Setear el modo Wifi y el tipo de conexion TCP/UDP */
			esp8266Wifi.wifiMode = wifiMode;
			esp8266Wifi.TCPUDPConnectionType = TCPUDPConnectionType;
			/* Copiar el nombre y contraseña de la red a la cual conectar el modulo */
			strcpy(esp8266Wifi.wifiName, wifiName);
			strcpy(esp8266Wifi.wifiPassword, wifiPassword);
			/* Setar el modo de Wifi */
			/* Enviar el comando AT+CWMODE */
			uartWriteString(esp8266Wifi.wifiUart, ATCWMODE);
			uartWriteString(esp8266Wifi.wifiUart, intToString(esp8266Wifi.wifiMode));
			uartWriteString(esp8266Wifi.wifiUart, "\r\n");
			/* Obtener el estado de funcionamiento del modulo Wifi de acuerdo a si se recibio o no la respuesta esperada */
			error = getResponse();
			/* Si el modulo Wifi funciona correctamente */
			if(ESP8266_WORKING == error){
				/* Setear el tipo de conexion TCP/UDP */
				/* Enviar el comando AT+CIPMUX */
				uartWriteString(esp8266Wifi.wifiUart, ATCIPMUX);
				uartWriteString(esp8266Wifi.wifiUart, intToString(esp8266Wifi.TCPUDPConnectionType));
				uartWriteString(esp8266Wifi.wifiUart, "\r\n");
				/* Obtener el estado de funcionamiento del modulo Wifi de acuerdo a si se recibio o no la respuesta esperada */
				error = getResponse();
				/* Si el modulo Wifi funciona correctamente */
				if(ESP8266_WORKING == error){
					/* Conectarse a la red */
					/* Enviar el comando AT+CWJAP */
					uartWriteString(esp8266Wifi.wifiUart, ATCWJAP);
					uartWriteString(esp8266Wifi.wifiUart, esp8266Wifi.wifiName);
					uartWriteString(esp8266Wifi.wifiUart, "\",\"");
					uartWriteString(esp8266Wifi.wifiUart, esp8266Wifi.wifiPassword);
					uartWriteString(esp8266Wifi.wifiUart, "\"\r\n");
					/* Reiniciar el buffer de respuesta esperada */
					memset( esp8266Wifi.responseToWait, 0, RESPONSE_TO_WAIT_LENGTH );
					/* Copiar al buffer de respuesta esperada la primer respuesta esperada al comando CW+JAP */
					strcpy(esp8266Wifi.responseToWait, responseCWJAP1);
					/* Obtener el estado de funcionamiento del modulo Wifi de acuerdo a si se recibio o no la respuesta esperada */
					error = getResponse();
					/* Reiniciar el buffer de respuesta esperada */
					memset( esp8266Wifi.responseToWait, 0, RESPONSE_TO_WAIT_LENGTH );
					/* Copiar al buffer de respuesta esperada la segunda respuesta esperada al comando CW+JAP */
					strcpy(esp8266Wifi.responseToWait, responseCWJAP2);
					/* Obtener el estado de funcionamiento del modulo Wifi de acuerdo a si se recibio o no la respuesta esperada */
					error &= getResponse();
				}
			}
		}
	}
	return error;
}

esp8266Error_t wifiSendTCPIPData(char *data, char* transportLayer, char* TCPUDPIp, int32_t TCPUDPPort, uint8_t TCPUDPConnectionId){
	esp8266Error_t error;				/** error : Estado de funcionamiento del modulo Wifi */

	/* Iniciar conexion TCP/UDP */
	/* Enviar el comando AT+CIPSTART */
	uartWriteString(esp8266Wifi.wifiUart, ATCIPSTART);
	/* Si la interfaz de conexion es distinta de 0 */
	if(TCPUDPConnectionId){
		/* Se envia el id de la interfaz de conexion al modulo Wifi */
		uartWriteString(esp8266Wifi.wifiUart, intToString(TCPUDPConnectionId));
		uartWriteString(esp8266Wifi.wifiUart, ",");
	}
	uartWriteString(esp8266Wifi.wifiUart, "\"");
	uartWriteString(esp8266Wifi.wifiUart, transportLayer);
	uartWriteString(esp8266Wifi.wifiUart, "\",\"");
	uartWriteString(esp8266Wifi.wifiUart, TCPUDPIp);
	uartWriteString(esp8266Wifi.wifiUart, "\",");
	uartWriteString(esp8266Wifi.wifiUart, intToString(TCPUDPPort));
	uartWriteString(esp8266Wifi.wifiUart, "\r\n");
	/* Reiniciar el buffer de respuesta esperada */
	memset( esp8266Wifi.responseToWait, 0, RESPONSE_TO_WAIT_LENGTH );
	/* Copiar al buffer de respuesta esperada la respuesta OK */
	strcpy(esp8266Wifi.responseToWait, responseOK);
	/* Obtener el estado de funcionamiento del modulo Wifi de acuerdo a si se recibio o no la respuesta esperada */
	error = getResponse();
	/* Si el modulo Wifi funciona correctamente */
	if(ESP8266_WORKING == error){
		/* Enviar datos */
		/* Enviar el comando AT+CIPSEND */
		uartWriteString(esp8266Wifi.wifiUart, ATCIPSEND);
		/* Si la interfaz de conexion es distinta de 0 */
		if(TCPUDPConnectionId){
			/* Se envia el id de la interfaz de conexion al modulo Wifi */
			uartWriteString(esp8266Wifi.wifiUart, intToString(TCPUDPConnectionId));
			uartWriteString(esp8266Wifi.wifiUart, ",");
		}
		/* Tamaño de datos a enviar + 2 por el \r\n */
		uartWriteString(esp8266Wifi.wifiUart, intToString(strlen(data)+2));
		uartWriteString(esp8266Wifi.wifiUart, "\r\n");
		memset( esp8266Wifi.responseToWait, 0, RESPONSE_TO_WAIT_LENGTH );
		strcpy(esp8266Wifi.responseToWait, ">");
		/* Obtener el estado de funcionamiento del modulo Wifi de acuerdo a si se recibio o no la respuesta esperada */
		error = getResponse();
		/* Si el modulo Wifi funciona correctamente */
		if(ESP8266_WORKING == error){
			uartWriteString(esp8266Wifi.wifiUart, data);
			uartWriteString(esp8266Wifi.wifiUart, "\r\n");
			/* Reiniciar el buffer de respuesta esperada */
			memset( esp8266Wifi.responseToWait, 0, RESPONSE_TO_WAIT_LENGTH );
			/* Copiar al buffer de respuesta esperada la respuesta OK */
			strcpy(esp8266Wifi.responseToWait, responseOK);
			/* Obtener el estado de funcionamiento del modulo Wifi de acuerdo a si se recibio o no la respuesta esperada */
			error = getResponse();
		}
		/* Cerrar la conexion TCP/UDP */
		/* Enviar el comando AT+CIPCLOSE */
		uartWriteString(esp8266Wifi.wifiUart, ATCIPCLOSE);
		/* Si la interfaz de conexion es distinta de 0 */
		if(TCPUDPConnectionId){
			/* Se envia el id de la interfaz de conexion al modulo Wifi */
			uartWriteString(esp8266Wifi.wifiUart, "=");
			uartWriteString(esp8266Wifi.wifiUart, intToString(TCPUDPConnectionId));
		}
		uartWriteString(esp8266Wifi.wifiUart, "\r\n");
		/* Obtener el estado de funcionamiento del modulo Wifi de acuerdo a si se recibio o no la respuesta esperada */
		error &= getResponse();
	}
	return error;
}

char* getWifiName(){
	return esp8266Wifi.wifiName;
}

char* getWifiPassword(){
	return esp8266Wifi.wifiPassword;
}

/*==================[end of file]============================================*/
