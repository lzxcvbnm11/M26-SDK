
#ifndef  _RX_RING_H_
#define _RX_RING_H_
#include "Commu_Config.h"

extern uint32_t RxRing_DeleteDataByLen(Commu_RingQueue_t *pRing,uint32_t deleteLen);
extern uint32_t RxRing_AddData(Commu_RingQueue_t *pRing,uint8_t* pData,uint32_t len);
#endif





