#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <cstdio>
#include "ARK/Ark.hpp"

int main() {
    stdio_init_all(); // Pico-specific peripheral init
    
    // Blink onboard LED to confirm firmware is running
    const uint LED_PIN = 25;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    // Blink 3 times to show we're alive
    for (int i = 0; i < 3; i++) {
        gpio_put(LED_PIN, 1);
        sleep_ms(200);
        gpio_put(LED_PIN, 0);
        sleep_ms(200);
    }
    
    // Wait for USB CDC to enumerate
    sleep_ms(2000);
    
    printf("ARK Firmware Started!\n");
    
    Setup();
    return 0;
}