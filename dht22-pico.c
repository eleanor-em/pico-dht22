/**
 * Loosely based on code by Caroline Dunn: https://github.com/carolinedunn/pico-weather-station
 **/
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <string.h>

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
const uint DHT_PIN = 15;

typedef struct {
    float humidity;
    float temp_celsius;
} dht_reading;

uint32_t wait_for(uint8_t expect) {
    uint32_t then = time_us_32();
    while (expect != gpio_get(DHT_PIN)) {
        sleep_us(10);
    }
    return time_us_32() - then;
}

inline float word(uint8_t first, uint8_t second) {
    return (float) ((first << 8) + second);
}

inline void blink() {
    gpio_put(LED_PIN, 1);
    sleep_ms(50);
    gpio_put(LED_PIN, 0);
}

// helper function to read from the DHT
// more: https://cdn-shop.adafruit.com/datasheets/Digital+humidity+and+temperature+sensor+AM2302.pdf
uint8_t read_from_dht(dht_reading *result) {
    uint8_t data[5] = {0, 0, 0, 0, 0};

    // request a sample
    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
    sleep_ms(10);
    gpio_put(DHT_PIN, 1);
    sleep_us(40);
    
    // wait for acknowledgement
    gpio_set_dir(DHT_PIN, GPIO_IN);
    wait_for(0);
    wait_for(1);
    wait_for(0);

    // read sample
    for (uint8_t bit = 0; bit < 40; ++bit) {
        wait_for(1);
        uint8_t count = wait_for(0);
        data[bit / 8] <<= 1;
        if (count > 50) {
            data[bit / 8] |= 1;
        }
    }

    // pull back up to mark end of read
    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 1);

    // checksum
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        float humidity = word(data[0], data[1]) / 10;
        float temp = word(data[2] & 0x7F, data[3]) / 10;

        // if the highest bit is 1, temperature is negative
        if (data[2] & 0x80) {
            temp = -temp;
        }

        // check if checksum was OK but something else went wrong
        if (isnan(result->temp_celsius) || isnan(result->humidity)
            || result->temp_celsius == 0) {
            return 2;
        } else {
            result->humidity = humidity;
            result->temp_celsius = temp;
            return 0;
        }
    } else {
        return 1;
    }
}

int main() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_init(DHT_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1) {
        sleep_ms(2000);

        dht_reading reading;
        uint8_t status = read_from_dht(&reading);
        if (status == 0) {
            printf("RH: %.1f%%\tTemp: %.1fC\n", reading.humidity, reading.temp_celsius);
        } else if (status == 1) {
            printf("Bad data (checksum)\n");
        } else {
            printf("Bad data (NaN)\n");
        }
    }
    return 0;
}
