/********************************************************
  Instituto Tecnológico de Chihuahua
  TAREA 6: LABORATORIO DE MACHINE LEARNING
  Archivo: ML.cpp
  Sistema Embebido: Nodo de Monitoreo de Voltaje y Control ON-OFF
  Equipo 5:      Gerardo Almeida Bermúdez 22061097
                 Josué Saul Grajeda Manríquez 21060696
*********************************************************/

#include "BSP.h"

float lr = 0.1;         //Tasa de aprendizaje
int total_patrones = 0;

int **tablaX = NULL;
int *tablaY = NULL;

/* ******** ACTIVACION ******** */
int hardlim(float x) {
    if (x >= 0) return 1;
    else return 0;
}

/* ******** PESOS ******** */
void initPesos() {
    pesos = (float*) malloc((N + 1) * sizeof(float));

    for (int i = 0; i < N + 1; i++) {
        pesos[i] = random(-10, 10) / 10.0;
    }
}

/* ******** TABLA DE VERDAD ******** */
void capturarTabla() {
    // Liberar memoria anterior (Si existe)
    if (tablaX != NULL) {
        for (int i = 0; i < total_patrones; i++) {
            free(tablaX[i]);
        }
        free(tablaX);
        tablaX = NULL;
    }

    if (tablaY != NULL) {
        free(tablaY);
        tablaY = NULL;
    }

    total_patrones = 1 << N;

    tablaX = (int**) malloc(total_patrones * sizeof(int*));
    tablaY = (int*) malloc(total_patrones * sizeof(int));

    PRINT_mensaje("Definir tabla de verdad (Solo 0 o 1):");

    for (int i = 0; i < total_patrones; i++) {

        tablaX[i] = (int*) malloc(N * sizeof(int));

        //Genera e imprime el patrón
        for (int j = 0; j < N; j++) {
            tablaX[i][j] = (i >> (N - j - 1)) & 1; // Orden conocido: 00 -> 11 (Según N)
            Serial.print(tablaX[i][j]);
        }

        Serial.print(" -> salida = ");
        int val = -1;

        while (val != 0 && val != 1) {

            while (Serial.available() == 0) {}

            val = Serial.parseInt();
            limpiarSerial();

            if (val != 0 && val != 1) {
                PRINT_mensaje("Valor invalido: solo se permite 0 o 1");
            }
        }
        
        Serial.println(val);
        tablaY[i] = val;
    }
}

/* ******** ENTRENAMIENTO ******** */
void entrenar() {

    float lr = 0.1;

    for (int epoch = 0; epoch < 50; epoch++) {

        if (xSemaphoreTake(mutexSerial, portMAX_DELAY)) {
            Serial.print("**** Iteracion ");
            Serial.print(epoch + 1);
            Serial.println(" ****");
            xSemaphoreGive(mutexSerial);
        }

        for (int i = 0; i < total_patrones; i++) {

            int y_pred = predecir(tablaX[i]);
            int error = tablaY[i] - y_pred;

            // Impresión de posible error
            if (xSemaphoreTake(mutexSerial, portMAX_DELAY)) {
                Serial.print("Error: ");
                Serial.println(error);
                xSemaphoreGive(mutexSerial);
    }
            // Actualizar pesos
            for (int j = 0; j < N; j++) {
                pesos[j] += lr * error * tablaX[i][j];
            }

            pesos[N] += lr * error; // Bias o tendencia

            // Imprimir pesos en cada paso
            if (xSemaphoreTake(mutexSerial, portMAX_DELAY)) {

                Serial.print("Pesos: ");

                for (int j = 0; j <= N; j++) {
                    Serial.print(pesos[j], 4);
                    Serial.print(" ");
                }

                Serial.println();
                xSemaphoreGive(mutexSerial);
            }
            
            vTaskDelay(DELAY_PRINT / portTICK_PERIOD_MS);
        }

        vTaskDelay(DELAY_PRINT / portTICK_PERIOD_MS);
    }

    PRINT_mensaje("Entrenamiento terminado");
    imprimirEcuacion();  // Ecuación final
}

/* ******** IMPRESIÓN DE ECUACIÓN ******** */
void imprimirEcuacion() {

    if (xSemaphoreTake(mutexSerial, portMAX_DELAY)) {

        Serial.print("y = ");

        for (int i = 0; i < N; i++) {
            Serial.print(pesos[i], 4);
            Serial.print("*x");
            Serial.print(i);

            if (i < N - 1) Serial.print(" + ");
        }

        Serial.print(" + ");
        Serial.print(pesos[N], 4); // Bias

        Serial.println();

        xSemaphoreGive(mutexSerial);
    }
}

/* ******** INFERENCIA ******** */
int predecir(int *entrada) {

    float suma = 0;

    for (int i = 0; i < N; i++) {
        suma += pesos[i] * entrada[i];
    }
    
    suma += pesos[N];       // Bias
    return hardlim(suma);
}

/* ******** EEPROM ******** */
void guardarPesos() {

    EEPROM.begin(64);
    EEPROM.write(0, N); // Guardar N en la primera posición

    for (int i = 0; i < N + 1; i++) {
        EEPROM.writeFloat(1 + i * sizeof(float), pesos[i]);
    }

    EEPROM.commit();
    PRINT_mensaje("Pesos y N guardados");
}

void cargarPesos() {

    EEPROM.begin(64);
    N = EEPROM.read(0); // Recuperar valor de N

    if (N < 2 || N > 5) {
        PRINT_mensaje("N invalido en EEPROM");
        return;
    }

    initPesos(); // Ajustar tamaño

    for (int i = 0; i < N + 1; i++) {
        pesos[i] = EEPROM.readFloat(1 + i * sizeof(float));
    }

    PRINT_mensaje("Pesos cargados:");
    imprimirEcuacion();
}