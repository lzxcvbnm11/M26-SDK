#ifndef    _AT_RING_H_
#define   _AT_RING_H_
#include "Commu_Config.h"

typedef uint32_t (*AtRing_GetTransLen_Callback)(uint8_t* data,uint32_t len);
typedef uint32_t  (*AtRing_TransMemcpyToRing_Callback)(uint8_t* ringBase,uint32_t ringSize,uint8_t* pCurrent,uint8_t* srcBuf,uint32_t srcLen,uint8_t flag);
typedef uint32_t (*AtRing_SendFailed_Callback)(uint8_t* ringBase,uint32_t ringSize,uint8_t* pData,uint32_t datalen,char* pCmd);



extern AtRing_GetTransLen_Callback             AtRing_GetTransLen;
extern uint32_t AtRing_DeleteDataOfFirst(Commu_RingQueue_t* pRing,uint32_t offset,uint32_t size);
extern COMMU_ERROR AtRing_AddAtCmd(Commu_RingQueue_t* pRing,char* pCmd,uint8_t* data,uint32_t dataLen,COMMU_BOOL isHoldRepeat);

extern int8_t AtRing_GetFirstAtCmd(Commu_RingQueue_t* pRing,char* buf,uint32_t size,uint32_t* pLen);
extern uint32_t AtRing_GetFirstData(Commu_RingQueue_t* pRing,uint32_t offset,uint32_t readlen,char* buf,uint32_t size);
uint32_t AtRing_GetFirstDataLen(Commu_RingQueue_t *pRing);
extern int8_t AtRing_DeleteFirst(Commu_RingQueue_t *pRing,COMMU_BOOL isSaveData);
extern int8_t AtRing_DeleteAll(Commu_RingQueue_t *pRing,COMMU_BOOL isSaveData);

extern COMMU_ERROR  AtRing_GetTransLen_RegCallFunc(AtRing_GetTransLen_Callback pFunc);
extern COMMU_ERROR  AtRing_TransMemcpyToRing_RegCallFunc(AtRing_TransMemcpyToRing_Callback pFunc);
extern COMMU_ERROR  AtRing_AtRing_SendFailed_RegCallFunc(AtRing_SendFailed_Callback pFunc);

#endif





