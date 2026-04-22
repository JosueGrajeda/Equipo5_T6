/********************************************************
  Instituto Tecnológico de Chihuahua
  TAREA 6: LABORATORIO DE MACHINE LEARNING
  Archivo: ADC.cpp
  Sistema Embebido: Nodo de Monitoreo de Voltaje y Control ON-OFF
  Equipo 5:      Gerardo Almeida Bermúdez 22061097
                 Josué Saul Grajeda Manríquez 21060696
*********************************************************/

#include "BSP.h"

// Inicialización del módulo ADC
void ADC_init() {
    analogReadResolution(12);
}

// Lectura directa
int ADC_read(int pin) {
    return analogRead(pin);
}

// Conversión a 0 o 1 usando umbral
int ADC_toBinary(int valor) {
    if (valor > UMBRAL) return 1;
    else return 0;
}