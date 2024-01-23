#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "player.h"

const char * cgi_volume_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if (strcmp(pcParam[0] , "volume") == 0){
        if(strcmp(pcValue[0], "1") == 0)
            change_volume(true);
            
        else if(strcmp(pcValue[0], "0") == 0)
            change_volume(false);
    }
    
    return "/index.shtml";
}
const char * cgi_play_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if (strcmp(pcParam[0] , "play") == 0){
        if(strcmp(pcValue[0], "1") == 0)
            play_pause();
        }

    return "/index.shtml";
}
static const tCGI cgi_handlers[] = {
    {
        "/volume.cgi", cgi_volume_handler
    },
    {
        "/play.cgi", cgi_play_handler

    },
};
void cgi_init(void)
{
    http_set_cgi_handlers(cgi_handlers, 2);
}
