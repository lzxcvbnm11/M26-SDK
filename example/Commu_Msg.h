#ifndef   _COMMU_MSG_H_
#define  _COMMU_MSG_H_


#define _CrLf               "\r\n"
#define _OKCrLf             "OK\r\n"
#define _CrLfOKCrLf         "\r\nOK\r\n"
#define _CrLfCrLfOKCrLf     "\r\n\r\nOK\r\n"
#define _CrLfOK             "\r\nOK"
#define _OK                  "OK"
#define _Lf                  "\n"


typedef struct 
{
  const char*            cmd;			    					//输出命令匹配头
   int8_t                   (*cmd_func)(uint8_t id,char* pStr,uint16_t strLen);          //打包处理函数，增加指令后的数据
   const char*            cmd_ackhead;  							//命令应答头
   const char*            cmd_acktail;          //命令应答尾
   uint8_t*               (*cmd_ack_func)(uint8_t id,uint8_t* pData,uint16_t len);					//应答函数指针
   uint16_t                waitTime;							//等待时间计单位（/200ms）
   const uint16_t          waitTimout;          	//等待超时处理时间 单位（/200ms）
   uint16_t                coutTimout;           //超时计数
   int8_t                 (*cmd_timout_func)(uint8_t id);   //超时处理函数
   uint8_t                errCount;               //错误处理计数
}Commu_Msg_AT_t;





typedef void (*Commu_Msg_RxProtocolData_Callback) (uint8_t monitor,uint8_t* pData,uint16_t datalen);
typedef uint32_t (*Commu_Msg_SendToUart_Callback) (uint8_t* pData,uint32_t len);
typedef void (*Commu_Msg_PhoneCallInto_Callback) (uint8_t *phoneNum);
typedef uint32_t (*Commu_Msg_SendFailed_Callback) (uint8_t* ringBase,uint32_t ringSize,uint8_t* pSave,uint32_t savelen,uint32_t flag,char* attach);
typedef void (*Commu_Msg_ReciveSMS_Callback)(char* phone,char* time,char* pSMS,uint32_t smsLen);


typedef void (*Commu_Msg_FTP_GetSize_Callback) (uint32_t size);
typedef void (*Commu_Msg_FTP_GetFile_Callback) (uint8_t* pData,uint32_t ui32Offset,int32_t len);
typedef struct
{

  

  COMMU_BOOL IsWaitAck;

  Commu_Msg_RxProtocolData_Callback RxProtocolData_CallFunc;
  Commu_Msg_SendToUart_Callback       SendToUart_CallFunc;
  Commu_Msg_PhoneCallInto_Callback    PhoneCallInto_CallFunc;
  Commu_Msg_SendFailed_Callback          Socket_SendFailed_CallFunc;
  Commu_Msg_SendFailed_Callback          SMS_SendFailed_CallFunc;
  Commu_Msg_ReciveSMS_Callback          ReciveSMS_Callfunc;

  Commu_Msg_FTP_GetSize_Callback        FTP_GetSize_Callback;
  Commu_Msg_FTP_GetFile_Callback         FTP_GetFile_Callback;

  
}Commu_Msg_t;

typedef struct
{
  //通讯模块的配置信息
 // commu_sysConfig_t                      config;

  //通讯模块的状态信息
 // commu_state_t                             state;

  //数据接收的循环队列
  Commu_RingQueue_t             atRxRing;

  //数据发送的循环队列
  Commu_RingQueue_t              atTxRing;
  
}commu_sys_t;


extern Commu_Msg_t Commu_Msg;

 extern  void Commu_Msg_ClearAT(void);
extern void Commu_Msg_Timer10ms(void);
extern void Commu_Msg_UnpackUsartData(Commu_RingQueue_t *pRing);

#endif
