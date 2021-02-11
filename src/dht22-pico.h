/**
 * DHT22-Pico v0.2.0 by Eleanor McMurtry (2021)
 * Loosely based on code by Caroline Dunn: https://github.com/carolinedunn/pico-weather-station
 **/
#ifndef DHT22_PICO_H
#define DHT22_PICO_H

#include "pico/stdlib.h"

#define DHT_OK              0
#define DHT_ERR_CHECKSUM    1
#define DHT_ERR_NAN         2

typedef struct {
    uint pin;
    float humidity;
    float temp_celsius;
} dht_reading;

dht_reading dht_init(uint8_t pin);

uint8_t dht_read(dht_reading *dht);

#endif