#pragma once

#include <stdint.h>

extern void set_flag(uint16_t *flags, uint16_t flag);

extern void reset_flag(uint16_t *flags, uint16_t flag);

extern uint8_t get_flag(uint16_t *flags, uint16_t flag);

extern void set_flag_without_crs(uint16_t *flags, uint16_t flag);

extern void reset_flag_without_crs(uint16_t *flags, uint16_t flag);

extern uint8_t get_flag_without_crs(uint16_t *flags, uint16_t flag);