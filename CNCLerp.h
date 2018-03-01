#ifndef CNCLERP_HH__
#define CNCLERP_HH__

#include <cmsis_os.h>
#include <stdint.h>
#include <stdbool.h>

#define NUM_AXISES 3

typedef struct CNCLerp_t {
	uint16_t	counter;
	uint16_t	period;
	uint16_t	AxisMask [NUM_AXISES];
} eCNCLerp_t;

void UpdateCNCLerp(eCNCLerp_t* lerp, uint16_t period, bool *update);
uint32_t brev(uint32_t value, uint8_t digits);

#endif
