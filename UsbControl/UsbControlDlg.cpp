
// UsbControlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UsbControl.h"
#include "UsbControlDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int g_width;
int g_height;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()

};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CUsbControlDlg �Ի���




CUsbControlDlg::CUsbControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUsbControlDlg::IDD, pParent)
	, m_iRdoDriver(0)
	, m_iProcType(0)
	, m_iChangeType(0)
	, m_sEdit_Width(_T(""))
	, m_sEdit_Height(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pFileRbf=NULL;
	m_bUsbOpen=FALSE;
	m_bSendData=FALSE;
	memset(m_byData,0,sizeof(BYTE)*64);
	m_sUsbOrder.pData=m_byData;
	m_pVideoDlg=NULL;
	m_pDataProcess=NULL;
	m_pDataCapture=NULL;
	m_hDisplayDC=NULL;
	m_hThread=NULL;
	m_pReadBuff=new char[ReadDataBytes];
	memset(m_pReadBuff,0,sizeof(char)*ReadDataBytes);
	m_bCapture=FALSE;
	m_bCloseWnd=FALSE;
	m_lBytePerSecond=0;
	m_CyDriver=NEW_DRIVER;
	m_pBrush=NULL;
	m_pVideoDataFile=NULL;
	m_bReview=FALSE;
	m_bSave=FALSE;
	m_Init = FALSE;
}

CUsbControlDlg::~CUsbControlDlg()
{
	if(m_pReadBuff!=NULL)
	{
		delete[] m_pReadBuff;
		m_pReadBuff=NULL;
	}
}

void CUsbControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_NEWDRIVER, m_iRdoDriver);
	DDX_Radio(pDX, IDC_RADIO_NORMAL, m_iProcType);
	DDX_Radio(pDX, IDC_RADIO_NORMAL_CHANGE, m_iChangeType);
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Width);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_Height);
	DDX_Text(pDX, IDC_EDIT1, m_sEdit_Width);
	DDX_Text(pDX, IDC_EDIT2, m_sEdit_Height);
}

BEGIN_MESSAGE_MAP(CUsbControlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_OPENUSB, &CUsbControlDlg::OnBnClickedBtnOpenusb)
	ON_BN_CLICKED(IDC_BTN_OPENFILE, &CUsbControlDlg::OnBnClickedBtnOpenfile)
	ON_WM_TIMER()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_VIDEOCAPTURE, &CUsbControlDlg::OnBnClickedBtnVideocapture)
	ON_BN_CLICKED(IDC_BTN_STOPCAPTURE, &CUsbControlDlg::OnBnClickedBtnStopcapture)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_NEWDRIVER, &CUsbControlDlg::OnBnClickedRadioDriver)
	ON_BN_CLICKED(IDC_RADIO_OLDDRIVER, &CUsbControlDlg::OnBnClickedRadioDriver)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_NORMAL, &CUsbControlDlg::OnBnClickedRadioProcType)
	ON_BN_CLICKED(IDC_RADIO_XMIRROR, &CUsbControlDlg::OnBnClickedRadioProcType)
	ON_BN_CLICKED(IDC_RADIO_YMIRROR, &CUsbControlDlg::OnBnClickedRadioProcType)
	ON_BN_CLICKED(IDC_RADIO_XYMIRROR, &CUsbControlDlg::OnBnClickedRadioProcType)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CUsbControlDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_REVIEW, &CUsbControlDlg::OnBnClickedBtnReview)
	ON_BN_CLICKED(IDC_RADIO_NORMAL_CHANGE, &CUsbControlDlg::OnBnClickedRadioChangeType)
	ON_BN_CLICKED(IDC_RADIO_RG_CHANGE, &CUsbControlDlg::OnBnClickedRadioChangeType)
	ON_BN_CLICKED(IDC_RADIO_RB_CHANGE, &CUsbControlDlg::OnBnClickedRadioChangeType)
	ON_BN_CLICKED(IDC_RADIO_GB_CHANGE, &CUsbControlDlg::OnBnClickedRadioChangeType)
	ON_BN_CLICKED(IDC_BTN_CREATEBMP, &CUsbControlDlg::OnBnClickedBtnCreatebmp)
	ON_EN_CHANGE(IDC_EDIT1, &CUsbControlDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &CUsbControlDlg::OnEnChangeEdit2)
END_MESSAGE_MAP()


// CUsbControlDlg ��Ϣ�������

BOOL CUsbControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//------------------------------------
	CRect cRect,wRect,mRect;
	GetDesktopWindow()->GetWindowRect(wRect);
	GetWindowRect(cRect);
	mRect.right=wRect.right-50;
	mRect.bottom=wRect.bottom-50;
	mRect.left=mRect.right-cRect.Width();
	mRect.top=mRect.bottom-cRect.Height();
	MoveWindow(mRect);
	//------------------------------------
	m_pFileRbf=new CFile();
	m_pVideoDlg=new CVideoDlg();
	m_pDataProcess=new CDataProcess();
	m_pDataCapture=new CDataCapture();
	m_pVideoDlg->Create(IDD_DLG_VIDEO,this);
	m_pVideoDlg->ShowWindow(FALSE);
	m_hDisplayDC=m_pVideoDlg->GetDisplayDC()->m_hDC;

	SetTimer(1,1000,NULL);

	CyUsb_Init(m_CyDriver);
	m_iRdoDriver=(int)m_CyDriver;

	m_pBrush=new CBrush[2];
	m_pBrush[0].CreateSolidBrush(RGB(99,208,242));
	m_pBrush[1].CreateSolidBrush(RGB(174,238,250));
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CUsbControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();//���ؿ���̨����ģ��
		int temp1 = 1;
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUsbControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CUsbControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUsbControlDlg::OnBnClickedBtnOpenusb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_bSendData)
	{
		AfxMessageBox(L"���ݷ����У����Եȣ�");
		return;
	}
	if(m_bUsbOpen)
	{
		CloseUsb();
	}
	m_bUsbOpen=OpenUsb()==0?TRUE:FALSE;
	if(!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_TEXT,L"USB�豸��ʧ�ܣ�");
		return;
	}
	SetDlgItemText(IDC_STATIC_TEXT,L"USB�豸�򿪳ɹ���");
}

void CUsbControlDlg::OnBnClickedBtnOpenfile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog fDlg(true);
	fDlg.m_ofn.lpstrFilter=L"RBF Files(*.rbf)\0*.rbf\0\0";
	if(fDlg.DoModal()==IDOK)
	{
		m_strRbfFileName=fDlg.GetPathName();
		BOOL bFileOpen=FALSE;
		bFileOpen=OpenRbfFile();
		if(bFileOpen&&m_bUsbOpen)
		{
			m_sUsbOrder.ReqCode=0xC0;
			m_sUsbOrder.DataBytes=1;
			m_sUsbOrder.Direction=1;
			for(int i=0;i<50;++i)	
			{
				if(SendOrder(&m_sUsbOrder)==0)
				{
					switch(m_byData[0])
					{
					case 0x00:
						Sleep(1);
						break;
					case 0xC0:
						SetDlgItemText(IDC_STATIC_TEXT,L"Ӳ����ʼ���ɹ�,��ʼ�����������ݣ�");
						SendData();
						CloseRbfFile();
						return;
					default:
						break;
					}
				}
			}
			CloseRbfFile();
			SetDlgItemText(IDC_STATIC_TEXT,L"Ӳ����ʼ��ʧ��,�����³�ʼ����");
		}
	}
}

BOOL CUsbControlDlg::CloseRbfFile()
{
	if(m_pFileRbf!=NULL)
	{
		if(m_pFileRbf->m_hFile!=CFile::hFileNull)
		{
			m_pFileRbf->Close();
		}
		delete m_pFileRbf;
		m_pFileRbf=NULL;
	}
	return TRUE;
}

BOOL CUsbControlDlg::OpenRbfFile()
{
	CloseRbfFile();
	m_pFileRbf=new CFile;
	return m_pFileRbf->Open(m_strRbfFileName,CFile::modeRead|CFile::shareDenyNone);
}

void CUsbControlDlg::SendData()
{
	try
	{
		m_bSendData=TRUE;
		BOOL bSend=TRUE;
		do 
		{
			m_sUsbOrder.DataBytes=m_pFileRbf->Read(m_sUsbOrder.pData,64);
			if(m_sUsbOrder.DataBytes>0)
			{
				m_sUsbOrder.ReqCode=0xC1;
				m_sUsbOrder.Direction=0;
				SendOrder(&m_sUsbOrder);
				SetDlgItemText(IDC_STATIC_TEXT,L"FPGA�������ݷ�����.......");
			}
			else
			{
				m_sUsbOrder.ReqCode=0xC2;
				m_sUsbOrder.DataBytes=1;
				m_sUsbOrder.Direction=1;
				SetDlgItemText(IDC_STATIC_TEXT,L"FPGA�������ݷ��ͽ���.......");

				SendOrder(&m_sUsbOrder);
				switch(m_byData[0])
				{
				case 0xC2:
					SetDlgItemText(IDC_STATIC_TEXT,L"FPGA���óɹ���");
					break;
				case 0x00:
				default:
					SetDlgItemText(IDC_STATIC_TEXT,L"FPGA����ʧ��,���������ã�");
					break;
				}
				bSend=FALSE;
			}
		} while(bSend);
		m_bSendData=FALSE;
	}
	catch (CException* e)
	{
		if(CloseUsb()==0)
		{
			m_bUsbOpen=FALSE;
			SetDlgItemText(IDC_STATIC_TEXT,L"�����쳣�����������ã�");
		}
		else
		{
			SetDlgItemText(IDC_STATIC_TEXT,L"�쳣�޷��޸�����������������");
		}
	}
}

void CUsbControlDlg::OnBnClickedBtnVideocapture()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_pVideoDlg->ShowWindow(TRUE);
	m_bCapture=TRUE;
	if (!m_Init)
	{
		m_pDataProcess->Open(m_hDisplayDC);
		m_pDataCapture->Open(m_pDataProcess);	
		m_hThread=(HANDLE)_beginthreadex(NULL,0,ThreadProcess,this,0,NULL);
		m_Init = TRUE;
	}
}

unsigned int __stdcall CUsbControlDlg::ThreadProcess( void* pParam )
{
	CUsbControlDlg* pThis=(CUsbControlDlg*)pParam;
	pThis->ThreadProcessFunction();
	return 0;
}

void CUsbControlDlg::ThreadProcessFunction()
{
	long lRet=0;
	while(TRUE)
	{
		if(m_bCloseWnd)
			break;
		if(m_bCapture)
		{
			lRet=ReadDataBytes;
			ReadData(m_pReadBuff,lRet);
			if(lRet>0)
			{
				m_lBytePerSecond+=lRet;
				m_pDataCapture->Input(m_pReadBuff,lRet);
			}
			if(m_bSave)
			{
				if(m_pVideoDataFile!=NULL&&m_pVideoDataFile->m_hFile!=CFile::hFileNull)
				{
					m_pVideoDataFile->Write(m_pReadBuff,lRet);
				}
			}
		}
		else if(m_bReview)
		{
			if(m_pVideoDataFile!=NULL&&m_pVideoDataFile->m_hFile!=CFile::hFileNull)
			{
				lRet=ReadDataBytes;
				lRet=m_pVideoDataFile->Read(m_pReadBuff,lRet);
				if(lRet>0)
				{
					m_lBytePerSecond+=lRet;
					m_pDataCapture->Input(m_pReadBuff,lRet);
				}
				else
				{
					m_bReview=FALSE;
					m_bSave=FALSE;
					AfxMessageBox(L"���ݶ�ȡ��ϣ�");
				}
				Sleep(18);//ʹ�ط�ʱ��fps�Ͳɼ�ʱ��fps���Ʊ���һ��
			}
		}
		else
		{
			Sleep(10);
		}
	}
}

void CUsbControlDlg::OnBnClickedBtnStopcapture()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bCapture=FALSE;
	m_pVideoDlg->ShowWindow(FALSE);
	m_bSave=FALSE;
	CloseDataFile();
}

void CUsbControlDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO: �ڴ˴������Ϣ����������
	KillTimer(1);
	m_bCapture=FALSE;
	m_bCloseWnd=TRUE;
	m_pDataProcess->Close();
	Sleep(100);
	CloseRbfFile();
	CloseDataFile();
	if(m_pDataCapture!=NULL)
	{
		m_pDataCapture->Close();
		delete m_pDataCapture;
		m_pDataCapture=NULL;
	}

	if(m_pDataProcess!=NULL)
	{
		m_pDataProcess->Close();
		delete m_pDataProcess;
		m_pDataProcess=NULL;
	}

	if(m_pVideoDlg!=NULL)
	{
		delete m_pVideoDlg;
		m_pVideoDlg=NULL;
	}
	CloseUsb();
	CyUsb_Destroy();
	if(m_pBrush!=NULL)
	{
		for(int i=0;i<2;++i)
		{
			if(m_pBrush[i].m_hObject!=NULL)
			{
				m_pBrush[i].DeleteObject();
			}
		}
		delete[] m_pBrush;
		m_pBrush=NULL;
	}
}

void CUsbControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int iFrame=0;
	CString str;
	switch(nIDEvent)
	{
	case 1:
		{
			if(m_pDataProcess!=NULL)
			{
				m_pDataProcess->GetFrameCount(iFrame);
				str.Format(L"%d Fps     %0.4f MBs",iFrame,float(m_lBytePerSecond)/1024.0/1024.0);
				m_lBytePerSecond=0;
				if(m_pVideoDlg!=NULL)
				{
					m_pVideoDlg->SetWindowText(str);
				}
			}
		}
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CUsbControlDlg::OnBnClickedRadioDriver()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_bCapture||m_bSendData)
	{
		UpdateData(FALSE);
		AfxMessageBox(L"����ֹͣ�ɼ��������ú���ѡ���������ͣ�");
		return;
	}
	UpdateData(TRUE);
	m_CyDriver=(CYUSB_DRIVER)m_iRdoDriver;
	CyUsb_Init(m_CyDriver);
}

void CUsbControlDlg::OnBnClickedRadioProcType()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_pDataProcess!=NULL)
	{
		UpdateData(TRUE);
		m_pDataProcess->SetProcType(DataProcessType(m_iProcType));
	}	
}

void CUsbControlDlg::OnBnClickedRadioChangeType()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_pDataProcess!=NULL)
	{
		UpdateData(TRUE);
		m_pDataProcess->SetChangeType(RgbChangeType(m_iChangeType));
	}	
}

HBRUSH CUsbControlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  �ڴ˸��� DC ���κ�����
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	int ID=pWnd->GetDlgCtrlID();
	if(ID==IDC_STATIC_TEXT)
	{
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
	}
	switch(nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_BTN:
		return m_pBrush[0];
	case CTLCOLOR_STATIC:
		return m_pBrush[0];
	default:
		return hbr;
	}
}

void CUsbControlDlg::OnBnClickedBtnSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(!m_bCapture)
	{
		AfxMessageBox(L"���Ƚ�����Ƶ�ɼ���");
		return;
	}
	m_bSave=TRUE;
	m_bReview=FALSE;

	CFileDialog fDlg(FALSE);
	fDlg.m_ofn.lpstrFilter=L"VideoData Files(*.vdata)\0*.vdata\0\0";
	fDlg.m_ofn.lpstrDefExt=L".vdata";
	if(fDlg.DoModal()==IDOK)
	{
		m_strDataFileName=fDlg.GetPathName();
		OpenDataFile();
	}
}

void CUsbControlDlg::OnBnClickedBtnReview()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_bCapture)
		m_bCapture=FALSE;
	m_pVideoDlg->ShowWindow(TRUE);
	m_bReview=TRUE;
	m_bSave=FALSE;

	CFileDialog fDlg(TRUE);
	fDlg.m_ofn.lpstrFilter=L"VideoData Files(*.vdata)\0*.vdata\0\0";
	if(fDlg.DoModal()==IDOK)
	{
		m_strDataFileName=fDlg.GetPathName();
		OpenDataFile();
	}
	if (!m_Init)
	{
		m_pDataProcess->Open(m_hDisplayDC);
		m_pDataCapture->Open(m_pDataProcess);	
		m_hThread=(HANDLE)_beginthreadex(NULL,0,ThreadProcess,this,0,NULL);
		m_Init = TRUE;
	}
}

BOOL CUsbControlDlg::OpenDataFile()
{
	CloseDataFile();
	m_pVideoDataFile=new CFile();
	BOOL bRet=FALSE;
	if(m_bSave&&!m_bReview)
	{
		 bRet=m_pVideoDataFile->Open(m_strDataFileName,CFile::modeCreate|CFile::modeWrite);
	}
	else if(!m_bSave&&m_bReview)
	{
		 bRet=m_pVideoDataFile->Open(m_strDataFileName,CFile::modeRead);
	}
	return bRet;
}

BOOL CUsbControlDlg::CloseDataFile()
{
	if(m_pVideoDataFile!=NULL)
	{
		if(m_pVideoDataFile->m_hFile!=CFile::hFileNull)
		{
			m_pVideoDataFile->Close();
		}
		delete m_pVideoDataFile;
		m_pVideoDataFile=NULL;
	}
	return TRUE;
}

void CUsbControlDlg::OnBnClickedBtnCreatebmp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_pDataProcess!=NULL)
	{
		m_pDataProcess->CreateBmp();
	}
}


void CUsbControlDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	/*-------�����ַ����룬������Ϣ��*/
	/*
	CString s;
	s.GetBufferSetLength(1024);
	m_Edit_Width.GetWindowTextW(s.GetBuffer(),s.GetLength());
	MessageBox(s,_T("��ȡͼ����"),MB_OK);
	s.ReleaseBuffer();
	*/	
	CString s_temp;
	UpdateData(true);
	s_temp = m_sEdit_Width.GetString();
	g_width = _tstoi(s_temp);
	s_temp.ReleaseBuffer();
}


void CUsbControlDlg::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString s_temp;
	UpdateData(true);
	s_temp = m_sEdit_Height.GetString();
	g_height = _tstoi(s_temp);
	s_temp.ReleaseBuffer();
}
