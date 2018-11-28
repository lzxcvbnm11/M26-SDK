#ifndef _COMM_FUNC_H_
#define _COMM_FUNC_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef unsigned char       	COMMU_BOOL;
#define  COMMU_TRUE                    (1)
#define  COMMU_FALSE                   (0)

typedef char                      COMMU_ERROR;
#define   COMMU_SUCCESS             (0)
#define   COMMU_FAILED                  (-1)

#ifndef NULL
#define NULL                                   (0)
#endif


#define _MAX(a,b)  (a>b?a:b)
#define _MIN(a,b)  (a<b?a:b)

#define _MAXWITHOUT_C(a,b,c)  ((c<=_MAX(a,b))?_MIN(a,b):_MAX(a,b))
#define _MINWITHOUT_C(a,b,c)  ((c>=_MIN(a,b))?_MAX(a,b):_MIN(a,b))

#define  IS_HEXASCII(c)   ((((c>=0x30)&&(c<=0x39))||((c>=0x41)&&(c<0x5a))||((c>=0x61)&&(c<0x7A)))?1:0)




 /* exact-width signed integer types */
typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed          long int int32_t;


/* exact-width unsigned integer types */
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned          long int uint32_t;






extern uint8_t* SearchnData(uint8_t* srcBuf,uint16_t srcLen,char* searchArray,uint16_t searchLen);
extern uint8_t* SearchnDataFromBuf(uint8_t* srcBuf,uint16_t srcSize,uint8_t* data,uint16_t datalen);
extern uint8_t* SearchnDataFromCycbuf(uint8_t* p_bufhead,uint16_t bufSize,uint8_t* pos,uint16_t datalen,uint8_t* searchArray,uint16_t searchLen);
extern char* GetDecAsciiFromBuf(char* pHead,uint16_t len,uint8_t field,char flag,char* buf,uint16_t size);
extern int8_t GetHexAsciiFromBuf(char* pHead,uint16_t len,uint8_t field,char flag,char* buf,uint16_t size);
extern int8_t GetSigDecAsciiFieldToInt32(char* pBuf,uint8_t len,int32_t *data);
extern int8_t GetDecAsciiFieldToUint32(char* pBuf,uint8_t len,uint32_t *data);



extern uint8_t AsciiToUint8(char c);
extern COMMU_BOOL AsciiToHex(char* pAscii,uint8_t len,uint32_t* p_ui32Hex);
extern COMMU_BOOL AsciiToDec(char* pAscii,uint8_t len,uint32_t* p_ui32Dec);
extern uint8_t AsciiToDeci(char hc,char lc);

extern int8_t CheckDecAsciiChar(char c);
extern uint8_t *GetDelimAddFromStrBuf(uint8_t* pdata,uint16_t len,char Delim,uint16_t count);//20140411


#endif


