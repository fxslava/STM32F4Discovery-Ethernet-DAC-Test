#include "PacketAssembler.h"
#include "DACFIFO.h"

void InitPacketAssembler(DACPacketAssembler *assembler)
{	
	assembler->pck_head = true;
	assembler->pck_received = false;
	assembler->pck_len = 0;
	assembler->pck_size = 0;
	assembler->pck_rest_size = 0;
	
	InitDACFIFO(&assembler->fifo, MAX_FIFO_SIZE);
}
