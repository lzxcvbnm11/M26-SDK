/*------------------------------������Ƽ��ɷ����޹�˾--------------------------------------*
*�ļ�����:
*����:
*�����汾:
*�����汾����:
*�ļ�����:
*
*-------------------------------��ʷ�޸ļ�¼-------------------------------------------------
*�޸���:
*�޶��汾��:
*�޸�����:
*�޸�����:
*
*--------------------------------------------------------------------------------------------*/




#include "string.h"
#include "Commu_Config.h"
#include "Commu_CommFunc.h"



/*--------------------------------------------------------------------------------
*�������ܣ��Ӵ��ڻ��������½���������������������   ���ǻ��λ�����
*���������*p_bufhead:���λ������׵�ַ
*           bufSize: ��������С
*          *pos��������ʼ��ַ
*           datalen: ���������ݿ鳤��
*          *searchArray:�����������׵�ַ
*          searchLen: ���������鳤��
*���ز����������ɹ������ص�һ�γ��ָ�������׵�ַ���������ʧ�ܣ�����NULL
*����˵����
*--------------------------------------------------------------------------------*/
uint8_t* SearchnDataFromBuf(uint8_t* srcBuf,uint16_t srcSize,uint8_t* data,uint16_t datalen)
{
  uint16_t dataLen_Tmp = datalen;
  uint8_t* pos = srcBuf;
  uint16_t sSize = srcSize;
	
  if((srcBuf== NULL)||(data == NULL)||(srcSize <datalen))
    return NULL;
	
  while(sSize >=dataLen_Tmp)
  {
    if(*pos == *data)
    {
      if(memcmp(pos,data,dataLen_Tmp) == 0)
        return pos;
    }
    pos++;
    sSize--;
  }
  return NULL;
}	



/*--------------------------------------------------------------------------------
*�������ܣ��Ӵ��ڻ��λ��������½���������������������   �ǻ��λ�����
*���������*p_bufhead:���λ������׵�ַ
*           bufSize: ��������С
*          *pos��������ʼ��ַ
*           datalen: ���������ݿ鳤��
*          *searchArray:�����������׵�ַ
*          searchLen: ���������鳤��
*���ز����������ɹ������ص�һ�γ��ָ�������׵�ַ���������ʧ�ܣ�����NULL
*����˵����
*--------------------------------------------------------------------------------*/
uint8_t* SearchnDataFromCycbuf(uint8_t* p_bufhead,uint16_t bufSize,uint8_t* pos,uint16_t datalen,uint8_t* searchArray,uint16_t searchLen)
{
  uint16_t dataLen_Tmp = datalen;
  uint8_t* pos1 = pos;
 // uint16_t sLen = 0;
  uint8_t* pos2 = searchArray;
  uint8_t* pos3 =NULL;
 // int8_t result = -1;
  uint8_t ui8i;



  if((p_bufhead== NULL)||(searchArray == NULL)||(searchLen >datalen)||(pos<p_bufhead))
    return NULL;



  while(dataLen_Tmp--)
  {
    pos2 = searchArray;
    if(*pos1 == *pos2)
    {
        
      pos3 = pos1;
      for(ui8i=0;ui8i<searchLen;)
      {
        ui8i++;
        if(ui8i == searchLen)
        {
          return pos3;
        }
        pos1++;
        if(pos1 > p_bufhead+bufSize-1)         //?-?��????
          pos1 = p_bufhead;
        pos2++;

        if(*pos1 != *pos2)
        {
        
           break;
        }
        

        
        
      }
      pos1 = pos3;

    }
    pos1++;
    if(pos1 > p_bufhead+bufSize-1)         //?-?��????
      pos1 = p_bufhead;
  }
  return NULL;

}











/*------------------------------------------------------------------------------
*�������ܣ���������ǲ���ʮ���Ƶ�ASCII�ַ�
*���������������ASCII�ַ�
*���ز�����-1������ʮ���Ƶ�ASCII�� 0����ʮ����ASCII��
*------------------------------------------------------------------------------*/
int8_t CheckDecAsciiChar(char c)
{
  if((c>=0x30)&&(c<=0x39))
    return 0;

  return -1;
}


/*-------------------------------------------------------------------------------
*�������ܣ���һ��ʮ���Ƶ�ASCII�ַ�ת����һ��ʮ��������
*���������c:��ת����ASCII�ַ�
*���ز�����ת�����ʮ��������
*--------------------------------------------------------------------------------*/
uint8_t AsciiToUint8(char c)
{
  if((c>=0x30)&&(c<=0x39))
    return c-0x30;
  if((c>=0x41)&&(c<=0x46))//20140418 �޸�
    return c-0x37;
  if((c>=0x61)&&(c<=0x66))//20140418 �޸�  a~z
    return c-0x57;

  return 0;
}






/*-------------------------------------------------------------------------------
*�������ܣ���һ��ʮ���Ƶ�ASCII�ַ�ת����һ��ʮ��������
*���������c:��ת����ASCII�ַ�
*���ز�����ת�����ʮ��������
*--------------------------------------------------------------------------------*/
uint8_t AsciiToDeci(char hc,char lc)
{
  uint8_t bcd = 0;;
	
	
  if((hc>=0x30)&&(hc<=0x39))
    bcd = hc-0x30;
  else if((hc>=0x41)&&(hc<=0x46))
    bcd = hc-0x37;
    
  bcd =bcd*10;
	
  if((lc>=0x30)&&(lc<=0x39))
    bcd += (lc-0x30);
  else if((lc>=0x41)&&(lc<=0x46))
    bcd += (lc-0x37);	

  return bcd;
}

/*-----------------------------------------------------------------------------------
*�������ܣ���ʮ���Ƶ�ASCII��ת����ʮ������
*���������*pBuf��ʮ����ASCII����ʼ��?
            len: ʮ����ASCII�볤��
            *data��ʮ�������ݵ�ַָ��
*����ֵ��-1��ʧ�� ��0���ɹ�
*-----------------------------------------------------------------------------------*/
int8_t GetDecAsciiFieldToUint32(char* pBuf,uint8_t len,uint32_t *data)
{
  uint32_t tmp = 0;
  uint8_t *pos = (uint8_t*)pBuf;
	
  if((pBuf == NULL)||( 0 != CheckDecAsciiChar(*pBuf))||(len > 10))
    return -1;
	
  while(len--)
  {
    if((*pos<0x30)||(*pos>0x39))
      return -1;

    tmp = tmp*10 + AsciiToUint8(*pos);
    pos++;

    if(0 != CheckDecAsciiChar(*pos))
      break;
	
  }
  *data = tmp;
  return 0;

}



/*-----------------------------------------------------------------------------------
*�������ܣ��������ŵ�ʮ���Ƶ�ASCII��ת�����з��ŵ�ʮ������
*���������*pBuf��ʮ����ASCII����ʼ��?
            len: ʮ����ASCII�볤��
            *data��ʮ�������ݵ�ַָ��
*����ֵ��-1��ʧ�� ��0���ɹ�
*-----------------------------------------------------------------------------------*/
int8_t GetSigDecAsciiFieldToInt32(char* pBuf,uint8_t len,int32_t *data)
{
  int32_t tmp = 0;
  uint8_t *pos = (uint8_t*)pBuf;
  int8_t  sig;
	
  if((pos == NULL)||(len > 10))
    return -1;
	
  if(*pos == '-')
  {
    pos++;
    len--;
    sig = -1;
  }
  else if(*pos == '+')
  {
    pos++;
    len--;
    sig = 1;
  }
  else
  {
    sig = 1;
  }		
  while(len--)
  {
    if((*pos<0x30)||(*pos>0x39))
      return -1;
		
    tmp = tmp*10 + AsciiToUint8(*pos);
    pos++;
  }
  *data = sig*tmp;
  return 0;

}

/*
*�������ܣ���һ���ڴ��ж�ȡһ��ʮ�����ַ���
*���������*pHead:��������ʼ��ַ
            len���������ڴ�鳤��
            field:��ȡ��һ���ֶ�
            flag���ֶα�ʶ��
            buf����ȡ���ݴ�ŵĵ�ַ
            size����������ڴ�鳤��
����ֵ��-1����ȡʧ�ܣ� 0����ȡ�ɹ�
*/

char* GetDecAsciiFromBuf(char* pHead,uint16_t len,uint8_t field,char flag,char* buf,uint16_t size)
{
  uint8_t ui8len = len;
  uint8_t ui8Field = field;
  char* pos;
  char *dist = buf;
  char* rePos = NULL;
  char* reP = NULL;

  pos = pHead;
  while(ui8len--)
  {
    if(ui8Field == 0)
    {
      if(((*pos>=0x30)&&(*pos<=0x39))||(*pos == '.'))
      {
        if(size == 0)
          break;
        *dist = *pos;
        dist++;
        size--;
      }
      else if(dist != buf)
      {
        if(field == 0)
			rePos = pHead;
		else	
          	rePos = reP;
        break;
      }		
    }
    else
    {
      if(*pos == flag)
      {
        ui8Field--;
        if(ui8Field == 0)
        {
           reP = pos;
        }
       }
		
    }
    pos++;
	
  }
  return rePos;
}



/*
*�������ܣ���һ���ڴ��ж�ȡһ��ʮ�����ַ���
*���������*pHead:��������ʼ��ַ
            len���������ڴ�鳤��
            field:��ȡ��һ���ֶ�
            flag���ֶα�ʶ��
            buf����ȡ���ݴ�ŵĵ�ַ
            size����������ڴ�鳤��
����ֵ��-1����ȡʧ�ܣ� 0����ȡ�ɹ�
*/

int8_t GetHexAsciiFromBuf(char* pHead,uint16_t len,uint8_t field,char flag,char* buf,uint16_t size)
{
  uint8_t ui8len = len;
  uint8_t ui8Field = field;
  char* pos;
  char *dist = buf;
  int8_t result = -1;
  pos = pHead;
  while(ui8len--)
  {
    if(ui8Field == 0)
    {
      if(((*pos>=0x30)&&(*pos<=0x39))||((*pos>=0x41)&&(*pos<= 0x5A))||((*pos>=0x61)&&(*pos<= 0x7A)))
      {
        if(size == 0)
          break;
        *dist = *pos;
        dist++;
        size--;
      }
      else if(dist != buf)
      {
        result = 0;
        break;
      }
		
    }
    else
    {
      if(*pos == flag)
        ui8Field--;
		
    }
    pos++;

  }
  return result;



}

COMMU_BOOL AsciiToHex(char* pAscii,uint8_t len,uint32_t* p_ui32Hex)
{
  uint8_t ui8i;
  uint32_t ui32Data = 0;
  
  
  if(len >8)
    return COMMU_FALSE;

  
  
  for(ui8i=0;ui8i<len;ui8i++)
  {
    ui32Data = ui32Data<<4;
    if((pAscii[ui8i]>=0x30)&&(pAscii[ui8i]<=0x39))
      ui32Data += pAscii[ui8i]-0X30;
    else if((pAscii[ui8i]>=0x41)&&(pAscii[ui8i]<=0x46))
      ui32Data += pAscii[ui8i]-0x37;
    else if((pAscii[ui8i]>=0x61)&&(pAscii[ui8i]<=0x66))
      ui32Data += pAscii[ui8i]-0x57;
     

  }
  *p_ui32Hex = ui32Data;
  
  return COMMU_TRUE;

   

}


COMMU_BOOL AsciiToDec(char* pAscii,uint8_t len,uint32_t* p_ui32Dec)
{
  uint8_t ui8i;
  uint32_t ui32Data = 0;
  
  
  if(len >10)
    return COMMU_FALSE;

  
  
  for(ui8i=0;ui8i<len;ui8i++)
  {
    ui32Data = ui32Data*10;
    if((pAscii[ui8i]>=0x30)&&(pAscii[ui8i]<=0x39))
      ui32Data += pAscii[ui8i]-0X30;
    else 
      break;
     

  }
  *p_ui32Dec = ui32Data;
  
  return COMMU_TRUE;

   

}





/*------------------------------------------------------------------------------------------
*�������ܣ���һ�δ���ڴ��ж�ȡĳ���ַ���count�γ��ֵĵ�ַ  ������
*���������*pHead:��������ʼ��ַ
            len���������ڴ�鳤��
            *Delim: ����֤���ַ�
	  count: ��count�γ���

����ֵ��NULL����ȡʧ�ܣ� ��Ч��ַ����ȡ�ɹ�
 ----------------------------------------------------------------------------------------*/
uint8_t *GetDelimAddFromStrBuf(uint8_t* pdata,uint16_t len,char Delim,uint16_t count)
{
  uint8_t ui8len = len;
  uint16_t counter = 0;
  uint8_t* pos = pdata;
	
  if((pos == NULL)||(count > ui8len)||(pos >= (pdata+len)))  return NULL;
  while(ui8len--)
  {
    if(*pos == Delim)
    {
      counter++;
      if(counter == count) 
        return pos;		
    }		
    pos++;
  }
  return NULL;
}



