#ifndef _PLAYER_H
#define _PLAYER_H

#define UART_ID uart0

static int curr_volume = 50; //in %

static bool is_playing = true; // playing flag

bool is_refresh_need = false; // refresh flag

char up[2] = {'v','+'}; //chars for volume up
char down[2] = {'v','-'}; //char for volume down

int change_volume(bool control) //changing player volume using pico webserver
{
    if(control)
    {
        if(curr_volume<100)
        {
            int8_t out = sizeof(up);
            for (size_t i = 0; i < out; i++)
            {
                uart_putc(UART_ID, up[i]);
            }
            curr_volume += 10;
        }
    }
    else
    {
        if(curr_volume>0)
        {
            int8_t out = sizeof(down);
            for (size_t i = 0; i < out; i++)
            {
                uart_putc(UART_ID, down[i]);
            }
            curr_volume -= 10;
        }   
    }
    is_refresh_need = true;
}
void play_pause() //changing player playback status using pico webserver
{
    uart_puts(UART_ID,"ss");

    if(is_playing)
        is_playing = false;
    else
        is_playing = true;
    is_refresh_need = true;
}


#endif