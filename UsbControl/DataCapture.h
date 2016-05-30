#pragma once
#include "DataProcess.h"

class CDataCapture
{
public:
	CDataCapture(void);
	virtual ~CDataCapture(void);

public:
	int Open(CDataProcess *pProcess);
	int Close();
	int Input(const LPVOID lpData,const DWORD dwSize);

private:
	CDataProcess *m_pDataProcess;
	int			m_iCount;		//���ݼ�����
	int			m_iRowIndex;	//������
	bool        m_bFindDbFive;	//����Ƿ��ҵ�55
	byte*		m_pInData;		//�������ݻ���
	byte*		m_pOutData;		//������ݻ���
};

