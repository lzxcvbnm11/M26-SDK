#ifndef _COMMU_CONFIG_H_
#define _COMMU_CONFIG_H_


//#include "gpOS_semaphore.h"
#include "Commu_CommFunc.h"
//#include "tbox_debug.h"
//typedef unsigned char uint8_t;
//typedef unsigned short uint16_t;
//typedef unsigned int uint32_t;

#define NULL (void *)0

#define COUMMU_TYPE_M26  0
#define COUMMU_TYPE_EC20 1



#define COUMMU_TYPE  COUMMU_TYPE_M26


#define COMMU_RECIVE_MAXLEN   (1024)


//通讯模块的循环队列结构
typedef struct
{
	 uint8_t* pWrite;
	 uint8_t* pRead;
	 uint8_t* m_buf;
     uint32_t m_size;
  //   gpOS_semaphore_t* Access_sem;
}Commu_RingQueue_t;



#define COMMU_ENABLE                     (1)

#define COMMU_AUDIO_ENABLE              (0) //tts语音使能
#define COMMU_CALL_ENABLE            (1) //通话功能使能
#define COMMU_SMS_ENABLE             (1) //短信功能使能
#define COMMU_BUDRATE_DEFAULT    (115200)
#define COMMU_UART_ID                     (1)
#define COMMU_AT_MAXLEN                (128)







#define COMMU_MAX_TCPLINK_MATCH    (2)           //支持最多的TCP链路条数
#define COMMU_PDP_MATCH                   (1)            //支持最多16个移动场景1~MATCH+1
#define COMMU_AT_RX_BUF_SIZE           (1024*4)    //通讯模块指令接收缓存区大小
#define COMMU_AT_TX_BUF_SIZE           (1024*2)    //通讯模块指令发送缓存区大小
#define COMMU_UART_RX_BUF_SIZE       (1024*4)    //通讯模块串口接收缓存区大小
#define COMMU_UART_TX_BUF_SIZE       (1024*2)    //通讯模块串口发送缓存区大小
#define COMMU_AUDIO_BUF_SIZE           (1024*1)    //语音部分的缓存



#define COMMU_PLOG_ENABLE   (1)

#if COMMU_PLOG_ENABLE == 1
#define COMMU_PLOG(fmt,...)   TBOX_COMMU_DEBUG((fmt, ##__VA_ARGS__))
#else
#define COMMU_PLOG(fmt,...) 

#endif





#endif



