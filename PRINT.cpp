/********************************************************
  Instituto Tecnológico de Chihuahua
  TAREA 6: LABORATORIO DE MACHINE LEARNING
  Archivo: PRINT.cpp
  Sistema Embebido: Nodo de Monitoreo de Voltaje y Control ON-OFF
  Equipo 5:      Gerardo Almeida Bermúdez 22061097
                 Josué Saul Grajeda Manríquez 21060696
*********************************************************/

#include "BSP.h"

// Inicializar consola
void PRINT_init() {
    Serial.begin(9600);
    delay(2000); // Tiempo para abrir el monitor serial
}

// Limpieza del Puerto-Serial
void limpiarSerial() {
    while (Serial.available() > 0) {
        Serial.read();
    }
}

// Impresión por consola de mensajes simples
void PRINT_mensaje(const char *msg) {

    if (xSemaphoreTake(mutexSerial, portMAX_DELAY)) {
        Serial.println(msg);
        xSemaphoreGive(mutexSerial);
    }
}

// Impresón por consola para mostrar datos del sistema
void PRINT_datos(int *x, int N, int salida) {

    if (xSemaphoreTake(mutexSerial, portMAX_DELAY)) {

        Serial.print("Entradas: ");

        for (int i = 0; i < N; i++) {
            Serial.print(x[i]);
            Serial.print(" ");
        }

        Serial.print(" -> Salida: ");
        Serial.println(salida);

        xSemaphoreGive(mutexSerial);
    }
}