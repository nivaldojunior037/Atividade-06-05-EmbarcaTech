#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "lib/matrizleds.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "AtvSemaforo.pio.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include <stdio.h>

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define BUTTON_A 5
#define ledG 11
#define ledR 13
#define BUZZER 21

static volatile bool modo = 0;

// Vetores usados para criar os desenhos na matriz de LEDs
double faixa1[NUM_PIXELS] = {0.0, 0.5, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.5, 0.0,
    0.0, 0.5, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.5, 0.0,
    0.0, 0.5, 0.0, 0.0, 0.0}; 

double faixa2[NUM_PIXELS] = {0.0, 0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0, 0.0};

double faixa3[NUM_PIXELS] = {0.0, 0.0, 0.0, 0.5, 0.0,
    0.0, 0.5, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.5, 0.0,
    0.0, 0.5, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.5, 0.0}; 


void tocarbuzzer(int duracao, int frequencia){
    for (int i = 0; i < duracao * 1000; i += (1000000 / frequencia) / 2)
        {
            gpio_put(BUZZER, 1);
            sleep_us((1000000 / frequencia) / 4);
            gpio_put(BUZZER, 0);
            sleep_us((1000000 / frequencia) / 4);
        }
    }

void vBlinkLEDSTask(){
    
    gpio_init(ledG);
    gpio_set_dir(ledG, GPIO_OUT);
    gpio_init(ledR);
    gpio_set_dir(ledR, GPIO_OUT);

    while (true){
        if(!modo){
            gpio_put(ledG, true);
            gpio_put(ledR, false);
            vTaskDelay(pdMS_TO_TICKS(6000));
            gpio_put(ledG, true);
            gpio_put(ledR, true);
            vTaskDelay(pdMS_TO_TICKS(6000));
            gpio_put(ledG, false);
            gpio_put(ledR, true);
            vTaskDelay(pdMS_TO_TICKS(6000));
        } else if(modo){
            gpio_put(ledG, true);
            gpio_put(ledR, true);
            vTaskDelay(pdMS_TO_TICKS(1000));
            gpio_put(ledG, false);
            gpio_put(ledR, false);
            vTaskDelay(pdMS_TO_TICKS(1000));
        } 
    }
}

void vMatrizTask(){

    static PIO pio = pio0;
    static uint offset;
    static uint sm;
    static uint32_t valor_led;

    // Determinação de variáveis usadas pela matriz de LEDs
    offset = pio_add_program(pio, &leds_matrix_program);
    sm = pio_claim_unused_sm(pio, true);
    leds_matrix_program_init(pio, sm, offset, LEDS_PIN);

    while (true){
        if(!modo){
            desenhos(faixa1, valor_led, pio, sm, 0.0, 0.0, 0.5);
            vTaskDelay(pdMS_TO_TICKS(6000));
            desenhos(faixa2, valor_led, pio, sm, 0.0, 0.5, 0.5);
            vTaskDelay(pdMS_TO_TICKS(6000));
            desenhos(faixa3, valor_led, pio, sm, 0.0, 0.5, 0.0);
            vTaskDelay(pdMS_TO_TICKS(6000));
        } else if(modo){
            for(int l=0; l<3; l++){
                desenhos(faixa2, valor_led, pio, sm, 0.0, 0.5, 0.5);
                vTaskDelay(pdMS_TO_TICKS(1000));
                desenhos(faixa2, valor_led, pio, sm, 0.0, 0.0, 0.0);
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
    }    
}

void vDisplayTask(){

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display
    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    bool cor = true;

    while (true){
        if(!modo){
            ssd1306_fill(&ssd, !cor);                          // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);      // Desenha um retângulo
            ssd1306_draw_string(&ssd, "Semaforo", 32, 6); // Desenha uma string
            ssd1306_draw_string(&ssd, "Verde", 44, 21);  // Desenha uma string
            ssd1306_draw_string(&ssd, "Ande", 48, 43); // Desenha uma string
            ssd1306_send_data(&ssd);                           // Atualiza o display
            vTaskDelay(pdMS_TO_TICKS(6000));

            ssd1306_fill(&ssd, !cor);                          // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);      // Desenha um retângulo
            ssd1306_draw_string(&ssd, "Semaforo", 32, 6); // Desenha uma string
            ssd1306_draw_string(&ssd, "Amarelo", 36, 21);  // Desenha uma string
            ssd1306_draw_string(&ssd, "Atencao", 36, 43); // Desenha uma string
            ssd1306_send_data(&ssd);                           // Atualiza o display
            vTaskDelay(pdMS_TO_TICKS(6000));

            ssd1306_fill(&ssd, !cor);                          // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);      // Desenha um retângulo
            ssd1306_draw_string(&ssd, "Semaforo", 32, 6); // Desenha uma string
            ssd1306_draw_string(&ssd, "Vermelho", 32, 21);  // Desenha uma string
            ssd1306_draw_string(&ssd, "Pare", 48, 43); // Desenha uma string
            ssd1306_send_data(&ssd);                           // Atualiza o display
            vTaskDelay(pdMS_TO_TICKS(6000));

        } else if(modo){
            ssd1306_fill(&ssd, !cor);                          // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);      // Desenha um retângulo
            ssd1306_draw_string(&ssd, "Modo Noturno", 16, 6); // Desenha uma string
            ssd1306_draw_string(&ssd, "Ande com", 32, 25);  // Desenha uma string
            ssd1306_draw_string(&ssd, "cuidado", 36, 47); // Desenha uma string
            ssd1306_send_data(&ssd);                           // Atualiza o display
            vTaskDelay(pdMS_TO_TICKS(6000));
        }
    }
}

void vBuzzerTask(){

    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);
    gpio_put(BUZZER, 0);

    while(true){
        if(!modo){
            for(int i=0; i<5; i++){
                tocarbuzzer(1000, 500);
                vTaskDelay(pdMS_TO_TICKS(200));
            }
            for(int j=0; j<12; j++){
                tocarbuzzer(300, 500);
                vTaskDelay(pdMS_TO_TICKS(200));
            }
            vTaskDelay(pdMS_TO_TICKS(500));
            for(int k=0; k<3; k++){
                tocarbuzzer(500, 500);
                vTaskDelay(pdMS_TO_TICKS(1500));
            }
        } else if(modo){
            tocarbuzzer(1500, 500);
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }

}

void vButtonTask(){

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    bool primeiro_estado = true;
    while(true){
        bool estado_atual = gpio_get(BUTTON_A);
        if(primeiro_estado && !estado_atual){
            modo = !modo;
            vTaskDelay(pdMS_TO_TICKS(150));
        }
        primeiro_estado = estado_atual;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(vBlinkLEDSTask, "Blink LEDs Task", configMINIMAL_STACK_SIZE,
         NULL, 1, NULL);
    xTaskCreate(vMatrizTask, "Matrix Task", configMINIMAL_STACK_SIZE, 
        NULL, 4, NULL);
    xTaskCreate(vDisplayTask, "Display Task", configMINIMAL_STACK_SIZE, 
        NULL, 2, NULL);
    xTaskCreate(vBuzzerTask, "Buzzer Task", configMINIMAL_STACK_SIZE, 
        NULL, 3, NULL);
    xTaskCreate(vButtonTask, "Button Task", configMINIMAL_STACK_SIZE, 
        NULL, 5, NULL);
    vTaskStartScheduler();
    panic_unsupported();
}
