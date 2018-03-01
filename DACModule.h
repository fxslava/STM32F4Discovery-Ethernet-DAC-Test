#ifndef DACMODULE_HH__
#define DACMODULE_HH__

#include "FB3def.h"
#include <cmsis_os.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct FB3Lerp_t {
	uint32_t x1;
	uint32_t y1;
	uint32_t x2;
	uint32_t y2;
	uint32_t dx;
	uint32_t dy;
	uint16_t period_exp;
	uint8_t dirx;
	uint8_t diry;
} eFB3Lerp_t;

typedef struct FB3FastLerp_t {
	uint16_t x1;
	uint16_t y1;
	uint16_t x2;
	uint16_t y2;
	uint16_t dx;
	uint16_t dy;
	uint16_t period_exp;
	uint8_t dirx;
	uint8_t diry;
} eFB3FastLerp_t;

typedef struct Vector2 {
	uint16_t x;
	uint16_t y;
} eVector2;

void InitDAC(void);
void DeInitDAC(void);
void ProcessDACFIFO(void);

void InitLerp(eFB3Lerp_t* lerp, uint16_t period_exp, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
eVector2 CalcLerp(eFB3Lerp_t* lerp, bool *update);
void UpdateLerp(eFB3Lerp_t* lerp, int16_t x, int16_t y);
#endif
