#include "RxRing.h"

/*******************************************************************
*函数名称:RxRing_DeleteDataByLen
*函数功能:通过长度删除pRing中的数据
*输入参数:pRing     :队列结构体
                            deleteLen:待删除的数据长度,0:表示删除所有数据
*返回参数: 删除数据的实际长度

*******************************************************************/
uint32_t RxRing_DeleteDataByLen(Commu_RingQueue_t *pRing,uint32_t deleteLen)
{
  uint8_t* pos = NULL;
  uint32_t ui32Count = 0;
  uint32_t ui32Remain = 0;
  uint32_t ui32deletelen = 0;
  if(pRing == NULL)
    return 0;

 

  if((deleteLen >= pRing->pWrite - pRing->m_buf)||(deleteLen == 0))
  {
     pRing->pWrite = pRing->m_buf;
     ui32deletelen = pRing->pWrite - pRing->m_buf;
  }
  else
  {
    ui32Count = 0;
    pos = pRing->m_buf + deleteLen;
    ui32Remain = pRing->pWrite - pos;
        
    while((ui32Count < deleteLen)&&(ui32Count < ui32Remain))
    {
       pRing->m_buf[ui32Count++] = *pos++;
    }
    pRing->pWrite = pRing->m_buf + deleteLen;
    ui32deletelen = deleteLen;
  }
  return ui32deletelen;

}



/*********************************************************************
*函数名称:RxRing_AddData
*函数功能:向队列中添加数据
*输入参数:pRing    :队列指针
                           pData    :数据起始地址
                           len         :数据长度
*返回参数:实际写入的数据长度
**********************************************************************/
uint32_t RxRing_AddData(Commu_RingQueue_t *pRing,uint8_t* pData,uint32_t len)
{
  uint32_t ui32RemainBytes = 0;
  uint32_t writeLen = 0;

  if(pRing == NULL)
    return 0;

  
  ui32RemainBytes = pRing->m_size - (pRing->pWrite - pRing->m_buf)-1;
  //如果剩余的空间大于要写入的空间
  if(ui32RemainBytes >= len)
  {
    while(len--)
    {
       *pRing->pWrite++ = *pData++;

    }
    writeLen = len; 
  }
  
  return writeLen;
}





