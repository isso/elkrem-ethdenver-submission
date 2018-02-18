#ifndef arduhdlc_h
#define arduhdlc_h

#include "Arduino.h"
#include <stdint.h>
#include <stdbool.h>

typedef void (* sendchar_type) (uint8_t);
typedef void (* frame_handler_type)(uint8_t *framebuffer, uint16_t framelength);

static inline uint16_t _crc_xmodem_update(uint16_t crc, uint8_t data) __attribute__((always_inline, unused));
static inline uint16_t _crc_xmodem_update(uint16_t crc, uint8_t data)
{
	unsigned int i;

	crc = crc ^ ((uint16_t)data << 8);
	for (i=0; i<8; i++) {
		if (crc & 0x8000) {
			crc = (crc << 1) ^ 0x1021;
		} else {
			crc <<= 1;
		}
	}
	return crc;
}

class Arduhdlc
{
  public:
    Arduhdlc (sendchar_type, frame_handler_type, uint16_t max_frame_length);
    void charReceiver(uint8_t data);
    void frameDecode(const uint8_t *framebuffer, uint8_t frame_length);

  private:
    /* User must define a function, that sends a 8bit char over the chosen interface, usart, spi, i2c etc. */
    sendchar_type sendchar_function;
    /* User must define a function, that will process the valid received frame */
    /* This function can act like a command router/dispatcher */
    frame_handler_type frame_handler;
    void sendchar(uint8_t data);

    bool escape_character;
    uint8_t * receive_frame_buffer;
    uint8_t frame_position;
    // 16bit CRC sum for _crc_ccitt_update
    uint16_t frame_checksum;
	uint16_t max_frame_length;
};

#endif
