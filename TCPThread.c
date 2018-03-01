#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_dac.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_gpio.h"

#include <cmsis_os.h>                                           // CMSIS RTOS header file
#include <string.h>
#include "rl_net.h"
#include "FB3def.h"
#include "DACFIFO.h"
#include "DACModule.h"
#include "GPIOModule.h"

extern bool startLerp;
extern TIM_HandleTypeDef g_hTIMDAC;
int32_t		tcp_socket;
int32_t		tcp_socket_cmd;

bool startFirst = false;
volatile	bool				pck_head = true;
volatile	bool				pck_received = false;
volatile	uint16_t		pck_len = 0;
volatile	uint16_t		pck_size = 0;
volatile	uint16_t		pck_rest_size = 0;
eDACFIFO_t	fifo;

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
void Thread (void const *argument);                             // thread function
osThreadId tid_TCPThread;                                       // thread id
osThreadDef (Thread, osPriorityNormal, 1, 0);                   // thread object

int Init_TCPThread (void) {

  tid_TCPThread = osThreadCreate (osThread(Thread), NULL);
  if(!tid_TCPThread) return(-1);
  
  return(0);
}

bool sendbuffer(const void* data_buf, int len)
{
	uint8_t *sendbuf;
  uint16_t maxlen;

  if (tcp_check_send (tcp_socket)) 
	{
    /* The socket is ready to send the data. */
    maxlen = tcp_max_data_size (tcp_socket);
    if (len <= maxlen)
		{			
			sendbuf = tcp_get_buf (len);
			memcpy(sendbuf, data_buf, len);
			tcp_send (tcp_socket, sendbuf, len);
		} else
		  __breakpoint(0);
    return true;
  }
  return false;
}

uint32_t tcp_cmd_callback (int32_t socket, tcpEvent event, const uint8_t *buf, uint32_t len) {
	uint16_t gpio_state;
	uint8_t buffer[2]; 
  /* This function is called on TCP event */
  switch (event) {
    case tcpEventConnect:
      /* Remote host is trying to connect to our TCP socket. */
      /* 'buf' points to Remote IP, 'len' holds the remote port. */

      /* Return 1 to accept connection, or 0 to reject connection */
      return (1);
    case tcpEventAbort:
      /* Connection was aborted */

      break;
    case tcpEventEstablished:
      /* Socket is connected to remote peer. */

      break;
    case tcpEventClosed:
      /* Connection has been closed */

      break;
    case tcpEventACK:
      /* Our sent data has been acknowledged by remote peer */

      break;
    case tcpEventData:
      /* TCP data frame has been received, 'buf' points to data */
      /* Data length is 'len' bytes */
		
			buffer[0] = buf[1];
			buffer[1] = buf[0];
		
			memcpy(&gpio_state, buffer, sizeof(uint16_t));
		
			HAL_GPIO_WritePin(GPIOD,  gpio_state, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, ~gpio_state, GPIO_PIN_RESET);

      break;
  }
  return (0);
}

uint32_t tcp_callback (int32_t socket, tcpEvent event, const uint8_t *buf, uint32_t len) {
  /* This function is called on TCP event */
  switch (event) {
    case tcpEventConnect:
      /* Remote host is trying to connect to our TCP socket. */
      /* 'buf' points to Remote IP, 'len' holds the remote port. */
			startLerp = true;
			startFirst = true;
			pck_head = true;
			pck_received = false;
			pck_len = 0;
			pck_size = 0;
			pck_rest_size = 0;
      /* Return 1 to accept connection, or 0 to reject connection */
      return (1);
    case tcpEventAbort:
      /* Connection was aborted */
			HAL_TIM_Base_Stop_IT(&g_hTIMDAC);
			startLerp = false;
			startFirst = false;
      break;
    case tcpEventEstablished:
      /* Socket is connected to remote peer. */

      break;
    case tcpEventClosed:
      /* Connection has been closed */

      break;
    case tcpEventACK:
      /* Our sent data has been acknowledged by remote peer */

      break;
    case tcpEventData:
      /* TCP data frame has been received, 'buf' points to data */
      /* Data length is 'len' bytes */
		
			if (startFirst)
			{
				HAL_TIM_Base_Start_IT(&g_hTIMDAC);
				startFirst = false;
			}
		
			if (pck_head)
			{
				pck_size = *((uint16_t*)&buf[2]) * sizeof(eOutCoord_t);
				
				if (pck_size < len)
				{
					pck_head = true;
					pck_len = pck_size;
					pck_rest_size = 0;
					pck_received = true;
				}
				else
				{
					pck_head = false;
					pck_len = len - 4;
					pck_rest_size = pck_size - pck_len;
					pck_received = false;
				}
				
				while (!PutDataIntoDACFIFO(&fifo, (char*)&buf[4], pck_len))
					;//ProcessDACFIFO();
			}
			else
			{
				if (pck_rest_size < len)
					pck_len = pck_rest_size;
				else
					pck_len = len;
				while (!PutDataIntoDACFIFO(&fifo, (char*)&buf[0], pck_len))
					;//ProcessDACFIFO();
				pck_rest_size -= pck_len;
				
				if (pck_rest_size == 0)
				{
					pck_head = true;
					pck_received = true;
				}
				else
					pck_received = false;
			}
      break;
  }
  return (0);
}

void TCPLoop(void)
{
	uint16_t	ackWord = FB3TCP_PACKET_ACK;
	
	netStatus status = net_initialize();
	if (status != netOK) return;
	
	// Initialize DAC FIFO
	InitDACFIFO(&fifo, MAX_FIFO_SIZE);
	
	tcp_socket			= tcp_get_socket(TCP_TYPE_SERVER, 0, 30, tcp_callback);
	tcp_socket_cmd	= tcp_get_socket(TCP_TYPE_SERVER, 0, 30, tcp_cmd_callback);
	
	if (tcp_socket >= 0)			status = tcp_listen(tcp_socket, 		1234);
	if (status != netOK) return;
	if (tcp_socket_cmd >= 0)	status = tcp_listen(tcp_socket_cmd, 1235);
	if (status != netOK) return;
	
  while (1) {
    ; // Insert thread code here...
    net_main();
		
		// Process DAC module
		//ProcessDACFIFO();
		
		if (pck_received)
		{
			//ProcessDACFIFO();
			
			ackWord = FB3TCP_PACKET_ACK;
			if (sendbuffer(&ackWord, sizeof(ackWord)))
				pck_received = false;
		}
		
		//osThreadYield();                                            // suspend thread
  }
}

void Thread (void const *argument) {
	TCPLoop();
}


