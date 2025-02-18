#include <stdio.h>
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include <string.h>

// Posições iniciais do quadrado
int square_x = 0;
int square_y = 0;


// Definição dos pinos usados para o joystick e LEDs
const int VRX = 26;          // Pino de leitura do eixo X do joystick (conectado ao ADC)
const int VRY = 27;          // Pino de leitura do eixo Y do joystick (conectado ao ADC)
const int ADC_CHANNEL_0 = 0; // Canal ADC para o eixo X do joystick
const int ADC_CHANNEL_1 = 1; // Canal ADC para o eixo Y do joystick
const int SW = 22;           // Pino de leitura do botão do joystick

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

const int LED_B = 12;
const int LED_R = 13;
const int LED_G = 11;
const uint button_A = 5;

const float DIVIDER_PWM = 16.0; // Divisor fracional do clock para o PWM
const uint16_t PERIOD = 4096;   // Período do PWM (valor máximo do contador)

uint16_t led_b_level, led_r_level = 100; // Inicialização dos níveis de PWM para os LEDs
uint slice_led_b, slice_led_r;           // Variáveis para armazenar os slices de PWM correspondentes aos LEDs

static volatile int antb = 0;
static volatile int anta = 0;

bool state = false; // variavel para botao do joystick
bool state2 = true; // variavel para ativação do pwm
int draw_border = 0; // Flag para indicar que a borda deve ser desenhada

// Função para configurar o joystick (pinos de leitura e ADC), led verde e botao
void setup_joystick()
{
    // Inicializa o ADC e os pinos de entrada analógica
    adc_init();
    adc_gpio_init(VRX);
    adc_gpio_init(VRY);

    // Inicializa o pino do botão do joystick
    gpio_init(SW);
    gpio_set_dir(SW, GPIO_IN);
    gpio_pull_up(SW);

    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN);
    gpio_pull_up(button_A);

    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);

    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

// Função para configurar o PWM de um LED (genérica para azul e vermelho)
void setup_pwm_led(uint led, uint *slice, uint16_t level)
{
    gpio_set_function(led, GPIO_FUNC_PWM);
    *slice = pwm_gpio_to_slice_num(led);
    pwm_set_clkdiv(*slice, DIVIDER_PWM);
    pwm_set_wrap(*slice, PERIOD);
    pwm_set_gpio_level(led, level);
    pwm_set_enabled(*slice, 1);
}

// Função de configuração geral
void setup()
{
    stdio_init_all();
    setup_joystick();
    setup_pwm_led(LED_B, &slice_led_b, led_b_level);
    setup_pwm_led(LED_R, &slice_led_r, led_r_level);
}

// Função para ler os valores dos eixos do joystick (X e Y)
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value)
{
    // Leitura do valor do eixo X do joystick
    adc_select_input(ADC_CHANNEL_0);
    sleep_us(2);
    *vrx_value = adc_read();

    // Leitura do valor do eixo Y do joystick
    adc_select_input(ADC_CHANNEL_1);
    sleep_us(2);
    *vry_value = adc_read();
}

uint8_t ssd[ssd1306_buffer_length];

static int map_value_to_screen(uint coordx, uint coordy)
{
    // Mapeia o valor para o intervalo da tela
    square_y = 55 - (coordx * 55) / 4084; // Mapeia para 0-119 (largura do display)
    square_x = (coordy * 119) / 4084;     // Mapeia para 0-55 (altura do display)
}

void ssd1306_draw_border(uint8_t *ssd)
{
    // Linha superior (de (0, 0) a (127, 0))
    ssd1306_draw_line(ssd, 0, 0, 127, 0, true);

    // Linha inferior (de (0, 63) a (127, 63))
    ssd1306_draw_line(ssd, 0, 63, 127, 63, true);

    // Linha esquerda (de (0, 0) a (0, 63))
    ssd1306_draw_line(ssd, 0, 0, 0, 63, true);

    // Linha direita (de (127, 0) a (127, 63))
    ssd1306_draw_line(ssd, 127, 0, 127, 63, true);
}
void ssd1306_draw_double_border(uint8_t *ssd)
{
    // Desenha a primeira borda (externa)
    // Linha superior (de (0, 0) a (127, 0))
    ssd1306_draw_line(ssd, 0, 0, 127, 0, true);

    // Linha inferior (de (0, 63) a (127, 63))
    ssd1306_draw_line(ssd, 0, 63, 127, 63, true);

    // Linha esquerda (de (0, 0) a (0, 63))
    ssd1306_draw_line(ssd, 0, 0, 0, 63, true);

    // Linha direita (de (127, 0) a (127, 63))
    ssd1306_draw_line(ssd, 127, 0, 127, 63, true);

    // Desenha a segunda borda (interna)
    // Linha superior interna (de (1, 1) a (126, 1))
    ssd1306_draw_line(ssd, 1, 1, 126, 1, true);

    // Linha inferior interna (de (1, 62) a (126, 62))
    ssd1306_draw_line(ssd, 1, 62, 126, 62, true);

    // Linha esquerda interna (de (1, 1) a (1, 62))
    ssd1306_draw_line(ssd, 1, 1, 1, 62, true);

    // Linha direita interna (de (126, 1) a (126, 62))
    ssd1306_draw_line(ssd, 126, 1, 126, 62, true);
}
// Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

void gpio_irq_handler1(uint gpio, uint32_t events)
{
    uint32_t current = to_ms_since_boot(get_absolute_time());

    if (gpio == SW && current - anta > 200)
    {
        anta = current;
        state = !state;
        draw_border = (draw_border+1)%3; // Ativa a flag para desenhar a borda
    }
    else if (gpio == button_A && current - antb > 200)
    {
        antb = current;
        state2 = !state2;
        
        pwm_set_enabled(slice_led_b, state2);
        pwm_set_enabled(slice_led_r, state2);
    }
}

uint16_t vrx_value, vry_value; // Variáveis para armazenar os valores do joystick (eixos X e Y)
int main()
{
    setup(); // Chama a função de configuração

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    calculate_render_area_buffer_length(&frame_area);

    // Zera o display inteiro
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    gpio_set_irq_enabled_with_callback(SW, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler1);
    gpio_set_irq_enabled_with_callback(button_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler1);

    while (true)
    {
        memset(ssd, 0, ssd1306_buffer_length); // Limpa o buffer para o próximo desenho

        joystick_read_axis(&vrx_value, &vry_value); // Lê os valores dos eixos do joystick

        // Calcula a intensidade dos LEDs com base na distância da posição central (2048)
        int intensity_x = abs(vrx_value - 2048);
        int intensity_y = abs(vry_value - 2048);

        // Mapeia a intensidade para o intervalo de PWM (0 a 4095)
        uint16_t led_r_intensity = (intensity_x * 4095) / 2048;
        uint16_t led_b_intensity = (intensity_y * 4095) / 2048;

        // Desliga os LEDs se o joystick estiver na posição central
        if (intensity_x < 100)
        { // Margem de tolerância para o centro
            led_r_intensity = 0;
        }
        if (intensity_y < 100)
        {
            led_b_intensity = 0;
        }

        // Verifica se o joystick está na posição central E o botão foi pressionado
        if (intensity_x < 100 && intensity_y < 100)
        {
            gpio_put(LED_G, state); // Atualiza o LED verde com base no estado do botão
        }
        else
        {
            gpio_put(LED_G, 0); // Desliga o LED verde se o joystick não estiver no centro
        }

        pwm_set_gpio_level(LED_R, led_r_intensity);
        pwm_set_gpio_level(LED_B, led_b_intensity);

        // Mapeia os valores do joystick para as posições da tela
        map_value_to_screen(vrx_value, vry_value);

        // Desenha o quadrado na nova posição
        ssd1306_draw_square(ssd, square_x, square_y, true);

        // Verifica se a borda deve ser desenhada
        if (draw_border){
            if(draw_border == 1)
                ssd1306_draw_border(ssd);
            if(draw_border==2)
                ssd1306_draw_double_border(ssd);
        }
        // Atualiza o display com o conteúdo do buffer
        render_on_display(ssd, &frame_area);

        sleep_ms(100);
    }
}