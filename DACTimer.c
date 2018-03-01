
#include <cmsis_os.h>                                           // CMSIS RTOS header file
#include "DACModule.h"

/*----------------------------------------------------------------------------
 *      Timer: Sample timer functions
 *---------------------------------------------------------------------------*/
	

/*----- Periodic Timer Example -----*/
static void TimerDAC_Callback  (void const *arg);           // prototype for timer callback function

static osTimerId id;                                     // timer id
static uint32_t  exec;                                   // argument for the timer call back function
static osTimerDef (TimerDAC, TimerDAC_Callback);                                      
 
// Periodic Timer Example
static void TimerDAC_Callback  (void const *arg)  {
  // add user code here
	ProcessDACFIFO();
}	

	
// Example: Create and Start timers
void Init_Timers (void) {
  osStatus  status;                                        // function return status
 
  // Create periodic timer
  exec = 2;
  id = osTimerCreate (osTimer(TimerDAC), osTimerPeriodic, &exec);
  if (id != NULL)  {     // Periodic timer created
    // start timer with periodic 1000ms interval
		status = osTimerStart (id, 1);            
    if (status != osOK)  {
      // Timer could not be started
    }
  }
}

