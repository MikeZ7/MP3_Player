#ifndef F45D77E7_79CA_4FD5_BB72_873F09E6C397
#define F45D77E7_79CA_4FD5_BB72_873F09E6C397

#include "i2c.h"
#include "i2s.h"

void cs43l22_write(uint8_t reg, uint8_t value);
void cs43l22_init(void);
void cs43l22_play_buffer();

#endif /* F45D77E7_79CA_4FD5_BB72_873F09E6C397 */
