/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Config.c
 * Purpose: Network Configuration
 * Rev.:    V5.00
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>System Definitions
// <i>Global Network System definitions
//   <s.15>Local Host Name
//   <i>This is the name under which embedded host can be
//   <i>accessed on a local area network.
//   <i>Default: "my_host"
#define NET_HOST_NAME           "my_host"

//   <o>Memory Pool size <1536-262144:4><#/4>
//   <i>This is the size of a memory pool in bytes. Buffers for
//   <i>Network packets are allocated from this memory pool.
//   <i>Default: 12000 bytes
#define NET_MEM_SIZE            12000

// </h>

#include "rl_usb.h"
#include <string.h>
#include "..\RTE_Components.h"

#ifdef  RTE_Network_Interface_ETH_0
#include "Net_Config_ETH_0.h"
#endif
#ifdef  RTE_Network_Interface_ETH_1
#include "Net_Config_ETH_1.h"
#endif

#ifdef  RTE_Network_Interface_PPP
#include "Net_Config_PPP.h"
#endif

#ifdef  RTE_Network_Interface_SLIP
#include "Net_Config_SLIP.h"
#endif

#ifdef  RTE_Network_Socket_UDP
#include "Net_Config_UDP.h"
#endif
#ifdef  RTE_Network_Socket_TCP
#include "Net_Config_TCP.h"
#endif
#ifdef  RTE_Network_Socket_BSD
#include "Net_Config_BSD.h"
#endif

#ifdef  RTE_Network_Web_Server_RO
#include "Net_Config_HTTP_Server.h"
#endif
#ifdef  RTE_Network_Web_Server_FS
#include "Net_Config_HTTP_Server.h"
#endif

#ifdef  RTE_Network_Telnet_Server
#include "Net_Config_Telnet_Server.h"
#endif

#ifdef  RTE_Network_TFTP_Server
#include "Net_Config_TFTP_Server.h"
#endif
#ifdef  RTE_Network_TFTP_Client
#include "Net_Config_TFTP_Client.h"
#endif

#ifdef  RTE_Network_FTP_Server
#include "Net_Config_FTP_Server.h"
#endif
#ifdef  RTE_Network_FTP_Client
#include "Net_Config_FTP_Client.h"
#endif

#ifdef  RTE_Network_DNS_Client
#include "Net_Config_DNS_Client.h"
#endif

#ifdef  RTE_Network_SMTP_Client
#include "Net_Config_SMTP_Client.h"
#endif

#ifdef  RTE_Network_SNMP_Agent
#include "Net_Config_SNMP_Agent.h"
#endif

#ifdef  RTE_Network_SNTP_Client
#include "Net_Config_SNTP_Client.h"
#endif

#include "net_config.h"

/**
\addtogroup net_genFunc
@{
*/
/**
  \fn          void net_sys_error (ERROR_CODE error)
  \ingroup     net_cores
  \brief       Network system error handler.
*/
void net_sys_error (ERROR_CODE error) {
  /* This function is called when a fatal error is encountered. */
  /* The normal program execution is not possible anymore.      */
	char* str0 = "ERR_MEM_ALLOC";
	char* str1 = "ERR_MEM_FREE";
	char* str2 = "ERR_MEM_CORRUPT";
	char* str3 = "ERR_MEM_LOCK";
	char* str4 = "ERR_UDP_ALLOC";
	char* str5 = "ERR_TCP_ALLOC";

  switch (error) {
    case ERR_MEM_ALLOC:
      /* Out of memory */
			
      break;

    case ERR_MEM_FREE:
      /* Trying to release non existing memory block */
			USBD_CDC_ACM_WriteData(0, (uint8_t*)str0, strlen(str0));
      break;

    case ERR_MEM_CORRUPT:
      /* Memory Link pointer Corrupted */
      /* More data written than the size of allocated mem block */
			USBD_CDC_ACM_WriteData(0, (uint8_t*)str1, strlen(str1));
      break;

    case ERR_MEM_LOCK:
      /* Locked Memory management function (alloc/free) re-entered */
			USBD_CDC_ACM_WriteData(0, (uint8_t*)str2, strlen(str2));
      break;

    case ERR_UDP_ALLOC:
      /* Out of UDP Sockets */
			USBD_CDC_ACM_WriteData(0, (uint8_t*)str3, strlen(str3));
      break;

    case ERR_TCP_ALLOC:
      /* Out of TCP Sockets */
			USBD_CDC_ACM_WriteData(0, (uint8_t*)str4, strlen(str4));
      break;

    case ERR_TCP_STATE:
      /* TCP State machine in undefined state */
			USBD_CDC_ACM_WriteData(0, (uint8_t*)str5, strlen(str5));
      break;
  }

  /* End-less loop */
  while (1);
}
/**
@}
*/
