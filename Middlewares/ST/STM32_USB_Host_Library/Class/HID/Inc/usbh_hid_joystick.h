#ifndef __USBH_HID_JOYSTICK_H
#define __USBH_HID_JOYSTICK_H

#include "usbh_hid.h"

void FIFO_GET(uint8_t *buf);
void FIFO_PUT(uint8_t *buf);
void FFInit(void);
void ForceFeedbackTest(void);

typedef struct _HID_JOYSTICK_Info
{
	uint32_t    RAW_IN[2];//joystick state
	uint8_t    	RAW_OUT[8];//force feedback x,y
	uint8_t    	RAW_OUT_OLD[8];//force feedback x,y

	uint16_t    X;
	uint16_t    Y;
	uint16_t    Z;
	uint8_t     Button[12];
	uint8_t     Hat;
	uint8_t     Throttle;
}
HID_JOYSTICK_Info_TypeDef;

typedef struct
{
	uint8_t    	OUT[7];
}
HID_JOYSTICK_Output_Fifo_TypeDef;

USBH_StatusTypeDef USBH_HID_JoyStickInit(USBH_HandleTypeDef *phost);
void ReadWriteJoyStick(void);
void Vibrate(float strength);
//HID_JOYSTICK_Info_TypeDef *USBH_HID_GetJoyStickInfo(USBH_HandleTypeDef *phost);
#endif
