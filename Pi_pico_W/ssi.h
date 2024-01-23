#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "player.h"

// SSI tags - tag length limited to 8 bytes by default
const char * ssi_tags[] = {"volume","playpaus","temp"};

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) {
  size_t printed;
  switch (iIndex) {
  case 0: // volume
    {
      printed = snprintf(pcInsert, iInsertLen, "%d", curr_volume);
    }
    break;
  case 1: // play_pause
    {
      if(is_playing == true){
        printed = snprintf(pcInsert, iInsertLen, "PLAY");
      }
      else{
        printed = snprintf(pcInsert, iInsertLen, "PAUSE");
      }
    }
    break;
    case 2: //temp
    {
      const float voltage = adc_read() * 3.3f / (1 << 12);
      const float tempC = 27.0f - (voltage - 0.706f) / 0.001721f;
      printed = snprintf(pcInsert, iInsertLen, "%2.2f", tempC);
    }
    break;
  default:
    printed = 0;
    break;
  }

  return (u16_t)printed;
}

void ssi_init() {

  adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);

  http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
}
