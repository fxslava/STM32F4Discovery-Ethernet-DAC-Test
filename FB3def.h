#ifndef __FB3DEF_HH
#define __FB3DEF_HH

#include "stm32f4xx.h"
#include <stdint.h>

#define FB3TCP_PACKET_ACK		      0x1111
#define FB3TCP_BUFFER_EMPTY			  0x2222
#define FB3TCP_BUFFER_EMPTY_STOP	0x3333
#define FB3TCP_BUFFER_HALF_FULL		0xCCCC
#define FB3TCP_BUFFER_FULL			  0xFFFF
#define FB3TCP_CRC_ERROR			    0xFCFC

// Painter protocol
typedef struct
{
	int16_t x;				// -32768 ... +32767
	int16_t y;				// -32768 ... +32767
	uint16_t cur;			// current
	uint16_t focus;		// focus
	uint16_t feed;		// feed
	uint16_t on_off;	// switch
}	eOutCoord_t;

// Kvant protocol
typedef struct
{
	uint16_t X;
	uint16_t Y;
	uint16_t Z;
}	eAnalogAxises_t;

typedef struct
{
	uint16_t maskX;
	uint16_t maskY;
	uint16_t maskZ;
} eMechanAxises_t;

typedef struct
{
	uint16_t flag;
	union
	{
		eAnalogAxises_t analog;
		eMechanAxises_t axises;
	} axisUnion;
} eKvantCoord_t;

#endif
