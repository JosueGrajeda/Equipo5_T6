/********************************************************
  Instituto Tecnológico de Chihuahua
  TAREA 6: LABORATORIO DE MACHINE LEARNING
  Sistema Embebido: Nodo de Monitoreo de Voltaje y Control ON-OFF
  Equipo 5:      Gerardo Almeida Bermúdez 22061097
                 Josué Saul Grajeda Manríquez 21060696
*********************************************************/

#include "BSP.h"

/* ******** VARIABLES ******** */
int N;
float *pesos;

int modo = 0;

int adc_pins[] = ADC_PINS;

// RTOS
TaskHandle_t taskADCHandle;
TaskHandle_t taskMLHandle;
TaskHandle_t taskPrintHandle;
TaskHandle_t taskControlHandle;

QueueHandle_t colaADC;
QueueHandle_t colaPrint;

SemaphoreHandle_t mutexSerial;

/* ******** ESTRUCTURA PARA COLA ******** */
typedef struct {
    int datos[5];
    int salida;
} paquete_t;

/* ******** BOTÓN - DIMENSIONES - MENÚ ******** */

// BOTÓN
void revisarBoton() {

    static int ultimoEstado = HIGH;

    int lectura = leerBoton();

    // Detectar flanco (Cuando se presiona)
    if (ultimoEstado == HIGH && lectura == LOW) {

        modo = !modo;
        PRINT_mensaje("Cambio de modo");
    }

    ultimoEstado = lectura;
}

// CAMBIAR N
void pedirN() {

    PRINT_mensaje("Ingrese N (2 a 5):");

    while (Serial.available() == 0) {}

    N = Serial.parseInt();
    limpiarSerial();

    if (N < 2) N = 2;
    if (N > 5) N = 5;

    // Reinicializar pesos al cambiar N
    initPesos();
}

// DEFINIR MENÚ
void menu() {

    PRINT_mensaje("Menu:");
    PRINT_mensaje("1: Entrenar");
    PRINT_mensaje("2: Guardar pesos");
    PRINT_mensaje("3: Cargar pesos");
    PRINT_mensaje("4: Cambiar N");
    PRINT_mensaje("5: Continuar");

    int op = -1;

    while (op < 1 || op > 5) {

        while (Serial.available() == 0) {}

        op = Serial.parseInt();
        limpiarSerial();
        
        if (op == 1) {
            capturarTabla();
            entrenar();
        }
        else if (op == 2) {
            guardarPesos();
        }
        else if (op == 3) {
            cargarPesos();
        }
        else if (op == 4) {
            pedirN();
        }
        else if (op == 5) {
            PRINT_mensaje("Continuando..."); modo = 1;
        }
        else {
            PRINT_mensaje("Opcion invalida");
        }
    }
}

/* ******** TAREA ADC ******** */
void taskADC(void *pvParameters) {

    int datos[5];

    while (1) {

        if (modo == 1) {

            for (int i = 0; i < N; i++) {
                int lectura = ADC_read(adc_pins[i]);
                datos[i] = ADC_toBinary(lectura);
            }

            xQueueSend(colaADC, &datos, portMAX_DELAY);
        }

        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

/* ******* TAREA ML ******** */
void taskML(void *pvParameters) {

    int datos[5];
    paquete_t paquete;

    while (1) {

        if (xQueueReceive(colaADC, &datos, portMAX_DELAY)) {

            paquete.salida = predecir(datos);

            for (int i = 0; i < N; i++) {
                paquete.datos[i] = datos[i];
            }

            xQueueSend(colaPrint, &paquete, portMAX_DELAY);
        }
    }
}

/* ******** TAREA PRINT ******** */
void taskPrint(void *pvParameters) {

    paquete_t paquete;

    while (1) {

        if (xQueueReceive(colaPrint, &paquete, portMAX_DELAY)) {

            PRINT_datos(paquete.datos, N, paquete.salida);
            escribirLED(PIN_LED_SALIDA, paquete.salida);
        }
    }
}

/* ******** TAREA-CONTROL ******** */

// TAREA HMI
void taskControl(void *pvParameters) {

    while (1) {

        revisarBoton();
        escribirLED(PIN_LED_ESTADO, modo);

        // Activar menu con tecla 'm'
        if (Serial.available()) {
            char c = Serial.read();

            if (c == 'm' || c == 'M') {
                
                modo = 0;
                PRINT_mensaje("Sistema en pausa (Menu)");
                imprimirEcuacion();
                limpiarSerial();
                menu();
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

/* ******** SETUP ******** */
void setup() {

    // Inicialización de módulos
    PRINT_init();
    GPIO_init();
    ADC_init();

    mutexSerial = xSemaphoreCreateMutex();

    pedirN();
    initPesos();

    menu();

    colaADC = xQueueCreate(5, sizeof(int) * 5);
    colaPrint = xQueueCreate(5, sizeof(paquete_t));

    // Creación de tareas RTOS
    xTaskCreate(taskControl, "Control", 2048, NULL, 1, &taskControlHandle);
    xTaskCreate(taskADC, "ADC", 2048, NULL, 1, &taskADCHandle);
    xTaskCreate(taskML, "ML", 2048, NULL, 1, &taskMLHandle);
    xTaskCreate(taskPrint, "PRINT", 2048, NULL, 1, &taskPrintHandle);
}

void loop() {
  // No se requiere, emplea RTOS (Mediante FreeRTOS) 
  // realiza la gestión de todo el sistema a traves de tareas, colas y un mutex
}