/********************************************************
  Instituto Tecnológico de Chihuahua
  TAREA 6: LABORATORIO DE MACHINE LEARNING
  Archivo: GPIO.cpp
  Sistema Embebido: Nodo de Monitoreo de Voltaje y Control ON-OFF
  Equipo 5:      Gerardo Almeida Bermúdez 22061097
                 Josué Saul Grajeda Manríquez 21060696
*********************************************************/

#include "BSP.h"

// Inicializar pines (Módulo GPIO)
void GPIO_init() {
    pinMode(PIN_LED_ESTADO, OUTPUT);
    pinMode(PIN_LED_SALIDA, OUTPUT);
    pinMode(PIN_BOTON, INPUT_PULLUP);
}

// Leer el botón
int leerBoton() {
    return digitalRead(PIN_BOTON);
}

// Escribir en LED
void escribirLED(int pin, int estado) {
    digitalWrite(pin, estado);
}