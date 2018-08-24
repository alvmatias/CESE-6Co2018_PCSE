/*==================[inlcusiones]============================================*/

#include "main.h"   // <= own header (optional)
#include "sapi.h"     // <= sAPI header

#include "lcd.h"
#include "gprs.h"
#include "oneWireSensor.h"
#include "wifi_esp8266.h"

#include <string.h>
#include <stdio.h>

/*==================[definiciones y macros]==================================*/
/**
* @def LCD_ADDRESS
* @brief Direccion I2C del LCD a utilizar.
*/
#define LCD_ADDRESS  0x3f

/**
* @def NETWORK
* @brief Nombre de la red a la cual se conecta el modulo Wifi.
*/
#define NETWORK     "FT_SANCHEZ"  //"INVITADOS-CADIEEL"

/**
* @def PASSWORD
* @brief Contraseña de la red a la cual se conecta el modulo Wifi.
*/
#define PASSWORD    "01414484115"  //"CORDOBA9504"

/**
* @def UBIDOTS
* @brief Servidor de IOT al cual enviar los datos de la temperatura.
*/
#define UBIDOTS               "things.ubidots.com"

/**
* @def UBIDOTS_PORT
* @brief Puerto TCP por el cual conectarse a UBIDOTS.
*/
#define UBIDOTS_PORT          80

/**
* @def UBIDOTS_TOKEN
* @Clave utilizada para identificar el repositorio al cual enviar los datos de la temperatura.
*/
#define UBIDOTS_TOKEN         "A1E-HVNk798E0eKnx7Sclb5yhuTUKEUWMq"

/**
* @def UBIDOTS_DEVICE_LABEL
* @brief Device Label del dispositivo en UBIDOTS utilizado para el proyecto
*/
#define UBIDOTS_DEVICE_LABEL  "pcse"

/**
* @def UBDITOS_TEMP_LABEL
* @brief Variable Label de la variable temperatura.
*/
#define UBIDOTS_TEMP_LABEL    "temp"

/**
* @def TRANSPORT_LAYER
* @brief Capa de transporte a ser utilizada para enviar los datos a UBIDOTS.
*/
#define TRANSPORT_LAYER 	  "TCP"

/**
* @def TCP_INTERFACE
* @brief Interfaz TCP del modulo Wifi utilizada para establecer la conexion con UBIDOTS.
*/
#define TCP_INTERFACE		  0

/**
* @def STRING_TO_LCD_LENGTH
* @brief Cantidad maxima de caracteres del string a enviar al LCD.
*/
#define STRING_TO_LCD_LENGTH        20

/**
* @def STRING_TO_GPRS_LENGTH
* @brief Cantidad maxima de caracteres del string a enviar al GPRS.
*/
#define STRING_TO_GPRS_LENGTH       40

/**
* @def STRING_TO_WIFI_LENGTH
* @brief Cantidad maxima de caracteres del string a enviar al Wifi.
*/
#define STRING_TO_WIFI_LENGTH		300

/**
* @def CELL_NUMBER
* @brief Numero de celular al cual enviar el mensaje de reporte cuando la temperatura supera el umbral.
*/
#define CELL_NUMBER            "2396486215"

/**
* @def MAX_TEMP
* @brief Umbral de temperatura.
*/
#define MAX_TEMP        26.0

/**
* @def DELAY
* @brief Espera entre mediciones sucesivas.
*/
#define DELAY           8000
/*==================[definiciones de datos internos]=========================*/
/**
* @var static char stringToWIFI[STRING_TO_WIFI_LENGTH]
* @brief String con los caracteres a enviar al Wifi. 
*/

static char stringToWIFI[STRING_TO_WIFI_LENGTH];
/**
* @var static char stringToWIFI[STRING_TO_WIFI_LENGTH]
* @brief String con los caracteres a enviar al LCD. 
*/
static char stringToLCD[STRING_TO_LCD_LENGTH];

/**
* @var static char stringToWIFI[STRING_TO_WIFI_LENGTH]
* @brief String con los caracteres a enviar al GPRS. 
*/
static char stringToGPRS[STRING_TO_GPRS_LENGTH];
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

/* Programa Principal */
int main( void ){
    float temp;                     /** temp : Temperatura del ds18b20 */
    uint8_t signal;                 /** signal : Señal del modulo GPRS SIM900 */
    gprsError_t gprsError;          /** gprsError : Estado de funcionamiento del GPRS SIM900 */
    esp8266Error_t esp8266Error;    /** esp8266Error : Estado de funcionamiento del Wifi ESP8266 */

    /* Inicializar y configurar la plataforma */
    boardConfig();
    /* Inicializar y configurar el lcd */
    lcdInit(LCD_ADDRESS);
    /* Mensaje de bienvenida en la PRIMER LINEA */
    lcdSetPosXY(LCD_LEFT_MARGIN, LCD_FIRST_LINE);
    lcdWriteString("Proyecto Final PCSE");
    /* Inicializar el sensor ds18b20 */
    oneWireSensorInit(TWELVE_BITS_RESOLUTION, GPIO0);
    /* Inicializar y configurar el GPRS SIM900 */
    gprsError = gprsInit(UART_GPIO);
    /* Inicializar y configurar el Wifi ESP8266 */
    esp8266Error = wifiInit(UART_232, STATION_MODE | ACCESS_POINT_MODE, SINGLE_CONNECTION, NETWORK, PASSWORD);
    /* Repetir Por Siempre */
    while(1){
        /* Si el llenado de la memoria SRAM SCRATCHPAD es correcta */
        if(ONE_WIRE_SENSOR_WORKING == oneWireSensorFillScratchpad()){
            /* Limpiar la SEGUNDA LINEA */
            lcdClearLine(LCD_SECOND_LINE);
            /* Obtener el valor de la temperatura y escribirlo en la TERCER LINEA */
            lcdSetPosXY(LCD_LEFT_MARGIN, LCD_THIRD_LINE);
            temp = oneWireSensorReadTempValue();
            /* Reiniciar el string al LCD */
            memset(stringToLCD, 0, STRING_TO_LCD_LENGTH);
            /* Formatear el string al LCD */
            sprintf(stringToLCD, "Temperature: %04.1fC", temp);
            /* Escribir el string en el LCD */
            lcdWriteString(stringToLCD);
            /* SI la inicializacion del Wifi ESP8266 fue correcta */
            if(ESP8266_WORKING == esp8266Error){
                /* Reiniciar el string al Wifi */
                memset(stringToWIFI, 0, STRING_TO_WIFI_LENGTH);
                /* Foramatear el string al Wifi con el metodo POST para UBIDOTS */
                sprintf(stringToWIFI, "POST /api/v1.6/devices/%s/?token=%s HTTP/1.1\r\nHost: things.ubidots.com\r\nContent-Type: application/json\r\nContent-Length: 13\r\n\r\n{\"%s\":%04.1f}",
                        UBIDOTS_DEVICE_LABEL, UBIDOTS_TOKEN, UBIDOTS_TEMP_LABEL, temp);

                /* Enviar el string por TCP a UBIDORS */
                wifiSendTCPIPData(stringToWIFI, TRANSPORT_LAYER, UBIDOTS, UBIDOTS_PORT, TCP_INTERFACE);
            } 
            lcdSetPosXY(LCD_LEFT_MARGIN, LCD_FOURTH_LINE);
            /* Si la inicializacion del GPRS fue correcta */
			if(GPRS_WORKING == gprsError){
                /* Obtener la señal del gprs */
                signal = gprsGetSignal();
                /* Reiniciar el string al LCD */
				memset(stringToLCD, 0, STRING_TO_LCD_LENGTH);
                /* Formatear el string con la señal del GPRS en dbm */
				sprintf(stringToLCD, "Signal: -%ddbm   ", signal);
                /* Enviar al LCD el string */
				lcdWriteString(stringToLCD);
			}
            /* Si la operacion previa sobre el GPRS arrojo que funcionaba y la temperatura es mayor al umbral */
			if(GPRS_WORKING == gprsError && temp >= MAX_TEMP){
				/* Reiniciar el string al GPRS */
                memset(stringToGPRS, 0, STRING_TO_LCD_LENGTH);
                /* Formatear el string al GPRS con el valor actual de temperatura y señal */
				sprintf(stringToGPRS, "Temperature: %04.1f, Signal: -%ddbm", temp, signal);
                /* Enviar al numero de celular especificado el reporte */
				gprsSendSMS(stringToGPRS, CELL_NUMBER);
			}
        }
        /* Delay Bloqueante */
        delay(DELAY);
   }

   /* No deberia alcanzar nunca esta porcion de codigo */
   return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
