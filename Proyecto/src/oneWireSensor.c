/*==================[inclusions]=============================================*/
#include "oneWireSensor.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal data declaration]==============================*/
/**
* @var static const oneWireSensorOperation_t operation[MAX_RESOLUTIONS]
* @brief Datos internos con las configuraciones posibles de las distintas resoluciones soportas por el sensor OneWire.
*/
static const oneWireSensorOperation_t operation[MAX_RESOLUTIONS] = {
	{NINE_BITS_RESOLUTION, (float)NINE_BITS_RESOLUTION_STEP/10, NINE_BITS_RESOLUTION_DELAY, NINE_BITS_RESOLUTION_MASK},
	{TEN_BITS_RESOLUTION, (float)TEN_BITS_RESOLUTION_STEP/100, TEN_BITS_RESOLUTION_DELAY, TEN_BITS_RESOLUTION_MASK},
	{ELEVEN_BITS_RESOLUTION, (float)ELEVEN_BITS_RESOLUTION_STEP/1000, ELEVEN_BITS_RESOLUTION_DELAY, ELEVEN_BITS_RESOLUTION_MASK},
	{TWELVE_BITS_RESOLUTION, (float)TWELVE_BITS_RESOLUTION_STEP/10000, TWELVE_BITS_RESOLUTION_DELAY, TWELVE_BITS_RESOLUTION_MASK}
};

/**
* @var oneWireSensor
* @brief Estructura de datos interna con los datos y configuraciones del sensor OneWire.
*/ 
static oneWireSensor_t oneWireSensor;

/*==================[internal functions declaration]=========================*/
/**
* @fn static uint8_t oneWireSensorCheckCRC(uint8_t crc, uint8_t data)
* @brief Funcion que calcula el CRC de los datos del SCRATCHPAD del sensor.
* @param crc : Valor de CRC actual.
* @param data : byte de datos proveniente del SCRATCHPAD del sensor.
* @return Valor de crc.
* @note Funcion privada.
*/
static uint8_t oneWireSensorCheckCRC(uint8_t crc, uint8_t data){
	uint8_t i;		/** i : Indice del for */
	/* crc XOR data */
	crc = crc ^ data;
	/* Por cada bit dentro del byte */
	for (i = 0; i < 8; i++){
		/* Si es un '1', se hace un corrimiento a derecha y un XOR con el polinomio */
	    if (crc & BIT_MASK)
	        crc = (crc >> 1) ^ CRC_POLYNOMIAL;
	    else /* Si no, solamente se hace el corrimiento */
	        crc >>= 1;
	}

	return crc;
}

/**
* @fn static uint8_t oneWireSensorReadBit()
* @brief Lectura de 1 bit del sensor 
* @return El bit leido.
* @note Funcion privada.
*/
static uint8_t oneWireSensorReadBit(){
	uint8_t bit = 0;  /** bit : Bit leido del sensor */

	/* Configuracion del GPIO como OUTPUT */
	gpioConfig(oneWireSensor.gpio, GPIO_OUTPUT);		
	/* Seteado del bus en bajo por al menos 1us*/
	gpioWrite(oneWireSensor.gpio, FALSE);
	delayInaccurateUs(10);
	/* Configuracion del GPIO como entrada para liberarlo */
	gpioConfig(oneWireSensor.gpio, GPIO_INPUT);		
	/* Se espera hasta que el valor en el bus es valido*/
	delayInaccurateUs(12);
	/* Lectura del GPIO */
	if(gpioRead(oneWireSensor.gpio)){
		bit = 1;   /* Se leyo un 1 */
	} 
	/* Else -> No hacer nada -> Un cero fue leido */
	/* Se espera a que el sensor libere el bus */
	delayInaccurateUs(50);

	return bit;
}

/**
* @fn static uint8_t oneWireSensorReadByte()
* @brief Lectura de 1 byte del sensor. 
* @return El byte leido.
* @note Funcion privada.
*/
static uint8_t oneWireSensorReadByte(){
	uint8_t currentBit;  /** currentBit : Numero actual de bit leido */
	uint8_t byte = 0;    /** byte : Byte leido */
	/* Por cada bit dentro del byte */
	for (currentBit = 0; currentBit < 8; currentBit++){
		/* Se lee un bit y se hace un corrimiento */
		byte = byte | (oneWireSensorReadBit() << currentBit);
	}
	return byte;
}

/**
* @fn static void oneWireSensorWriteBit(uint8_t bit)
* @brief Escritura de 1 bit del sensor 
* @param bit : Bit a escribir.
* @return Nada.
* @note Funcion privada.
*/
static void oneWireSensorWriteBit(uint8_t bit){
	/* Configuracion de GPIO como OUTPUT */
	gpioConfig(oneWireSensor.gpio, GPIO_OUTPUT);	
	/* Seteado del bus en bajo*/
	gpioWrite(oneWireSensor.gpio, FALSE);
	if (bit){ /* Escribir un '1' */
		/* Liberacion del bus despues de 10us */
		delayInaccurateUs(10);
		gpioConfig(oneWireSensor.gpio, GPIO_INPUT);
		/* Esperar fin slot escritura + 2us de recuperacion entre bits sucesivos */
		delayInaccurateUs(52);
	}
	else{ /* Escribir un '0' */
		/* Mantener en bajo el bus durante todo el slot de escritura */
		delayInaccurateUs(60);
		/* Liberacion del bus*/
		gpioConfig(oneWireSensor.gpio, GPIO_INPUT);
		/* 2us de recuperacion entre bits sucesivos */
		delayInaccurateUs(2);

	}
	
	
}

/**
* @fn static void oneWireSensorWriteByte(oneWireSensorCommand_t command)
* @brief Escritura de un byte al sensor
* @param command : Comando a enviar.
* @return Nada.
* @note Funcion privada.
*/
static void oneWireSensorWriteByte(oneWireSensorCommand_t command){
	uint8_t currentBit;  /** currentBit : Bit que en cada momento se esta escribiendo */
	/* Por cada bit dentro del comando, se hace un OR con la mascara de un bit(0x01) y un corrimiento del comando a derecha */
	for (currentBit = 0; currentBit < 8; currentBit++){
		oneWireSensorWriteBit(command & BIT_MASK);
		command = command >> 1;	
	}
}

/**
* @fn sensorFunctionalState_t oneWireSensorReset()
* @brief Reseteo y deteccion del estado del sensor
* @return SENSOR_WORKING si el sensor contesto con el pulso de presencia, SENSOR_NOT_WORKING caso contrario.
* @note Funcion privada.
*/
static oneWireSensorError_t oneWireSensorReset(){
	oneWireSensorError_t error;	/** error : Estado del sensor */
	
	/* Configuracion del GPIO como OUTPUT */
	gpioConfig(oneWireSensor.gpio, GPIO_OUTPUT);		
	/* Seteo del bus en bajo */
	gpioWrite(oneWireSensor.gpio, FALSE);
	/* Liberacion del bus despues de 480us */
	delayInaccurateUs(480);
	/* Configuracion del GPIO como entrada para liberar el bus */
	gpioConfig(oneWireSensor.gpio, GPIO_INPUT);		
	/* Espera de 80us para detectar el pulso de presencia del sensor */
	delayInaccurateUs(80);
	/* Lectura del bus para verificar si el sensor esta funcionando */
	if(!gpioRead(oneWireSensor.gpio)){ /* Si el sensor seteo el bus en bajo es porque esta funcionando */
		error = ONE_WIRE_SENSOR_WORKING;
	}
	else{ /* Si el bus sigue en alto producto de la resistencia de pullup es porque NO esta funcionando */
		error = ONE_WIRE_SENSOR_NOT_WORKING;
	}
	/* Espera de 400us para que el sensor libere el bus */
	delayInaccurateUs(400);
	return error;
}

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void oneWireSensorInit(oneWireSensorResolution_t resolution, gpioMap_t gpio){
	oneWireSensor.operation.resolution = operation[resolution - NINE_BITS_RESOLUTION].resolution;
	oneWireSensor.operation.step = operation[resolution - NINE_BITS_RESOLUTION].step;
	oneWireSensor.operation.delay = operation[resolution - NINE_BITS_RESOLUTION].delay;
	oneWireSensor.operation.mask = operation[resolution - NINE_BITS_RESOLUTION].mask;
	oneWireSensor.gpio = gpio;
}

oneWireSensorError_t oneWireSensorFillScratchpad(){	
	uint8_t currentByte;  					/** currentByte : Byte leido en cada momento*/
	uint8_t crc = 0;						/** crc : Valor del calculo de CRC */
	/* Reset Sensor */
	if(ONE_WIRE_SENSOR_WORKING == oneWireSensorReset()){
		/* Skip ROM */
		oneWireSensorWriteByte(SKIP_ROM);
		/* Conversion Temperatura */
		oneWireSensorWriteByte(CONVERT_T);
		/* Espera tiempo conversion */
		delay(oneWireSensor.operation.delay);
		/* Reset Sensor */
		if(ONE_WIRE_SENSOR_WORKING == oneWireSensorReset()){
			/* Skip ROM */
			oneWireSensorWriteByte(SKIP_ROM);
			/* Lectura SRAM SCRATCHPAD */
			oneWireSensorWriteByte(READ_SCRATCHPAD);
			/* Llenado del SCRATCHPAD y calculo CRC */
			for(currentByte = 0; currentByte < SCRATCHPAD_LENGTH - 1; currentByte++){
				oneWireSensor.scratchpad[currentByte] = oneWireSensorReadByte();
				crc = oneWireSensorCheckCRC(crc, oneWireSensor.scratchpad[currentByte]);
			}
			/* Obtencion CRC del SCRATCHPAD */
			oneWireSensor.scratchpad[SCRATCHPAD_CRC_BYTE] = oneWireSensorReadByte();
			/* Si el CRC calculado y el CRC del SCRATCHPAD son iguales, la lectura fue correcta */
			if(crc == oneWireSensor.scratchpad[SCRATCHPAD_CRC_BYTE]){
				return ONE_WIRE_SENSOR_WORKING;
			}
			else return ONE_WIRE_SENSOR_NOT_WORKING;
			
		}
		else return ONE_WIRE_SENSOR_NOT_WORKING;
		
	}
	else return ONE_WIRE_SENSOR_NOT_WORKING;
	
}

float oneWireSensorReadTempValue(){
	/* Retorno del calculo de temperatura */
	return (float) ( ( ( oneWireSensor.scratchpad[SCRATCHPAD_TEMPERATURE_MSB]&0x0f ) <<4 )
						+ (( oneWireSensor.scratchpad[SCRATCHPAD_TEMPERATURE_LSB]&0xf0 ) >>4) ) +
								(float)( ( oneWireSensor.scratchpad[SCRATCHPAD_TEMPERATURE_LSB]&oneWireSensor.operation.mask )*
						oneWireSensor.operation.step );
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
