#include <stdio.h>
#include "pico/stdlib.h"

#define PIN_LED_RED 11
#define PIN_LED_GREEN 12
#define PIN_LED_BLUE 13
#define TIMER_INTERVAL_TEMP 3000
#define TIMER_SLEEP 1000

volatile uint8_t traffic_light_on = 0;

// Função de callback para o timer
// Essa função alterna entre os estados do semáforo de acordo com traffic_light_on
bool repeating_timer_callback(struct repeating_timer *t)
{
    gpio_put(PIN_LED_RED, false);
    gpio_put(PIN_LED_GREEN, false);
    gpio_put(PIN_LED_BLUE, false);

    switch (traffic_light_on)
    {
    case 0:
        gpio_put(PIN_LED_RED, true);
        printf("🔴 PARE \n");
        break;
    case 1:
        gpio_put(PIN_LED_GREEN, true);
        printf("🟢 ATENÇÂO \n");
        break;
    case 2:
        gpio_put(PIN_LED_BLUE, true);
        printf("🔵 CONTINUE \n");
        break;
    }

    traffic_light_on += 1;

    if (traffic_light_on >= 3)
    {
        traffic_light_on = 0;
    }

    return true;
}

int main()
{
    stdio_init_all();

    // Inicialização do pino vermelho
    gpio_init(PIN_LED_RED);
    gpio_set_dir(PIN_LED_RED, GPIO_OUT);

    // Inicialização do pino verde
    gpio_init(PIN_LED_GREEN);
    gpio_set_dir(PIN_LED_GREEN, GPIO_OUT);

    // Inicialização do pino azul
    gpio_init(PIN_LED_BLUE);
    gpio_set_dir(PIN_LED_BLUE, GPIO_OUT);

    // configuração do temporizador para chamar a função repetidamente a cada 3 segundos
    struct repeating_timer timer;
    add_repeating_timer_ms(TIMER_INTERVAL_TEMP, repeating_timer_callback, NULL, &timer);

    // mantem o loop funcionando sem ser interrompido pelo temporizador
    while (true)
    {
        // Imprime o tempo passado desde o início do programa
        int current_time = time_us_32() / 1000000;
        printf("Tempo passado: %i\n", current_time);
        sleep_ms(TIMER_SLEEP);
    }
}