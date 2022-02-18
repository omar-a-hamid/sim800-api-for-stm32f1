/*
 * gsm.h
 *
 *  Created on: Feb 17, 2022
 *      Author: Omar
 */

#ifndef SRC_GSM_H_
#define SRC_GSM_H_




#include "main.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>



#define _GSM_ENABL_PIN_		GPIO_PIN_12  //pin used to boot GSM module-GPIO OUTPUT
#define _GSM_UART_HANDLE  	huart2        // GSM UART HANDLE
#define _DBG_UART_HANDLE  	huart1		//define to enable debug prints through debug uart

//#define NUMBER 			"+201064598xxx"
//#define SERVER_IP

#define max_number_legnth	20     		//max length of any phone number that will be used
#define max_text_legnth		50			//ax size of sms msgs
#define RX_BUFFER_SIZE 		50			//Receiving buffer size for maximum received response
#define RX_TIMEOUT 		    500			//time out for reading a response from GSM module

#define ON   		1
#define OFF 		0

#define _GSM_UART_		&_GSM_UART_HANDLE

#ifdef  _DBG_UART_HANDLE
#define _DBG_UART_		 &_DBG_UART_HANDLE
extern UART_HandleTypeDef _DBG_UART_HANDLE;
#endif

extern UART_HandleTypeDef _GSM_UART_HANDLE;

extern char rx_buffer[RX_BUFFER_SIZE];


typedef enum
AT_STATUS				//to store AT-cmd states
{
	GSM_OK,
	GSM_ERORR,
	GSM_INVALID_INPUT
} AT_STATUS;



AT_STATUS gsm_init(void);       													//Initialization
AT_STATUS send_sms(char number[max_number_legnth],char msg[max_text_legnth]);  		//sending sms
AT_STATUS at_command(char command[]);												//sends command to module

char* at_cmd_response(char command[]);												//sends a custom AT command and returns pointer to array containing response
char* gsm_location(void);															//returns pointer to location array

//AT_STATUS send_call(char number [max_number_legnth]);
//AT_STATUS latch_to_internet(void)
//AT_STATUS TCP_IP(char ip[8])

#endif /* SRC_GSM_H_ */
