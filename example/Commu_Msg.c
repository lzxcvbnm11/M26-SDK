/*---------------------------------------------------------------------*
*文件名称：Commu_Msg.c
*文件描述：处理Commu通讯模块关于TCP连接，SMS短信，电话及其他AT控制指令
*作者：bruce
*发布版本：V1.0
*版本发布日期：2015-7-24
*-------------------------------历史修改记录-------------------------------------------------
*修改者：
*修改内容：
*修改日期：
*修改版本：
*
*
*---------------------------------------------------------------------*/

#include "Commu_Config.h"
#include "Commu_CommFunc.h"
#include "Commu_Msg.h"
//#include "Commu_Dev.h"
#include "AtRing.h"
#include "RxRing.h"
//#include "Commu_App.h"


#if COUMMU_TYPE == COUMMU_TYPE_EC20
#define Commu_AT_NUM  49
#elif COUMMU_TYPE == COUMMU_TYPE_M26
#define Commu_AT_NUM  62
#endif



Commu_Msg_t Commu_Msg;
commu_sys_t CommuSys;

static int8_t Commu_Msg_EmptyFunc(uint8_t id);
static int8_t Commu_Msg_GenCmdFunc(uint8_t id,char *pStr,uint16_t len);
static int8_t Commu_Msg_GenTimout(uint8_t id);
static int8_t Commu_Msg_QSCLKTimout(uint8_t id);
static uint8_t* Commu_Msg_GenRespond(uint8_t id,uint8_t* pData,uint16_t len);
#if 0
static  uint8_t* Commu_Msg_TCPRcRespond (uint8_t id,uint8_t* pData,uint16_t dlen);
static int8_t Commu_Msg_tcpSendCmdFunc(uint8_t id,char* pStr,uint16_t strLen);
static uint8_t* Commu_Msg_tcpSendOverRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_TCPSendRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_TCPStateRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_CSQRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_RegRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_CPINRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_CPASRespond(uint8_t id,uint8_t * pData,uint16_t len);
static uint8_t* Commu_Msg_IPRRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_SetIPRRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_CCIDRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_CGMRRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_GSNRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_CMGLRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_CMGSRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_TTSRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_RecordRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_FlieListRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_SpaceRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_checkPDPRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_CFUNRespond(uint8_t id,uint8_t* pData,uint16_t len);

static uint8_t* Commu_Msg_FTPSTATRespond(uint8_t id,uint8_t* pData,uint16_t len);

static uint8_t* Commu_Msg_FTPCloseRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_FTPGetSizeRespond(uint8_t id,uint8_t* pData,uint16_t len);

static uint8_t* Commu_Msg_FTPGetFileRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_FTPGetFileErrorRespond(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_RingWithCLIP(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_Ring(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_TTSPlayOver(uint8_t id,uint8_t* pData,uint16_t len);
static uint8_t* Commu_Msg_RecordPlayOver(uint8_t id,uint8_t* pData,uint16_t len);
static int8_t Commu_Msg_IPRTimout(uint8_t id);
static int8_t Commu_Msg_SetIPRTimout(uint8_t id);
static int8_t Commu_Msg_TCPSendTimout(uint8_t id);
#endif
static uint8_t* Commu_Msg_ErrProc(uint8_t* pHead,uint16_t len ,uint8_t cmdId);
static uint8_t* Commu_Msg_ErrProcHook(uint8_t* pHead,uint16_t len ,uint8_t cmdId);
static uint32_t Commu_Msg_SendToUart(uint8_t* pData,uint32_t len);
static void Commu_Msg_CheckToSend(void);
static void Commu_Msg_CheckAtCmdTimoutByTimer10ms(void);
static uint8_t* Commu_Msg_QSCLKRespond(uint8_t id,uint8_t* pData,uint16_t len);




static Commu_Msg_AT_t Commu_Msg_AT[Commu_AT_NUM]={
{"AT+QSCLK=1"                ,Commu_Msg_GenCmdFunc       ,_CrLf           ,_OKCrLf   ,Commu_Msg_QSCLKRespond      ,0   ,200  ,0  ,Commu_Msg_QSCLKTimout},
{"AT+QSCLK=0"                ,Commu_Msg_GenCmdFunc       ,_CrLf           ,_OKCrLf   ,Commu_Msg_QSCLKRespond      ,0   ,200  ,0  ,Commu_Msg_GenTimout},
};


void Commu_Msg_ClearAT(void)
{
  uint8_t ui8i ;
  for(ui8i=0;ui8i<Commu_AT_NUM;ui8i++)
  {
    Commu_Msg_AT[ui8i].coutTimout =0;
    Commu_Msg_AT[ui8i].waitTime = 0;
    Commu_Msg_AT[ui8i].errCount =0;
  }
  Commu_Msg.IsWaitAck = COMMU_FALSE;


}
 
 

/**=========================AT回调函数集===================================**/


/*------------------------------------------------------------------------------
*空函数
*-----------------------------------------------------------------------------*/
static int8_t Commu_Msg_EmptyFunc(uint8_t id)
{
  return 0;
}

/***************************************************************
*通用的命令发送函数
***************************************************************/
static int8_t Commu_Msg_GenCmdFunc(uint8_t id,char *pStr,uint16_t len)
{

 TBOX_COMMU_DEBUG(("Commu: >>%s",pStr));
  Commu_Msg.SendToUart_CallFunc((uint8_t*)pStr,len);
  Commu_Msg_AT[id].waitTime = Commu_Msg_AT[id].waitTimout;

  return 0;
}


/**********************************************************************************
*函数功能：Commu模块AT指令响应的通用超时处理函数
*输入参数：对应超时的函数ID
*返回参数：:

**********************************************************************************/
static int8_t Commu_Msg_GenTimout(uint8_t id)
{
  Commu_Msg_AT[id].coutTimout++;
  if(Commu_Msg_AT[id].coutTimout <= 5)
  {
    Commu_Msg_AT[id].waitTime = Commu_Msg_AT[id].waitTimout;
    Commu_Msg.IsWaitAck= COMMU_FALSE;
    COMMU_PLOG("[tbox_commu]:cmd id=%d,coutTimout=%d\r\n",id,Commu_Msg_AT[id].coutTimout);
  }
  else
  {
        COMMU_PLOG("[tbox_commu]:1,cmd id=%d,coutTimout=%d\r\n",id,Commu_Msg_AT[id].coutTimout);
       AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_TRUE);
       COMMU_PLOG("[tbox_commu]:cmd id=%d,timout clear over\r\n",id);
       Commu_Msg_AT[id].coutTimout = 0;
       Commu_Msg_AT[id].waitTime = 0;
       Commu_Msg_AT[id].errCount =0;

       Commu_Msg.IsWaitAck = COMMU_FALSE;  
       //Commu_Sys_SetRunState(COMMU_PRDOWN); /*lzx add */
  }
  return 0;	
}


/**********************************************************************************
*函数功能：AT+QSCLK 超时处理函数
*输入参数：对应超时的函数ID
*返回参数：:

**********************************************************************************/
static int8_t Commu_Msg_QSCLKTimout(uint8_t id)
{
  Commu_Msg_AT[id].coutTimout++;
  if(Commu_Msg_AT[id].coutTimout <= 0)
  {
    Commu_Msg_AT[id].waitTime = Commu_Msg_AT[id].waitTimout;
    Commu_Msg.IsWaitAck= COMMU_FALSE;
    COMMU_PLOG("[tbox_commu]:cmd id=%d,coutTimout=%d\r\n",id,Commu_Msg_AT[id].coutTimout);
  }
  //如果超过5次没有应答，则说明模块已进入休眠，无需进行异常处理
  else
  {
      
        COMMU_PLOG("[tbox_commu]:1,cmd id=%d,coutTimout=%d\r\n",id,Commu_Msg_AT[id].coutTimout);
       AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_TRUE);
       COMMU_PLOG("[tbox_commu]:cmd id=%d,timout clear over\r\n",id);
       Commu_Msg_AT[id].coutTimout = 0;
       Commu_Msg_AT[id].waitTime = 0;
       Commu_Msg_AT[id].errCount =0;

       Commu_Msg.IsWaitAck = COMMU_FALSE;  

  }
  return 0;	
}


/**************************************************************************
*函数功能：Commu模块AT指令响应的通用处理函数
*输入参数：id：指令对应在指令表中的id号
           *pData：指令头后的第一个数据地址
*          len：指令头后的接收数据的总长度
*返回值：  返回该地址后下一条处理指令的头地址，
           NULL:表示由调用函数自己寻找下一条指令应答的起始地址。
           
**************************************************************************/
static uint8_t* Commu_Msg_GenRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;

  Commu_Msg.IsWaitAck = COMMU_FALSE;  

  return NULL;
}


/**************************************************************************
*函数功能：Commu模块设置休眠命令相应
*输入参数：id：指令对应在指令表中的id号
           *pData：指令头后的第一个数据地址
*          len：指令头后的接收数据的总长度
*返回值：  返回该地址后下一条处理指令的头地址，
           NULL:表示由调用函数自己寻找下一条指令应答的起始地址。
           
**************************************************************************/
static uint8_t* Commu_Msg_QSCLKRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
   char cmdBuf[COMMU_AT_MAXLEN];
   char c_array[8];
   uint32_t cmdLen;
   int8_t i8GetCmdState;
   int32_t  i32QSCLKState = 0;

  
  i8GetCmdState = AtRing_GetFirstAtCmd(&CommuSys.atTxRing,cmdBuf,sizeof(cmdBuf),&cmdLen);
  if(i8GetCmdState != 0)
  {
    AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
    Commu_Msg_AT[id].coutTimout = 0;
    Commu_Msg_AT[id].waitTime = 0;
    Commu_Msg_AT[id].errCount =0;
    Commu_Msg.IsWaitAck = COMMU_FALSE;  

    return NULL;

  }

  memset(c_array,0,sizeof(c_array));  
  if(NULL == GetDecAsciiFromBuf(cmdBuf,cmdLen,1,'=',c_array,sizeof(c_array)-1))
  {
    AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
    Commu_Msg_AT[id].coutTimout = 0;
    Commu_Msg_AT[id].waitTime = 0;
    Commu_Msg_AT[id].errCount =0;
    Commu_Msg.IsWaitAck = COMMU_FALSE;  

    return NULL;
  }


  i32QSCLKState = atoi(c_array);
  
  if((i32QSCLKState == 0x00)||(i32QSCLKState == 0x01)||(i32QSCLKState == 0x02))
    Commu_Sys_SetQSCLKState(i32QSCLKState);


  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;

  Commu_Msg.IsWaitAck = COMMU_FALSE;  

  return NULL;


}
#if 0
/************************************************************************
*AT+CIPSEND  发送数据

*************************************************************************/
static int8_t Commu_Msg_tcpSendCmdFunc(uint8_t id,char* pStr,uint16_t strLen)
{
  int8_t result = 0;
  char cmdBuf[COMMU_AT_MAXLEN];
  uint32_t cmdlen = 0;
  TBOX_COMMU_DEBUG(("\r\nCommu: >>%s\r\n",pStr));


  memset(cmdBuf,0,sizeof(cmdBuf));
  result = AtRing_GetFirstAtCmd(&CommuSys.atTxRing,cmdBuf,sizeof(cmdBuf),&cmdlen);
  if(result == 0)
  {
    Commu_Msg_SendToUart(cmdBuf,cmdlen);
  }
  else if(result == -2)
  {
     AtRing_DeleteAll(&CommuSys.atTxRing,COMMU_TRUE);
  }

  Commu_Msg_AT[id].waitTime = Commu_Msg_AT[id].waitTimout;
  return result;
}



/********************************************************
*函数功能：发送TCP数据发送完成应答

********************************************************/
static uint8_t* Commu_Msg_tcpSendOverRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
	
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;	
  Commu_Msg.IsWaitAck = COMMU_FALSE;      


  return NULL;
}


/*****************************************************
*函数功能：通过TCP发送数据处理函数
*输入参数：匹配到的AT指令头"AT+ZIPSEND="

******************************************************/
static uint8_t* Commu_Msg_TCPSendRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  int8_t sendResult;
  uint8_t  *rePos = NULL;
  uint8_t  endMark = 0x1A;
  uint32_t dataTotalLen;
  uint32_t sendLen;
  uint32_t readLen;
  uint8_t ui8Array[64];
  uint8_t SOI = 0x1A;
  


  if(Commu_Msg_AT[id].cmd_acktail != NULL)
  	rePos = SearchnDataFromBuf(pData,len,(uint8_t*)Commu_Msg_AT[id].cmd_acktail,strlen(Commu_Msg_AT[id].cmd_acktail));
  else
    rePos = pData;

    
  if((rePos>=pData)&&(rePos<= pData+len))
  {
  
  	if(Commu_Msg_AT[id].cmd_acktail != NULL)
    	rePos += strlen(Commu_Msg_AT[id].cmd_acktail);
    	
    dataTotalLen = AtRing_GetFirstDataLen(&CommuSys.atTxRing);
    sendLen = 0;
    while(sendLen < dataTotalLen)
    {
         if(dataTotalLen - sendLen > sizeof(ui8Array))
           readLen = sizeof(ui8Array);
         else
           readLen = dataTotalLen - sendLen ;
          readLen = AtRing_GetFirstData(&CommuSys.atTxRing,sendLen,readLen,ui8Array,sizeof(ui8Array));
          Commu_Msg_SendToUart(ui8Array,readLen);

         sendLen+= readLen;
         if(readLen == 0)
         {
            AtRing_DeleteAll(&CommuSys.atTxRing,COMMU_TRUE);
            break;
         }
    }
    Commu_Msg_SendToUart(&SOI,1);

  }

  return rePos;	


}



#if COUMMU_TYPE == COUMMU_TYPE_M26
static uint8_t* Commu_Msg_TCPStateRespond(uint8_t id,uint8_t* pData,uint16_t len)
{

 
  uint8_t* pos;
  uint16_t ui16ReaminBytes = 0;
  uint8_t* pTemp = NULL;
  uint8_t* pNext = NULL;
  uint8_t* pos2;
  uint8_t   socketNUm = 0;
  uint8_t  ui8i = 0;
  gpOS_bool_t isConnected = gpOS_FALSE;
  char c_Array[16];
  uint8_t connectState;

	
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;

  //清除连接状态
  for(ui8i=1;ui8i<=COMMU_PDP_MATCH;ui8i++)
    Commu_Sys_SetPdpState(ui8i,COMMU_FALSE,NULL);

 
  //清除连接状态
  for(ui8i=0;ui8i<COMMU_MAX_TCPLINK_MATCH;ui8i++)
    Commu_Sys_SetSocketState(ui8i,COMMU_SOCKET_TCPINIT);



  pos = pData;
  ui16ReaminBytes = len;
  //搜索数据尾
  if((NULL != (pTemp = SearchnDataFromBuf(pos,ui16ReaminBytes, (uint8_t*)"IP STATUS",strlen("IP STATUS"))))
     ||(NULL != (pTemp = SearchnDataFromBuf(pos,ui16ReaminBytes, (uint8_t*)"IP PROCESSING",strlen("IP PROCESSING"))))
     ||(NULL != (pTemp = SearchnDataFromBuf(pos,ui16ReaminBytes, (uint8_t*)"IP GPRSACT",strlen("IP GPRSACT")))))
  {
      for(ui8i=1;ui8i<COMMU_PDP_MATCH+1;ui8i++)
      {
          //if(COMMU_TRUE == Commu_Sys_GetPdpObj(ui8i,NULL,NULL,NULL))
          {
				Commu_Sys_SetPdpState(ui8i,COMMU_TRUE,NULL);
          }

      }
      pos =  pTemp;
  } 

  
  for(ui8i = 0;((ui8i<COMMU_MAX_TCPLINK_MATCH)&&(pos < pData+len));ui8i++)
  {
    ui16ReaminBytes = len-(pos - pData);
    pTemp = SearchnDataFromBuf(pos,ui16ReaminBytes, (uint8_t*)"+QISTATE:",strlen("+QISTATE:"));//搜索数据尾
    
     if(NULL != pTemp)
    { 
      memset(c_Array,0,sizeof(c_Array));
      GetDecAsciiFromBuf((char*)pTemp,ui16ReaminBytes,0,',',c_Array,sizeof(c_Array));
      socketNUm = atoi(c_Array);
      pNext = SearchnDataFromBuf(pTemp,len-(pTemp - pData),(uint8_t*)"+QISTATE:",strlen("+QISTATE:"));

      //搜索TCP链接状态
      if(pNext > pTemp)
          pos2 = SearchnDataFromBuf(pTemp,pNext-pTemp, (uint8_t*)"CONNECTED",strlen("CONNECTED"));
      else
          pos2 = SearchnDataFromBuf(pos,ui16ReaminBytes, (uint8_t*)"CONNECTED",strlen("CONNECTED"));
  
      if(pos2 != NULL)
      {
         Commu_Sys_SetSocketState(socketNUm,COMMU_SOCKET_TCPCONNECTED);
      }
      pos = pTemp + strlen("+QISTATE:\r\n");
    }
    else
    {
       break;
    }

  }
 
  Commu_Msg.IsWaitAck= COMMU_FALSE;
  return NULL;



}

#elif COUMMU_TYPE == COUMMU_TYPE_EC20

static uint8_t* Commu_Msg_TCPStateRespond(uint8_t id,uint8_t* pData,uint16_t len)
{

 
  uint8_t* pos;
  uint16_t ui16ReaminBytes = 0;
  uint8_t* pTemp = NULL;
  uint8_t   socketNUm = 0;
  uint8_t  ui8i = 0;
  gpOS_bool_t isConnected = gpOS_FALSE;
  char c_Array[16];
  uint8_t connectState;

	
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;
 
  //清除连接状态
  for(ui8i=0;ui8i<COMMU_MAX_TCPLINK_MATCH;ui8i++)
    Commu_Sys_SetSocketState(ui8i,COMMU_SOCKET_TCPINIT);





  pos = pData;
  ui16ReaminBytes = len;
  for(ui8i = 0;((ui8i<COMMU_MAX_TCPLINK_MATCH)&&(pos < pData+len));ui8i++)
  {
    ui16ReaminBytes = len-(pos - pData);
    pTemp = SearchnDataFromBuf(pos,ui16ReaminBytes, (uint8_t*)"+QISTATE:",strlen("+QISTATE:"));//搜索数据尾
    
     if(NULL != pTemp)
    { 
      memset(c_Array,0,sizeof(c_Array));
      GetDecAsciiFromBuf((char*)pTemp,ui16ReaminBytes,5,',',c_Array,sizeof(c_Array));
      connectState = atoi(c_Array);
      
      memset(c_Array,0,sizeof(c_Array));
      GetDecAsciiFromBuf((char*)pTemp,ui16ReaminBytes,1,':',c_Array,sizeof(c_Array));
      socketNUm = atoi(c_Array);      
 
      if(socketNUm < COMMU_MAX_TCPLINK_MATCH)
      {
        COMMU_PLOG("socketNUm=%d,connectState=%d-----------\r\n",socketNUm,connectState);
        if(connectState == 0)
          Commu_Sys_SetSocketState(socketNUm,COMMU_SOCKET_TCPINIT);
        else if(connectState == 1)
          Commu_Sys_SetSocketState(socketNUm,COMMU_SOCKET_TCPCONNECTING);
        else if(connectState == 2)
          Commu_Sys_SetSocketState(socketNUm,COMMU_SOCKET_TCPCONNECTED);
        else if(connectState == 3)
          Commu_Sys_SetSocketState(socketNUm,COMMU_SOCKET_TCPINIT);
        else if(connectState == 4)
          Commu_Sys_SetSocketState(socketNUm,COMMU_SOCKET_TCPCLOSEING);
        else 
          Commu_Sys_SetSocketState(socketNUm,COMMU_SOCKET_TCPINIT);
      }
      pos = pTemp + strlen("+QISTATE:\r\n");
    }
    else
    {
       break;
    }

  }
 
  Commu_Msg.IsWaitAck= COMMU_FALSE;
  return NULL;



}

#endif


/************************************************************
*查询网络型号质量+CSQ: 26, 99

************************************************************/
static uint8_t* Commu_Msg_CSQRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  uint8_t* pos;
  uint8_t tmp[5];
  uint8_t rssi = 0;
  uint8_t ber = 0;
  uint32_t ui32tmp = 0;


  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;
 
  pos = pData;
  memset(tmp,0,sizeof(tmp));

  if(NULL != GetDecAsciiFromBuf((char*)pos,len,0,',',(char*)tmp,sizeof(tmp)))
  {
    GetDecAsciiFieldToUint32((char*)tmp,strlen((char*)tmp),&ui32tmp);
    rssi = ui32tmp;

    memset(tmp,0,sizeof(tmp));
    if(NULL != GetDecAsciiFromBuf((char*)pos,len,1,',',(char*)tmp,sizeof(tmp)))
    {
      GetDecAsciiFieldToUint32((char*)tmp,strlen((char*)tmp),&ui32tmp);
      ber = ui32tmp;
    }
  }
  Commu_Sys_SetCSQ(rssi,ber);
  Commu_Msg.IsWaitAck= COMMU_FALSE;

  return NULL;

}




/***********************************************************************
*AT+CREG?  +CREG: 0,1

************************************************************************/
static uint8_t* Commu_Msg_RegRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  uint8_t ui8i=0;
  uint8_t *pos = pData;
  uint32_t ui32Data = 0;
	
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;
	
  for(ui8i=0;ui8i<len;ui8i++)
  {
    if(*pos++ == ',')
      break;
  }

  AsciiToHex((char*)pos,1,&ui32Data);
  if(ui32Data <= 5)
  {
    Commu_Sys_SetRegState((Commu_RegState_t)ui32Data);
    Commu_Sys_UpdateRunState();
  }
  Commu_Msg.IsWaitAck = COMMU_FALSE;

  return NULL;

}




/**************************************************************************
*AT+CPIN?  +CPIN: READY

**************************************************************************/
static uint8_t* Commu_Msg_CPINRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  COMMU_BOOL isReadySIM = COMMU_FALSE;
  
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;	

  if(NULL != SearchnDataFromBuf(pData,len,(uint8_t*)"NOT",strlen("NOT")))
  {
  	Commu_Sys_SetRunState(COMMU_PRDOWN);
	
    isReadySIM =COMMU_FALSE;
  }
  else if(NULL != SearchnDataFromBuf(pData,len,(uint8_t*)"READY",strlen("READY")))
  {
    isReadySIM =COMMU_TRUE;
  }
  else
  {
    isReadySIM =COMMU_FALSE;
  }
  COMMU_PLOG("isReadySIM=%d\r\n",isReadySIM);
  Commu_Sys_SetSIMState(isReadySIM);
  Commu_Msg.IsWaitAck = COMMU_FALSE;
  return NULL;
}





/*------------------------------------------------------------------------------------------------------------
*函数功能：查询通讯模块状态
*输入参数：id：指令对应在指令表中的id号
           *pData：指令头后的第一个数据地址
*          len：指令头后的接收数据的总长度
*返回值：  返回该地址后下一条处理指令的头地址，
           NULL:表示由调用函数自己寻找下一条指令应答的起始地址。

*----------------------------------------------------------------------------------------------------------------*/
static uint8_t* Commu_Msg_CPASRespond(uint8_t id,uint8_t * pData,uint16_t len)
{

  uint8_t* pos = pData;
  uint8_t  ui8State;
	
	
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
	
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;


  ui8State = *pos;
  

  if(ui8State == '0')
  {
    Commu_Sys_SetCallState(COMMU_NO_CALL);
  }
  else if(ui8State == '3')
  {
    Commu_Sys_SetCallState(COMMU_WAIT_CONNECT_CALL);
  }
  else if(ui8State == '4')
  {
    Commu_Sys_SetCallState(COMMU_HAVED_CONNECT_CALL);
  }
  else 
  {
    Commu_Sys_SetCallState(COMMU_NO_CALL);
  }
	
  Commu_Msg.IsWaitAck = COMMU_FALSE;
  return NULL;
}





/******************************************************
*函数功能：查询波特率响应函数

*******************************************************/
static uint8_t* Commu_Msg_IPRRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  extern uint8_t g_ui8Count;
  uint32_t ui32Buadrate = 0;
  char c_Array[8];
  uint16_t ui16Len;
  uint8_t* pos;
  Commu_RunStatus_t state;
  


  

  
  //ui8Count = 0;
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;

  pos = pData+1;
  if(pos <pData+len)
  {
    ui16Len = pData+len-pos;

    memset(c_Array,0,sizeof(c_Array));
    GetHexAsciiFromBuf((char*)pos,ui16Len,0,'\r',c_Array,sizeof(c_Array));
    AsciiToDec(c_Array,strlen(c_Array),&ui32Buadrate);
 
    COMMU_PLOG("ui32Buadrate=%d,CommuSys.state.uartBud=%d\r\n",ui32Buadrate,CommuSys.state.uartBud);
    //如果当前波特率不是115200，则设置模块波特率为115200，并且保存参数
    if((ui32Buadrate != 115200)||(ui32Buadrate != CommuSys.state.uartBud))
    {

      AtRing_AddAtCmd(&CommuSys.atTxRing,"AT+IPR=115200&W\r\n",NULL,0,COMMU_TRUE);
      COMMU_PLOG("add:AT+IPR=115200&W\r\n");
    }
 
   state = Commu_Sys_GetRunState();
   if(COMMU_PRUPED == state)
   {
       Commu_Sys_SetRunState(COMMU_RES);
   }
    
  }

  Commu_Msg.IsWaitAck = COMMU_FALSE;
  return NULL;

}


/*************************************************
*函数功能：设置Commu模块波特率应答处理

***************************************************/
static uint8_t* Commu_Msg_SetIPRRespond(uint8_t id,uint8_t* pData,uint16_t len)
{

  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;


  Commu_Msg.IsWaitAck = COMMU_FALSE;
 
  return NULL;
}



/*********************************************************************
*AT+CCID    

*********************************************************************/
static uint8_t* Commu_Msg_CCIDRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  uint8_t head,tail;
  uint8_t* pos;
  char     c_array[32];

  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;
	

  pos = pData;
  for(head=0;head<25;head++)
  {
    if(1 == IS_HEXASCII(pos[head]))
    {
      break;
    }	
  }
	
  for(tail=head;tail<25;tail++)
  {
    if(0 == IS_HEXASCII(pos[tail]))
    {
      break;
    }
	
  }

  if(tail -head ==20)
  {
    memset(c_array,0,sizeof(c_array));
    memcpy(c_array,&pos[head],20);
    Commu_Sys_SetICCID(c_array);
    
  }

  Commu_Msg.IsWaitAck = COMMU_FALSE;
  return NULL;	
}





/*********************************************************************
*AT+CCID    

*********************************************************************/
static uint8_t* Commu_Msg_CGMRRespond(uint8_t id,uint8_t* pData,uint16_t len)
{

  uint8_t* pos;
  char       c_array[32];
  uint8_t ui8i=0;
 
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;
  memset(c_array,0,sizeof(c_array));
  pos = pData;
  for(ui8i=0;ui8i<sizeof(c_array)-1;ui8i++)
  {
    c_array[ui8i] = *pos++;
    if(*pos == '\r')
      break;
  }
  Commu_Sys_SetModuleVersion(c_array);
  

 Commu_Msg.IsWaitAck = COMMU_FALSE;
  return NULL;	
}

/*********************************************************************
*AT+GSN  

*********************************************************************/
static uint8_t* Commu_Msg_GSNRespond(uint8_t id,uint8_t* pData,uint16_t len)
{

  uint8_t* pos;
  char       c_array[32];
  uint8_t ui8i=0;
 
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;
  memset(c_array,0,sizeof(c_array));
  pos = pData;
  for(ui8i=0;ui8i<sizeof(c_array)-1;ui8i++)
  {
    c_array[ui8i] = *pos++;
    if(*pos == '\r')
      break;
  }
  Commu_Sys_SetIMEI(c_array);
  

 Commu_Msg.IsWaitAck = COMMU_FALSE;
  return NULL;	
}


/*********************************************************************
AT+CMGL="ALL"

+CMGL: 0,"REC READ","18018739717",,"17/02/28,15:55:47+32"
123456789
+CMGL: 1,"REC UNREAD","18018739717",,"17/02/28,15:56:42+32"
668

OK

*********************************************************************/
static uint8_t* Commu_Msg_CMGLRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  char c_phone[16];        
  char c_time[32];
  uint8_t* pos;
  uint8_t* pos2;
  uint8_t* pMsgHead = NULL;
  uint8_t* pMsgTail    = NULL;
  uint8_t* pFrameHead = NULL;
  uint32_t msgLen = 0;

	
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;
 
  pos = pData;
  //遍历所有短信内容
  while(1)
  {
    //搜索短信头
    pFrameHead = SearchnDataFromBuf(pData, len-(pos - pData), (uint8_t*)"+CMGL:",strlen("+CMGL:"));
    memset(pFrameHead,0,strlen("+CMGL:"));
    
    if((pFrameHead >= pData)&&(pFrameHead < pData+len))
    {

        memset(c_phone,0,sizeof(c_phone));
        GetDecAsciiFromBuf((char*)pFrameHead, len-(pFrameHead - pData),3,'"',c_phone,sizeof(c_phone));
        pos   = GetDelimAddFromStrBuf((char*)pFrameHead,len-(pFrameHead - pData),'"',5);
        pos+=1;
        pos2 = GetDelimAddFromStrBuf((char*)pFrameHead,len-(pFrameHead - pData),'"',6);
        if((pos != NULL)&&(pos < pos2)&&(pos2 < pData+len)&&(pos2-pos < sizeof(c_time)))
        {
           memset(c_time,0,sizeof(c_time));
           memcpy(c_time,pos,pos2-pos);
        }


        pMsgHead = GetDelimAddFromStrBuf((char*)pFrameHead,len-(pFrameHead - pData),'\n',1);
        pMsgHead+=1;
        if((pMsgHead >= pData)&&(pMsgHead < pData+len))
        {
           pMsgTail = GetDelimAddFromStrBuf((char*)pMsgHead,len-(pMsgHead - pData),'\r',1);
           if((pMsgTail > pMsgHead )&&(pMsgTail < pData+len))
           {
                msgLen = pMsgTail - pMsgHead;
                if(Commu_Msg.ReciveSMS_Callfunc != NULL)
                {
                   Commu_Msg.ReciveSMS_Callfunc(c_phone,c_time,pMsgHead,msgLen);
                }

           }
        }
    }
    else
    {
        break;
    }
  }
 
  Commu_Msg.IsWaitAck= COMMU_FALSE;
  return NULL;	
}




/*********************************************************************
AT+CMGS="18018739717"

> 1234
+CMGS: 8

OK
*********************************************************************/
static uint8_t* Commu_Msg_CMGSRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  int8_t sendResult;
  uint8_t  *rePos = NULL;
  uint8_t  endMark = 0x1A;
  uint32_t dataTotalLen;
  uint32_t sendLen;
  uint32_t readLen;
  uint8_t ui8Array[64];
  uint8_t SOI = 0x1A;
  

  if(Commu_Msg_AT[id].cmd_acktail != NULL) 
    rePos = SearchnDataFromBuf(pData,len,(uint8_t*)Commu_Msg_AT[id].cmd_acktail,strlen(Commu_Msg_AT[id].cmd_acktail));
  else
    rePos = pData;

    
  if((rePos>=pData)&&(rePos<= pData+len))
  {
    if(Commu_Msg_AT[id].cmd_acktail != NULL)
    {
      rePos += strlen(Commu_Msg_AT[id].cmd_acktail);
    }

    dataTotalLen = AtRing_GetFirstDataLen(&CommuSys.atTxRing);
    sendLen = 0;
    while(sendLen < dataTotalLen)
    {
         if(dataTotalLen - sendLen > sizeof(ui8Array))
           readLen = sizeof(ui8Array);
         else
           readLen = dataTotalLen - sendLen ;
         
          readLen = AtRing_GetFirstData(&CommuSys.atTxRing,readLen,sendLen,ui8Array,sizeof(ui8Array));
          Commu_Msg_SendToUart(ui8Array,readLen);
         sendLen+= readLen;
         if(readLen == 0)
         {
            AtRing_DeleteAll(&CommuSys.atTxRing,COMMU_TRUE);
            break;
         }
    }
    Commu_Msg_SendToUart(&SOI,1);

  }

  return rePos;	

}



/*********************************************************************
*函数名称:Commu_Msg_TTSRespond
*函数功能:播放TTS语音的相应函数
*********************************************************************/
static uint8_t* Commu_Msg_TTSRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  Audio_TTSPlay_Respond_Hook();
   AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;

  Commu_Msg.IsWaitAck = COMMU_FALSE;  

  return NULL;

}
/*********************************************************************
*函数名称:Commu_Msg_TTSRespond
*函数功能:播放TTS语音的相应函数
*********************************************************************/
static uint8_t* Commu_Msg_RecordRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  Audio_RecordPlay_Respond_Hook();
   AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;

  Commu_Msg.IsWaitAck = COMMU_FALSE;  

  return NULL;

}

/*********************************************************************
*函数名称:Commu_Msg_FlieListRespond
*函数功能:查询文件列表的响应处理函数
*********************************************************************/
static uint8_t* Commu_Msg_FlieListRespond(uint8_t id,uint8_t* pData,uint16_t len)
{

  uint8_t* pos = NULL;
  uint8_t* pos2 = NULL;
  uint8_t* pFrameHead;
  uint8_t   c_fileName[32];
  uint8_t  c_array[8];
  uint32_t ui32Size ;
  
  

  
   AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;


  pos = pData;
  //遍历所有文件
  while(1)
  {
    //搜索短信头
    pFrameHead = SearchnDataFromBuf(pData, len-(pos - pData), (uint8_t*)"+QFLST:",strlen("+QFLST:"));
    memset(pFrameHead,0,strlen("+QFLST:"));
    
    if((pFrameHead >= pData)&&(pFrameHead < pData+len))
    {
        
        memset(c_fileName,0,sizeof(c_fileName));
        pos   = GetDelimAddFromStrBuf((char*)pFrameHead,len-(pFrameHead - pData),'"',1);
        pos+=1;
        pos2 = GetDelimAddFromStrBuf((char*)pFrameHead,len-(pFrameHead - pData),'"',2);
        if((pos != NULL)&&(pos < pos2)&&(pos2 < pData+len)&&(pos2-pos < sizeof(c_fileName)))
        {
           memset(c_fileName,0,sizeof(c_fileName));
           memcpy(c_fileName,pos,pos2-pos);
           memset(c_array,0,sizeof(c_array));
           GetDecAsciiFromBuf((char*)pFrameHead, len-(pFrameHead - pData),1,',',c_array,sizeof(c_array));
           ui32Size = atoi(c_array);
           Audio_CheckRecordFile_Repond_Hook(c_array,ui32Size);
        }

    }
    else
    {
        break;
    }
  }

  Commu_Msg.IsWaitAck = COMMU_FALSE;  

  return NULL;

}



/*********************************************************************
AT+QFLDS="UFS"

+QFLDS: 46679488,48250880

OK
*********************************************************************/
static uint8_t* Commu_Msg_SpaceRespond(uint8_t id,uint8_t* pData,uint16_t len)
{

  uint32_t freeSize;
  uint32_t totalSize;
  char       c_array[8];
  
   AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;

  memset(c_array,0,sizeof(c_array));
  GetDecAsciiFromBuf((char*)pData, len,1,':',c_array,sizeof(c_array));
  freeSize = atoi(c_array);

  
  memset(c_array,0,sizeof(c_array));
  GetDecAsciiFromBuf((char*)pData, len,1,',',c_array,sizeof(c_array));
  totalSize = atoi(c_array);

  Audio_CheckSpace_Repond_Hook(freeSize,totalSize);
 

  Commu_Msg.IsWaitAck = COMMU_FALSE;  

  return NULL;

}









/**********************************************************************
*查询场景开启状态
AT+QIACT?

+QIACT: 1,1,1,"10.38.186.198"
+QIACT: 2,1,1,"10.70.111.131"

OK

**********************************************************************/
static uint8_t* Commu_Msg_checkPDPRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  uint8_t* pos = NULL;
  uint8_t* pHead = NULL;
  uint8_t ui8i;
  uint8_t  ui8DosCount = 0;
  uint8_t ui8Len = 0;
  char   c_array[32];
  uint32_t ui32Temp;
  uint8_t contextId;
 
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;

  //清除连接状态
  for(ui8i=1;ui8i<COMMU_PDP_MATCH+1;ui8i++)
    Commu_Sys_SetPdpState(ui8i,COMMU_FALSE,NULL);


  pos = pData;

 

  

  pos = pData;
 
   for(ui8i = 0;((ui8i<COMMU_PDP_MATCH)&&(pos < pData+len));ui8i++)
  {
    pHead = SearchnDataFromBuf(pos,len,(uint8_t*)"+QIACT:",strlen("+QIACT:"));
    if((NULL != pHead)&&((pHead - pData)< len))
    {
         memset(c_array,0,sizeof(c_array));
         if(NULL != GetDecAsciiFromBuf(pHead,len-(pHead - pData),1,':',c_array,sizeof(c_array)))
         {
             contextId = atoi(c_array);
             //获取本地IP
             memset(c_array,0,sizeof(c_array));
             if(NULL != GetDecAsciiFromBuf(pHead,len-(pHead - pData),1,'"',c_array,sizeof(c_array)))
             {
                 Commu_Sys_SetPdpState(contextId,COMMU_TRUE,c_array);
             }
         }
    }  
  }


  Commu_Msg.IsWaitAck = COMMU_FALSE;
  return NULL;
}

/**************************************************************************
*函数功能：设置模块功能模式应答
*输入参数：id：指令对应在指令表中的id号
           *pData：指令头后的第一个数据地址
*          len：指令头后的接收数据的总长度
*返回值：  返回该地址后下一条处理指令的头地址，
           NULL:表示由调用函数自己寻找下一条指令应答的起始地址。
           
**************************************************************************/
static uint8_t* Commu_Msg_CFUNRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
   char cmdBuf[COMMU_AT_MAXLEN];
   char c_array[8];
   uint32_t cmdLen;
   int8_t i8GetCmdState;
   int32_t  i32Func = 0;

  
  i8GetCmdState = AtRing_GetFirstAtCmd(&CommuSys.atTxRing,cmdBuf,sizeof(cmdBuf),&cmdLen);
  if(i8GetCmdState != 0)
  {
    AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
    Commu_Msg_AT[id].coutTimout = 0;
    Commu_Msg_AT[id].waitTime = 0;
    Commu_Msg_AT[id].errCount =0;
    Commu_Msg.IsWaitAck = COMMU_FALSE;  

    return NULL;

  }



  memset(c_array,0,sizeof(c_array));  
  if(NULL == GetDecAsciiFromBuf(cmdBuf,cmdLen,1,'=',c_array,sizeof(c_array)))
  {
    AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
    Commu_Msg_AT[id].coutTimout = 0;
    Commu_Msg_AT[id].waitTime = 0;
    Commu_Msg_AT[id].errCount =0;
    Commu_Msg.IsWaitAck = COMMU_FALSE;  

    return NULL;
  }


  i32Func = atoi(c_array);
  
  if(i32Func == 0x00)
    Commu_Sys_SetFuncState(FUNC_MIN_CONSUMPTION);
  else if(i32Func == 0x01)
    Commu_Sys_SetFuncState(FUNC_ALL);
  else if(i32Func == 0x04)
    Commu_Sys_SetFuncState(FUNC_CLOSE_RF);








  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;

  Commu_Msg.IsWaitAck = COMMU_FALSE;  

  return NULL;
}




/**************************************************************************
*函数功能：FTP获取状态
*输入参数：id：指令对应在指令表中的id号
           *pData：指令头后的第一个数据地址
*          len：指令头后的接收数据的总长度
*返回值：  返回该地址后下一条处理指令的头地址，
           NULL:表示由调用函数自己寻找下一条指令应答的起始地址。
           
**************************************************************************/
static uint8_t* Commu_Msg_FTPSTATRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  uint8_t* pos= NULL;
  
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;


  if(NULL != (pos = SearchnDataFromBuf(pData,len,(uint8_t*)"IDLE\r\n",strlen("IDLE\r\n"))))
    Commu_Sys_FTP_SetState(FTP_IDLE);
  else if(NULL != (pos = SearchnDataFromBuf(pData,len,(uint8_t*)"OPENING\r\n",strlen("OPENING\r\n"))))
    Commu_Sys_FTP_SetState(FTP_OPENING);
  else if(NULL != (pos = SearchnDataFromBuf(pData,len,(uint8_t*)"OPENED\r\n",strlen("OPENED\r\n"))))
    Commu_Sys_FTP_SetState(FTP_OPENED);
  else if(NULL != (pos = SearchnDataFromBuf(pData,len,(uint8_t*)"WORKING\r\n",strlen("WORKING\r\n"))))
    Commu_Sys_FTP_SetState(FTP_WORKING);
  else if(NULL != (pos = SearchnDataFromBuf(pData,len,(uint8_t*)"TRANSFER\r\n",strlen("TRANSFER\r\n"))))
    Commu_Sys_FTP_SetState(FTP_TRANSFER);
  else if(NULL != (pos = SearchnDataFromBuf(pData,len,(uint8_t*)"CLOSING\r\n",strlen("CLOSING\r\n"))))
    Commu_Sys_FTP_SetState(FTP_CLOSING);
  else if(NULL != (pos = SearchnDataFromBuf(pData,len,(uint8_t*)"CLOSED\r\n",strlen("CLOSED\r\n"))))
    Commu_Sys_FTP_SetState(FTP_CLOSED);




  Commu_Msg.IsWaitAck = COMMU_FALSE;  
  
  return NULL;
}



/**************************************************************************
*函数功能：FTP关闭的响应处理函数
*输入参数：id：指令对应在指令表中的id号
           *pData：指令头后的第一个数据地址
*          len：指令头后的接收数据的总长度
*返回值：  返回该地址后下一条处理指令的头地址，
           NULL:表示由调用函数自己寻找下一条指令应答的起始地址。
           
**************************************************************************/
static uint8_t* Commu_Msg_FTPCloseRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;

  Commu_Msg.IsWaitAck = COMMU_FALSE;  
  Commu_Sys_FTP_SetState(FTP_IDLE);
  return NULL;
}




/**************************************************************************
*函数功能：FTP获取文件大小的命令
*输入参数：id：指令对应在指令表中的id号
           *pData：指令头后的第一个数据地址
*          len：指令头后的接收数据的总长度
*返回值：  返回该地址后下一条处理指令的头地址，
           NULL:表示由调用函数自己寻找下一条指令应答的起始地址。
           
**************************************************************************/
static uint8_t* Commu_Msg_FTPGetSizeRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
  char c_array[8];
  uint32_t ui32Size;
  AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
  Commu_Msg_AT[id].coutTimout = 0;
  Commu_Msg_AT[id].waitTime = 0;
  Commu_Msg_AT[id].errCount =0;


  memset(c_array,0,sizeof(c_array));  
  if(NULL != GetDecAsciiFromBuf(pData,len,0,'\r',c_array,sizeof(c_array)))
  {
    ui32Size = atoi(c_array);
    if(NULL != Commu_Msg.FTP_GetSize_Callback)
      Commu_Msg.FTP_GetSize_Callback(ui32Size);
  }




  Commu_Msg.IsWaitAck = COMMU_FALSE;  

  
  return NULL;
}



/**************************************************************************
*函数功能：FTP获取文件内容的命令
*输入参数：id：指令对应在指令表中的id号
           *pData：指令头后的第一个数据地址
*          len：指令头后的接收数据的总长度
*返回值：  返回该地址后下一条处理指令的头地址，
           NULL:表示由调用函数自己寻找下一条指令应答的起始地址。
           
**************************************************************************/
static uint8_t* Commu_Msg_FTPGetFileErrorRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
    AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
    Commu_Msg_AT[id].coutTimout = 0;
    Commu_Msg_AT[id].waitTime = 0;
    Commu_Msg_AT[id].errCount =0;
    Commu_Msg.IsWaitAck = COMMU_FALSE;  
    Commu_Sys_FTP_SetState(FTP_IDLE);

    return NULL;
}



/**************************************************************************
*函数功能：FTP获取文件内容的命令
*输入参数：id：指令对应在指令表中的id号
           *pData：指令头后的第一个数据地址
*          len：指令头后的接收数据的总长度
*返回值：  返回该地址后下一条处理指令的头地址，
           NULL:表示由调用函数自己寻找下一条指令应答的起始地址。
           
**************************************************************************/
static uint8_t* Commu_Msg_FTPGetFileRespond(uint8_t id,uint8_t* pData,uint16_t len)
{
   char cmdBuf[COMMU_AT_MAXLEN];
   char c_array[8];
   uint32_t cmdLen;
   int8_t i8GetCmdState;
   int32_t  i32ReqLen = 0;
   int32_t  i32GetLen = 0;
   uint8_t *pos = NULL;
   uint8_t* pHead = NULL;
   uint8_t* pResult = NULL;
   uint32_t ui32Offset = 0;




  i8GetCmdState = AtRing_GetFirstAtCmd(&CommuSys.atTxRing,cmdBuf,sizeof(cmdBuf),&cmdLen);
  if(i8GetCmdState != 0)
  {
    AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
    Commu_Msg_AT[id].coutTimout = 0;
    Commu_Msg_AT[id].waitTime = 0;
    Commu_Msg_AT[id].errCount =0;
    Commu_Msg.IsWaitAck = COMMU_FALSE;  

    return pData+len;

  }

  memset(c_array,0,sizeof(c_array));  
  if(NULL == GetDecAsciiFromBuf(cmdBuf,cmdLen,1,',',c_array,sizeof(c_array)))
  {
    AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
    Commu_Msg_AT[id].coutTimout = 0;
    Commu_Msg_AT[id].waitTime = 0;
    Commu_Msg_AT[id].errCount =0;
    Commu_Msg.IsWaitAck = COMMU_FALSE;  

    return pData+len;
  }
   ui32Offset = atoi(c_array);

  memset(c_array,0,sizeof(c_array));  
  if(NULL == GetDecAsciiFromBuf(cmdBuf,cmdLen,2,',',c_array,sizeof(c_array)))
  {
    AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
    Commu_Msg_AT[id].coutTimout = 0;
    Commu_Msg_AT[id].waitTime = 0;
    Commu_Msg_AT[id].errCount =0;
    Commu_Msg.IsWaitAck = COMMU_FALSE;  

    return pData+len;
  }

  pHead = NULL;
  i32ReqLen = atoi(c_array);

  if(len > strlen("\r\nOK\r\n"))
  {
    
     pos = pData+len - strlen("\r\nOK\r\n");
     pHead = SearchnDataFromBuf(pos,strlen("\r\nOK\r\n"),(uint8_t*)"\r\nOK\r\n",strlen("\r\nOK\r\n"));
     if(pHead != NULL)
     {
        COMMU_PLOG("[commu_ftp]44444444\r\n");
         Commu_Msg.FTP_GetFile_Callback(pData,ui32Offset,i32ReqLen);
          
     }
   }

   if(pHead == NULL)
   {
      
     if(len > strlen("\r\n+QFTPGET:1\r\n"))
     {
        COMMU_PLOG("[commu_ftp]55553\r\n");
        pos = pData+len - strlen("\r\n+QFTPGET:1\r\n");
        pHead = SearchnDataFromBuf(pos,strlen("\r\n+QFTPGET:1\r\n"),(uint8_t*)"TPGET:",strlen("TPGET:"));
        if(pHead != NULL)
        {
           COMMU_PLOG("[commu_ftp]333666666666633333\r\n");
           memset(c_array,0,sizeof(c_array));  
           pHead +=strlen("TPGET:");
           if(NULL != GetDecAsciiFromBuf(pHead,pData+len-pHead,0,'\r',c_array,sizeof(c_array)))
           {
               
               i32GetLen = atoi(c_array);
               if((*pHead == '-')&&(i32GetLen >0))
                 i32GetLen = -i32GetLen;
                 
               COMMU_PLOG("[commu_ftp]3377777,ui32GetLen=%d\r\n",i32GetLen);
               if(i32GetLen >0 )
               {
                   if(i32GetLen >i32ReqLen )
                      Commu_Msg.FTP_GetFile_Callback(pData,ui32Offset,i32ReqLen);
                   else
                      Commu_Msg.FTP_GetFile_Callback(pData,ui32Offset,i32GetLen);
               }
                 
               else
                 Commu_Sys_FTP_SetState(FTP_IDLE);
            
           }
           else
           {
              COMMU_PLOG("[commu_ftp]get error!\r\n");
           }
        }
     }

   }
   pResult= pData+len;
   COMMU_PLOG("[commu_ftp]3388888888888883\r\n");

    AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
    Commu_Msg_AT[id].coutTimout = 0;
    Commu_Msg_AT[id].waitTime = 0;
    Commu_Msg_AT[id].errCount =0;
    Commu_Msg.IsWaitAck = COMMU_FALSE;  

  
  return pResult;
}



/*********************************************************************************
*函数功能：处理电话接听
*函数说明：判断来电的电话号码，然后按照配置信息处理相关功能

*********************************************************************************/
static uint8_t* Commu_Msg_RingWithCLIP(uint8_t id,uint8_t* pData,uint16_t len)
{
  char c_array[16];

  memset(c_array,0,sizeof(c_array));
  
  GetDecAsciiFromBuf(pData,len-len,1,'"',c_array,sizeof(c_array));
  if(strlen(c_array)>=1)
  {
    if(Commu_Msg.PhoneCallInto_CallFunc != NULL)
      Commu_Msg.PhoneCallInto_CallFunc(c_array);
  }
  else
  {
    if(Commu_Msg.PhoneCallInto_CallFunc != NULL)
      Commu_Msg.PhoneCallInto_CallFunc(NULL);
  }
  return NULL;
}


/*********************************************************************************
*函数功能：处理电话接听
*函数说明：判断来电的电话号码，然后按照配置信息处理相关功能

*********************************************************************************/
static uint8_t* Commu_Msg_Ring(uint8_t id,uint8_t* pData,uint16_t len)
{
   if(Commu_Msg.PhoneCallInto_CallFunc != NULL)
      Commu_Msg.PhoneCallInto_CallFunc(NULL);
   return 0;
}





/*********************************************************************************
*函数功能：TTS语音播放完毕
*函数说明：

*********************************************************************************/
static uint8_t* Commu_Msg_TTSPlayOver(uint8_t id,uint8_t* pData,uint16_t len)
{
  Audio_TTSOver_Respond_Hook();
  return NULL;
}

/*********************************************************************************
*函数功能：录音播放完毕
*函数说明：

*********************************************************************************/
static uint8_t* Commu_Msg_RecordPlayOver(uint8_t id,uint8_t* pData,uint16_t len)
{
   Audio_RecordOver_Respond_Hook();
  return NULL;
}



/**********************************************************************************
*函数功能:查询波特率相应超时处理
*输入参数：对应超时的函数ID
*返回参数：:

**********************************************************************************/
static int8_t Commu_Msg_IPRTimout(uint8_t id)
{


  Commu_Msg_AT[id].coutTimout++;
  COMMU_PLOG("[tbox_commu]:1,cmd id=%d,coutTimout=%d\r\n",id,Commu_Msg_AT[id].coutTimout);

  if(Commu_Msg_AT[id].coutTimout <= 15)
  {
    Commu_Msg_AT[id].waitTime = Commu_Msg_AT[id].waitTimout;
    Commu_Msg.IsWaitAck= COMMU_FALSE;

  }
  else
  {
       AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
       Commu_Msg_AT[id].coutTimout = 0;
       Commu_Msg_AT[id].waitTime = 0;
       Commu_Msg_AT[id].errCount =0;

       Commu_Msg.IsWaitAck = COMMU_FALSE;  
       Commu_Sys_Restart();
  }
  
  COMMU_PLOG("[tbox_commu]:timout over!");

  return 0;
 }



static int8_t Commu_Msg_SetIPRTimout(uint8_t id)
{
  Commu_Msg_AT[id].coutTimout++;
  if(Commu_Msg_AT[id].coutTimout <=5)
  {
    Commu_Msg_AT[id].waitTime = Commu_Msg_AT[id].waitTimout;
     Commu_Msg.IsWaitAck = COMMU_FALSE;  
  }
  else
  {
       AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
       Commu_Msg_AT[id].coutTimout = 0;
       Commu_Msg_AT[id].waitTime = 0;
       Commu_Msg_AT[id].errCount =0;

       Commu_Msg.IsWaitAck = COMMU_FALSE;  
       Commu_Sys_SetRunState(COMMU_PRDOWN);
  }
  return 0;

}
static int8_t Commu_Msg_TCPSendTimout(uint8_t id)
{
   AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_TRUE);
   Commu_Msg_AT[id].coutTimout = 0;
   Commu_Msg_AT[id].waitTime = 0;
   Commu_Msg_AT[id].errCount =0;

   Commu_Msg.IsWaitAck = COMMU_FALSE;  

   AtRing_AddAtCmd(&CommuSys.atTxRing,"AT+QIACT?\r\n",NULL,0,COMMU_TRUE);
   AtRing_AddAtCmd(&CommuSys.atTxRing,"AT+QIACT?\r\n",NULL,0,COMMU_TRUE);

   return 0;
}





/*************************************************************************************
*函数功能：Commu异常错误处理机制
*输入参数：*pHead:待处理数据起始地址
           len：待处理数据长度
           cmdId AT命令的序列号
返回参数： *pos：处理数据的结束地址，如果没有查找到ERR数据，则返回NULL
函数说明：该函数处理常见的错误代码情况，对于不常见的错误代码统一发送模块复位指令，
          让模块复位重启后开始新的流程

**************************************************************************************/
static uint8_t* Commu_Msg_ErrProc(uint8_t* pHead,uint16_t len ,uint8_t cmdId)
{
  #define _CMD_ERROR  "ERROR"
	
  uint8_t *pos;
  uint8_t *posTail = NULL;
  uint8_t* rePos ;
  pos = SearchnDataFromBuf(pHead,len,(uint8_t*)_CMD_ERROR,strlen(_CMD_ERROR));
  if(pos != NULL)
  {
    posTail = pos +strlen(_CMD_ERROR);
    
    rePos  = Commu_Msg_ErrProcHook(pHead,len ,cmdId);
    if((rePos >posTail)&&(rePos < pHead+len))
    {
      posTail = rePos;
    }
 
  }	
  return posTail;
}






static uint8_t* Commu_Msg_ErrProcHook(uint8_t* pHead,uint16_t len ,uint8_t cmdId)
{

  if((Commu_Msg_AT[cmdId].cmd != NULL)&&
     (strncmp(Commu_Msg_AT[cmdId].cmd,"AT+CPIN?\r\n",strlen("AT+CPIN?\r\n"))==0))
  {
       
    Commu_Msg_AT[cmdId].errCount++; 
    if(Commu_Msg_AT[cmdId].errCount < 10)
    {
      Commu_Msg_AT[cmdId].coutTimout = 0;
      Commu_Msg_AT[cmdId].waitTime = 0;
      Commu_Msg.IsWaitAck = COMMU_FALSE;
    }
    else
    {
       AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_TRUE);
       Commu_Msg_AT[cmdId].coutTimout = 0;
       Commu_Msg_AT[cmdId].waitTime = 0;
       Commu_Msg_AT[cmdId].errCount =0;

       Commu_Msg.IsWaitAck = COMMU_FALSE;  
       Commu_Sys_SetSIMState(COMMU_FALSE);
       
       //如果需要注册到本地网络，则重启模块
       if(COMMU_REG == Commu_Sys_GetRunObj())
         Commu_Sys_Restart();
    }

  }
  //如果是不需要处理的错误，直接跳过
  else
  {
       AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_TRUE);
       Commu_Msg_AT[cmdId].coutTimout = 0;
       Commu_Msg_AT[cmdId].waitTime = 0;
       Commu_Msg_AT[cmdId].errCount =0;

       Commu_Msg.IsWaitAck = COMMU_FALSE;  
  }


  return NULL;
}



#endif




/**************************************************************
*函数名称:Commu_Msg_SendToUart
*函数功能:将数据发送到串口
*输入参数:pData:数据的起始地址
                           len     :待发送数据长度
*返回参数:实际发送数据的长度


***************************************************************/
static uint32_t Commu_Msg_SendToUart(uint8_t* pData,uint32_t len)
{
  if(Commu_Msg.SendToUart_CallFunc != NULL)
    return Commu_Msg.SendToUart_CallFunc(pData,len);


  return 0;


}

/**********************************************************
*函数名称:Commu_Msg_Timer10ms
*输入参数:Commu AT序列处理结构的定时器
*返回参数:void
*函数说明:该函数每10ms调用一次


**********************************************************/
void Commu_Msg_Timer10ms(void)
{
  static uint32_t s_ui32Ticks = 0;


  s_ui32Ticks++;
  if(s_ui32Ticks%10 == 0)
  {
     Commu_Msg_CheckToSend();
  }
  Commu_Msg_CheckAtCmdTimoutByTimer10ms();
}


/******************************************************************
*函数名称:Commu_Msg_CheckToSend
*函数功能:检测是否有新的AT命令需要发送
*
******************************************************************/
static void Commu_Msg_CheckToSend(void)
{
  char cmdBuf[COMMU_AT_MAXLEN];
  uint32_t cmdLen=0;
  uint8_t i;
  int8_t i8GetCmdState;



  if(Commu_Msg.IsWaitAck== COMMU_FALSE)//如果模块处理空闲，查询是不是有新的AT指令需要发送
  {
    memset(cmdBuf,0,sizeof(cmdBuf));
    i8GetCmdState = AtRing_GetFirstAtCmd(&CommuSys.atTxRing,cmdBuf,sizeof(cmdBuf),&cmdLen);
    if(i8GetCmdState == 0)
    {
      for(i=0;i<Commu_AT_NUM;i++)
      {
        if((Commu_Msg_AT[i].cmd !=NULL)&&(strncmp(cmdBuf,Commu_Msg_AT[i].cmd,strlen(Commu_Msg_AT[i].cmd)) == 0))//不比对最后的‘\n’符号
        {
	       Commu_Msg_AT[i].cmd_func(i,cmdBuf,cmdLen);
          Commu_Msg.IsWaitAck= COMMU_TRUE;
	       break;
        }
      }

      //遍历指令集，未能匹配到指令，
      if(i>= Commu_AT_NUM)
      {
         TBOX_COMMU_DEBUG(("Commu:cmdLen=%d, Send Buf Err:%s|***************************|\r\n",cmdLen,cmdBuf));
           if(0)
          {
             uint32_t ui32ii;
             uint8_t* p_pos = NULL;
              COMMU_PLOG("\r\n|*data start*,pRead=%x,m_buf=%x,m_size=%d|:",CommuSys.atTxRing.pRead,CommuSys.atTxRing.m_buf,CommuSys.atTxRing.m_size);
             for(ui32ii=0;ui32ii<cmdLen;ui32ii++)
             {
                COMMU_PLOG("%x ",cmdBuf[ui32ii]);

             }
             COMMU_PLOG("\r\n|*data end*| \r\n");
              p_pos= CommuSys.atTxRing.pRead;
              COMMU_PLOG("\r\n|*buf data start,p_pos=%x*| \r\n",p_pos);
              
              
             for(ui32ii=0;ui32ii<cmdLen+12;ui32ii++)
             {
                COMMU_PLOG("%x ",*p_pos++);

             }
             COMMU_PLOG("\r\n|*bufdata end*| \r\n");
          }

         AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
      }
    }
    else if(i8GetCmdState == -2)
    {
       AtRing_DeleteAll(&CommuSys.atTxRing,COMMU_TRUE);
    }
    else if(i8GetCmdState == -1)
    {
       AtRing_DeleteFirst(&CommuSys.atTxRing,COMMU_FALSE);
    }
  }


}







/*
*Commu检测指令超时
*/
static void Commu_Msg_CheckAtCmdTimoutByTimer10ms(void)
{
  //遍历数组查询指令是否有超时

  uint8_t i;
  uint8_t result = 0;

  if(Commu_Msg.IsWaitAck== 0)
    return ;

  for(i=0;i<Commu_AT_NUM;i++)
  {
    if(Commu_Msg_AT[i].waitTime > 0)
    {
      Commu_Msg_AT[i].waitTime--;
      if(Commu_Msg_AT[i].waitTime == 0)//延时时间到
      {
        Commu_Msg_AT[i].cmd_timout_func(i);
        COMMU_PLOG("[tbox_commu]timout id=%d\r\n",i);
      }
      result = 1;
    }	
  }
  
  if(result == 0)//当检测到所有的等待信息都为空闲时，置模块为空闲状态
    Commu_Msg.IsWaitAck= COMMU_FALSE;	

}







/**********************************************************************
*函数名称:Commu_Msg_UnpackUsartData
*函数功能:解释Commu模块通过串口接收到的数据
*输入参数:
*返回参数:
*函数说明:pRing不做循环队列，读指针无效，每次
                           使用从pRing->m_ buf起始到pRing->write之间的数据
                           均是需要解包的数据，使用以后清零并
                           复位写指针，未查找到有效数据则保留缓存区域
**********************************************************************/
void Commu_Msg_UnpackUsartData(Commu_RingQueue_t *pRing)
{
  COMMU_BOOL isHaveData = COMMU_TRUE;
  char cmdBuf[COMMU_AT_MAXLEN];
  uint32_t cmdLen=0;
  uint8_t  i; 
  uint16_t dataLen =0;
  uint16_t unpackLen ;
  uint8_t* searchHead;
  uint8_t* searchTail;
  uint8_t* dataTail;
  uint8_t mode = 0;
  uint16_t procLen = 0;
  uint8_t *clearHead;
  uint8_t *clearTail;
  uint8_t* proTail =NULL;
  uint16_t procTotalLen = 0;
  uint8_t ui8CmdAck = 0;

  

  memset(cmdBuf,0,sizeof(cmdBuf));
  if( 0 == AtRing_GetFirstAtCmd(&CommuSys.atTxRing,cmdBuf,sizeof(cmdBuf),&cmdLen))//如果发送了数据，则进行数据应答对比
  {
      mode = 1;//有发送的数据
  }
  else
  {
     mode = 2;//没有发送的数据
  }
     

  #if 1
  {
  	uint8_t* pos_temp =NULL;
  	uint16_t ui16len_temp;
  	uint8_t   ui8return_temp = 0; 
  	COMMU_PLOG("[tbox_commu]Rc,len=%d;bufsize=%d\r\n",pRing->pWrite-pRing->m_buf,pRing->m_size);
  	for(pos_temp=pRing->m_buf;pos_temp<pRing->pWrite;pos_temp++)
  	{
  		   COMMU_PLOG("%c",*pos_temp);

  	}

  }
  #endif


  while(isHaveData) //如果有数据，循环遍历
  {
    isHaveData = COMMU_FALSE;
    ui8CmdAck = 0;
    for(i=0;i<Commu_AT_NUM;i++)//这里做一次AT指令遍历
    {
      if(((mode == 1)&&(Commu_Msg_AT[i].cmd != NULL)&&
         ((0 == strncmp(cmdBuf,Commu_Msg_AT[i].cmd,strlen(Commu_Msg_AT[i].cmd)))))||
         (Commu_Msg_AT[i].cmd == NULL))//找到发送帧不比对最后的‘\n’符号
      {
        
        
        dataLen = pRing->pWrite - pRing->m_buf;
        unpackLen = dataLen;
        /*匹配搜索返回指令搜索内容在内存块中的首次出现的首地址*/
        if(Commu_Msg_AT[i].cmd_ackhead != NULL)
          searchHead = SearchnDataFromBuf(pRing->m_buf,unpackLen,(uint8_t*)Commu_Msg_AT[i].cmd_ackhead,strlen(Commu_Msg_AT[i].cmd_ackhead));
        else
          searchHead = pRing->m_buf;

			
        if(searchHead != NULL)//如果匹配到头
        {  
          clearHead = searchHead;
          //移除指令头
          if(Commu_Msg_AT[i].cmd_ackhead != NULL)
          {
            searchHead = searchHead + strlen(Commu_Msg_AT[i].cmd_ackhead);
          }

          unpackLen = (pRing->m_buf +dataLen )- searchHead;
          //匹配指令尾
          if(Commu_Msg_AT[i].cmd_acktail == NULL)
          {
            searchTail = searchHead;
          }
          else
          {
              searchTail = SearchnDataFromBuf(searchHead,unpackLen,(uint8_t*)Commu_Msg_AT[i].cmd_acktail,strlen(Commu_Msg_AT[i].cmd_acktail));
          }
          
           if(searchTail != NULL)
          {
            
            if(Commu_Msg_AT[i].cmd_acktail != NULL)
            {
               clearTail = searchTail + strlen(Commu_Msg_AT[i].cmd_acktail);
            }
            else
            {
				clearTail = searchTail;
            }
            
            unpackLen =   (pRing->m_buf +dataLen )- searchHead;//此次接收未解释的数据
            dataTail = Commu_Msg_AT[i].cmd_ack_func(i, searchHead,unpackLen);//清除头跟数据体，保留帧尾
            //移除指令尾
            //COMMU_PLOG("******dataTail=%x;pRing->m_buf=%x;dataLen=%x;searchHead=%x,unpackLen=%x\r\n",dataTail,pRing->m_buf,dataLen,searchHead,unpackLen);
            if((dataTail <= pRing->m_buf+ dataLen)&&(dataTail >= searchHead))//检测返回尾指针的有效性
            {
              //由于在"+ZIPRECV:"数据块中可能存在命令结束符，如果数据处理结束地址有效，我们需要再次搜索命令结束符
              clearTail = dataTail; 
            }
            COMMU_PLOG("******dataTail=%x;pRing->m_buf=%x;dataLen=%x;searchHead=%x,unpackLen=%x,clearHead=%x,clearTail=%x,strlen(null)=%d\r\n",dataTail,pRing->m_buf,dataLen,searchHead,unpackLen,clearHead,clearTail,strlen(Commu_Msg_AT[i].cmd_acktail));
            memset(clearHead,0,(uint32_t)(clearTail-clearHead));
             ui8CmdAck = 1;
            /*计算得到最尾部的指针*/
            if(clearTail > proTail)
              proTail = clearTail;
				
            /*如果还有数据未处理,退出for重新遍历结构*/
            if(proTail < pRing->m_buf+dataLen)
            {
              isHaveData = COMMU_TRUE;	
              break;
            }
                			
          }

        }

        //如果接收到的数据不是AT 指令的正确响应，则处理是否为ERROR响应

      }
    }		

    
    for(i=0;i<Commu_AT_NUM;i++)
    {
        if(((mode == 1)&&(Commu_Msg_AT[i].cmd != NULL)&&((0 == strncmp(cmdBuf,Commu_Msg_AT[i].cmd,strlen(Commu_Msg_AT[i].cmd)))))&&(ui8CmdAck == 0))
        {
          
          dataTail = Commu_Msg_ErrProc(pRing->m_buf,dataLen,i);
          //移除指令尾
          if((dataTail <= pRing->m_buf+ dataLen)&&(dataTail > pRing->m_buf))//检测返回尾指针的有效性
          {
            //由于在"+ZIPRECV:"数据块中可能存在命令结束符，如果数据处理结束地址有效，我们需要再次搜索命令结束符
            clearTail = dataTail; 

            memset(pRing->m_buf,0,(uint32_t)(clearTail-pRing->m_buf));
             /*计算得到最尾部的指针*/
            if(clearTail > proTail)
              proTail = clearTail;
            isHaveData = COMMU_TRUE;
          }   
        }

    }

    
  }
   //将指针移动到处理的数据块后
   if(proTail != 0)
   {
	  procLen = proTail - pRing->m_buf;
	  if(procLen != 0)
	  {
	    RxRing_DeleteDataByLen(pRing,procLen);
	  }

   }
   else
   {
   }


  //如果存储空间仍然是满的，则直接清除该空间
  if(pRing->pWrite-pRing->m_buf  >= pRing->m_size -1)
  {
     RxRing_DeleteDataByLen(pRing,pRing->m_size);
  }

}



