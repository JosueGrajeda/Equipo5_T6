/********************************************************
  Instituto Tecnológico de Chihuahua
  TAREA 6: LABORATORIO DE MACHINE LEARNING
  Archivo: BSP.h
  Sistema Embebido: Nodo de Monitoreo de Voltaje y Control ON-OFF
  Equipo 5:      Gerardo Almeida Bermúdez 22061097
                 Josué Saul Grajeda Manríquez 21060696
*********************************************************/

#ifndef BSP_H
#define BSP_H

#include <Arduino.h>            // Necesaria para utilizar Arduino IDE
#include <EEPROM.h>             // Util para almacenar pesos
#include "freertos/FreeRTOS.h"  // Escencial para emplear RTOS (FreeRTOS)
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

/* ******** PINES ******** */
#define PIN_LED_ESTADO 2
#define PIN_LED_SALIDA 4
#define PIN_BOTON 15

#define ADC_PINS {34, 35, 32, 33, 25}

/* ******** CONFIG: UMBRAL ******** */
#define UMBRAL 2000             // Punto medio aproximado para 4095 valores
#define DELAY_PRINT 250         // Asignar medio segundo de espera para impresiones

/* ******** VARIABLES ******** */
extern int N;
extern float *pesos;

// RTOS
extern TaskHandle_t taskADCHandle;
extern TaskHandle_t taskMLHandle;
extern TaskHandle_t taskPrintHandle;
extern TaskHandle_t taskControlHandle;

// COLAS
extern QueueHandle_t colaADC;
extern QueueHandle_t colaPrint;

// MUTEX
extern SemaphoreHandle_t mutexSerial;

/* ******** PROTOTIPOS ******** */

// ADC
void ADC_init();
int ADC_read(int pin);
int ADC_toBinary(int valor);

// GPIO
void GPIO_init();
int leerBoton();
void escribirLED(int pin, int estado);

// ML
void initPesos();
void capturarTabla();
void entrenar();
void imprimirEcuacion();
int predecir(int *entrada);
void guardarPesos();
void cargarPesos();

// PRINT
void PRINT_init();
void limpiarSerial();
void PRINT_mensaje(const char *msg);
void PRINT_datos(int *x, int N, int salida);

// SISTEMA
void revisarBoton();
void menu();

#endif