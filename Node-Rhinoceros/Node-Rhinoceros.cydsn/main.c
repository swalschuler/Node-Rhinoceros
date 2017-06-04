/* Node-Rhinoceros
	 Charger board
 */

#include <project.h>
#include "data.h"
#include "can_manager.h"



int main(void) {
	CYGlobalIntEnable;      //Uncomment this line to enable global interrupts 

	uint8_t atomic_state = CyEnterCriticalSection(); // BEGIN ATOMIC
	CAN_1_GlobalIntEnable(); // CAN Initialization
	CAN_1_Init();
	CAN_1_Start();
	CyDelay(500);				//give some time to finish setup
	CyExitCriticalSection(atomic_state);               // END ATOMIC
	
	for(;;)	{
	    can_test_send();
		//can_test_receive();
		CyDelay(1000);
	} // main loop

	return 0;
} // main()

