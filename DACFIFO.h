#ifndef DACFIFO_HH__
#define DACFIFO_HH__

#include "FB3def.h"
#include <cmsis_os.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_FIFO_SIZE	32768

typedef enum DACFIFO_STATE
{
	DACFIFO_EMPTY,
	DACFIFO_FULL
} *PDACFIFO_STATE;

typedef struct DACFIFO
{
	uint16_t head;
	uint16_t tail;
	uint16_t size; // must by power of two
	uint16_t mask;
	enum DACFIFO_STATE state;
} eDACFIFO_t;

void InitDACFIFO(eDACFIFO_t* fifo, uint16_t size);
uint16_t GetDACFIFOFreeSize(eDACFIFO_t* fifo);
uint16_t GetDACFIFOFillSize(eDACFIFO_t* fifo);

bool PutDataIntoDACFIFO(eDACFIFO_t* fifo, char* buffer, uint16_t size);
bool GetDataFromDACFIFO(eDACFIFO_t* fifo, char* buffer, uint16_t size);
bool GetDataFastFromDACFIFO(eDACFIFO_t* fifo, char* buffer, uint16_t size);

#endif
