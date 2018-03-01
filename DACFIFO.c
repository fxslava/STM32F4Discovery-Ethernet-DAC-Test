#include "DACFIFO.h"
#include <string.h>

char dacfifo [MAX_FIFO_SIZE];

void InitDACFIFO(eDACFIFO_t* fifo, uint16_t size)
{
	if (size <= MAX_FIFO_SIZE)
	{
		fifo->head = 0;
		fifo->tail = 0;
		fifo->size = size;
		fifo->mask = size-1;
		fifo->state = DACFIFO_EMPTY;
	}
}

uint16_t GetDACFIFOFreeSize(eDACFIFO_t* fifo)
{
	if ((fifo->state == DACFIFO_EMPTY) && (fifo->tail == fifo->head))
		return fifo->size;
	else
		return (fifo->tail - fifo->head) & fifo->mask;
}

uint16_t GetDACFIFOFillSize(eDACFIFO_t* fifo)
{
	if ((fifo->state == DACFIFO_FULL) && (fifo->tail == fifo->head))
		return fifo->size;
	else
		return (fifo->head - fifo->tail) & fifo->mask;
}

bool PutDataIntoDACFIFO(eDACFIFO_t* fifo, char* buffer, uint16_t size)
{
	if (size <= GetDACFIFOFreeSize(fifo))
	{
		if (fifo->head > fifo->tail)
		{
			uint16_t split_pos = fifo->size - fifo->head;
			if (size < split_pos)
			{
				memcpy(&dacfifo[fifo->head], (void*)&buffer[0], size);
				fifo->head += size;
			}
			else
			{
				memcpy(&dacfifo[fifo->head], (void*)&buffer[0], split_pos);
				memcpy(&dacfifo[0], (void*)&buffer[split_pos], size - split_pos);
				fifo->head = size - split_pos;
			}
		}
		else
		{
			memcpy(&dacfifo[fifo->head], (void*)&buffer[0], size);
			fifo->head += size;
		}
		if (fifo->head == fifo->tail) fifo->state = DACFIFO_FULL;
		return true;
	}
	else
		return false;
}

bool GetDataFromDACFIFO(eDACFIFO_t* fifo, char* buffer, uint16_t size)
{
	if (size <= GetDACFIFOFillSize(fifo))
	{
		if (fifo->tail > fifo->head)
		{
			uint16_t split_pos = fifo->size - fifo->tail;
			if (size < split_pos)
			{
				memcpy((void*)&buffer[0], &dacfifo[fifo->tail], size);
				fifo->tail += size;
			}
			else
			{
				memcpy((void*)&buffer[0], &dacfifo[fifo->tail], split_pos);
				memcpy((void*)&buffer[split_pos], &dacfifo[0], size - split_pos);
				fifo->tail = size - split_pos;
			}
		}
		else
		{
			memcpy((void*)&buffer[0], &dacfifo[fifo->tail], size);
			fifo->tail += size;
		}
		if (fifo->head == fifo->tail) fifo->state = DACFIFO_EMPTY;
		return true;
	}
	else
		return false;
}

bool GetDataFastFromDACFIFO(eDACFIFO_t* fifo, char* buffer, uint16_t size)
{
	if (size <= GetDACFIFOFillSize(fifo))
	{
		memcpy((void*)&buffer[0], &dacfifo[fifo->tail], size);
		fifo->tail += size;
		fifo->tail &= fifo->mask;
		if (fifo->head == fifo->tail) fifo->state = DACFIFO_EMPTY;
		return true;
	}
	else
		return false;
}
