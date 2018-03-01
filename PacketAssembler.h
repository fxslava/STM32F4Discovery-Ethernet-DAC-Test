#ifndef PACKETASSEMBLER_HH__
#define PACKETASSEMBLER_HH__

#include <stdint.h>
#include <stdbool.h>
#include "DACFIFO.h"

typedef struct eDACPacketAssembler
{
	bool				pck_head;
	bool				pck_received;
	uint16_t		pck_len;
	uint16_t		pck_size;
	uint16_t		pck_rest_size;
	eDACFIFO_t	fifo;
} eDACPacketAssembler_t, *PDACPacketAssembler;

void InitPacketAssembler(eDACPacketAssembler_t *assembler);
void PacketAssemblerAddData(eDACPacketAssembler_t *assembler, const uint8_t *buf, uint32_t len);

#endif
