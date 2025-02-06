#include <stdio.h>
#include "pico/stdlib.h"

#define PIN_BUTTON_A 5 
#define PIN_LED_GREEN 11
#define PIN_LED_BLUE 12
#define PIN_LED_RED 13
#define TIMER_INTERVAL_ALARM 3000 // Intervalo usado para o alarm
#define TIMER_SLEEP 1000 // Intervalo usado no loop principal

const uint64_t DEBOUNCE_TIME = 500000;     // Tempo de debounce em microssegundos
volatile uint64_t last_interrupt_time = 0; // Tempo da última interrupção do botão A
volatile bool status_led = false; // Variável para controlar o estado do LED

int64_t turn_off_red_led(alarm_id_t id, void *user_data)
{
    gpio_put(PIN_LED_RED, false);    
    status_led = false; // Atualiza o estado do LED para que o botão A possa iniciar o ciclo novamente
    
    return 0;
}

int64_t turn_off_blue_led(alarm_id_t id, void *user_data)
{
    gpio_put(PIN_LED_BLUE, false);
    
    // Adiciona um alarme para desligar o LED verde após 3 segundos
    add_alarm_in_ms(TIMER_INTERVAL_ALARM, turn_off_red_led, NULL, false);
    
    return 0;
}

int64_t turn_off_green_led(alarm_id_t id, void *user_data)
{
    gpio_put(PIN_LED_GREEN, false);

    // Adiciona um alarme para desligar o LED azul após 3 segundos
    add_alarm_in_ms(TIMER_INTERVAL_ALARM, turn_off_blue_led, NULL, false);

    return 0;
}

void button_isr(uint gpio, uint32_t events)
{

    if (!status_led)
    {
        uint64_t current_time = time_us_64(); // Obtém o tempo atual em microssegundos

        // configuração do debounce
        if (current_time - last_interrupt_time < DEBOUNCE_TIME) return; // Ignora bouncing caso o tempo entre interrupções seja menor que 500ms
        last_interrupt_time = current_time;

        // Liga os LEDs vermelho, verde e azul
        gpio_put(PIN_LED_RED, true);
        gpio_put(PIN_LED_GREEN, true);
        gpio_put(PIN_LED_BLUE, true);
        status_led = true;

        // Adiciona um alarme para desligar o LED vermelho após 3 segundos
        add_alarm_in_ms(TIMER_INTERVAL_ALARM, turn_off_green_led, NULL, false);
    }
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

    // Configuração do botão A
    gpio_init(PIN_BUTTON_A);
    gpio_set_dir(PIN_BUTTON_A, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_A);

    // Configuração da interrupção do botão A
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_isr);

    // mantem o loop funcionando sem ser interrompido pelo temporizador
    while (true)
    {
        // Imprime o tempo passado desde o início do programa
        int current_time = time_us_32() / 1000000;
        printf("Tempo passado: %i\n", current_time);
        sleep_ms(TIMER_SLEEP);
    }
}