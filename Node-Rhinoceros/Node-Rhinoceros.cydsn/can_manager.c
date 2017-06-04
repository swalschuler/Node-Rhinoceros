#include "can_manager.h"
#include <stdio.h>

#define BMS_CURRENT_ID		0x288
#define BMS_VOLTAGE_ID		0x388
#define BMS_TEMP_ID			0x488

DataPacket BMS_STATUS;		//0x188
DataPacket BMS_CURRENT;		//0x288
DataPacket BMS_VOLTAGE;		//0x388
DataPacket BMS_TEMP;		//0x488

void can_init() {
	CAN_GlobalIntEnable(); // CAN Initialization
	CAN_Init();
	CAN_Start();
	
	
	can_msg_init(&BMS_CURRENT, BMS_CURRENT_ID); 
	can_msg_init(&BMS_VOLTAGE, BMS_VOLTAGE_ID);
	can_msg_init(&BMS_TEMP, BMS_TEMP_ID);
}

//initalize CAN message variables with FF's everywhere
void can_msg_init(DataPacket* can_msg, uint16_t id) {
	uint8_t i;
	can_msg->id = id;
	can_msg->length = 8;
	can_msg->millicounter = 0;
	for(i=0; i<can_msg->length; i++) {
		can_msg->data[i] = 0x00;
	}
	
} //can_msg_init()


/*  recieves can message from bus can compares it with is respective id*/
int can_process(DataPacket* can_msg){
	int status = 0;		//returns 1 if message is updated
    switch (can_msg->id) {
	case BMS_CURRENT_ID:		//0x288
		status = can_compare(&BMS_STATUS, can_msg);
		if(status)
			can_process_BMS_CURR();
		break;
	case BMS_VOLTAGE_ID:	//0x388
		status = can_compare(&BMS_VOLTAGE, can_msg);
		if(status)
			can_process_BMS_VOLT();
		break;
	case BMS_TEMP_ID:		//0x488
		status = can_compare(&BMS_TEMP, can_msg);
		if(status)
			can_process_BMS_TEMP();
		break;
    default:
		status = 0;
        break;
    }
	return status;
}

/*	compares received can message with the latest can message of the same id.
	if it is different, it will add update that DataPacket variable	
	returns: 1 if message was updated, 0 if message same as before */
int can_compare(DataPacket* prev_msg, DataPacket* new_msg) {
	uint8_t i, j;
	uint8_t offset = 0;
	DataPacket temp;
 
	temp.id = new_msg->id;
	temp.length = new_msg->length;
	temp.millicounter = new_msg->millicounter;
	for(i=0; i<temp.length; i++) {
		temp.data[i] = new_msg->data[i];
	}
	
	//bytes received in little endian so have to reverse to big endian 32bits
	for(i=0; i<(new_msg->length/2); i++) {		//swap bytes 0-3
		new_msg->data[i+offset] = temp.data[offset + temp.length/2 - i - 1];
	}//for
	offset = 4;
	for(i=0; i<(new_msg->length/2); i++) {		//swap bytes 4-7
		new_msg->data[i+offset] = temp.data[offset + temp.length/2 - i - 1];
	}//for
	
	for(i=0; i<new_msg->length; i++) {					//compare all 8 data bytes
		if(prev_msg->data[i] != new_msg->data[i]) {		//update it if data[] has changed
			prev_msg->id = new_msg->id;
			prev_msg->length = new_msg->length;
			prev_msg->millicounter = new_msg->millicounter;	
			for(j=0; j<new_msg->length; j++) {
				prev_msg->data[j] = new_msg->data[j];
			}
			
			return 1;
		}//if
	}//for
	return 0;
}

//formats and gets lcd pos
void can_process_BMS_CURR() {
	uint16_t current;
	char8 curr_string[4]; 
	
	uint16_t byte2 = BMS_CURRENT.data[1] << 8;
	uint16_t byte1 = BMS_CURRENT.data[2];
	current = byte2 | byte1;
	
	current = 0x10;
	sprintf(curr_string, "%03dC", current); 
	LCD_Position(1, 3);
	LCD_PrintString(curr_string);
}

//gets pack voltage
void can_process_BMS_VOLT() {
	uint32_t voltage;
	char8 volt_string[6];
	
	uint32_t byte4 = BMS_VOLTAGE.data[7] << 24;
	uint32_t byte3 = BMS_VOLTAGE.data[6] << 16;
	uint32_t byte2 = BMS_VOLTAGE.data[5] << 8;
	uint32_t byte1 = BMS_VOLTAGE.data[4];
	voltage = byte4 | byte3 | byte2 | byte1;
	
	voltage = 0x0001C3E3;
	sprintf(volt_string, "%03d.%01dV", voltage/1000, voltage/100%10);
	LCD_Position(0, 3);
	LCD_PrintString(volt_string);
}

//highest temp
void can_process_BMS_TEMP() {
	uint8_t temperature = BMS_TEMP.data[7];	//displays as decimal
	temperature = 0x25;
	LCD_Position(1, 13);
	LCD_PrintHexUint8(temperature);
}


void can_test_send() {
	uint8_t i;
	CAN_TX_MSG message;
	CAN_DATA_BYTES_MSG test_data;

	message.id = 0x100;
	message.rtr = 0;
	message.ide = 0;
	message.dlc = 0x08;
	message.irq = 1;
	message.msg = &test_data;

	for(i=0;i<8;i++)
		test_data.byte[i] = 2;

	CAN_SendMsg(&message);
} // can_test_send()


void can_test_receive() {
	DataPacket test_msg;
    uint8 i;
	
    //test packets
    for(i=100; i<115; i++) { 
		//test_msg.millicounter = millis_timer_ReadCounter();    
		test_msg.id = 0x0923;
		test_msg.length = 8;
		test_msg.data[0]= 1;
		test_msg.data[1]= 2;
		test_msg.data[2]= 3;
		test_msg.data[3]= 4;
		test_msg.data[4]= 5;
		test_msg.data[5]= 6;
		test_msg.data[6]= 7;
        test_msg.data[7]= 8;
		
		//msg_recieve(&test_msg);
    } //for
}
