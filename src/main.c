#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "dht22-pico.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
const uint DHT_PIN = 15;

void blink() {
    sleep_ms(50);
    gpio_put(LED_PIN, 1);
    sleep_ms(50);
    gpio_put(LED_PIN, 0);
}

int main() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    dht_reading dht = dht_init(DHT_PIN);

    while (1) {
        sleep_ms(2000);

        uint8_t status = dht_read(&dht);
        if (status == DHT_OK) {
            printf("RH: %.1f%%\tTemp: %.1fC\n", dht.humidity, dht.temp_celsius);
            blink();
        } else if (status == DHT_ERR_CHECKSUM) {
            printf("Bad data (checksum)\n");
            blink();
            blink();
        } else {
            printf("Bad data (NaN)\n");
            blink();
            blink();
        }
    }
    return 0;
}
