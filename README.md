# Perceptrón Simple en ESP32

Proyecto de implementación de un perceptrón simple usando Arduino IDE y FreeRTOS.

## Características

- Entrenamiento con tabla de verdad
- Soporte para N entradas (2 a 5)
- Persistencia en EEPROM
- Multitarea con FreeRTOS
- Interfaz por consola

## Uso

1. Definir N
2. Entrenar o cargar pesos
3. Ejecutar sistema en tiempo real

## Limitaciones

- Solo resuelve problemas linealmente separables (No es posible la función lógica XOR)