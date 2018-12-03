/*---------------------------------------------------------------------------
*AtRing.c
*


*说明:
   AtSendRing 存储格式为:
   |4Bytes NextAddr|4Bytes DataStartAddr|N Bytes data|

   存储样式为:
   |4Bytes NextAddr| 4Bytes DataStartAddr| AT CMD| PROTOCOL DATA|
   




*---------------------------------------------------------------------------*/


#include "AtRing.h"
#include <stdlib.h>
#include <string.h>

static uint32_t AtRing_GetTransLen_Default(uint8_t* data,uint32_t len);
static uint32_t AtRing_TransMemcpyToRing_Default(uint8_t* ringBase,uint32_t ringSize,uint8_t* pCurrent,uint8_t* srcBuf,uint32_t srcLen,uint8_t flag);
static uint32_t AtRing_SendFailed_Default(uint8_t* ringBase,uint32_t ringSize,uint8_t* pData,uint32_t datalen,char* pCmd);
static uint8_t* AtRing_GetAddr(uint8_t* baseAddr,uint32_t ringSize,uint8_t* addr);
static void AtRing_ClearRing(Commu_RingQueue_t *pRing);




 AtRing_GetTransLen_Callback             AtRing_GetTransLen = AtRing_GetTransLen_Default;
static AtRing_TransMemcpyToRing_Callback AtRing_TransMemcpyToRing = AtRing_TransMemcpyToRing_Default;
static AtRing_SendFailed_Callback                  AtRing_SendFailed                  = AtRing_SendFailed_Default;

static uint32_t AtRing_GetTransLen_Default(uint8_t* data,uint32_t len)
{

  return len;
}

static uint32_t AtRing_TransMemcpyToRing_Default(uint8_t* ringBase,uint32_t ringSize,uint8_t* pCurrent,uint8_t* srcBuf,uint32_t srcLen,uint8_t flag)
{
  uint32_t ui32i;

  if((ringBase== NULL)||(pCurrent==NULL)||(srcBuf == NULL))
    return 0;
  
  for(ui32i=0;ui32i<srcLen;ui32i++)
  {
    *pCurrent++ = *srcBuf++;
    if(pCurrent >=ringBase+ringSize  )
      pCurrent = ringBase;
  }
  return srcLen;


}

static uint32_t AtRing_SendFailed_Default(uint8_t* ringBase,uint32_t ringSize,uint8_t* pData,uint32_t datalen,char* pCmd)
{

  return 0;
}


/*********************************************************************
*函数名称:AtRing_DeletePartOfFirst
*函数功能:删除队列里面的数据
*输入参数: pRing :循环队列指针
                            offset :从数据开始地址的偏移量
                            size    :待删除数据的长度，0 表示删除整帧
*返回值      : 删除的数据长度
*
*********************************************************************/
uint32_t AtRing_DeleteDataOfFirst(Commu_RingQueue_t* pRing,uint32_t offset,uint32_t size)
{
  uint8_t* nextAddr;
  uint8_t* dataStartAddr;
  uint8_t* pos;
  uint32_t tmp = 0;
  uint8_t   ui8i=4;
  uint8_t* deleteAddr = NULL;
  uint8_t   shift;
  uint32_t ui32DataLen;
  uint32_t ui32DeleteLen;
	
  if(pRing == NULL)
    return 0;
	
  if(pRing->pRead == pRing->pWrite)//没有可读取的数据
    return 0;
 //gpOS_semaphore_wait(pRing->Access_sem);

    
    
  pos = pRing->pRead;
  nextAddr = AtRing_GetAddr(pRing->m_buf,pRing->m_size,pos);
  pos += 4;
  if(pos >= pRing->m_buf+pRing->m_size)
    pos -= pRing->m_size;
  dataStartAddr = AtRing_GetAddr(pRing->m_buf,pRing->m_size,pos);

  //清除该帧所有数据
  if(size == 0)
  {
    dataStartAddr = nextAddr;
    
    if(nextAddr >= dataStartAddr)
       ui32DeleteLen = nextAddr - dataStartAddr;
    else
       ui32DeleteLen = pRing->m_size - (dataStartAddr - nextAddr);
    
  }
  else
  {
    deleteAddr = dataStartAddr + offset;
    if(deleteAddr >= pRing->m_buf + pRing->m_size)
    {
        deleteAddr -= pRing->m_size;
    }
    
    if(nextAddr >= deleteAddr)
       ui32DataLen = nextAddr - deleteAddr;
    else
       ui32DataLen = pRing->m_size - (deleteAddr - nextAddr);
       
    if(ui32DataLen >size)
      ui32DeleteLen = size;
    else
      ui32DeleteLen = ui32DataLen;
    
     deleteAddr +=  ui32DeleteLen;
     
     if(deleteAddr >= pRing->m_buf + pRing->m_size)
       deleteAddr -= pRing->m_size;

     dataStartAddr = deleteAddr;
  }
      //修改Data的起始地址
  pos = pRing->pRead;
  pos+= 4;
  if(pos >= pRing->m_buf+pRing->m_size)
    pos -= pRing->m_size;

  ui8i=4;
  tmp = (uint32_t)dataStartAddr;
  while(ui8i)
  {
    ui8i--;
    shift = 8*ui8i;
    *pos++ = (tmp&(0xff<<shift))>>shift;
    if(pos >=  pRing->m_buf + pRing->m_size)
      pos= pRing->m_buf;	
  }
    

 //gpOS_semaphore_signal(pRing->Access_sem);  
 
  return ui32DeleteLen;

}





/*********************************************************************
*函数名称:AtRing_AddHead
*函数功能:向队列中添加一帧数据的头信息
*输入参数: pRing       :循环队列指针
                            nextAddr  :头信息的内容
*返回值      : 0:添加头成功;-1:添加头失败
*函数说明:如果头添加成功，说明循环队列有足够的空间
                            来装载待添加的数据
*********************************************************************/
static int8_t AtRing_AddHead(Commu_RingQueue_t* pRing,uint8_t* nextAddr)
{
  uint32_t tmp = 0;
  uint8_t  ui8i;
  uint8_t shift;
  
  if((nextAddr < pRing->m_buf)||(nextAddr >= pRing->m_buf+pRing->m_size))
    return -1;

  //判断是否越界
  if(((pRing->pWrite < pRing->pRead)&&((nextAddr >= pRing->pRead)||(nextAddr<=pRing->pWrite)))||
      ((pRing->pWrite > pRing->pRead)&&((nextAddr>=pRing->pRead)&&(nextAddr <=pRing->pWrite ))))
    return -1;

  tmp = (uint32_t)nextAddr;
  ui8i=4;
  while(ui8i)
  {
    ui8i--;
    shift = 8*ui8i;
    *pRing->pWrite++ = (tmp&(0xff<<shift))>>shift;
    if(pRing->pWrite >=  pRing->m_buf + pRing->m_size)
      pRing->pWrite = pRing->m_buf;	
  }
  return 0;


}


/*********************************************************************
*函数名称:AtRing_AddDataStartAddr
*函数功能:向队列中添加数据起始地址
*输入参数: pRing               :循环队列指针
                            dataStartAddr   :数据起始地址
*返回值      : 0:添加头成功;-1:添加头失败
*函数说明:如果头添加成功，说明循环队列有足够的空间
                            来装载待添加的数据
*********************************************************************/
static int8_t AtRing_AddDataStartAddr(Commu_RingQueue_t* pRing,uint8_t* dataStartAddr)
{
  uint32_t tmp = 0;
  uint8_t  i=4;
  uint8_t shift;
  if((dataStartAddr < pRing->m_buf)||(dataStartAddr >= pRing->m_buf+pRing->m_size))
    return -1;

  tmp = (uint32_t)dataStartAddr;
  while(i)
  {
    i--;
    shift = 8*i;
    *pRing->pWrite++ = (tmp&(0xff<<shift))>>shift;
    if(pRing->pWrite >= pRing->m_buf + pRing->m_size)
      pRing->pWrite = pRing->m_buf;	
  }
  return 0;


}



/*********************************************************************
*函数名称:AtRing_AddData
*函数功能:向队列中添加数据
*输入参数: pRing               :循环队列指针
                            data                 :数据内容
                            len                    :数据长度
*返回值      : 0:添加头成功;-1:添加头失败
*函数说明:如果头添加成功，说明循环队列有足够的空间
                            来装载待添加的数据
*********************************************************************/
static int8_t AtRing_AddData(Commu_RingQueue_t* pRing,uint8_t* data,uint16_t len)
{
  uint16_t freeBytes;
  uint16_t addBytes;
  uint8_t* pBuf = data;

  if(data == NULL)
    return -1;

  if(pRing->pWrite <= pRing->pRead)
    freeBytes = pRing->pRead-pRing->pWrite-1;//写指针不能跟读指针重合
  else 
    freeBytes =pRing->m_size - (pRing->pWrite - pRing->pRead)-1;
	
  addBytes = len;
  if(freeBytes < addBytes)//没有足够的存储空间
    return -1;
	
  while(addBytes--)
  {
    *pRing->pWrite++ = *pBuf++;
    if(pRing->pWrite >= pRing->m_buf + pRing->m_size)
      pRing->pWrite = pRing->m_buf;
  }
  return 0;	


}


/***********************************************************************
*函数名称:AtRing_AddAtCmd
*函数功能:向队列中添加一个AT指令序列
*输入参数:pRing   :队列指针
                           pCmd   :AT命令部分信息
                           data     :AT命令带有数据部分内容，例如
                                        "AT+QSEND=0,10\r\n"0123456789
                           dataLen:数据部分内容的长度
                           isHoldRepeat:是否防止重复添加，该指令只判断pCmd部分
*返回参数:COMMU_FAILED:操作失败
                            COMMU_SUCCESS:操作成功

*函数说明:


***********************************************************************/
COMMU_ERROR AtRing_AddAtCmd(Commu_RingQueue_t* pRing,char* pCmd,uint8_t* data,uint32_t dataLen,COMMU_BOOL isHoldRepeat)
{

  uint8_t* pos = NULL;
  uint8_t* pos1 = NULL;
  COMMU_BOOL isHaveSame = COMMU_FALSE;
  uint32_t ui32i;
  uint32_t frameLen;
  uint32_t transLen;
  uint8_t* nextAddr ;
  uint8_t* dataStartAddr;
  uint32_t ui32CopyLen;
  COMMU_ERROR     result =COMMU_FAILED;

   //gpOS_semaphore_wait(pRing->Access_sem);

  if((isHoldRepeat == COMMU_TRUE)&&(pRing->pRead != pRing->pWrite))
  {
    if(pRing->pRead < pRing->pWrite)
    {
        pos = pRing->pRead;
        while(pos<pRing->pWrite)
        {
             pos1 =pos + 8;
             isHaveSame = COMMU_TRUE;
             for(ui32i=0;ui32i<strlen(pCmd);ui32i++)
             {
                if(pCmd[ui32i] != *pos1++)
                {
                   isHaveSame = COMMU_FALSE;
                   break;//for
                }
             }
             if(isHaveSame == COMMU_TRUE)
             {
                break;//while
             }  
             pos = AtRing_GetAddr(pRing->m_buf,pRing->m_size,pos);

        }
    }
    else if(pRing->pRead > pRing->pWrite)
    {
        pos = pRing->pRead;
        while(pos < pRing->m_buf + pRing->m_size)
        {
             pos1 =pos + 8;
             isHaveSame = COMMU_TRUE;
             for(ui32i=0;ui32i<strlen(pCmd);ui32i++)
             {
                if(pCmd[ui32i] != *pos1++)
                {
                   isHaveSame = COMMU_FALSE;
                   break;//for
                }
             }
             if(isHaveSame == COMMU_TRUE)
             {
                break;//while
             } 
             pos = AtRing_GetAddr(pRing->m_buf,pRing->m_size,pos);
        }

        pos = pRing->m_buf;
        while(pos < pRing->pWrite)
        {
             pos1 =pos + 8;
             isHaveSame = COMMU_TRUE;
             for(ui32i=0;ui32i<strlen(pCmd);ui32i++)
             {
                if(pCmd[ui32i] != *pos1++)
                {
                   isHaveSame = COMMU_FALSE;
                   break;//for
                }
             }
             if(isHaveSame == COMMU_TRUE)
             {
                break;//while
             } 
             pos = AtRing_GetAddr(pRing->m_buf,pRing->m_size,pos);
        }

    }

  }

  //如果没有相同的AT命令
  if((isHoldRepeat == COMMU_FALSE)||(isHaveSame == COMMU_FALSE))
  {
    if((data != NULL)&&(dataLen != 0))
   {
       transLen = AtRing_GetTransLen(data,dataLen );
    }
    
    
    frameLen = strlen(pCmd) + dataLen;
    
    /*下帧数据起始地址 = 当前写入地址 +待写入数据长度 +帧头长度4 +帧数据起始地址长度4*/
    nextAddr = pRing->pWrite + frameLen+4+4;
    if(nextAddr >=pRing->m_buf+pRing->m_size)
      nextAddr -= pRing->m_size;	

    if( 0 == AtRing_AddHead(pRing,nextAddr))
    {
		
      /*帧头已写入，接着写入数据起始地址 = 当前地址 +帧数据起始地址长度4+帧头长度*/
      dataStartAddr =pRing->pWrite +4+strlen(pCmd);
      if(dataStartAddr >= pRing->m_buf+pRing->m_size)
        dataStartAddr -= pRing->m_size;					
      AtRing_AddDataStartAddr(pRing,dataStartAddr);
   
      /*帧数据起始头已写入，开始写入数据*/
      AtRing_AddData(pRing,(uint8_t*)pCmd,strlen(pCmd));
      //前面已经检测过了有足够的空间存储该段数据信息
      if((data != NULL)&&(dataLen != 0))
      {
        ui32CopyLen = AtRing_TransMemcpyToRing(pRing->m_buf,
                                                                              pRing->m_size,
                                                                              pRing->pWrite,
                                                                              data,
                                                                              dataLen,
                                                                              0);
        pRing->pWrite += ui32CopyLen;
      }

      result = COMMU_SUCCESS;
      if(pRing->pWrite >= pRing->m_buf + pRing->m_size)
        pRing->pWrite -= pRing->m_size;
    }
  }

 //gpOS_semaphore_signal(pRing->Access_sem);
  return result;
}








/************************************************************************
*函数名称:AtRing_GetFirstAtCmd
*函数功能:从队列中获取第一帧数据的命令部分信息
*输入参数:pRing:队列指针
                           buf   :命令部分存放的地址指针
                           size  :buf的空间大小
                           pLen :读取命令的长度
            
*返回参数：-3：没有数据可读； 
                                -2：首条命令为无效命令，待清除。
                                -1：存放命令的空间不够 
                                  0：读取命令成功
*函数说明:


***************************************************************************/
int8_t AtRing_GetFirstAtCmd(Commu_RingQueue_t* pRing,char* buf,uint32_t size,uint32_t* pLen)
{
  uint8_t* pos =NULL;
  uint8_t* nextAddr = NULL;
  uint8_t* dataStartAddr = NULL;
  uint8_t* pRead = NULL;
  uint32_t cmdLen = 0;
  int8_t     result = -3;


  if(pRing == NULL)
    return -2;
    
  else if(buf == NULL)
    return  -1;
  
  if(pRing->pRead == pRing->pWrite)
    return -3;

// gpOS_semaphore_wait(pRing->Access_sem);

  pos = pRing->pRead;
  nextAddr = AtRing_GetAddr(pRing->m_buf,pRing->m_size,pos);
  pos += 4;
  if(pos >= pRing->m_buf+pRing->m_size)
    pos -= pRing->m_size;
  dataStartAddr = AtRing_GetAddr(pRing->m_buf,pRing->m_size,pos);



  if(((nextAddr>=pRing->m_buf)&&(nextAddr <= pRing->m_buf + pRing->m_size))&&
      ((dataStartAddr>=pRing->m_buf)&&(dataStartAddr <= pRing->m_buf + pRing->m_size)))
  {
    pos = buf;
    pRead = pRing->pRead +4+4;
    while(size--)
    {
      if(pRead >= pRing->m_buf + pRing->m_size)
        pRead -= pRing->m_size;

      if(pRead == dataStartAddr)
        break;

      *pos++ = *pRead++;
      cmdLen++;

        

      
    }
    result = 0;
  }


 //gpOS_semaphore_signal(pRing->Access_sem);
 
  *pLen = cmdLen;
  return result;
}


/***************************************************************
*函数名称:AtRing_GetFirstData
*函数功能:从队列中获取第一帧命令的数据段
*输入参数:pRing   :队列指针
                           offset   :获取数据的偏移量
                           readlen :读取数据的长度
                           buf       :读取数据完毕后存储的空间
                           size      :buf的大小
*返回参数:读取数据的长度
*函数说明:当帧里面的数据长度小于读取长度时，
                           直接读取帧内所有的数据长度
**************************************************************************/
uint32_t AtRing_GetFirstData(Commu_RingQueue_t* pRing,uint32_t offset,uint32_t readlen,char* buf,uint32_t size)
{
  uint8_t* pos =NULL;
  uint8_t* nextAddr = NULL;
  uint8_t* dataStartAddr = NULL;
  uint8_t* pRead = NULL;
  uint32_t getDataLen = 0;
  uint32_t totalDataLen;


  if(pRing == NULL)
    return 0;
    
  else if(buf == NULL)
    return  0;
  
  if(pRing->pRead == pRing->pWrite)
    return 0;

    
 //gpOS_semaphore_wait(pRing->Access_sem);


  pos = pRing->pRead;
  nextAddr = AtRing_GetAddr(pRing->m_buf,pRing->m_size,pos);
  pos += 4;
  if(pos >= pRing->m_buf+pRing->m_size)
    pos -= pRing->m_size;
  dataStartAddr = AtRing_GetAddr(pRing->m_buf,pRing->m_size,pos);

  if(((nextAddr>=pRing->m_buf)&&(nextAddr < pRing->m_buf + pRing->m_size))&&
      ((dataStartAddr>=pRing->m_buf)&&(dataStartAddr < pRing->m_buf + pRing->m_size)))
  {
    if(nextAddr >= dataStartAddr)
      totalDataLen =  nextAddr - dataStartAddr;
    else
      totalDataLen =  pRing->m_size - (dataStartAddr - nextAddr);

    //移动的长度是否过大
    if(offset < totalDataLen)
    {
      pos = buf;
      pRead = dataStartAddr + offset;
      if(pRead >= pRing->m_buf + pRing->m_size)
        pRead -= pRing->m_size;

      getDataLen = 0;
      while((readlen--)&&(size--))
      {
        if(pRead >= pRing->m_buf + pRing->m_size)
          pRead -= pRing->m_size;
          
        if(pRead == nextAddr)
          break;
      
        *pos++ = *pRead++;
        getDataLen++;

      }

    }

  }



 // gpOS_semaphore_signal(pRing->Access_sem);
  return getDataLen;
}



/***************************************************************
*函数名称:AtRing_GetFirstDataLen
*函数功能:从队列中获取第一帧命令的数据段的长度
*输入参数:pRing   :队列指针
                           
*返回参数:数据段长度
*函数说明:
**************************************************************************/
uint32_t AtRing_GetFirstDataLen(Commu_RingQueue_t *pRing)
{

   uint8_t* pos =NULL;
  uint8_t* nextAddr = NULL;
  uint8_t* dataStartAddr = NULL;

  uint32_t totalDataLen = 0;
  if(pRing == NULL)
    return 0;




  if(pRing == NULL)
    return 0;
    
  
  if(pRing->pRead == pRing->pWrite)
    return 0;

 //gpOS_semaphore_wait(pRing->Access_sem);

 
  pos = pRing->pRead;
  nextAddr = AtRing_GetAddr(pRing->m_buf,pRing->m_size,pos);
  pos += 4;
  if(pos >= pRing->m_buf+pRing->m_size)
    pos -= pRing->m_size;
  dataStartAddr = AtRing_GetAddr(pRing->m_buf,pRing->m_size,pos);

  if(((nextAddr>=pRing->m_buf)&&(nextAddr < pRing->m_buf + pRing->m_size))&&
      ((dataStartAddr>=pRing->m_buf)&&(dataStartAddr < pRing->m_buf + pRing->m_size)))
  {
    if(nextAddr >= dataStartAddr)
      totalDataLen =  nextAddr - dataStartAddr;
    else
      totalDataLen =  pRing->m_size - (dataStartAddr - nextAddr);


  }

 //gpOS_semaphore_signal(pRing->Access_sem);
  return totalDataLen;
    

}


/*************************************************************************
*函数名称:AtRing_DeleteAll
*函数功能:清除队列中所有数据
*输入参数:pRing 队列指针
                           isSaveData :是否调用数据存储的回调函数
*返回参数:0 ：清除成功；
                           -1：没数据可清除；
                           -2：清除失败
***************************************************************************/
int8_t AtRing_DeleteAll(Commu_RingQueue_t *pRing,COMMU_BOOL isSaveData)
{
  volatile int8_t deleteResult = 0;
  if(isSaveData == COMMU_TRUE)
  {
    while(deleteResult == 0)
    {
        deleteResult = AtRing_DeleteFirst(pRing,isSaveData);

    }

  }
  AtRing_ClearRing(pRing);

}



/*************************************************************************
*函数名称:AtRing_DeleteFirst
*函数功能:清除第一条命令
*输入参数:pRing 队列指针
                           isSaveData :是否调用数据存储的回调函数
*返回参数:0 ：清除成功；
                           -1：没数据可清除；
                           -2：清除失败
***************************************************************************/
int8_t AtRing_DeleteFirst(Commu_RingQueue_t *pRing,COMMU_BOOL isSaveData)
{
  uint8_t* nextAddr;
  uint32_t tmp = 0;
  uint8_t i=4;
  int8_t result = -1;
  uint8_t* pos = NULL;
  uint8_t* dataStartAddr = NULL;
  uint32_t datalen = 0;
  char c_array[COMMU_AT_MAXLEN];
  uint32_t ui32Count = 0;
  
	
	
  if(pRing->pRead == pRing->pWrite)//没有可读取的数据
    return -1;

    
 //gpOS_semaphore_wait(pRing->Access_sem);

  nextAddr = AtRing_GetAddr(pRing->m_buf,pRing->m_size,pRing->pRead);
  
  pos = pRing->pRead+4;
  if(pos >= pRing->m_buf+pRing->m_size)
    pos -= pRing->m_size;

    
  dataStartAddr = AtRing_GetAddr(pRing->m_buf,pRing->m_size,pRing->pRead);
  
  if(((nextAddr >=pRing->m_buf)&&(nextAddr < pRing->m_buf +pRing->m_size))&&
     ((dataStartAddr >=pRing->m_buf)&&(dataStartAddr < pRing->m_buf +pRing->m_size)))
  {
    //调用数据存储回调接口
    if(isSaveData == COMMU_TRUE)
    {
      if(nextAddr >= dataStartAddr)
        datalen = nextAddr - dataStartAddr;
      else
         datalen = pRing->m_size - (dataStartAddr - nextAddr);
      pos = pRing->pRead + 8;
      if(pos >= pRing->m_buf+pRing->m_size)
        pos -= pRing->m_size;    

        
      memset(c_array,0,sizeof(c_array));
      ui32Count = 0;
      while((pos != dataStartAddr)&&(ui32Count < sizeof(c_array)))
      {
         c_array[ui32Count++] = *pos++;
      }
      
      if(datalen != 0)
      {
          AtRing_SendFailed(pRing->m_buf,pRing->m_size,dataStartAddr,datalen,c_array);
      }
    }

    while((  pRing->pRead != nextAddr)&&(NULL != nextAddr))
    {
      *pRing->pRead++ =0;
      if(pRing->pRead > pRing->m_buf + pRing->m_size-1)
        pRing->pRead = pRing->m_buf;
    }	
    result = 0;
  }
  else
  {
    result = -2;
  }

 //gpOS_semaphore_signal(pRing->Access_sem);

 
  return result;
}


/***************************************************************
*函数名称:tbox_commu_clearRing
*函数功能:清除RingQueue里的所有数据
*输入参数:pRing 队列指针
*返回参数: 0:清除成功;
                           -1:清除失败;

***************************************************************/
static void AtRing_ClearRing(Commu_RingQueue_t *pRing)
{

  //gpOS_semaphore_wait(pRing->Access_sem);

  pRing->pRead = pRing->m_buf;
  pRing->pWrite = pRing->m_buf;
  memset(pRing->m_buf,0,pRing->m_size);


 //gpOS_semaphore_signal(pRing->Access_sem);
}




/*****************************************************************
*函数名称:AtRing_GetAddr
*函数功能:通过Ring的起始地址，合成一个uint8_t* 的指针
*输入参数:baseAddr  :Ring Buf的起始地址
                           ringSize     :RING空间大小
                           addr          :待合成uint8_t* 的起始地址
*返回参数:获取到的指针地址

*函数说明:指针地址按照大端排列
                           


******************************************************************/
static uint8_t* AtRing_GetAddr(uint8_t* baseAddr,uint32_t ringSize,uint8_t* addr)
{
  uint8_t ui8i;
  uint32_t ui32Temp;
  
  ui8i=4;
  ui32Temp = 0;
  while(ui8i--)
  {

    if(addr >= baseAddr + ringSize)
      addr = baseAddr;

    ui32Temp = (ui32Temp<<8) +(*addr++);

  }

  return (uint8_t*)ui32Temp;

}



/*********************************************************************
*函数名称:AtRing_GetTransLen_RegCallFunc
*函数功能:注册获取转义数据长度的回调函数
*输入参数:pFunc 函数指针
*返回参数:COMMU_SUCCESS:注册成功
                           COMMU_FALSE      :注册失败
*函数说明:


*********************************************************************/
COMMU_ERROR  AtRing_GetTransLen_RegCallFunc(AtRing_GetTransLen_Callback pFunc)
{
   if(pFunc == NULL)
     return COMMU_FALSE;
     
   AtRing_GetTransLen = pFunc;

   return COMMU_SUCCESS;
}


/*********************************************************************
*函数名称:AtRing_TransMemcpyToRing_RegCallFunc
*函数功能:注册转义拷贝的回调函数
*输入参数:pFunc 函数指针
*返回参数:COMMU_SUCCESS:注册成功
                           COMMU_FALSE      :注册失败
*函数说明:


*********************************************************************/
COMMU_ERROR  AtRing_TransMemcpyToRing_RegCallFunc(AtRing_TransMemcpyToRing_Callback pFunc)
{
   if(pFunc == NULL)
     return COMMU_FALSE;
     
   AtRing_TransMemcpyToRing = pFunc;

   return COMMU_SUCCESS;
}


/*********************************************************************
*函数名称:AtRing_TransMemcpyToRing_RegCallFunc
*函数功能:注册转义拷贝的回调函数
*输入参数:pFunc 函数指针
*返回参数:COMMU_SUCCESS:注册成功
                           COMMU_FALSE      :注册失败
*函数说明:


*********************************************************************/
COMMU_ERROR  AtRing_AtRing_SendFailed_RegCallFunc(AtRing_SendFailed_Callback pFunc)
{
   if(pFunc == NULL)
     return COMMU_FALSE;
     
   AtRing_SendFailed = pFunc;

   return COMMU_SUCCESS;
}




















