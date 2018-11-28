/*------------------------------美赛达科技股份有限公司--------------------------------------*
*文件名称:
*作者:
*发布版本:
*发布版本日期:
*文件描述:
*
*-------------------------------历史修改记录-------------------------------------------------
*修改者:
*修订版本号:
*修改日期:
*修改内容:
*
*--------------------------------------------------------------------------------------------*/




#include "string.h"
#include "Commu_Config.h"
#include "Commu_CommFunc.h"



/*--------------------------------------------------------------------------------
*函数功能：从串口缓存区的新接收数据区域内搜索数组   不是环形缓冲区
*输入参数：*p_bufhead:环形缓存区首地址
*           bufSize: 缓存区大小
*          *pos：搜索起始地址
*           datalen: 搜索的数据块长度
*          *searchArray:待搜索数组首地址
*          searchLen: 待搜索数组长度
*返回参数：搜索成功，返回第一次出现该数组的首地址，如果搜索失败，返回NULL
*函数说明：
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
*函数功能：从串口环形缓存区的新接收数据区域内搜索数组   是环形缓冲区
*输入参数：*p_bufhead:环形缓存区首地址
*           bufSize: 缓存区大小
*          *pos：搜索起始地址
*           datalen: 搜索的数据块长度
*          *searchArray:待搜索数组首地址
*          searchLen: 待搜索数组长度
*返回参数：搜索成功，返回第一次出现该数组的首地址，如果搜索失败，返回NULL
*函数说明：
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
        if(pos1 > p_bufhead+bufSize-1)         //?-?·????
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
    if(pos1 > p_bufhead+bufSize-1)         //?-?·????
      pos1 = p_bufhead;
  }
  return NULL;

}











/*------------------------------------------------------------------------------
*函数功能：检测数据是不是十进制的ASCII字符
*输入参数：待检测的ASCII字符
*返回参数：-1：不是十进制的ASCII； 0：是十进制ASCII码
*------------------------------------------------------------------------------*/
int8_t CheckDecAsciiChar(char c)
{
  if((c>=0x30)&&(c<=0x39))
    return 0;

  return -1;
}


/*-------------------------------------------------------------------------------
*函数功能：将一个十进制的ASCII字符转换成一个十进制数据
*输入参数：c:待转换的ASCII字符
*返回参数：转换后的十进制数据
*--------------------------------------------------------------------------------*/
uint8_t AsciiToUint8(char c)
{
  if((c>=0x30)&&(c<=0x39))
    return c-0x30;
  if((c>=0x41)&&(c<=0x46))//20140418 修改
    return c-0x37;
  if((c>=0x61)&&(c<=0x66))//20140418 修改  a~z
    return c-0x57;

  return 0;
}






/*-------------------------------------------------------------------------------
*函数功能：将一个十进制的ASCII字符转换成一个十进制数据
*输入参数：c:待转换的ASCII字符
*返回参数：转换后的十进制数据
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
*函数功能：将十进制的ASCII码转换成十进制数
*输入参数：*pBuf：十进制ASCII买起始地?
            len: 十进制ASCII码长度
            *data：十进制数据地址指针
*返回值：-1：失败 ；0：成功
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
*函数功能：将带符号的十进制的ASCII码转换成有符号的十进制数
*输入参数：*pBuf：十进制ASCII买起始地?
            len: 十进制ASCII码长度
            *data：十进制数据地址指针
*返回值：-1：失败 ；0：成功
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
*函数功能：从一段内存中读取一段十进制字符串
*输入参数：*pHead:待查找起始地址
            len：待查找内存块长度
            field:读取第一个字段
            flag：字段标识符
            buf：读取数据存放的地址
            size：存放数据内存块长度
返回值：-1：读取失败； 0：读取成功
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
*函数功能：从一段内存中读取一段十进制字符串
*输入参数：*pHead:待查找起始地址
            len：待查找内存块长度
            field:读取第一个字段
            flag：字段标识符
            buf：读取数据存放的地址
            size：存放数据内存块长度
返回值：-1：读取失败； 0：读取成功
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
*函数功能：从一段存放内存中读取某个字符第count次出现的地址  并返回
*输入参数：*pHead:待查找起始地址
            len：待查找内存块长度
            *Delim: 待查证的字符
	  count: 第count次出现

返回值：NULL：读取失败； 有效地址：读取成功
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



