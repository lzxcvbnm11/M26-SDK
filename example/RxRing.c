#include "RxRing.h"

/*******************************************************************
*��������:RxRing_DeleteDataByLen
*��������:ͨ������ɾ��pRing�е�����
*�������:pRing     :���нṹ��
                            deleteLen:��ɾ�������ݳ���,0:��ʾɾ����������
*���ز���: ɾ�����ݵ�ʵ�ʳ���

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
*��������:RxRing_AddData
*��������:��������������
*�������:pRing    :����ָ��
                           pData    :������ʼ��ַ
                           len         :���ݳ���
*���ز���:ʵ��д������ݳ���
**********************************************************************/
uint32_t RxRing_AddData(Commu_RingQueue_t *pRing,uint8_t* pData,uint32_t len)
{
  uint32_t ui32RemainBytes = 0;
  uint32_t writeLen = 0;

  if(pRing == NULL)
    return 0;

  
  ui32RemainBytes = pRing->m_size - (pRing->pWrite - pRing->m_buf)-1;
  //���ʣ��Ŀռ����Ҫд��Ŀռ�
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





