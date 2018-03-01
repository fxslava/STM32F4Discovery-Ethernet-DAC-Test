#include "PacketAssembler.h"
#include "DACFIFO.h"

void InitPacketAssembler(eDACPacketAssembler_t *assembler)
{	
	assembler->pck_head = true;
	assembler->pck_received = false;
	assembler->pck_len = 0;
	assembler->pck_size = 0;
	assembler->pck_rest_size = 0;
	
	InitDACFIFO(&assembler->fifo, MAX_FIFO_SIZE);
}

void PacketAssemblerAddData(eDACPacketAssembler_t *assembler, const uint8_t *buf, uint32_t len)
{		
	if (assembler->pck_head)
	{
		assembler->pck_size = *((uint16_t*)&buf[2]) * sizeof(eOutCoord_t);
		
		if (assembler->pck_size < len)
		{
			assembler->pck_head = true;
			assembler->pck_len = assembler->pck_size;
			assembler->pck_rest_size = 0;
			assembler->pck_received = true;
		}
		else
		{
			assembler->pck_head = false;
			assembler->pck_len = len - 4;
			assembler->pck_rest_size = assembler->pck_size - assembler->pck_len;
			assembler->pck_received = false;
		}
		
		while (!PutDataIntoDACFIFO(&assembler->fifo, (char*)&buf[4], assembler->pck_len))
			;//ProcessDACFIFO();
	}
	else
	{
		if (assembler->pck_rest_size < len)
			assembler->pck_len = assembler->pck_rest_size;
		else
			assembler->pck_len = len;
		while (!PutDataIntoDACFIFO(&assembler->fifo, (char*)&buf[0], assembler->pck_len))
			;//ProcessDACFIFO();
		assembler->pck_rest_size -= assembler->pck_len;
		
		if (assembler->pck_rest_size == 0)
		{
			assembler->pck_head = true;
			assembler->pck_received = true;
		}
		else
			assembler->pck_received = false;
	}
}
