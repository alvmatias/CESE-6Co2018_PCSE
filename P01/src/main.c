/* Copyright 2014, ChaN
 * Copyright 2016, Matias Marando
 * Copyright 2016, Eric Pernia
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*==================[inlcusiones]============================================*/

#include "main.h"   // <= own header (optional)
#include "sapi.h"     // <= sAPI header

#include "ff.h"       // <= Biblioteca FAT FS

#include <string.h>
#include "rtc.h"
/*==================[definiciones y macros]==================================*/

#define FILENAME "muestras.txt"

/*==================[definiciones de datos internos]=========================*/

static FATFS fs;           // <-- FatFs work area needed for each volume
static FIL fp;             // <-- File object needed for each open file


static char rtcString[22];
static char CH1String[6];
static char CH2String[6];
static char CH3String[6];
static char textToWrite[52];
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

// Guarda en str la fecha y hora convertido a string en formato "DD/MM/YYYY HH:MM:SS"
static void rtcToString( char* str, rtc_t* rtc ){

   char rtcMemberString[10];
   rtcString[0] = 0;

   // "YYYY/MM/DD"

   // Conversion de entero a string con base 10 (decimal)
   int64ToString( (rtc->year), rtcMemberString, 10 );
   // Concateno rtcString+year+"/"
   strncat( rtcString, rtcMemberString, strlen(rtcMemberString) );
   strncat( rtcString, "/", strlen("/") );

   // Conversion de entero a string con base 10 (decimal)
   int64ToString( (rtc->month), rtcMemberString, 10 );
   if( (rtc->month)<10 ){
      // Concateno rtcString+"0"
      strncat( rtcString, "0", strlen("0") );
   }
   // Concateno rtcString+month+"/"
   strncat( rtcString, rtcMemberString, strlen(rtcMemberString) );
   strncat( rtcString, "/", strlen("/") );

   // Conversion de entero a string con base 10 (decimal)
   int64ToString( (rtc->mday), rtcMemberString, 10 );
   if( (rtc->mday)<10 ){
      // Concateno rtcString+"0"
      strncat( rtcString, "0", strlen("0") );
   }
   // Concateno rtcString+day+"_"
   strncat( rtcString, rtcMemberString, strlen(rtcMemberString) );
   strncat( rtcString, "_", strlen("_") );

   // "HH:MM:SS"

   // Conversion de entero a string con base 10 (decimal)
   int64ToString( (rtc->hour), rtcMemberString, 10 );
   if( (rtc->hour)<10 ){
      // Concateno rtcString+"0"
      strncat( rtcString, "0", strlen("0") );
   }
   // Concateno rtcString+hour+":"
   strncat( rtcString, rtcMemberString, strlen(rtcMemberString) );
   strncat( rtcString, ":", strlen(":") );

   // Conversion de entero a string con base 10 (decimal)
   int64ToString( (rtc->min), rtcMemberString, 10 );
   if( (rtc->min)<10 ){
      // Concateno rtcString+"0"
      strncat( rtcString, "0", strlen("0") );
   }
   // Concateno rtcString+min+":"
   strncat( rtcString, rtcMemberString, strlen(rtcMemberString) );
   strncat( rtcString, ":", strlen(":") );

   // Conversion de entero a string con base 10 (decimal)
   int64ToString( (rtc->sec), rtcMemberString, 10 );
   if( (rtc->sec)<10 ){
      // Concateno rtcString+"0"
      strncat( rtcString, "0", strlen("0") );
   }
   // Concateno rtcString+sec
   strncat( rtcString, rtcMemberString, strlen(rtcMemberString) );
}

/*==================[declaraciones de funciones externas]====================*/

// FUNCION que se ejecuta cada vezque ocurre un Tick
void diskTickHook( void *ptr );


/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void ){
   UINT nbytes;
   /* Estructura RTC */
   rtc_t rtc;
   //delay
   delay_t delay1s;
   //adc value
   uint16_t adcValueCH1 = 0;
   uint16_t adcValueCH2 = 0;
   uint16_t adcValueCH3 = 0;
   uint8_t i;
   // ---------- CONFIGURACIONES ------------------------------
   // Inicializar y configurar la plataforma
   boardConfig();

   // SPI configuration
   spiConfig( SPI0 );

   // Inicializar el conteo de Ticks con resolucion de 10ms,
   // con tickHook diskTickHook
   tickConfig( 10 );
   tickCallbackSet( diskTickHook, NULL );


   //ADC Config
   adcInit(ADC_ENABLE);

   /* Inicializar RTC */
   rtc.year = 2018;
   rtc.month = 7;
   rtc.mday = 1;
   rtc.wday = 1;
   rtc.hour = 13;
   rtc.min = 17;
   rtc.sec= 0;

   rtcConfig( &rtc );

   delayConfig( &delay1s, 1000 );

   delay(2000); // El RTC tarda en setear la hora, por eso el delay

   // Give a work area to the default drive
   if( f_mount( &fs, "", 0 ) != FR_OK ){
      // If this fails, it means that the function could
      // not register a file system object.
      // Check whether the SD card is correctly connected
   }

   // ---------- REPETIR POR SIEMPRE --------------------------
   while(TRUE){
      if( delayRead( &delay1s ) ){
    	  gpioWrite(LEDG, 0);
         /* Leer fecha y hora */
         rtcRead( &rtc );
         // Convertir lectura de fecha y hora a string en formato "DD/MM/YYYY HH:MM:SS"
         rtcToString( rtcString, &rtc );
         // Agrego SEPARATOR al final
         strncat( rtcString, ";", 1 );
         /* Muestrear los ADC's y escribir en la SD */
         if( FR_OK == f_open( &fp, FILENAME, FA_WRITE | FA_OPEN_APPEND ) ){
            
            adcValueCH1 = adcRead(CH1);
            // Convertir lectura del ADC a string
            int64ToString( (int64_t)adcValueCH1, CH1String, 10 );
            // Agrego ';' al final
            strncat( CH1String, ";", 1 );
            adcValueCH2 = adcRead(CH2);
            // Convertir lectura del ADC a string
            int64ToString( (int64_t)adcValueCH2, CH2String, 10 );
            // Agrego ';' al final
            strncat( CH2String, ";", 1 );
            adcValueCH3 = adcRead(CH3);
            // Convertir lectura del ADC a string
            int64ToString( (int64_t)adcValueCH3, CH3String, 10 );
            // Agrego ';' al final
            strncat( CH3String, ";", 1 );
            
            // Concatento textToWrite con magString quedando textToWrite: CH1String
            strncat( textToWrite, CH1String, strlen(CH1String) );
            // Concatento textToWrite con magString quedando textToWrite: CH1String+CH2String
            strncat( textToWrite, CH2String, strlen(CH2String) );
            // Concatento textToWrite con magString quedando textToWrite: CH1String+CH2String+CH3String
            strncat( textToWrite, CH3String, strlen(CH3String) );
            // Concatento textToWrite con rtcString quedando en textToWrite: CH1String+CH2String+CH3String+rtcString
            strncat( textToWrite, rtcString, strlen(rtcString) );
            // Concatento textToWrite con "\r\n" quedando textToWrite: rtcString+adcString+"\r\n"
            strncat( textToWrite, "\r\n", 2 );

            f_write( &fp, textToWrite, strlen(textToWrite), &nbytes );

            if(nbytes == strlen(textToWrite)){
            	gpioWrite(LEDG, 1);
            }

            f_close(&fp);
         }
         adcValueCH1 = 0;
         adcValueCH2 = 0;
         adcValueCH3 = 0;
         textToWrite[0] = 0;

      }

      sleepUntilNextInterrupt();
   }

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamenteno sobre un microcontroladore y no es llamado/ por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

// FUNCION que se ejecuta cada vezque ocurre un Tick
void diskTickHook( void *ptr ){
   disk_timerproc();   // Disk timer process
}


/*==================[fin del archivo]========================================*/
