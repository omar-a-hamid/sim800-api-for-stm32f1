#include "gsm.h"


char rx_buffer[RX_BUFFER_SIZE];   //reciver buffer


/*
 * function to boot and initialize GSM module
 * */
AT_STATUS gsm_init(void){

	static char boot_cmd[] = "AT\r\n"; //BOOTING SEQUNCE

	memset(rx_buffer, '\0',strlen(rx_buffer)); //nulls reciving buffer

	//booting sequence
	HAL_GPIO_WritePin(GPIOA, _GSM_ENABL_PIN_, GPIO_PIN_SET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOA, _GSM_ENABL_PIN_, GPIO_PIN_RESET);
	HAL_Delay(1000);

	AT_STATUS  init_status = at_command(boot_cmd);		//sending booting command

#ifdef  _DBG_UART_
	if(init_status==GSM_OK){
		HAL_UART_Transmit(_DBG_UART_,(uint8_t *)"Initialization successful\n",27,HAL_TIMEOUT);
	}
#endif

	return (init_status);
}


/*
 * sends an sms msg, parameters-> phone number and text in msg
 * */
AT_STATUS send_sms(char number[max_number_legnth],char msg[max_text_legnth]){

	if(number == NULL ||msg == NULL){			//checking for invalid inputs
		return GSM_INVALID_INPUT;
	}

	static char sms_string[max_text_legnth+18];  // to store AT_cmd
	AT_STATUS sms_status;						//to store status




	sms_status = at_command("AT+CMGF=1\r\n");	//sets module to text mode
	if(sms_status==GSM_ERORR){
		return GSM_ERORR;
	}

	sms_status = at_command("AT+CSCS=\"GSM\"\r\n");
	if(sms_status==GSM_ERORR){
		return GSM_ERORR;
	}




	sprintf(sms_string,"AT+CMGS=\"%s\"\r\n",number);
	at_command(sms_string);//AT+CMGS="+2010645982xx"  doesn't return "GSM_OK"

	memset(sms_string, '\0',strlen(sms_string));  		//nulls array to use for next command

	sprintf(sms_string, "%s%c", msg, 26);
	sms_status = at_command(sms_string);
	if(sms_status==GSM_ERORR){
		return GSM_ERORR;
	}



	return (GSM_OK); 			//returns OKAY successful
}

/*
 * sends custom at commands and returns GSM module response
 */
char* at_cmd_response(char command[]){

	static char error[]="no response-error\r\n";

	AT_STATUS response_status = at_command(command);

	if(response_status==GSM_INVALID_INPUT || rx_buffer[0] == '\0'){
		return error;
	}


	return  rx_buffer;
}


/*
 *   returns pointer to location array
 * */
char* gsm_location(void){

	 char* location_ptr;										//format of response-> +CIPGSMLOC: 0,0.000000,0.000000,2019/11/29,05:00:38  size = 51
	static char error[]="error\r\n";
	AT_STATUS loc_sta;
	loc_sta = at_command("AT+CGATT=1\r\n");			// to attach GPRS

	if(loc_sta==GSM_ERORR){
		return error;
	}



	loc_sta = at_command("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n");		//activate bearer profile
	if(loc_sta==GSM_ERORR){
		return error;
	}


	loc_sta = at_command("AT+SAPBR=3,1,\"APN\",\"RCMNET\"\r\n");
	if(loc_sta==GSM_ERORR){
		return error;
	}



	loc_sta = at_command("AT+SAPBR=1,1\r\n"); 					        //activate bearer context
	if(loc_sta==GSM_ERORR){
		return error;
	}


	loc_sta = at_command("AT+SAPBR=2,1\r\n");     					    //response maynot have "OK"



	location_ptr = at_cmd_response("AT+CIPGSMLOC=1,1\r\n");			//to get gsm location, date and time. used at_cmd_response to get response

	loc_sta = at_command("AT+SAPBR=0,1\r\n");						 //to deactivate bearer profile


	return location_ptr;
}



/*
 * sends AT CMD to module using a string and checks for OK response
 */

AT_STATUS at_command(char command []){

	if(command == NULL){//checking for invalid inputs

		return GSM_INVALID_INPUT;

	}
	memset(rx_buffer, '\0',strlen(rx_buffer));            //nulls receiving array buffer for next input
	HAL_UART_Transmit(_GSM_UART_,(uint8_t *) command, strlen(command),HAL_TIMEOUT); 	//send the command to GSM module

	HAL_Delay(20);
	HAL_UART_Receive(_GSM_UART_, (uint8_t *)rx_buffer, 10,RX_TIMEOUT);   //Receives response from GSM module

	if(strstr(rx_buffer, "OK") != NULL) {     		//checks if response contains "OK"

#ifdef  _DBG_UART_
				HAL_UART_Transmit(_DBG_UART_,(uint8_t *)"msg_ok\n", 8,HAL_TIMEOUT);
#endif
		return GSM_OK;

	}
	else{

#ifdef  _DBG_UART_
				HAL_UART_Transmit(_DBG_UART_,(uint8_t *)"time out\n", 10,HAL_TIMEOUT);
#endif
		return GSM_ERORR;


	}


}

