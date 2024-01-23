#include "lwip/apps/httpd.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/binary_info.h"
#include "lwipopts.h"
#include <stdio.h>
#include "ssi.h"
#include "cgi.h"
#include "ssd1306.h"
#include "font.h"

//UART settings
#define UART_ID uart0
#define BAUD_RATE 9600

#define UART_TX_PIN 0
#define UART_RX_PIN 1

const char WIFI_SSID[] = "iPhone (Jakub_K)"; 
const char WIFI_PASSWORD[] = "12345678";

const char levels[11] = {'0','1','2','3','4','5','6','7','8','9'};

u_int8_t whic = 0;

//using for refresing screen without space where title is
void refresh_screen()
{
    ssd1306_Without_Title();
    ssd1306_Print_Word(0,5,"VOLUME:",font_8x5);
    ssd1306_Print_Bar_Hor(20,curr_volume);
    if (is_playing)
        ssd1306_Print_Word(0,40,"PLAY",font_8x5);
    else
        ssd1306_Print_Word(0,40,"PAUSE",font_8x5);
    ssd1306_Show();
}
//UART handler
void on_uart_rx() {
      
        while (uart_is_readable(UART_ID))
            {
                char ch = uart_getc(UART_ID);
                if(ch == '?') //changing volume
                {
                    char d = uart_getc(UART_ID);
                    char j = uart_getc(UART_ID);
                    curr_volume = 0;
                    for(u_int8_t swap = 0; swap < sizeof(levels);swap++)
                    {
                        if(d == levels[swap])
                            curr_volume = 10*swap;
                    }
                    for(u_int8_t swap = 0; swap < sizeof(levels);swap++)
                    {
                        if(j == levels[swap])
                            curr_volume += 1*swap;
                    }
                    refresh_screen();
                    break;
                }
                if (ch == '!')
                {
                    whic = 0;
                    break;
                } //getting new title
                else
                {
                    if(whic == 0)
                    {
                        ssd1306_Clear_Title();
                    }
                    ssd1306_Print_Char(6*whic,60,ch,font_8x5);
                    whic++;
                    ssd1306_Show();
                }    
            }  
}
//initlializing UART and UART irq
void enable_uart()
{
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    uart_set_irq_enables(UART_ID, true, false);

}
//initializing i2c
void enable_i2c(void)
{
    i2c_init(i2c1, 400000);

    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    
    gpio_pull_up(2);
    gpio_pull_up(3);
}
//simple led blink
static void blinkLED(){
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(100);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}
int main() {
    stdio_init_all(); //initializing pico stdio

    cyw43_arch_init(); //initializing pico wifi 

    cyw43_arch_enable_sta_mode();

    while(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0){
        printf("Attempting to connect...\n");
    }
    httpd_init();

    ssi_init(); 

    cgi_init();

    enable_uart();
    enable_i2c();

    ssd1306_Init(); //oled initialize
    ssd1306_Clear(); //oled clear 
    ssd1306_Show();
    
    refresh_screen();

    //blink loop
    while(1)
    {
        sleep_ms(500);
        blinkLED();
        if(is_refresh_need) //using when controlled by webserver
        {
            is_refresh_need = false;
            refresh_screen();
        }
    };
}