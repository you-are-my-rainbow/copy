/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.2.1
  * @date    17-March-2018
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      <http://www.st.com/SLA0044>
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */
#include <string.h>
#include "usbh_usr.h"
#include "ff.h"                 /* FATFS */
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"

/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_MSC_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR 
* @brief    This file includes the usb host stack user callbacks
* @{
*/

/** @defgroup USBH_USR_Private_TypesDefinitions
* @{
*/
/**
* @}
*/


/** @defgroup USBH_USR_Private_Defines
* @{
*/
#define IMAGE_BUFFER_SIZE    512
/**
* @}
*/


/** @defgroup USBH_USR_Private_Macros
* @{
*/
extern USB_OTG_CORE_HANDLE USB_OTG_Core;
extern USBH_HOST USB_Host;
/**
* @}
*/


/** @defgroup USBH_USR_Private_Variables
* @{
*/
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
uint8_t filenameString[15] = { 0 };

FATFS fatfs;
FIL file;
uint8_t Image_Buf[IMAGE_BUFFER_SIZE];
uint8_t line_idx = 0;

/* Points to the DEVICE_PROP structure of current device */
/* The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb = {
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
};


/**
* @}
*/


/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/
static uint8_t Explore_Disk(char *path, uint8_t recu_level);
static uint8_t Image_Browser(char *path);
static void Show_Image(void);
static void Toggle_Leds(void);
/**
* @}
*/


/** @defgroup USBH_USR_Private_Functions
* @{
*/


/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)//USB HOST ?????? 
{
  printf("USB OTG HS MSC Host\r\n");
  printf("> USB Host library started.\r\n");
  printf("  USB Host Library v2.1.0\r\n\r\n");
}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)//??????U??????
{
  printf("??????USB????????!\r\n");
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError(void)//??????????????!!  
{
  printf("??????????????!!!\r\n\r\n");
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Status
*/
void USBH_USR_DeviceDisconnected(void)//??????U??????
{
  printf("USB????????!\r\n");
}

/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)//????????
{
  /* callback for USB-Reset */
  printf("????????...\r\n");
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)//????????????????DeviceSpeed:????????(0,1,2 / ????)
{
  if(DeviceSpeed==HPRT0_PRTSPD_HIGH_SPEED)
  {
    printf("????(HS)USB????!\r\n");
  }  
  else if(DeviceSpeed==HPRT0_PRTSPD_FULL_SPEED)
  {
    printf("????(FS)USB????!\r\n"); 
  }
  else if(DeviceSpeed==HPRT0_PRTSPD_LOW_SPEED)
  {
    printf("????(LS)USB????!\r\n");  
  }
  else
  {
    printf("????????!\r\n");  
  }
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)//????????????????????DeviceDesc:??????????????
{
  USBH_DevDesc_TypeDef *hs;
  hs=DeviceDesc;   
  printf("VID: %04Xh\r\n" , (uint32_t)(*hs).idVendor); 
  printf("PID: %04Xh\r\n" , (uint32_t)(*hs).idProduct); 
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)//????????????????
{
  printf("????????????????!\r\n");   
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,//????????????????
                                          USBH_InterfaceDesc_TypeDef * itfDesc,
                                          USBH_EpDesc_TypeDef * epDesc)
{
  USBH_InterfaceDesc_TypeDef *id; 
  id = itfDesc;   
  if((*id).bInterfaceClass==0x08)
  {
    printf("????????????????!\r\n"); 
  }else if((*id).bInterfaceClass==0x03)
  {
    printf("HID ????!\r\n"); 
  } 
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)//??????????Manufacturer String
{
  printf("Manufacturer: %s\r\n",(char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)//??????????Product String 
{
  printf("Product: %s\r\n",(char *)ProductString);
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)//??????????SerialNum String 
{
  printf("Serial Number: %s\r\n",(char *)SerialNumString);
}



/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)//????USB????????
{
  printf("????????????!\r\n\r\n");
}


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)//??????????USB????
{
  printf("??????????USB????!\r\n\r\n");
}


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)//????????????????,??????????????
{
  printf("????????????????!\r\n");
  return USBH_USR_RESP_OK;
}

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Status
*/
void USBH_USR_OverCurrentDetected(void)//USB????????????
{
  printf("????????????!!!\r\n");
}


/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Status
*/
extern FATFS fs1[_VOLUMES];
extern uint32_t line;
extern FIL fil;
extern FRESULT res;
extern UINT bww;
int USBH_USR_MSC_Application(void)//USB HOST MSC??????????????
{
  FRESULT res;
  uint16_t bytesWritten, bytesToWrite;
  char buf[6];
  
  uint32_t num = 0;

  switch (USBH_USR_ApplicationState)
  {
  case USH_USR_FS_INIT:

    /* Initialises the File System */
    //if (f_mount(&fatfs, "", 0) != FR_OK)
    if (f_mount(&fs1[2],"2:",0)!= FR_OK)
    {
      /* efs initialisation fails */
      printf("Cannot initialize File System.\n");
      return (-1);
    }
    printf("File System initialized\r\n");
    printf("Disk capacity : %lu Bytes\r\n", USBH_MSC_Param.MSCapacity *
               USBH_MSC_Param.MSPageLength);

    if (USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
    {
      printf("The disk is write protected\r\n");
    }

    USBH_USR_ApplicationState = USH_USR_FS_OK;
    break;

  case USH_USR_FS_OK:
    
    res=f_mount(&fs1[2],"2:",1);
    if(res == FR_OK){
      res=f_open (&fil,"2:/message.txt", FA_CREATE_ALWAYS|FA_WRITE);	
      if(res == FR_OK){
        f_write (&fil, "onbon", 5, &bww);
      }
      
      f_close(&fil);
      for(num=0;num<10;num++){
        buf[num] = 'A';
      }
      
      res=f_open (&fil,"2:/message.txt", FA_READ);
      if(res == FR_OK){
        f_read (&fil, buf,5,&bww);
      }
      f_close(&fil);
      buf[5] = 0;
      printf("read data is ");
      printf(buf);
      printf("\r\n");
      LCD_ShowString(0,16*(line++),buf);
    }
    

    
    USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE;
    
    break;

  default:
    break;
  }
  return (0);
}

/**
* @brief  USBH_USR_DeInit
*         Deinit User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
  USBH_USR_ApplicationState = USH_USR_FS_INIT;
}

//????U??????????????:0,U??????????1,????
uint8_t USBH_UDISK_Status(void)
{
  return HCD_IsDeviceConnected(&USB_OTG_Core);//????U??????
}


//??U??
//buf:????????????
//sector:????????
//cnt:????????	
//??????:????????;0,????;????,????????;		 
uint8_t USBH_UDISK_Read(uint8_t* buf,uint32_t sector,uint32_t cnt)
{
  uint8_t res=1;
  if(HCD_IsDeviceConnected(&USB_OTG_Core)&&USBH_USR_ApplicationState==USH_USR_FS_OK)//??????&APP????
  {  		    
    do
    {
      res=USBH_MSC_Read10(&USB_OTG_Core,buf,sector,512*cnt);
      USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);		      
      if(!HCD_IsDeviceConnected(&USB_OTG_Core))
      {
        res=1;//????????
        break;
      };   
    }while(res==USBH_MSC_BUSY);
  }
  else 
    res=1;		  
  if(res==USBH_MSC_OK)
    res=0;	
  return 
    res;
}


//??U??
//buf:????????????
//sector:????????
//cnt:????????	
//??????:????????;0,????;????,????????;		 
uint8_t USBH_UDISK_Write(uint8_t* buf,uint32_t sector,uint32_t cnt)
{
  uint8_t res=1;
  if(HCD_IsDeviceConnected(&USB_OTG_Core)&&USBH_USR_ApplicationState==USH_USR_FS_OK)//??????&APP????
  {  		    
    do
    {
      res=USBH_MSC_Write10(&USB_OTG_Core,buf,sector,512*cnt); 
      USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);		      
      if(!HCD_IsDeviceConnected(&USB_OTG_Core))
      {
        res=1;//????????
        break;
      };   
    }while(res==USBH_MSC_BUSY);
  }else{
    res=1;
  }
    		  
  if(res==USBH_MSC_OK)
    res=0;	
  return res;
}

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
