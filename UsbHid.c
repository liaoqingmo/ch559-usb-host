#include "Type.h"
#include "Protocol.h"

#include "Uart.h"

#include "UsbDef.h"
#include "UsbHost.h"
#include "UsbHid.h"

#include "ParseHidData.h"

#include "Packet.h"

#include "Trace.h"

void ProcessHIDData(INTERFACE *pInterface, const UINT8 *pData, UINT16 len)
{
	UINT8 buffer[KEYBOARD_LEN + 2];
	UINT8 pktLen;
	UINT8 interfaceClass = pInterface->InterfaceClass;
	UINT8 interfaceProtocol = pInterface->InterfaceProtocol;

	len = len;
	if (interfaceClass == USB_DEV_CLASS_HID)
	{
		if (interfaceProtocol == HID_PROTOCOL_KEYBOARD)
		{	    
#ifdef DEBUG
			UINT16 i;
#endif		

#ifdef DEBUG		
            TRACE("keyboard data:\r\n");
            for (i = 0; i < len; i++)
            {
                TRACE1("0x%02X ", (UINT16)pData[i]);
            }
            TRACE("\r\n");
#endif	

            if (!UsbKeyboardParse(pData, NULL, &pInterface->HidSegStruct))
            {
                return;
            }
            
			if (BuildUsbKeyboardPacket(buffer, sizeof(buffer), &pktLen, 
			        pData + pInterface->HidSegStruct.HIDSeg[HID_SEG_KEYBOARD_INDEX].segStart / 8))
			{
#ifdef DEBUG
				for (i = 0; i < pktLen; i++)
				{
					TRACE1("0x%02X ", (UINT16)buffer[i]);
				}
				TRACE("\r\n");
#else
				if (pktLen == KEYBOARD_LEN + 2)
				{
					CH559UART0SendData(buffer, pktLen);
				}
#endif
			}
		}
		else if (interfaceProtocol == HID_PROTOCOL_MOUSE)
		{
			UINT8 mouseData[MOUSE_LEN];
			
#ifdef DEBUG
			UINT16 i;
#endif

#ifdef DEBUG		
			TRACE("mouse data:\r\n");
			for (i = 0; i < len; i++)
			{
				TRACE1("0x%02X ", (UINT16)pData[i]);
			}
			TRACE("\r\n");
#endif	

			UsbMouseParse(pData, mouseData, &pInterface->HidSegStruct);
						
			if (BuildUsbMousePacket(buffer, sizeof(buffer), &pktLen, mouseData))
			{
#ifdef DEBUG	
				TRACE("after process:\r\n");	
				for (i = 0; i < pktLen; i++)
				{
					TRACE1("0x%02X ", (UINT16)buffer[i]);
				}
				TRACE("\r\n");
#else	
				if (pktLen == MOUSE_LEN + 2)
				{
					CH559UART0SendData(buffer, pktLen);
				}
#endif
			}
		}
	}
}


