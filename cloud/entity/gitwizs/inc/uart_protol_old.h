#ifdef __UART_PROTOL__
#define __UART_PROTOL__
//#include "stdio.h"
//
// |0x23 0x23 2byte|  |head 7byte| | message xbyte| |checkout sum 2byte|
// 

/*高字节命令标识*/
enum CMD_H_Tag
{
	RESERVED = 1, 			/*保留*/
	RT_REPORT, 				/*实时上报*/
	REPLACEMENT_REPORT，		/*补发上报*/ 
	CONTROL_CMMOND,			/*控制命令*/
	STATUS_INFO_REPORT,		/*状态信息上报*/
};

/*低位应答标识*/
enum CMD_H_Tag
{
	SUCCEED_ACK = 1, 			/*成功应答*/
	FAILE_ACK, 					/*失败应答*/
	SEND_MESSAGE=0xfe,			/*发送消息*/ 
	
};



typedef struct UART_PROTOL_HEAD_Tag
{
	u8 cmd_h;				/*高位命令标识*/
	u8 cmd_l;				/*低位命令标识*/
	u8 message_id;			/*消息ID*/
	u8 is_encrypt;			/*是否加密*/
	u8 is_encrypt_reserved; /*加密属性保留*/
	u8 data_len_h;			/*数据长度高位*/
	u8 data_len_l;			/*数据长度低位*/
}UART_PROTOL_HEAD_ST;

enum MESSAGE_ID_Tag
{
	U2P_ALARM1 = 0x06,   /*BMU发送给PC的报警数据1*/

	U2P_ALARM2, 			//BMU发送给PC的报警数据2
	U2P_SUMDATA1,			//BMU发送给PC的概要数据1
	U2P_SUMDATA2,			//BMU发送给PC的概要数据2
	U2P_SUMDATA3,			//BMU发送给PC的概要数据3
	U2P_SUMDATA4,			//BMU发送给PC的概要数据4系统单体最高最低及位置报文
	U2P_SUMDATA5,			//BMU发送给PC的概要数据5
// 系统单体最高最低及位置报文
	U2P_SUMDATA6,			//BMU发送给PC的概要数据6
	U2P_SUMDATA7,			//BMU发送给PC的概要数据7
	U2P_SUMDATA8,			//BMU发送给PC的概要数据8
	U2P_SUMDATA9,			//BMU发送给PC的概要数据9
	U2P_SUMDATA10,			//BMU发送给PC的概要数据10
	U2P_SUMDATA11,			//BMU发送给PC的概要数据11
	U2P_SUMDATA12,			//BMU发送给PC的概要数据12
	U2P_SUMDATA13,			//BMU发送给PC的概要数据13
	U2P_SUMDATA14,			//BMU发送给PC的概要数据14
	U2P_SUMDATA15,			//BMU发送给PC的概要数据15
	U2P_SUMDATA16,			//BMU发送给PC的概要数据16
	U2P_SUMDATA17,      		//BMU发送给PC的概要数据17(支路电流数据)
	U2P_SUMDATA18,			//BMU发送给PC的概要数据18 (状态信息)
	U2P_SUMDATA19,			//BMU发送给PC的概要数据19
				//….
	U2P_SUMDATA38=0x2d,			//BMU发送给PC的概要数据38
	U2A_BalStatus1,//(01--18) 	//2s更新周期   BMU发送到DCAN的均衡状态信息
	U2A_BalStatus2,//(01-18)	//BMU发送到DCAN的均衡状态信息
	U2A_ErrStatus1,//(01-18)
	U2A_ErrStatus2,//(01-18)
	U2A_SumVolt	 // (01-18)
	U2A_SumTemp,//(01-18)
	H2A_SUMDATA1_D,				//HVB发出的SD1
			//…..
    H2A_SUMDATA5_D=0x38,					////HVB发出的SD5
    U2A_PN,//(01-18)					//电箱PN码
	U2A_SN,//(01-18)			//电箱SN码
	C2A_FdbackInfo1_D,//(61-64)   //CSU的版本信息
    H2A_ID1_D,					//HVB的软件版本号1
    H2A_ID2_D,					//HVB的软件版本号2
    H2A_ID3_D,					//HVB的软件版本号3
    H2A_ID4_D,					//HVB的软件版本号4
    R2U_RU_Response,				//远程更新BMU反馈的Response报文（非诊断报文）
		//下面是RDB发送给BMU
	DIAG_FunReq=0x80,		//Tester发送到BMU的功能寻址诊断的信号
	R2A_Life	,	//DB生命帧的CRC/RDB生命信号,0~15循环
	R2U_RQPNSN,		//RDB发送到BMU的请求PNSN的CSC编号/
	//RDB发送到BMU的请求PNSN的指令类型
	P2U_PNSN1,		//PC发送到BMU的待写入PNSN1
	P2U_PNSN2,		//PC发送到BMU的待写入PNSN2
	R2U_RU_FunRequest,	 //远程更新功能寻址报文（非诊断报文）
	R2U_RU_FhyRequest,  //远程更新物理寻址报文（非诊断报文）
	P2A_Life,	//PC DCAN生命帧的CRC/PC DCAN生命号,0~15循环
	// M26 发送给MCU的网络相关状态
};

enum STATUS_INFO_Tag
{
	SPI_FALSG=1,			//SPI flash 故障信息上报
	CAN_BUS_OFF,			//CAN BUS OFF 状态上报
	BMS_LOST,				//BMS 节点丢失状态上报
};

enum CONTROL_CMD_Tag
{
	CLEAR_MCU_FLAG=0x01,     //  M26 发送给MCU清除存储数据区
	RESET_MCU,					//M26 发送给MCU 复位命令
	CHANGE_WORK_MODE,			//M26 发送给MCU 工作模式切换
	BMU_UPGRADE_PACK=0x10,		//M26发送给MCU 的BMU升级文件
	RDB_UPGRADE_PACK,			//M26发送给MCU的 RDB自身的升级文件
	SYS_TIME_UPDATE,			//M26发送给MCU 的系统时间更新命令
	NET_WORK_STATUS,			//M26 发送给MCU的网络相关状态
};

unsigned short Crc16Count(unsigned char* pArray,intlen);
#endif