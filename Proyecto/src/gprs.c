/*==================[inclusions]=============================================*/
#include "gprs.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
/**
* @var static gprs_t gprs
* @brief Variable local donde se almacenan los datos del GPRS.
* @note static
*/
static gprs_t gprs;

/*
* @var static const char responseOK[]
* @brief Respuesta OK esperada luego de ciertos comandos enviados al GPRS.
* @note static const
*/
static const char responseOK[]  = "OK";
/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
/**
* @fn static gprsError_t gprsGetResponse()
* @brief Obtencion de la respuesta a un comando por parte del modulo GPRS
* @return GPRS_WORKING si la respuesta recibida es la esperada, GPRS_NOT_WORKING caso contrario.
* @note static
*/
static gprsError_t getResponse(){
   uint8_t index = 0;                              /** index : Indice actual dentro del buffer de la respuesta esperada */
   uint8_t byteReceived;                           /** byteReceived : Byte recibido */
   delay_t gprsDelay;                              /** gprsDelay : Tiempo de espera de la respuesta por parte del modulo GPRS */
   gprsError_t moduleResponse = GPRS_NOT_WORKING;  /** moduleResponse : Estado de funcionamiento del modulo GPRS */
   /* Inicializacion del delay */
   delayInit(&gprsDelay, GPRS_DELAY);
   /* Mientras el delay no expire y no se reciba la respuesta esperada */
   while (!delayRead(&gprsDelay) && GPRS_NOT_WORKING == moduleResponse){
      /* Si se recibio un caracter por la UART del GPRS */
      if(uartReadByte(gprs.gprsUart, &byteReceived)){
         /* Si el caracter recibido coincide con el caracter esperado */
         if (byteReceived == gprs.responseToWait[index]){
             /* Se aumenta en uno el indice para obtener el siguiente caracter esperado */
            index++;
            /* Si se llego al final de la respuesta esperada es porque se recibio la respuesta esperada de forma correcta */
            if (gprs.responseToWait[index] == '\0'){
               /* Se setea el valor de retorno como GPRS en funcionamiento */
               moduleResponse = GPRS_WORKING;
            }
         } 
         else { /* Si el caracter recibido no coincide con el caracter esperado */
            /* Se reinicia el indice para comenzar de nuevo */
            index = 0;
         }
      }
   }
   /* Sse retorna GPRS_NOT_WORKING si no se encontro la respuesta esperada y GPRS_WORKING en caso contrario */
   return moduleResponse;
}
/*==================[external functions definition]==========================*/
gprsError_t gprsInit(uartMap_t gprsUart){
   /* Se setea la UART a utilizar por el GPRS */
   gprs.gprsUart = gprsUart;
   uartInit(gprsUart, 115200);
   /* Copiar al buffer de respuesta esperada la respuesta OK */
   strcpy(gprs.responseToWait, responseOK);
   /* Enviar el comando AT */
   uartWriteString(gprsUart, AT);
   /* Retornar el estado de funcionamiento del modulo GPRS de acuerdo a si se recibio o no la respuesta esperada */
   return getResponse();
}


gprsError_t gprsSendSMS(char *sms, char *cellNumber){
   gprsError_t error;      /** error : Estado de funcionamiento del modulo GPRS */
   /* Reiniciar el buffer de respuesta esperada */
   memset(gprs.responseToWait, 0, RESPONSE_TO_WAIT_LENGTH );
   /* Copiar al buffer de respuesta esperada la respuesta OK */
   strcpy(gprs.responseToWait, responseOK);
   /* Enviar el comando de seteo de modo de texto */
   uartWriteString(gprs.gprsUart, ATCMGF);
   /* Obtener el estado de funcionamiento del modulo GPRS de acuerdo a si se recibio o no la respuesta esperada */
   error = getResponse();
   /* Si el modulo GPRS funciona correctamente */
   if(GPRS_WORKING == error){
      delay(100);
      /* Enviar el comando de envio de SMS con el indice del numero de celular indicado*/
      uartWriteString(gprs.gprsUart, ATCMGS);
      uartWriteString(gprs.gprsUart, cellNumber);
      uartWriteString(gprs.gprsUart, "\"\r\n");
      /* Reiniciar el buffer de respuesta esperada */
      memset(gprs.responseToWait, 0, RESPONSE_TO_WAIT_LENGTH );
      /* Copiar al buffer de respuesta esperada el caracter '>' */
      strcpy(gprs.responseToWait, ">");
      /* Obtener el estado de funcionamiento del modulo GPRS de acuerdo a si se recibio o no la respuesta esperada */
      error = getResponse();
      /* Si el modulo GPRS funciona correctamente */
      if(GPRS_WORKING == error){
         /* Reiniciar el buffer de respuesta esperada */
         memset(gprs.responseToWait, 0, RESPONSE_TO_WAIT_LENGTH );
         /* Copiar al buffer de respuesta esperada la respuesta OK */
         strcpy(gprs.responseToWait, responseOK);
         /* Enviar por UART el SMS indicado */
         uartWriteString(gprs.gprsUart, sms);
         /* Enviar por UART el Delimitador de Fin de SMS */
         uartWriteByte(gprs.gprsUart, GPRS_END_OF_SMS);
         delay(200);
         /* Obtener el estado de funcionamiento del modulo GPRS de acuerdo a si se recibio o no la respuesta esperada */
         error = getResponse();
      }
   }
   return error;   
}

uint8_t gprsGetSignal(){
   delay_t gprsDelay;                  /** gprsDelay : Tiempo de espera de la respuesta por parte del modulo GPRS*/
   uint8_t signal = 0;                 /** signal : Valor de señal recibido*/
   uint8_t byteReceived = ' ';         /** byteReceived : Byte recibido*/
   uint8_t pow = 10;                   /** pow : Multiplicador del caracter del valor de señal recibido en cada instante */
   gprsError_t error;                  /** error : Estado de funcionamiento del modulo GPRS */

   /* Reiniciar el buffer de respuesta esperada */
   memset(gprs.responseToWait, 0, RESPONSE_TO_WAIT_LENGTH );
   /* Copar al buffer de respuesta esperada el caracter ':' */
   strcpy(gprs.responseToWait, ":");
   /* Enviar por UART el comando de obtencion de señal del modulo GPRS */
   uartWriteString(gprs.gprsUart, ATCSQ);
   /* Obtener el estado de funcionamiento del modulo GPRS de acuerdo a si se recibio o no la respuesta esperada */
   error = getResponse();
   /* Si el modulo GPRS funciona correctamente */
   if(GPRS_WORKING == error){
      /* Inicializacion del delay */
      delayInit(&gprsDelay, GPRS_DELAY);
      /* Mientras el delay no expire y no se reciba el caracter esperado (',') */
      while (!delayRead(&gprsDelay) && byteReceived != ',') {
         /* Si se recibio un caracter */
         if(uartReadByte(gprs.gprsUart, &byteReceived)) {
            /* Si el caracter recibido es un digito entre 0 y 9 */
            if (byteReceived >= '0' && byteReceived <= '9') {
               /* Se setea la señal */
               signal += (charToInt(byteReceived)*pow);
               /* Se divide el multiplicador */
               pow/=10;
            }
         }
      }
   }
   /* Si se recibio el caracter ',' significa que la señal se recibio correctamente */
   if(byteReceived == ','){
      /* Se hace el calculo de la señal a dbm */
      return MIN_SIGNAL_IN_DBM - (signal - 2)*2;  /* Notar que lo devuelve positivo, cuando en realidad es negativo */
   }
   else{
      /* Si no se recibio el caracter ',', hubo un error al obtener la señal y se devuelve que el GPRS no funciona correctamente */
      return GPRS_NOT_WORKING; 
   }

}
/*==================[end of file]============================================*/
