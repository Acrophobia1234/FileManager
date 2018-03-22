
// FileManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileManager.h"
#include "FileManagerDlg.h"
#include "afxdialogex.h"

#define WM_FIXFILEINFO WM_USER + 100
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFileManagerDlg �Ի���
static UINT __Indicators[] =
{
	ID_SEPARATOR,
	IDR_STATUSBAR_PROGRESS,
	ID_SEPARATOR,

};
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileManagerDlg �Ի���



CFileManagerDlg::CFileManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FILEMANAGER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SHFILEINFO	sfi;
	HIMAGELIST ImageListHwnd;
	//����ϵͳͼ���б�
	ImageListHwnd = (HIMAGELIST)SHGetFileInfo(NULL, 0,
		&sfi, sizeof(SHFILEINFO), SHGFI_LARGEICON | SHGFI_SYSICONINDEX);
	m_CImageList_Large = CImageList::FromHandle(ImageListHwnd);   //CimageList*
																  //����ϵͳͼ���б�
	ImageListHwnd = (HIMAGELIST)SHGetFileInfo(NULL, 0, &sfi,
		sizeof(SHFILEINFO), SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
	m_CImageList_Small = CImageList::FromHandle(ImageListHwnd);
}

void CFileManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILE, m_CListCtrl_File);
}

BEGIN_MESSAGE_MAP(CFileManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE, &CFileManagerDlg::OnNMDblclkListFile)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_FIXFILEINFO,OnFixFileInfo)
END_MESSAGE_MAP()


// CFileManagerDlg ��Ϣ�������

BOOL CFileManagerDlg::OnInitDialog()
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	DWORD WindowWidth = 0;
	DWORD WindowHeight = 0;

	RECT Rect;
	GetWindowRect(&Rect);  //��ǰDialog��С
	WindowWidth = Rect.right  ;
	SetWindowPos(NULL, 0, 0, WindowWidth, Rect.bottom, SWP_NOMOVE);

	if (!m_StatusBar_File.Create(this) ||
		!m_StatusBar_File.SetIndicators(__Indicators,
			sizeof(__Indicators) / sizeof(UINT)))
	{
		return -1;
	}

	CRect v1;
	m_StatusBar_File.GetWindowRect(&Rect);
	v1.bottom = Rect.bottom - 8;
	v1.left = Rect.left - 8;
	v1.right = Rect.right - 8;
	v1.top = v1.bottom - 20;
	m_StatusBar_File.SetPaneInfo(0, m_StatusBar_File.GetItemID(0), SBPS_STRETCH, NULL);
	m_StatusBar_File.SetPaneInfo(1, m_StatusBar_File.GetItemID(1), SBPS_NORMAL, WindowWidth / 7);
	m_StatusBar_File.SetPaneInfo(2, m_StatusBar_File.GetItemID(2), SBPS_NORMAL, WindowWidth / 3);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //��ʾ״̬��
	//m_StatusBar_File.MoveWindow(v1);
	m_ProgressCtrl_File = new CProgressCtrl;
	m_StatusBar_File.GetItemRect(2, &Rect);
	m_ProgressCtrl_File->Create(PBS_SMOOTH | WS_VISIBLE, Rect, &m_StatusBar_File, 2);


	//ControlList
	m_CListCtrl_File.SetImageList(m_CImageList_Large, LVSIL_NORMAL);
	m_CListCtrl_File.SetImageList(m_CImageList_Small, LVSIL_SMALL);

	InitServerUSN();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFileManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFileManagerDlg::OnPaint()
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

BOOL CFileManagerDlg::InitServerUSN()
{
	// ����������Disable
	this->EnableWindow(FALSE);
	int Count = 0;

	// �ҵ������̷�
	CHAR  VolumeName[] = "A:\\";
	CString FileSystem;

	while (VolumeName[0] <= 'Z')
	{
		// �жϵ�ǰ�̷��ǲ���NTFS��
		if (GetVolumeFileSystem(VolumeName, FileSystem) == FALSE)
		{
			VolumeName[0]++;
			continue;
		}
		if (_stricmp(FileSystem.GetBuffer(0), "NTFS") == 0)	// NTFSϵͳ -
		{
			// ���� CVolume �࣬ѹ������
			m_Volumes.push_back(_CVolume(VolumeName[0]));
			Count++;
		}
		VolumeName[0]++;
	}
	m_ProgressCtrl_File->SetRange(0, Count * 100);
	m_ProgressCtrl_File->SetPos(0);
	AfxBeginThread(ThreadProcedure, (LPVOID)this);
	return TRUE;
}
UINT CFileManagerDlg::ThreadProcedure(LPVOID ParameterData)
{
	CFileManagerDlg* This = (CFileManagerDlg*)ParameterData;
	if (This != nullptr)
	{
		return This->RealThreadProcedure(NULL);
	}
	return 0;
}
UINT CFileManagerDlg::RealThreadProcedure(LPVOID ParameterData)
{
	int Count = 0;
	for (vector<_CVolume>::iterator i = m_Volumes.begin(); i != m_Volumes.end(); i++)
	{
		m_StatusBar_File.SetWindowText("���ڳ�ʼ�� " + CString(i->m_VolumeValue) + ":\\");
		// ÿ���̷����ʼ���Լ���MAP
		i->InitVolumeUSN();
		m_ProgressCtrl_File->SetPos((Count + 1) * 100);
		// ���������غ��߳� 
		CHAR EventName[] = "MyEventForC";
		EventName[10] = i->m_VolumeValue;
		HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, EventName);
		m_MonitorThreadHandle[Count] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MonitorProcedure, (PVOID)&(*i), 0, 0);
		if (m_MonitorThreadHandle[Count] == NULL)
		{
			MessageBox("Start Monitor Thrad Failed");
		}
		Count++;
	}
	this->EnableWindow();
	FixedVolumeInfo();
	
	m_StatusBar_File.SetWindowText("��ʼ�����");
	m_ProgressCtrl_File->SetPos(0);
	return true;
}
//ע�⣺��D�̺�ֻ��D�ļ����߳̿�������
DWORD MonitorProcedure(LPVOID ParameterData)
{
	CHAR EventName[] = "MyEventForC";
	
	const DWORD MonitorUSNReason = USN_REASON_FILE_CREATE | USN_REASON_FILE_DELETE |
		USN_REASON_RENAME_OLD_NAME | USN_REASON_RENAME_NEW_NAME;

	_CVolume* This = (_CVolume*)ParameterData;  //Char Map
	BOOL IsOk = FALSE;
	DWORD ReturnLength = 0;

	// �򿪴���
	CHAR   DiskVolumeName[] = "\\\\.\\A:";
	DiskVolumeName[4] = This->m_VolumeValue;
	EventName[10] = This->m_VolumeValue;
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, NULL, EventName);
	WaitForSingleObject(hEvent, INFINITE);

	HANDLE DiskVolumeHandle = CreateFileA(DiskVolumeName, GENERIC_READ |
		GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (DiskVolumeHandle == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	// �õ�USN_DATA �õ���һ�α����������
	USN_JOURNAL_DATA_V0  USNJournalData = { 0 };
	IsOk = DeviceIoControl(DiskVolumeHandle,
		FSCTL_QUERY_USN_JOURNAL, nullptr, 0, &USNJournalData, sizeof(USN_JOURNAL_DATA_V0), &ReturnLength,
		NULL);
	if (!IsOk)
	{
		return -1;
	}
	// ������ӽṹ�� - �ؼ��Ǽ������� 
	READ_USN_JOURNAL_DATA_V0 ReadUSNJournalData = { 0 };
	ReadUSNJournalData.BytesToWaitFor = 1;//or 0
	ReadUSNJournalData.ReasonMask = MonitorUSNReason;
	ReadUSNJournalData.ReturnOnlyOnClose = FALSE;
	ReadUSNJournalData.StartUsn = USNJournalData.NextUsn;
	ReadUSNJournalData.Timeout = 0;
	ReadUSNJournalData.UsnJournalID = USNJournalData.UsnJournalID;



	BYTE BufferData[USN_PAGE_SIZE] = { 0 };
	PUSN_RECORD_V2 USNRecordData = NULL;
	DWORD USNRecordLength = 0;
	while (true)
	{
		Sleep(rand() % 1000 + 1000);
		// �������� - ���ӵ��µ�USN�Ż᷵��
		IsOk = DeviceIoControl(DiskVolumeHandle, FSCTL_READ_USN_JOURNAL, &ReadUSNJournalData, sizeof(READ_USN_JOURNAL_DATA_V0),
			BufferData, USN_PAGE_SIZE, &ReturnLength, NULL);
		if (!IsOk)
		{
			return -1;
		}
		// �������ݽṹ: USN + n��USN_RECORD
		if (ReturnLength < sizeof(USN))
			continue;

		USNRecordLength = ReturnLength - sizeof(USN);
		USNRecordData = (PUSN_RECORD)(BufferData + sizeof(USN));

		This->m_CriticalSection.Lock();
		while (USNRecordLength > 0)
		{
			//�޸����ݽṹ
			This->ChangeFileMap(USNRecordData->FileAttributes, USNRecordData->FileReferenceNumber,
				USNRecordData->Reason, USNRecordData->ParentFileReferenceNumber,
				PWCHAR(USNRecordData->FileName), USNRecordData->FileNameLength);
			USNRecordLength -= USNRecordData->RecordLength;
 			USNRecordData = (PUSN_RECORD)(((PBYTE)USNRecordData) + USNRecordData->RecordLength);
		}
		This->m_CriticalSection.Unlock();
		ReadUSNJournalData.StartUsn = *(USN*)BufferData;
		HWND pWnd = ::FindWindowA(0, "FileManager");
		SendMessageA(pWnd, WM_FIXFILEINFO, 0, 0);
	}
	

	return 0;
}
BOOL CFileManagerDlg::GetVolumeFileSystem(CHAR * VolumeName, CString& FileSystem)
{
	CHAR v1[MAX_PATH] = { 0 };
	BOOL IsOk = GetVolumeInformationA(VolumeName, NULL, 0, NULL, NULL, NULL, v1, MAX_PATH);
	if (IsOk == TRUE)
	{
		FileSystem = v1;   //��̬�ڴ�
	}
	return IsOk;
}



//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFileManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFileManagerDlg::OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (m_CListCtrl_File.GetSelectedCount() == 0 ||
		m_CListCtrl_File.GetItemData(m_CListCtrl_File.GetSelectionMark()) != 1)
		return;

	FixedFileInfo();
	*pResult = 0;
}


void CFileManagerDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnClose();
}
LRESULT CFileManagerDlg::OnFixFileInfo(WPARAM wParam, LPARAM lParam)
{
	this->FixedFileInfo();
	return LRESULT();
}
VOID CFileManagerDlg::FixedVolumeInfo()
{
	m_CListCtrl_File.DeleteAllItems();
	while (m_CListCtrl_File.DeleteColumn(0) != 0);


	//��ʼ���б���Ϣ
	//m_CurrentPathReference = 0;	// ��ʾ���� - m_CurrenPathReference ���� 0
	// ��¼
	//m_PathHistroy.emplace_back(m_CurrentPathReference);

	//CurrentPathIt = --m_PathHistroy.end();		// ָ�����һ�������Ԫ��

	m_CListCtrl_File.InsertColumn(0, "����", LVCFMT_CENTER, 40);
	m_CListCtrl_File.InsertColumn(1, "����", LVCFMT_CENTER, 80);
	m_CListCtrl_File.InsertColumn(2, "�ļ�ϵͳ", LVCFMT_CENTER, 80);
	m_CListCtrl_File.InsertColumn(3, "�ܴ�С", LVCFMT_CENTER, 80);
	m_CListCtrl_File.InsertColumn(4, "���ÿռ�", LVCFMT_CENTER, 80);
	m_CListCtrl_File.InsertColumn(5, "Reference", LVCFMT_CENTER, 0);	// �����з�����ǰ�� - ��¼Reference �������ı���

	m_CListCtrl_File.SetExtendedStyle(m_CListCtrl_File.GetExtendedStyle()
		| LVS_EX_FULLROWSELECT);

	CHAR	FileSystem[MAX_PATH] = { 0 };     //ntfs 
	unsigned __int64	HardDiskAmountBytes = 0;
	unsigned __int64	HardDiskFreeSpaceBytes = 0;
	unsigned long		HardDiskAmountMB = 0;      // �ܴ�С
	unsigned long		HardDiskFreeSpaceMB = 0;   // ʣ��ռ�
	int  VolumeCount = 0;
	CHAR VolumeValue[] = "A:\\";
	for (vector<_CVolume>::iterator i = m_Volumes.begin(); i != m_Volumes.end(); i++)
	{
		VolumeValue[0] = i->m_VolumeValue;
		GetVolumeInformationA(VolumeValue, NULL, 0, NULL, NULL, NULL, FileSystem, MAX_PATH);

		ULONG	FileSystemLength = lstrlen(FileSystem) + 1;
		if (GetDiskFreeSpaceEx(VolumeValue,
			(PULARGE_INTEGER)&HardDiskFreeSpaceBytes, (PULARGE_INTEGER)&HardDiskAmountBytes, NULL))
		{
			HardDiskAmountMB = HardDiskAmountBytes / 1024 / 1024;
			HardDiskFreeSpaceMB = HardDiskFreeSpaceBytes / 1024 / 1024;
		}
		else
		{
			HardDiskAmountMB = 0;
			HardDiskFreeSpaceMB = 0;
		}


		int	Item = m_CListCtrl_File.InsertItem(VolumeCount,
			VolumeValue, GetServerIconInfo(VolumeValue, NULL, GetFileAttributes(VolumeValue)));    //���ϵͳ��ͼ��		

		m_CListCtrl_File.SetItemData(Item, 1);  //�������� 1=Ŀ¼ 0=�ļ�

		SHFILEINFO	sfi;
		SHGetFileInfo(VolumeValue, FILE_ATTRIBUTE_NORMAL,
			&sfi, sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
		m_CListCtrl_File.SetItemText(Item, 1, sfi.szTypeName); //���ش��� �ƶ�����
		m_CListCtrl_File.SetItemText(Item, 2, FileSystem);     //�ļ�ϵͳ
		CString	v1;
		v1.Format("%10.1f GB", (float)HardDiskAmountMB / 1024);
		m_CListCtrl_File.SetItemText(Item, 3, v1);
		v1.Format("%10.1f GB", (float)HardDiskFreeSpaceMB / 1024);
		m_CListCtrl_File.SetItemText(Item, 4, v1);
		CString Reference;
		Reference.Format("%lld", 0x5000000000005);
		m_CListCtrl_File.SetItemText(Item, 5, Reference);
		VolumeCount++;
	}
}
int	CFileManagerDlg::GetServerIconInfo(LPCTSTR VolumeValue, LPCTSTR FileFullPathData,
	DWORD FileAttributes)
{
	SHFILEINFO	sfi;
	if (FileAttributes == INVALID_FILE_ATTRIBUTES)
		FileAttributes = FILE_ATTRIBUTE_NORMAL;
	else
		FileAttributes |= FILE_ATTRIBUTE_NORMAL;

	SHGetFileInfo
	(
		VolumeValue,
		FileAttributes,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | SHGFI_ICON
	);

	// 3 ��ϵͳ�ļ���ͼ������ - ����������жϣ�ʹ���Լ����ļ���ͼ��
	if (sfi.iIcon == 3)
	{
		if (PathIsDirectoryEmptyA(FileFullPathData))
		{
			//return m_EmptyFolderIconIndex;
		}
		else
		{

		}
		//return m_FolderIconIndex;
	}
	else
	{
		return sfi.iIcon;
	}

	return sfi.iIcon;
}
VOID CFileManagerDlg::FixedFileInfo(CString FileDirectory)
{
	vector<FILE_NODE> FindedFiles;   //��ǰĿ¼�µ�������Ϣ  
									 //                                            0x5000000000005
	CString Reference ;   //Map���ݽṹ������  ��USN_NODE����USN_NODE����USN_NODE����USN_NODE��������������

	if (FileDirectory.GetLength() == 0)
	{
		int	Item = m_CListCtrl_File.GetSelectionMark();

		//ѡ������Ŀ
		if (Item != -1)
		{
			//����ΪĿ¼
			if (m_CListCtrl_File.GetItemData(Item) == 1)
			{

				FileDirectory = m_CListCtrl_File.GetItemText(Item, 0);
				CHeaderCtrl* v1 = m_CListCtrl_File.GetHeaderCtrl();  //��ø���

																			//�������ж��ٸ���ʾ����Ϣ(�м���)
				int Count = v1->GetItemCount();
				//��ʾ���̱���Ĭ���ȵ㿪��Ŀ¼������������Ҫ�ĸ�ֵ������
				if (Count == 6)  //��Ŀ¼  C:\  D:\  
				{
					// ��Ŀ¼��
					Reference.Format("%lld", 0x5000000000005);
					// ���õ�ǰ���ڵ��̷���
					for (vector<_CVolume>::iterator i = m_Volumes.begin(); i != m_Volumes.end(); i++)
					{
						if (i->m_VolumeValue == FileDirectory[0])
						{
							//iʵ������_CVolume��ָ��
							m_Volume = i;
							break;
						}
					}
					m_CurrentPathReferenceFileServer = 0x5000000000005;
					CHAR EventName[] = "MyEventForC";
					EventName[10] = FileDirectory[0];
					
					HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, NULL, EventName);
					SetEvent(hEvent);
				}
				else if (Count == 5)  //���˸�Ŀ¼
				{
					//��һ���ļ���
					Reference = m_CListCtrl_File.GetItemText(Item, 4);
					m_CurrentPathReferenceFileServer = _atoi64(Reference);
				}
			}
		}
		// ����Ͽ���õ�·��
		else
		{
			 //m_CCombo_File_Server.GetWindowText(m_File_Server_Path);
			Reference.Format("%llu", m_CurrentPathReferenceFileServer);
		}
	}


	if (FileDirectory == "..")
	{
		GetParentDirectory(m_ParentDirectoryFileServer, m_ParentDirectoryFileServer);

		m_CurrentPathReferenceFileServer = m_Volume->FindParentReference(m_CurrentPathReferenceFileServer);
		Reference.Format("%lld", m_CurrentPathReferenceFileServer);

	}
	else if (FileDirectory != ".")
	{

		m_ParentDirectoryFileServer += FileDirectory;
		if (m_ParentDirectoryFileServer.Right(1) != "\\")
		{
			m_ParentDirectoryFileServer += "\\";
		}

		// �����¼ vector
		//m_PathHistroy.emplace_back(m_CurrentPathReference);
		//CurrentPathIt = --m_PathHistroy.end();
	}
	else
	{

	}


	// �ص����Ը�Ŀ¼
	if (m_ParentDirectoryFileServer.GetLength() == 0 || m_CurrentPathReferenceFileServer == 0)
	{
		FixedVolumeInfo();  //ˢ������Ϣ
		return;
	}

	// ����Combox
	/*m_CCombo_File_Server.InsertString(0, m_File_Server_Path);
	m_CCombo_File_Server.SetCurSel(0);*/
	//CListCtrl��ʼ��
	m_CListCtrl_File.DeleteAllItems();
	while (m_CListCtrl_File.DeleteColumn(0) != 0);  //ɾ��
	m_CListCtrl_File.InsertColumn(0, "����", LVCFMT_LEFT, 150);
	m_CListCtrl_File.InsertColumn(1, "��С", LVCFMT_LEFT, 100);
	m_CListCtrl_File.InsertColumn(2, "����", LVCFMT_LEFT, 100);
	m_CListCtrl_File.InsertColumn(3, "�޸�����", LVCFMT_LEFT, 115);
	m_CListCtrl_File.InsertColumn(4, "Reference", LVCFMT_LEFT, 0);

	int	FileCount = 0;  //��
	int ItemCount = 0;  //��

						// .. �����ϲ�Ŀ¼
	ItemCount = m_CListCtrl_File.InsertItem(FileCount++, "..",
		GetServerIconInfo(NULL, NULL, FILE_ATTRIBUTE_DIRECTORY));

	m_CListCtrl_File.SetItemText(ItemCount, 4, Reference);

	m_CListCtrl_File.SetItemData(ItemCount, 1);      //����ʾ����Ϣ      


	int i = 0;

	HANDLE FileHandle = INVALID_HANDLE_VALUE;

	// ����map
	m_Volume->FindFile(_atoi64(Reference), FindedFiles);  //��Map���ݽṹ�в�ѯ��Ϣ
	for (vector<FILE_NODE>::iterator i = FindedFiles.begin(); i != FindedFiles.end(); ++i)
	{
		ItemCount = m_CListCtrl_File.InsertItem(FileCount,
			i->FileNameData,
			GetServerIconInfo(i->FileNameData,
				m_ParentDirectoryFileServer + i->FileNameData,
				GetFileAttributes(m_ParentDirectoryFileServer + i->FileNameData)));	//  ����CtrlList
		DWORD FileAttributes = 0;

		if (PathIsDirectoryA(m_ParentDirectoryFileServer + i->FileNameData))
		{
			FileHandle = CreateFileA(m_ParentDirectoryFileServer + i->FileNameData,
				GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
			if (FileHandle == INVALID_HANDLE_VALUE)
			{
				continue;
			}
			m_CListCtrl_File.SetItemText(ItemCount, 1, 0);   //�ļ���С
			m_CListCtrl_File.SetItemText(ItemCount, 2, "�ļ���");


			FILETIME LastWriteTime = { 0 };
			GetFileTime(FileHandle, NULL, NULL, &LastWriteTime);
			m_CListCtrl_File.SetItemText(ItemCount, 3, CTime(LastWriteTime).Format("%Y-%m-%d %H:%M"));

			m_CListCtrl_File.SetItemData(ItemCount, 1);      //����ʾ��ʽ���� 

			CloseHandle(FileHandle);
			FileHandle = INVALID_HANDLE_VALUE;

		}
		else
		{
			FileHandle = CreateFileA(m_ParentDirectoryFileServer + i->FileNameData,
				GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (FileHandle == INVALID_HANDLE_VALUE)
			{
				continue;
			}

			LARGE_INTEGER FileSize = { 0 };
			GetFileSizeEx(FileHandle, &FileSize);
			CString v1;
			v1.Format("%10d KB", FileSize.QuadPart / 1024 + (FileSize.QuadPart % 1024 ? 1 : 0));
			m_CListCtrl_File.SetItemText(ItemCount, 1, v1);

			SHFILEINFO	SHFileInfo = { 0 };
			SHGetFileInfoA(i->FileNameData, FILE_ATTRIBUTE_NORMAL,
				&SHFileInfo, sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
			m_CListCtrl_File.SetItemText(ItemCount, 2, SHFileInfo.szTypeName);



			FILETIME LastWriteTime = { 0 };
			GetFileTime(FileHandle, nullptr, nullptr, &LastWriteTime);
			m_CListCtrl_File.SetItemText(ItemCount, 3, CTime(LastWriteTime).Format("%Y-%m-%d %H:%M"));
			m_CListCtrl_File.SetItemData(ItemCount, 0);

			CloseHandle(FileHandle);
			FileHandle = INVALID_HANDLE_VALUE;

		}
		Reference.Format("%lld", i->FileReferenceNumber);
		m_CListCtrl_File.SetItemText(ItemCount, 4, Reference);

		FileCount++;
	}
}
BOOL CFileManagerDlg::GetParentDirectory(CString& DestinationString, CString& SourceString)
{
	if (SourceString.IsEmpty())
	{
		return FALSE;
	}
	int Index = SourceString.ReverseFind('\\');
	if (Index == -1)
	{
		return FALSE;
	}
	printf("Index:%d\r\n", Index);
	DestinationString = SourceString.Left(Index);
	Index = DestinationString.ReverseFind('\\');
	printf("Index:%d\r\n", Index);
	if (Index == -1)
	{
		DestinationString = "";
		return TRUE;
	}
	DestinationString = DestinationString.Left(Index);
	if (DestinationString.Right(1) != "\\")
	{
		DestinationString += "\\";
	}
	if (DestinationString.IsEmpty())
	{
		return FALSE;
	}
	return TRUE;
}
