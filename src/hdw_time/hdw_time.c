#include <stdlib.h>
#include <pic32m_builtins.h>

#include "hdw_time/hdw_time.h"

static uint32_t milliseconds;

void hdw_1ms_isr(void) {
	// isr context
	milliseconds++;
}

uint32_t hdw_time(void) {
	__builtin_disable_interrupts();
	
	uint32_t time = milliseconds;
	
	__builtin_enable_interrupts();
	
	return time;
}