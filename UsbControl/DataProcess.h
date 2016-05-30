#pragma once
#include "MessageQueue.h"
#include "Display.h"
#include "ScreenShot.h"

extern int g_width;
extern int g_height;
//extern int g_width_L;

//#define RGB565
//#define	Array2_Index(i,j)	  i*Video_Width+j
#ifdef RGB565
#define g_width_L                       g_width*2
#else
#define g_width_L                       g_width
#endif
#define Sig_L							4
#define ReadDataBytes                   524288//(Video_Width_L+Sig_L)*Video_Height
#define MONOY	
enum DataProcessType
{
	Normal_Proc,Xmirror_Proc,Ymirror_Proc,XYmirror_Proc,RGExchange_Proc
};

enum RgbChangeType//R G B��Ԫ�ػ�������
{
	Normal_Change,RG_Change,RB_Change,GB_Change
};

struct tagRGB
{
	byte B;
	byte G;
	byte R;
	tagRGB()
	{
		memset(this,0,sizeof(*this));
	}
};

class CDataProcess
{
public:
	CDataProcess(void);
	~CDataProcess(void);

public:
	int Open(HDC hDC);
	int Close();
	int	Input(VOID * pData,int dwSizes);
	int GetFrameCount(int& fCount);
	int SetProcType(DataProcessType type);
	int SetChangeType(RgbChangeType type);
	int CreateBmp();

private:
	void ThreadProcessFunction();
	static unsigned int __stdcall ThreadProcess(void *handle);

private:
	//int ByteToRGB(byte pIn[Video_Height][Video_Width_L] ,tagRGB pOut[Video_Height][Video_Width]);
	int	ProcessData();
	int ByteToRGB(byte *pIn ,tagRGB* pOut);
	int PutMessage(UD_MESSAGE *msg);
	void CloseMsgQueue();
	void DoXmirrorProc();//X�᾵����
	void DoYmirrorProc();//Y�᾵����
	void RgbChangeProc(tagRGB& DestRgb,const tagRGB& OrgRgb);//RGBԪ�ػ�������
	void CreateBmpFile();

private:
	HANDLE m_hThread;
	UD_MESSAGE *m_pPutMsg, *m_pGetMsg;
	CMessageQueue m_MsgQueue;
	C_Mutex	m_Mutex;
	BOOL m_bEnd;
	int m_lFrameCount;//ͼ��֡����
	DataProcessType m_ProcType;
	RgbChangeType m_ChangeType;

private:
	BITMAPINFO	m_BitmapInfo;
	HDC			m_hDC;
	byte*       m_In;
	tagRGB*     m_Out;
	CDisplay*   m_pDisplay;
	BOOL        m_bCreateBmp;
};
