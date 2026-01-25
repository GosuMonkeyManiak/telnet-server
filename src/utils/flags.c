#include "definitions.h"

#include "utils/flags.h"

void set_flag(uint16_t *flags, uint16_t flag) {
	__builtin_disable_interrupts();
	
	*flags = (*flags) & (~(1 << flag));
	*flags = (*flags) | (1 << flag);
	
	__builtin_enable_interrupts();
}

void reset_flag(uint16_t *flags, uint16_t flag) {
	__builtin_disable_interrupts();
	
	(*flags) = (*flags) & (~(1 << flag));
	
	__builtin_enable_interrupts();
}

uint8_t get_flag(uint16_t *flags, uint16_t flag) {
	__builtin_disable_interrupts();
	
	uint8_t flag_value = ((*flags) & (1 << flag)) > 0 ? 1 : 0;
	
	__builtin_enable_interrupts();
	
	return flag_value;
}