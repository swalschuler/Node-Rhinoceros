/* Node-Rhinoceros
	 Charger board FRUCD
	Simply prints BMS statuses to LCD
 */

#include <project.h>
#include "data.h"
#include "can_manager.h"
#include "lcd_manager.h"


int main(void) {
	CYGlobalIntEnable;      //Uncomment this line to enable global interrupts 

	uint8_t atomic_state = CyEnterCriticalSection(); // BEGIN ATOMIC
	can_init();
	lcd_init();
    lcd_print_bms();
	CyDelay(10);				//give some time to finish setup
	CyExitCriticalSection(atomic_state);               // END ATOMIC
	
	
	//CyDelay(3000);
	
	for(;;)	{
	    //can_test_send();
		//can_test_receive();
		//lcd_test_print();
		//can_process_BMS_VOLT();
		//can_process_BMS_TEMP();
		//can_process_BMS_CURR();
		//CyDelay(1000);
	} // main loop

	return 0;
} // main()

void msg_recieve(DataPacket * msg) {
	can_process(msg);
}