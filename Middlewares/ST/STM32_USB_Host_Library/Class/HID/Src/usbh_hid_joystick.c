/*
 * usbh_hid_joystick.c
 *
 *  Created on: 30 Sep 2022
 *      Author: antcz
 */
#include "usbh_hid_joystick.h"
#include "usbh_hid_parser.h"

//static USBH_StatusTypeDef USBH_HID_JoyStickDecode(USBH_HandleTypeDef *phost);

volatile HID_JOYSTICK_Info_TypeDef  DJoyStick;
HID_JOYSTICK_Output_Fifo_TypeDef OUT_FIFO[256];
uint32_t                  joystick_report_data[5];
uint32_t                  joystick_rx_report_buf[5];
uint32_t                  joystick_tx_report_buf[5];

volatile uint8_t					FiFoHead;
volatile uint8_t					FiFoTail;
volatile char 					JInit;

void FIFO_GET(uint8_t *buf)
	{
	for(uint8_t i=0;i<7;i++)
		buf[i]=OUT_FIFO[FiFoHead].OUT[i];
	FiFoHead--;
	}
void FIFO_PUT(uint8_t *buf)
	{
		for(uint8_t i=0;i<7;i++)
			{
			OUT_FIFO[FiFoHead].OUT[i]=buf[i];
			}
		FiFoHead++;
	}

void FFInit(void)
	{
uint8_t tab[7];
uint8_t i;
	tab[0]=0x53;
	tab[1]=0x00;
	tab[2]=0x00;
	tab[3]=0x00;
	tab[4]=0x00;
	tab[5]=0x00;
	tab[6]=0x00;
	FIFO_PUT(tab);
	for(i=0;i<4;i++)
		{
		tab[0]=0xFE;
		tab[1]=0x0D;
		tab[2]=0x09;
		tab[3]=0x09;
		tab[4]=0x59;
		tab[5]=0x00;
		tab[6]=0x00;
		FIFO_PUT(tab);
		tab[0]=0xF4;
		tab[1]=0x00;
		tab[2]=0x00;
		tab[3]=0x00;
		tab[4]=0x00;
		tab[5]=0x00;
		tab[6]=0x00;
		FIFO_PUT(tab);
		}
		tab[0]=0xFE;
		tab[1]=0x0D;
		tab[2]=0x09;
		tab[3]=0x09;
		tab[4]=0x59;
		tab[5]=0x00;
		tab[6]=0x00;
		FIFO_PUT(tab);
		tab[0]=0xF5;
		tab[1]=0x00;
		tab[2]=0x00;
		tab[3]=0x00;
		tab[4]=0x00;
		tab[5]=0x00;
		tab[6]=0x00;
		FIFO_PUT(tab);
	}
int PX=0;
int PY=0;
int swc=0;
int X = 0;
char canVibrate = 0;
char vibOffset = 0;
long lastTick = 0;
void Vibrate(float strength)
{
//	if(DJoyStick.Button[0])
//		canVibrate=1;
//	if(DJoyStick.Button[1])
//		canVibrate=0;
	strength = 1.0 - strength;

	if(canVibrate && (HAL_GetTick() - lastTick > strength*100+20))
	{
		vibOffset = strength*100;
		switch(swc)
		{
		case 0:
			PX=0+vibOffset;
			PY=128;
			swc=1;
			break;
		case 1:
			PX=255-vibOffset;
			PY=128;
			swc=0;
		}



	DJoyStick.RAW_OUT[0]=0x51;
	DJoyStick.RAW_OUT[1]=0x08;
	DJoyStick.RAW_OUT[2]=PX;
	DJoyStick.RAW_OUT[3]=PY;
	DJoyStick.RAW_OUT[4]=0;
	DJoyStick.RAW_OUT[5]=0;
	DJoyStick.RAW_OUT[6]=0;
	lastTick = HAL_GetTick();
	}
	else
	{
		DJoyStick.RAW_OUT[0]=0x53;
		DJoyStick.RAW_OUT[1]=0x00;
	}
}
void ForceFeedbackTest(void)
	{
int16_t temp;
uint8_t PX,PY;

	if(JInit==1)
		{
		FIFO_GET(DJoyStick.RAW_OUT);
		if(FiFoHead==FiFoTail++)
			JInit=1;
		}
		else
		{

		if(DJoyStick.Button[0])
			X=1;

		if(DJoyStick.Button[1])
			X=0;


	if(X==0)
		{
		DJoyStick.RAW_OUT[0]=0x53;
		DJoyStick.RAW_OUT[1]=0x00;
		PX=0;
		PY=0;
		swc=0;
		}


	if(X==1) {
		switch(swc)
		{
		case 0:
			PX=0;
			PY=128;
			swc=1;
			break;
		case 1:
			PX=255;
			PY=128;
			swc=0;
		}
		DJoyStick.RAW_OUT[0]=0x51;
		DJoyStick.RAW_OUT[1]=0x08;
		}

	DJoyStick.RAW_OUT[2]=PX;
	DJoyStick.RAW_OUT[3]=PY;
	DJoyStick.RAW_OUT[4]=0;
	DJoyStick.RAW_OUT[5]=0;
	DJoyStick.RAW_OUT[6]=0;
	}
	}
//static const HID_Report_ItemTypedef joystick_X =
//{
//  (uint8_t *)(void *)joystick_report_data + 0, /*data*/
//  16,     /*size*/
//  0,     /*shift*/
//  0,     /*count (only for array items)*/
//  0,     /*signed?*/
//  0,     /*min value read can return*/
//  0xFF,     /*max value read can return*/
//  0,     /*min value device can report*/
//  0xFF,     /*max value device can report*/
//  1      /*resolution*/
//};
//
//static const HID_Report_ItemTypedef joystick_Y =
//{
//  (uint8_t *)(void *)joystick_report_data + 0, /*data*/
//  16,     /*size*/
//  16,     /*shift*/
//  0,     /*count (only for array items)*/
//  0,     /*signed?*/
//  0,     /*min value read can return*/
//  0xFFFF,     /*max value read can return*/
//  0,     /*min value device can report*/
//  0xFFFF,     /*max value device can report*/
//  1      /*resolution*/
//};

char Edge(void)
{
uint8_t i;
char ret;
		ret = 0;
	for(i=0;i<8;i++)
		{
		if(DJoyStick.RAW_OUT[i]!=DJoyStick.RAW_OUT_OLD[i])
			ret = 1;
		DJoyStick.RAW_OUT_OLD[i]=DJoyStick.RAW_OUT[i];
		}
return ret;
}

USBH_StatusTypeDef USBH_HID_JoyStickInit(USBH_HandleTypeDef *phost)
{
	HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;
	HID_Handle->length = 7;
	HID_Handle->pData = (uint8_t *)(void *)DJoyStick.RAW_IN;
	USBH_HID_FifoInit(&HID_Handle->fifo, phost->device.Data, (uint16_t)(HID_QUEUE_SIZE * 20));
	return USBH_OK;
}

extern USBH_HandleTypeDef hUsbHostHS;
void ReadWriteJoyStick(void)
{
HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) hUsbHostHS.pActiveClass->pData;
if(HID_Handle->state == HID_IDLE)
	{
		HID_Handle->state = HID_GET_DATA;
	}
}

