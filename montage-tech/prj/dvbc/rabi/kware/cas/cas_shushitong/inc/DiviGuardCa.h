//******************************************************************//
//	深圳市数视通科技股份有限公司 CopyRight 1998-2010
//	条件接收系统 V2.0
//	机顶盒移植库头文件
//	版本：2.1.0.0
//	日期：2007-9-3
//  更新：2009-12-29
//******************************************************************//
#ifndef __Pub_DiviGuard
#define __Pub_DiviGuard

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char INT8;
typedef signed short INT16;
typedef signed long  INT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long  UINT32;

//------------------------------------------------------------------------------------
//#define debug

//------------------------------------------------------------------------------------
#ifndef BOOL
#define BOOL unsigned char
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef FAILTURE
#define FAILTURE -1
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif


//------------------------------------------------------------------------------------
#define CA_MAX_SERVICE_PER_ECM 10


//------------------------------------------------------------------------------------
#define CA_STB_FILTER_1 1
#define CA_STB_FILTER_2 2
#define CA_STB_FILTER_3 3
#define CA_STB_FILTER_4 4

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#define DIVI_WAIT_INFO 0
#define DIVI_WAIT_PRIO 1
#define DIVI_TASK_NOWAIT 2
#define DIVI_TASK_WAIT 3

//--------------------邮件系统用到的宏
#define   MAX_EMAIL_NUM            10
#define   EMAIL_TITLE_LEN          36
#define   EMAIL_CONTENT_LEN      128

//-----------------------------------------------------------------------------------
typedef struct _ProductInfo{
     UINT8 PackageId;
     UINT16 StartYear;
     UINT16 StartMonth;
     UINT8   StartDate;
     UINT16 EndYear;
     UINT16 EndMonth;
     UINT8   EndDate;	 
     UINT16 ExpireDate;
     UINT16 PackagePrice;
     UINT8 PackageScript[20];	 
}ProductInfo;

typedef struct _TotalProductInfo{
     UINT8 Package_Count;
     ProductInfo PackageInfo[32];
}PackageInfo;

//harvey, 2009.06.17
typedef struct _TotalChildInfo{

     UINT16  ExpireYear;        //年
     UINT16  ExpireMonth;       //月
     UINT8   ExpireDate;        //日
     UINT8   ExpireHour;        //小时
     UINT8   ExpireMinute;	    //分
     UINT8   ExpireSecond;      //秒
	 
     UINT8 Main_Card_NO[8];     //母卡卡号
     UINT8 Active_Duration[6];  //激活区间 
     UINT8 Effective_Days;      //有效天数
     UINT8 Lead_Time;           //激活提醒时间
     UINT8 Temp_Active_Duration;//临时激活区间
	 
}ChildSMCInfo;


typedef struct _PayRecordInfo{
	UINT8 Record_Num;
	UINT8 Content[18][8];
}PayRecordInfo;

//-----------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
typedef struct {
        UINT32 q1stWordOfMsg;
	 UINT32 q2ndWordOfMsg;
	 UINT32 q3rdWordOfMsg;
	 UINT32 q4thWordOfMsg;
}DIVI_QUEUE_MESSAGE;

//------------------------------------------------------------------------------------
typedef struct _CAServiceInfo {
	UINT16 m_wEcmPid;
	UINT16 m_wOriNetworkID;
	UINT16 m_wTSID;
	UINT8 m_bServiceCount;
	UINT8 m_reserved;
	UINT16 m_wServiceId[CA_MAX_SERVICE_PER_ECM];
}DIVI_CAServiceInfo;

//---------------------邮件系统需要使用的结构----------------------------
typedef struct _TDIVI_DATETIME
{
	UINT16 Y;   //年
	UINT16 M;  //月
	UINT8   D;  //日
	UINT8   H;  //时
	UINT8   MI; //分
	UINT8   S;  //秒
}TDIVI_DATETIME;

typedef struct _TDIVI_EmailHead{
	TDIVI_DATETIME	m_tCreateTime;      /*EMAIL创建的时间*/
	UINT8    		m_bEmail_Level;     /*邮件重要程度*/
	UINT8		m_bNewEmail;         /*0 不是新邮件 1是新邮件*/
	UINT16		m_bEmailID;            /*Email ID  */
	char   		m_szEmailTitle[EMAIL_TITLE_LEN];/*邮件标题，最长为36*/
}TDIVI_EmailHead;

typedef struct _TDIVI_EmailContent{
	char		m_szEmail[EMAIL_CONTENT_LEN];	/*Email的正文*/
}TDIVI_EmailContent;

//------------------------------------------------------------------------------------
typedef enum{
	DIVI_ERR = -100,
	DIVI_PIN_ERR,
	DIVI_PARENTLEVEL_ERR,
	DIVI_CARD_NOPAIR,
	DIVI_CARD_MISPAIR,
	DIVI_INPUT_PARA_ERR,
	DIVI_OK = 0
}DIVI_RESULT;
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
/*
INT32 DIVI_Initialize(UINT8 mailManagerType); 
提 供 者： CA模块。 
使 用 者： 机顶盒。 
何时调用： 在机顶盒初始化的时候调用该函数。 
功能描述： 初始化CA模块。 
输入参数： 保留不用 
输出参数： 无。 
返 回 值： 
SUCCESS：成功， 
FAILURE： 失败。
*/
INT32 DIVI_Initialize(UINT8 mailManagerType);

/*
BOOL DIVI_IsMatchCAID(UINT16 wCASystemID) 
提 供 者： CA模块。 
使 用 者： 机顶盒。 
何时使用： 收到CA描述符的内容后。 
功能描述： 判断CA系统ID是否是DIVI。 
输入参数： 
		wCASystemID，CA系统ID。 
输出参数： 无。 
返 回 值： 
		TRUE：是本CA；
		FALSE：不是本CA。
*/
BOOL  DIVI_IsMatchCAID(UINT16 wCASystemID);

/*
void DIVI_SetEcmPID (); 
提 供 者： CA模块。 
使 用 者： 机顶盒。 
何时使用： 当有新ECM PID时(如切换频道)调用该接口把相关信息传递给CA模块。 
功能描述： 将ECM的PID等信息传递给CA模块。 
输入参数： 
		bType： 保留无用；
		pEcmInfo:T_CAServiceInfo结构，描述节目的详细信息; 若pEcmInfo->m_wEcmPid为0xffff,则释放channel和filter。
 
输出参数： 无。 
返 回 值： 无。 

*/
void  DIVI_SetEcmPID(UINT8 bType,DIVI_CAServiceInfo * pEcmInfo);

/*
void DIVI_SetEmmPID (); 
提 供 者： CA模块。 
使 用 者： 机顶盒。 
何时使用： 当有新的EMM PID时调用该函数将数据传递给CA模块。 
功能描述： 分析完CAT表后将数据传递给CA模块。 
输入参数： 
		bType，保留无用； 
		wEmmPid，对应于DIVI 的CAS_System_ID的EMM的PID值；若wEmmPid为0xffff,则释放channel和filter。 

输出参数： 无。 
返 回 值： 无。
*/
void  DIVI_SetEmmPID(UINT8 bType,UINT16 wEmmPid);

/*
void DIVI_TableReceived (); 
提 供 者： CA模块。 
使 用 者： 机顶盒。 
何时使用： 当有CA的私有数据的时候，将私有数据接收下来，此时调用这个接口。 
功能描述： 当有CA的私有数据的时候，将私有数据接收下来，并将该数据传递给 CA模块，使得CA模块可以处理这些数据。 
输入参数： 
		bRequestID：CA_STB_FILTER_4为Ecm Filter接收到数据，
					CA_STB_Filter_1为Emm Filter1接收到数据，
					CA_STB_Filter_2为Emm Filter2接收到数据，
					CA_STB_Filter_3为Emm Filter3接收到数据。
		bOK： 收取数据有没有成功；TRUE：成功，FALSE：失败。 
		wPid： 接收的流的PID。 
		pbReceiveData： 收取私有数据的指针，CA模块不负责其空间的释放。 
		wLen： 收取到的私有数据的长度。 
输出参数： 无。 
返 回 值： 无。
*/
void  DIVI_TableReceived(UINT8 bRequestID,UINT8 bOK,UINT16 wPid,const UINT8 * pbReceiveData,UINT16 wLen);

/*
INT32 DIVI_GetSMCNO(); 
提 供 者： CA模块。 
使 用 者： 机顶盒。 
何时调用： 需要获取用户的卡NO时。 
功能描述： 取得用户智能卡的NO。 
输入参数： 
		pbCardNO： 存放卡NO的地址指针 
输出参数： 
		pbCardNO， 卡NO。 
返 回 值： 
		DIVI_OK：成功； 
		DIVI_ERR, 发生错误,可能issueDate或expireDate输入的指针为空指针，或者智能卡的未插入。 
其他说明： 卡NO为BCD码（8个字节）。
*/
INT32 DIVI_GetSMCNO(UINT8 * pbCardNO);



/*
INT32 DIVI_GetSMCUserType() 
提 供 者： CA模块。 
使 用 者： 机顶盒。 
功能描述： 获取智能卡用户的类型 
输入参数： 无 
输出参数： 无 
返 回 值： 
		-1,操作失败
		其他,卡类型。08:母卡，09:子卡
其他说明：
		用户类型由SMS系统定义
*/
INT32 DIVI_GetSMCUserType(void);

/*
INT32 DIVI_GetSMCUserLevel() 
提 供 者： CA模块。 
使 用 者： 机顶盒。 
功能描述： 获取智能卡用户的等级 
输入参数： 无 
输出参数： 无 
返 回 值： 
		-1,操作失败
		其他,卡级别。
其他说明： 
		智能卡用户等级由用户管理系统定义,机顶盒用户可以通过此函数查询。
*/
INT32 DIVI_GetSMCUserLevel(void);

/*
void  DIVI_SCStatusChange(UINT8 status)
提 供 者： CA模块 
使 用 者： 机顶盒。 
何时使用：在系统检测到插拔卡事件时调用。 

功能描述：通知CA模块插拔卡事件发生。 
输入参数： status,0x01表示智能卡插入事件发生，0x02表示智能卡拔出事件发生
输出参数： 无。 
返 回 值： 无
*/
void  DIVI_SCStatusChange(UINT8 status);

/*
UINT16 DIVI_GetEmailHeads(TDIVI_EmailHead* pEmailHead); 
提 供 者： CA模块
使 用 者： 机顶盒
功能描述：获取邮件的所有标题头
输入参数： 
			pEmailHeads: 标题头序列
			nTitleNum  : 标题头序列的元素个数
输出参数：
                     pEmailHead: 填充标题 头序列
返 回 值： 
                     邮件个数
*/
UINT16 DIVI_GetEmailHeads(TDIVI_EmailHead* pEmailHeads, UINT16 nTitleNum);


/*
UINT8 DIVI_GetNewMailCount();
提 供 者： CA模块
使 用 者： 机顶盒
功能描述：获取未读邮件的个数
输入参数：无
输出参数：无
返 回 值: 未读邮件的个数                     
*/
INT32 DIVI_GetNewMailCount();

/*
BOOL DIVI_GetEmailHead(UINT8 bEmailID,TDIVI_EmailHead* pEmailHead); 
提 供 者： CA模块
使 用 者： 机顶盒
功能描述：获取特定邮件的标题头
输入参数： 
			pEmailHead: 标题头指针
			bEmailID    : 邮件序号
输出参数：
                     pEmailHead: 填充标题 头
返 回 值： 
                     TRUE: 成功
                     FALSE: 失败
*/
BOOL DIVI_GetEmailHead(UINT16 bEmailID,TDIVI_EmailHead* pEmailHead); 
/*
BOOL DIVI_GetEmailContent(UINT8 bEmailID,TDIVI_EmailContent* pEmailContent);
提 供 者： CA模块
使 用 者： 机顶盒
功能描述：获取特定邮件的内容
输入参数： 
			pEmailContent: 内容结构指针
			bEmailID    : 邮件序号
输出参数：
                     pEmailContent: 填充内容结构
返 回 值： 
                     TRUE: 成功
                     FALSE: 失败
*/
BOOL DIVI_GetEmailContent(UINT16 bEmailID,TDIVI_EmailContent* pEmailContent) ;
/*
BOOL DIVI_DelEmail(UINT8 bEmailID);
提 供 者： CA模块
使 用 者： 机顶盒
功能描述：删除特定邮件
输入参数：	bEmailID : 邮件序号
输出参数：无
返 回 值： 
                     TRUE: 成功
                     FALSE: 失败
*/
BOOL DIVI_DelEmail(UINT16 bEmailID); 

//INT32 DIVI_ClearPaired();
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------

/*
UINT8 DIVI_SC_Get_CardState(void);
提供者:机顶盒
使用者:CA模块
何时调用:CA模块需要获得卡状态时。
功能描述:获得智能卡状态。
输入参数:无
输出参数:无
返回值:
0x00:未插卡
0x01:已插卡
*/
extern UINT8 DIVI_SC_Get_CardState(void);


/*
INT32 DIVI_SC_DRV_Initialize(void); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
何时调用： CA模块初始化时调用该函数。 
功能描述： 初始化智能卡驱动。 
输入参数： 无。 
输出参数： 无。 
返 回 值： 
SUCCESS：成功， 
FAILURE： 失败。 
*/
extern INT32 DIVI_SC_DRV_Initialize(void);

/*
INT32 DIVI_SC_ValidateATR( UINT8 *pbNewATR, UINT16 wLen,); 
提 供 者： CA模块。  
使 用 者： CA模块。 
何时调用： 当有收到新的ATR信息的时。 
功能描述： 智能卡驱动调用该函数用于判断是否是DIVI智能卡的有效ATR。 
输入参数： 
	pbNewATR： ATR数据； 
	wLen： ATR数据长度； 
输出参数： 无。 
返 回 值： 
SUCCESS：是CA的智能卡， 
FAILTURE： 不是CA的智能卡。 
*/
INT32  DIVI_SC_ValidateATR(UINT8 * pbNewATR,UINT16 wLen);

/*
INT32 DIVI_SC_DRV_ResetCard(UINT8 bCardNumber,UINT8 * ATRBuf,UINT8 BufferLen,
UINT8 * RLength); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 复位智能卡。 
输入参数： 
        bCardNumber， 为智能卡的卡号； 
	ATRBuf,返回的ATR数据存入的内存地址；
	BufferLen,返回的ATR数据存入的内存长度；
	RLength,实际返回的ATR长度。
输出参数： 无。 
返 回 值： SUCCESS---复位成功;FAILTURE---复位失败
*/
extern INT32  DIVI_SC_DRV_ResetCard(UINT8 bCardNumber,UINT8 * ATRBuf,UINT8 BufferLen,UINT8 * RLength);

/*
INT32 DIVI_SC_DRV_SendDataEx() 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
何时使用： 向智能卡发送数据时。 
功能描述： CA模块调用此函数向智能卡发送数据。 
输入参数： 
		bCardNumber： 暂时保留不用； 
		bLength： pabMessage的长度； 
		pabMessage： 发送命令的消息指针； 
		pabResponse： 接收响应结果的数据块的指针； 
		bRLength： 响应结果的数据块长度指针。 
输出参数： 
		pabResponse： 响应结果的数据块； 
		bRLength： 响应结果的数据块长度。 
		pbSW1 智能卡状态字节1 
		pbSW2 智能卡状态字节2 
返 回 值： 
		TRUE：成功
		FALSE：失败 
其他说明： 
*/
extern INT32 DIVI_SC_DRV_SendDataEx(UINT8 bCardNumber,UINT8 bLength,
	                                                      UINT8 * pabMessage,
	                                                      UINT8 * pabResponse,
	                                                      UINT8 RBufferLen,
	                                                      UINT8 * bRLength,
	                                                      UINT8 * pbSW1,
	                                                      UINT8 * pbSW2);

/*
INT32 DIVI_TableStart(UINT8 bRequestID,const UINT8 * pbFilterMatch,const UINT8 * pbFilterMask,
	                                            UINT8 bLen,
	                                            UINT16 wPid,
	                                            UINT8 bWaitTime);
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 启动过滤器去接收CA的数据。 
输入参数： 
		bRequestID,为以下的几个值之一。
		                  CA_STB_FILTER_1,
		                  CA_STB_FILTER_2,
		                  CA_STB_FILTER_3,
		                  CA_STB_FILTER_4;
		pbFilterMatch,filter的数据。
		pbFilterMask,filter的mask。
		bLen， filter的长度。 
		wPid， 通道的PID值。 
		bWaitTime， 通道过期时间，即从多长时间内通道无法接收到与 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功， 
		FAILURE： 失败。 
其他说明：
		filter match与filter mask的关系：每个filter match的位对应有一个filter mask的位。如果相应的filter mask的位为1，则表示该filter match位需要匹配。如：filter match中的一个字节为：0x5a， filter mask 中相应的一个字节为：0x55，则过滤器中该filter match字节起作用的仅仅为第0、2、4、6位，即须匹配0、0、1、1。另外，filter match和filter mask不包含table中的第1、第2字节（从0开始计数），因为这两个字节在DVB中的定义比较特殊。详细看13818中Table的格式。 ?? 

*/
extern INT32 DIVI_TableStart(UINT8 bRequestID,
                                                   const UINT8 * pbFilterMatch,
                                                   const UINT8 * pbFilterMask,
	                                            UINT8 bLen,
	                                            UINT16 wPid,
	                                            UINT8 bWaitTime);


/*
void DIVI_SetCW (); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 设置解扰通道的控制字。 
输入参数： 
		wEcmPid， 控制字的所在的ecm包的PID值。 
		szOddKey， 奇控制字数据的指针。 
		szEvenKey， 偶控制字数据的指针。 
		bKeyLen， 控制字长度。 
		bReservedFlag, 保留。 
输出参数： 无。 
返 回 值： 无。 

*/
extern void DIVI_SetCW(UINT16 wEcmPid,const UINT8 * szOddKey,const UINT8 * szEvenKey,
	                              UINT8 bKeyLen,UINT8 bReservedFlag);

/*
BOOL DIVI_GetBuffer (); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 获得机顶盒提供给CA模块保存信息的起始地址和空间大小 
输入参数： 
输出参数： 
		lStartAddr 机顶盒分配空间的起始地址 
		lSize 机顶盒分配空间的大小 
返 回 值： 
		TRUE 成功 
		FALSE 失败 
*/
extern BOOL DIVI_GetBuffer(UINT32 * lStartAddr,UINT32 * lSize);

/*
提供者：  机顶盒
使用者：CA模块
功能描述：获得机顶盒提供给CA模块用于保存机卡配对信息的起始地址和空间大小；空间大小最少为16个字节
输入参数：
输出参数：
          lStartAddr 机卡配对信息存储的起始地址
          lSize 机卡配对信息存储区大小，须大于等于16个字节
返回值：  TRUE 成功 FALSE失败
*/
extern BOOL DIVI_Get_STBToCardSuit_Buffer(UINT32 *lStartAddr,UINT32 * lSize);

/*
BOOL DIVI_ReadBuffer(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 从机顶盒分配空间的指定起始地址读指定长度的数据。 
输入参数： 
		lStartAddr： 要读取数据的存储空间的地址。 
		pbData： 被读取数据的存放地址指针。 
		nLen： 要读的数据的长度 
输出参数： 
		pbData： 被读出的数据。 
返 回 值： 
		TRUE：成功
		FALSE 失败。
*/
extern BOOL  DIVI_ReadBuffer(UINT32 lStartAddr,UINT8 *pData,INT32 nLen);

/*
BOOL DIVI_WriteBuffer(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 向机顶盒的存储空间写信息 
输入参数： 
		lStartAddr： 要写的存储空间的目标地址。 
		pData： 要写的数据 
		nLen： 要写的数据的长度 
输出参数： 无。 
返 回 值： 
		TRUE：成功
		FALSE： 失败。
*/
extern BOOL DIVI_WriteBuffer(const UINT32 lStartAddr,const UINT8 *pData,INT32 nLen);


/*
extern INT32 DIVI_OSPSCTaskCreate(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 创建SC任务 
输入参数： 
		name[]， 4个字节的任务名称。 
		stackSize， 任务所使用的堆栈的大小。 
		entryPoint， 任务的入口地址。 
		priority， 任务的优先级。 
		arg1， 传递给任务的第一个参数。 
		arg2， 传递给任务的第二个参数。 taskId， 任务的ID。 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 发生错误。
*/
extern INT32 DIVI_OSPSCTaskCreate(char name[],UINT32 stackSize,
	                                   void (*entryPoint)(void *),
	                                   INT32 priority,
	                                   UINT32 arg1,
	                                   UINT32 arg2,
	                                   UINT32 * taskId);
/*
extern INT32 DIVI_OSPCaCoreTaskCreate(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 创建CaCore任务 
输入参数： 
		name[]， 4个字节的任务名称。 
		stackSize， 任务所使用的堆栈的大小。 
		entryPoint， 任务的入口地址。 
		priority， 任务的优先级。 
		arg1， 传递给任务的第一个参数。 
		arg2， 传递给任务的第二个参数。 taskId， 任务的ID。 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 发生错误。
*/
extern INT32 DIVI_OSPCaCoreTaskCreate(char name[],UINT32 stackSize,
	                                   void (*entryPoint)(void *),
	                                   INT32 priority,
	                                   UINT32 arg1,
	                                   UINT32 arg2,
	                                   UINT32 * taskId);

/*
extern INT32 DIVI_OSPOSDTaskCreate(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 创建OSD任务 
输入参数： 
		name[]， 4个字节的任务名称。 
		stackSize， 任务所使用的堆栈的大小。 
		entryPoint， 任务的入口地址。 
		priority， 任务的优先级。 
		arg1， 传递给任务的第一个参数。 
		arg2， 传递给任务的第二个参数。 taskId， 任务的ID。 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 发生错误。
*/
extern INT32 DIVI_OSPOSDTaskCreate(char name[],UINT32 stackSize,
	                                   void (*entryPoint)(void *),
	                                   INT32 priority,
	                                   UINT32 arg1,
	                                   UINT32 arg2,
	                                   UINT32 * taskId);
/*
extern INT32 DIVI_OSPUserTaskCreate(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 创建User任务 
输入参数： 
		name[]， 4个字节的任务名称。 
		stackSize， 任务所使用的堆栈的大小。 
		entryPoint， 任务的入口地址。 
		priority， 任务的优先级。 
		arg1， 传递给任务的第一个参数。 
		arg2， 传递给任务的第二个参数。 taskId， 任务的ID。 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 发生错误。
*/
extern INT32 DIVI_OSPUserTaskCreate(char name[],UINT32 stackSize,
	                                   void (*entryPoint)(void *),
	                                   INT32 priority,
	                                   UINT32 arg1,
	                                   UINT32 arg2,
	                                   UINT32 * taskId);

/*
extern INT32 DIVI_OSPTaskTemporarySleep(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 暂停任务一段指定的时间。 
输入参数： 
		milliSecsToWait， 终止任务的时间。（以毫秒为单位。） 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 发生错误。 
*/
extern INT32 DIVI_OSPTaskTemporarySleep(UINT32 milliSecsToWait);

/*
extern INT32 DIVI_OSPSemaphoreCreate(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 
输入参数： 
		name[]， 4个字节的信号量名称。 
		initialTokenCount， 初始化时信号量的数量。 
		taskWaitMode， 取得该信号量的方式。为以下两种之一。 
		DIVI_WAIT_FIFO， 按先等待者先得的方式取得。 
		DIVI_WAIT_PRIO， 按优先级高者先得的方式取得。 
		semaphoreId， 该信号量的ID。 
输出参数： 无 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 发生错误。 
其他说明： 
		目前CA模块只用到了DIVI_WAIT_FIFO模式，如果无法实现DIVI_WAIT_PRIO则可以暂时不实现。
*/
extern INT32 DIVI_OSPSemaphoreCreate(char name[4],UINT32 initialTokenCount,UINT32 taskWaitMode,UINT32 * semaphoreId);

/*
extern INT32 DIVI_OSPSemaphoreGetToken(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 取得信号量 
输入参数： 
		semaphoreId， 所要等待的信号量的ID。 
		waitMode， 等待信号量的方式，为以下两种之一。 
		DIVI_TASK_WAIT，等待信号量直到超时。 
		DIVI_TASK_NOWAIT，不管信号量存在与否立即返回。
		milliSecsToWait， 等待的时间。 如果waitMode为
		DIVI_TASK_NOWAIT，则该参数无效。 
		DIVI_TASK_WAIT，则代表时间。 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 失败。 
其他说明： 
		注意区别这里的等待模式和创建信号量的等待模式之间的区别。
		创建信号量的是指同时有多个任务在等待同一个信号量时谁优先取得信号量。
		而OSPSemaphoreGetToken的信号量的等待是指该任务等待信号量,是等待一段时间直到取得信号量
		为止还是不做任何事务的等待，不管是否可以取得信号量，立即返回。

*/
extern INT32 DIVI_OSPSemaphoreGetToken(UINT32 semaphoreId,UINT32 waitMode,UINT32 milliSecsToWait);

/*
extern void * DIVI_OSPMalloc(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 分配内存。 
输入参数： 
		numberOfBytes， 分配的内存空间的大小。（以字节为单位。） 
输出参数： 无。 
返 回 值： 
		NULL：失败
		>0: 分配内存地址指针。
其他说明：
*/
extern void * DIVI_OSPMalloc(UINT32 numberOfBytes);

/*
extern INT32 DIVI_OSPFree(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 释放掉分配了的内存空间。 
输入参数： 
		ptrToSegment， 要释放的内存空间指针。 
输出参数： 无。 
返 回 值： 
		SUCCESS： 成功释放。 
		FAILURE： 释放不成功，可能是因为所传递进来的指针无效。 
*/
extern INT32 DIVI_OSPFree(void * ptrToSegment);

/*
extern INT32 DIVI_OSPSemaphoreReturnToken(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 增加一个信号量。 
输入参数： 
		semaphoreId， 信号量的ID。 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 发生错误。 
*/
extern INT32 DIVI_OSPSemaphoreReturnToken(UINT32 semaphoreId);

/*
extern INT32 DIVI_OSPQueueCreate(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 创建一个消息队列。 
输入参数： 
		name[]， 4个字节的队列名称。 
		maxQueueLength，消息队列中可以存放的消息的数量。当消息队列中该数量达到 该数量时，再往该消息队列发消息将会失败。 
		taskWaitMode， 取得消息队列中消息的方式。为以下两种之一。
		DIVI_WAIT_FIFO， 按先等待者先得的方式取得。 
		DIVI_WAIT_PRIO， 按优先级高者先得的方式取得。
		queueId， 消息队列的ID。 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 发生错误
*/
extern INT32 DIVI_OSPQueueCreate(char name[],UINT32 maxQueueLength,UINT32 taskWaitMode,UINT32 * queueId);

/*
extern INT32 DIVI_OSPQueueGetMessage(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 
输入参数： 
		queueId， 所要取得的消息的ID。 
		messsage， 为消息的格式。参照DIVI_QUEUE_MESSAGE。 
		waitMode， 等待信号量的方式，为以下两种之一。 
				DIVI_TASK_WAIT，等待消息直到超时。 
				DIVI_TASK_NOWAIT，不管消息存在与否立即返 回。 
		milliSecsToWait， 等待的时间。如果waitMode为 
				DIVI_TASK_NOWAIT，则该参数无效
				DIVI_TASK_WAIT，则代表时间。 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 发生错误。
*/
extern INT32 DIVI_OSPQueueGetMessage(UINT32 queueId,DIVI_QUEUE_MESSAGE * message,UINT32 waitMode,UINT32 milliSecsToWait);

/*
extern INT32 DIVI_OSPQueueSendMessage(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 发送消息到消息队列。 
输入参数： 
		queueId， 消息队列ID。 
		message， 要发送的消息。其格式见DIVI_QUEUE_MESSAGE结构。 
输出参数： 无。 
返 回 值： 
		SUCCESS：成功；
		FAILURE： 发生错误。
*/
extern INT32 DIVI_OSPQueueSendMessage(UINT32 queueId,DIVI_QUEUE_MESSAGE * message);

/*
extern UINT32 DBGPrint (); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 调试时串口打印信息。 
输入参数： fmt, 格式控制，与c语言中的printf()函数语法相同。
输出参数： 无。 
返 回 值： 返回打印的字符数目。 
其他说明： 
*/
extern UINT32 DBGPrint(const char * fmt,...);

/*
extern UINT32 DIVI_GetRandomNumber(); 
提 供 者： 机顶盒。 
使 用 者： CA模块。 
功能描述： 生成一个不超过指定上限的随机数。 
输入参数： 无 
输出参数： 无。 
返 回 值： 随机数 
*/
extern UINT32 DIVI_GetRandomNumber();

/*
UINT8 GxShowPPDatas(UINT8 Value)
提 供 者： 机顶盒 
使 用 者： CA模块 
功能描述：关闭或开启视频层 
输入参数： Value,0x00关闭视频层，0x01开启视频层
输出参数： 无。 
返 回 值： 无意义
*/
extern UINT8 GxShowPPDatas(UINT8 Value);

/*
UINT16 DIVI_GetCurr_ServiceID(void)
提 供 者： 机顶盒 
使 用 者： CA模块 
功能描述：获得当前节目的ServiceID 
输入参数： 无
输出参数： 无。 
返 回 值： 当前节目的ServiceID*/
extern UINT16 DIVI_GetCurr_ServiceID(void);

/*
UINT16 DIVI_GetCurr_EmmID(void)
提 供 者： 机顶盒 
使 用 者： CA模块 
功能描述：获得当前的Emm Pid 
输入参数： 无
输出参数： 无。 
返 回 值： 当前的Emm Pid*/
extern UINT16 DIVI_GetCurr_EmmID(void);

/*
UINT16 DIVI_GetCurr_EcmID(void)
提 供 者： 机顶盒 
使 用 者： CA模块 
功能描述：获得当前的Ecm Pid 
输入参数： 无
输出参数： 无。 
返 回 值： 当前的Ecm Pid*/
extern UINT16 DIVI_GetCurr_EcmID(void);

/*
UINT16 DIVI_GetCurr_NetRegionID(void)
提 供 者： 机顶盒 
使 用 者： CA模块 
功能描述： 获得当前的NetRegionID 
输入参数： 无
输出参数： 无。 
返 回 值： 
           0xff: 不进行区域控制
           其他：当前的NetRegionID
*/
extern UINT16 DIVI_GetCurr_NetRegionID(void);//danlly, 2009-11-26

/*
INT32 DIVI_Modify_Pin(UINT8 * OldPin,UINT8 * NewPin,UINT8 PinLength,UINT8 * Repeattime)
提供者:CA模块
使用者:机顶盒
功能描述:更换Pin码 
输入参数:  
        OldPin,旧Pin码
                       NewPin,新Pin码
                       PinLength,Pin码长度
输出参数:无
返回值:
		SUCCESS---更换成功
                  FAILTURE---更换失败
其它说明:  Repeattime,PIN码输入错误剩余次数(0-7);若为17，则表示卡PIN功能被锁定
*/
INT32 DIVI_Modify_Pin(UINT8 * OldPin,UINT8 * NewPin,UINT8 PinLength,UINT8 * Repeattime);

/*
提供者：  CA模块。
使用者：机顶盒。
何时调用：机顶盒需要获取授权信息时。
功能描述：获取授权信息。
输入参数：无
输出参数：授权信息；最多暂定为6个产品。
返回值：
        0---查询失败；
        1---无授权信息；
        2---查询成功
其他说明：无
*/
INT32 DIVI_GetSMCEntitleInfo(PackageInfo * Package);

/*
提供者：  CA模块。
使用者：机顶盒。
何时调用：机顶盒需要获取子卡的配对母卡信息时。
功能描述：获取子卡信息。
输入参数：无
输出参数：子卡信息
返回值：
		0-----成功;
		-1----未插卡;
		-2----不是子卡 
其他说明：无
*/
INT32 DIVI_GetSMCChildInfo(ChildSMCInfo * ChildInfo);


/*
INT32 DIVI_GetSMCWorkTime(UINT8 * Enable,UINT8 * BeginTime,UINT8 * EndTime)
提 供 者： CA模块 
使 用 者： 机顶盒 
功能描述：获得工作时段信息 
输入参数： 无
输出参数： 
		Enable,1字节，0x00---工作时段未启用；0x01---工作时段已启用；
                            BeginTime, 3字节，开始时间,HMS格式
                            EndTime,    3字节，结束时间,HMS格式 
返 回 值：
		DIVI_ERR---查询失败;
                        DIVI_OK---查询成功
*/ 
INT32 DIVI_GetSMCWorkTime(UINT8 * Enable,UINT8 * BeginTime,UINT8 * EndTime);

/*
INT32 DIVI_GetRegion(UINT8 * Region)
提 供 者： CA模块 
使 用 者： 机顶盒 
功能描述：获得区域信息 
输入参数： 无
输出参数： Region---- 共4个字节，显示格式如下例001.002.003.004
返 回 值：
		DIVI_ERR---查询失败;
                        DIVI_OK---查询成功
*/ 
INT32 DIVI_GetRegion(UINT8 * Region);

/*
INT32 DIVI_SetCardLevel(UINT8 * Pin, UINT8 Level,UINT8 * Repeattime)
提 供 者： CA模块 
使 用 者： 机顶盒 
功能描述：设置家长控制级别 
输入参数： 
        Pin,6位Pin码
                             Level,家长控制级别
输出参数： 无 
返 回 值： 
		SUCCESS---成功;
		FAILTURE --- 失败
其它说明:  Repeattime,PIN码输入错误剩余次数(0-7);若为17，则表示卡PIN功能被锁定
*/
INT32 DIVI_SetCardLevel(UINT8 * Pin, UINT8 Level,UINT8 * Repeattime);

/*
INT32 DIVI_SetWorkPeriod(UINT8 * Pin, UINT8 * BeginTime,UINT8 * EndTime,UINT8 Enable,UINT8 * Repeattime)提 供 者： CA模块 
使 用 者： 机顶盒 
功能描述：设置工作时段
输入参数： 
        Pin,6位Pin码
                             BeginTime,开始时间，3字节，4bitBCD格式
                             EndTime,结束时间，3字节，4bitBCD格式
                             Enable,0x01---使能工作时段，0x00---关闭工作时段
输出参数： 无 
返 回 值： 
        DIVI_OK---成功;
        DIVI_ERR --- 失败
其它说明:  Repeattime,PIN码输入错误剩余次数(0-7);若为17，则表示卡PIN功能被锁定
*/
INT32 DIVI_SetWorkPeriod(UINT8 * Pin, UINT8 * BeginTime,UINT8 * EndTime,UINT8 Enable,UINT8 * Repeattime);

/*
UINT32 DIVI_GetPaired()
提 供 者： CA模块 
使 用 者： 机顶盒 
功能描述：获得机卡配对状态 
输入参数： 无
输出参数： 无 
返 回 值： 
        0x01---机卡配对;
        0x02---未机卡配对;
        0x00---操作失败
*/
UINT32 DIVI_GetPaired(void);

/*提供者：机顶盒
使用者：CA模块
何时调用：CA模块需要显示信息时；
功能描述：显示OSD信息
输入参数：见其他说明
输出参数：无
返回值：无
其他说明:见移植文档
*/
extern void  DIVI_EventHandle(UINT32 event,UINT32 param1,UINT32 param2,UINT32 param3);

/*提供者：CA模块
使用者：机顶盒
何时调用：开始进行子母卡操作时
功能描述：开始进行子母卡操作
输入参数： 无
输出参数： 无 
返回值：  0x01--成功
其他说明:无
*/
INT32 DIVI_Active_ChildCard();
/*提供者：CA模块
使用者：机顶盒
何时调用：停止进行子母卡操作时
功能描述：停止进行子母卡操作
输入参数：无
输出参数：无
返回值：  0x01--成功
其他说明:无
*/
INT32 DIVI_Finish_ChildCard();

//获取当前日期和时间, 2013-02-26
//用于在机顶盒无法获取TDT时显示当前时间
//建议格式: YYYY-MM-DD HH:MM
//CurrentTime[0]-CurrentTime[4]分别为年月日时分
//其中年份需要加2000
INT32 Divi_GetCurrentTime(UINT8 *CurrentTime);

//2015-06-17, 可用于控制清流节目、OTT等其他特定业务有无收视权限 
//返回值为1: 表示DVB基础产品包有授权，返回值为0: 表示DVB基础产品包无授权 
BOOL DIVI_GetProductOneIs();

//以下两个函数暂置为空
extern BOOL DIVI_Show_Finger(UINT8 Position,UINT8 Duriaon,UINT8 * Finger_Content,UINT8 Content_Length);

extern INT8 DIVI_GetFingerNum();


#ifdef __cplusplus
}
#endif

#endif

