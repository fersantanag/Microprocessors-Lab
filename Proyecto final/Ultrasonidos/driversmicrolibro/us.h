/**
 * @file us.h
 *
 * @author Jaime Boal Martín-Larrauri
 *
 * @version 1.0.0
 *
 * @date 17/11/2014
 *
 * @brief Interfaz del driver de sensores US.
 */
// ------------------------------------------------------
#ifndef _US_H
#define _US_H
// ------------------------------------------------------
/**
 * Configura los pines de un sensor de ultrasonidos.
 *
 * Cualquier pin del PORTB es válido. No verifica si
 * los pines están ya en uso. A lo largo del driver se
 * utiliza el pin de eco para identificar el sensor.
 *
 * @param pin_disparo Trigger (salida digital).
 * @param pin_eco Medida (entrada digital).
 */
void configurarPinesUS(unsigned char pin_disparo,
        unsigned char pin_eco);
// ------------------------------------------------------
/**
 * Función de configuración del driver de ultrasonidos.
 *
 * Debe llamarse después de haber configurado los pines.
 * Usa el Timer 3.
 */
void inicializarUS(void);
// ------------------------------------------------------
/**
 * Devuelve la última medida de un sensor US.
 *
 * Si aún no se ha realizado una medida o no se
 * ha detectado un obstáculo, devuelve 511.
 *
 * @param pin_eco Índice del pin del PORTB [0, 15].
 *
 * @return Tiempo de vuelo. Cada unidad equivale a 50 us.
 */
unsigned int getMedidaUS(unsigned char pin_eco);
#endif