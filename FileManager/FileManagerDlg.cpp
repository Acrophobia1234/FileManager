
// FileManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FileManager.h"
#include "FileManagerDlg.h"
#include "afxdialogex.h"

#define WM_FIXFILEINFO WM_USER + 100
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFileManagerDlg 对话框
static UINT __Indicators[] =
{
	ID_SEPARATOR,
	IDR_STATUSBAR_PROGRESS,
	ID_SEPARATOR,

};
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CFileManagerDlg 对话框



CFileManagerDlg::CFileManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FILEMANAGER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SHFILEINFO	sfi;
	HIMAGELIST ImageListHwnd;
	//加载系统图标列表
	ImageListHwnd = (HIMAGELIST)SHGetFileInfo(NULL, 0,
		&sfi, sizeof(SHFILEINFO), SHGFI_LARGEICON | SHGFI_SYSICONINDEX);
	m_CImageList_Large = CImageList::FromHandle(ImageListHwnd);   //CimageList*
																  //加载系统图标列表
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


// CFileManagerDlg 消息处理程序

BOOL CFileManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	DWORD WindowWidth = 0;
	DWORD WindowHeight = 0;

	RECT Rect;
	GetWindowRect(&Rect);  //当前Dialog大小
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
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //显示状态栏
	//m_StatusBar_File.MoveWindow(v1);
	m_ProgressCtrl_File = new CProgressCtrl;
	m_StatusBar_File.GetItemRect(2, &Rect);
	m_ProgressCtrl_File->Create(PBS_SMOOTH | WS_VISIBLE, Rect, &m_StatusBar_File, 2);


	//ControlList
	m_CListCtrl_File.SetImageList(m_CImageList_Large, LVSIL_NORMAL);
	m_CListCtrl_File.SetImageList(m_CImageList_Small, LVSIL_SMALL);

	InitServerUSN();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFileManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

BOOL CFileManagerDlg::InitServerUSN()
{
	// 将整个窗口Disable
	this->EnableWindow(FALSE);
	int Count = 0;

	// 找到所有盘符
	CHAR  VolumeName[] = "A:\\";
	CString FileSystem;

	while (VolumeName[0] <= 'Z')
	{
		// 判断当前盘符是不是NTFS盘
		if (GetVolumeFileSystem(VolumeName, FileSystem) == FALSE)
		{
			VolumeName[0]++;
			continue;
		}
		if (_stricmp(FileSystem.GetBuffer(0), "NTFS") == 0)	// NTFS系统 -
		{
			// 构造 CVolume 类，压入容器
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
		m_StatusBar_File.SetWindowText("正在初始化 " + CString(i->m_VolumeValue) + ":\\");
		// 每个盘符类初始化自己的MAP
		i->InitVolumeUSN();
		m_ProgressCtrl_File->SetPos((Count + 1) * 100);
		// 各自启动守候线程 
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
	
	m_StatusBar_File.SetWindowText("初始化完成");
	m_ProgressCtrl_File->SetPos(0);
	return true;
}
//注意：打开D盘后只有D的监视线程可以运行
DWORD MonitorProcedure(LPVOID ParameterData)
{
	CHAR EventName[] = "MyEventForC";
	
	const DWORD MonitorUSNReason = USN_REASON_FILE_CREATE | USN_REASON_FILE_DELETE |
		USN_REASON_RENAME_OLD_NAME | USN_REASON_RENAME_NEW_NAME;

	_CVolume* This = (_CVolume*)ParameterData;  //Char Map
	BOOL IsOk = FALSE;
	DWORD ReturnLength = 0;

	// 打开磁盘
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

	// 得到USN_DATA 得到上一次遍历的最后结果
	USN_JOURNAL_DATA_V0  USNJournalData = { 0 };
	IsOk = DeviceIoControl(DiskVolumeHandle,
		FSCTL_QUERY_USN_JOURNAL, nullptr, 0, &USNJournalData, sizeof(USN_JOURNAL_DATA_V0), &ReturnLength,
		NULL);
	if (!IsOk)
	{
		return -1;
	}
	// 构造监视结构体 - 关键是监视类型 
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
		// 阻塞函数 - 监视到新的USN才会返回
		IsOk = DeviceIoControl(DiskVolumeHandle, FSCTL_READ_USN_JOURNAL, &ReadUSNJournalData, sizeof(READ_USN_JOURNAL_DATA_V0),
			BufferData, USN_PAGE_SIZE, &ReturnLength, NULL);
		if (!IsOk)
		{
			return -1;
		}
		// 返回数据结构: USN + n个USN_RECORD
		if (ReturnLength < sizeof(USN))
			continue;

		USNRecordLength = ReturnLength - sizeof(USN);
		USNRecordData = (PUSN_RECORD)(BufferData + sizeof(USN));

		This->m_CriticalSection.Lock();
		while (USNRecordLength > 0)
		{
			//修改数据结构
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
		FileSystem = v1;   //动态内存
	}
	return IsOk;
}



//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFileManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFileManagerDlg::OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	if (m_CListCtrl_File.GetSelectedCount() == 0 ||
		m_CListCtrl_File.GetItemData(m_CListCtrl_File.GetSelectionMark()) != 1)
		return;

	FixedFileInfo();
	*pResult = 0;
}


void CFileManagerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

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


	//初始化列表信息
	//m_CurrentPathReference = 0;	// 显示磁盘 - m_CurrenPathReference 就是 0
	// 记录
	//m_PathHistroy.emplace_back(m_CurrentPathReference);

	//CurrentPathIt = --m_PathHistroy.end();		// 指向最后一个插入的元素

	m_CListCtrl_File.InsertColumn(0, "名称", LVCFMT_CENTER, 40);
	m_CListCtrl_File.InsertColumn(1, "类型", LVCFMT_CENTER, 80);
	m_CListCtrl_File.InsertColumn(2, "文件系统", LVCFMT_CENTER, 80);
	m_CListCtrl_File.InsertColumn(3, "总大小", LVCFMT_CENTER, 80);
	m_CListCtrl_File.InsertColumn(4, "可用空间", LVCFMT_CENTER, 80);
	m_CListCtrl_File.InsertColumn(5, "Reference", LVCFMT_CENTER, 0);	// 隐藏列放在最前面 - 记录Reference 方便后面的遍历

	m_CListCtrl_File.SetExtendedStyle(m_CListCtrl_File.GetExtendedStyle()
		| LVS_EX_FULLROWSELECT);

	CHAR	FileSystem[MAX_PATH] = { 0 };     //ntfs 
	unsigned __int64	HardDiskAmountBytes = 0;
	unsigned __int64	HardDiskFreeSpaceBytes = 0;
	unsigned long		HardDiskAmountMB = 0;      // 总大小
	unsigned long		HardDiskFreeSpaceMB = 0;   // 剩余空间
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
			VolumeValue, GetServerIconInfo(VolumeValue, NULL, GetFileAttributes(VolumeValue)));    //获得系统的图标		

		m_CListCtrl_File.SetItemData(Item, 1);  //隐藏数据 1=目录 0=文件

		SHFILEINFO	sfi;
		SHGetFileInfo(VolumeValue, FILE_ATTRIBUTE_NORMAL,
			&sfi, sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
		m_CListCtrl_File.SetItemText(Item, 1, sfi.szTypeName); //本地磁盘 移动磁盘
		m_CListCtrl_File.SetItemText(Item, 2, FileSystem);     //文件系统
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

	// 3 是系统文件夹图标索引 - 在这里进行判断，使用自己的文件夹图标
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
	vector<FILE_NODE> FindedFiles;   //当前目录下的所有信息  
									 //                                            0x5000000000005
	CString Reference ;   //Map数据结构的索引  【USN_NODE】【USN_NODE】【USN_NODE】【USN_NODE】【】【】【】

	if (FileDirectory.GetLength() == 0)
	{
		int	Item = m_CListCtrl_File.GetSelectionMark();

		//选择有项目
		if (Item != -1)
		{
			//该项为目录
			if (m_CListCtrl_File.GetItemData(Item) == 1)
			{

				FileDirectory = m_CListCtrl_File.GetItemText(Item, 0);
				CHeaderCtrl* v1 = m_CListCtrl_File.GetHeaderCtrl();  //获得该排

																			//该排中有多少个显示的信息(有几列)
				int Count = v1->GetItemCount();
				//显示过程必须默认先点开根目录，（里面有重要的赋值操作）
				if (Count == 6)  //根目录  C:\  D:\  
				{
					// 根目录打开
					Reference.Format("%lld", 0x5000000000005);
					// 设置当前所在的盘符类
					for (vector<_CVolume>::iterator i = m_Volumes.begin(); i != m_Volumes.end(); i++)
					{
						if (i->m_VolumeValue == FileDirectory[0])
						{
							//i实际上是_CVolume的指针
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
				else if (Count == 5)  //除了根目录
				{
					//打开一般文件夹
					Reference = m_CListCtrl_File.GetItemText(Item, 4);
					m_CurrentPathReferenceFileServer = _atoi64(Reference);
				}
			}
		}
		// 从组合框里得到路径
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

		// 插入记录 vector
		//m_PathHistroy.emplace_back(m_CurrentPathReference);
		//CurrentPathIt = --m_PathHistroy.end();
	}
	else
	{

	}


	// 回到电脑根目录
	if (m_ParentDirectoryFileServer.GetLength() == 0 || m_CurrentPathReferenceFileServer == 0)
	{
		FixedVolumeInfo();  //刷磁盘信息
		return;
	}

	// 插入Combox
	/*m_CCombo_File_Server.InsertString(0, m_File_Server_Path);
	m_CCombo_File_Server.SetCurSel(0);*/
	//CListCtrl初始化
	m_CListCtrl_File.DeleteAllItems();
	while (m_CListCtrl_File.DeleteColumn(0) != 0);  //删除
	m_CListCtrl_File.InsertColumn(0, "名称", LVCFMT_LEFT, 150);
	m_CListCtrl_File.InsertColumn(1, "大小", LVCFMT_LEFT, 100);
	m_CListCtrl_File.InsertColumn(2, "类型", LVCFMT_LEFT, 100);
	m_CListCtrl_File.InsertColumn(3, "修改日期", LVCFMT_LEFT, 115);
	m_CListCtrl_File.InsertColumn(4, "Reference", LVCFMT_LEFT, 0);

	int	FileCount = 0;  //排
	int ItemCount = 0;  //列

						// .. 返回上层目录
	ItemCount = m_CListCtrl_File.InsertItem(FileCount++, "..",
		GetServerIconInfo(NULL, NULL, FILE_ATTRIBUTE_DIRECTORY));

	m_CListCtrl_File.SetItemText(ItemCount, 4, Reference);

	m_CListCtrl_File.SetItemData(ItemCount, 1);      //不显示的信息      


	int i = 0;

	HANDLE FileHandle = INVALID_HANDLE_VALUE;

	// 遍历map
	m_Volume->FindFile(_atoi64(Reference), FindedFiles);  //在Map数据结构中查询信息
	for (vector<FILE_NODE>::iterator i = FindedFiles.begin(); i != FindedFiles.end(); ++i)
	{
		ItemCount = m_CListCtrl_File.InsertItem(FileCount,
			i->FileNameData,
			GetServerIconInfo(i->FileNameData,
				m_ParentDirectoryFileServer + i->FileNameData,
				GetFileAttributes(m_ParentDirectoryFileServer + i->FileNameData)));	//  插入CtrlList
		DWORD FileAttributes = 0;

		if (PathIsDirectoryA(m_ParentDirectoryFileServer + i->FileNameData))
		{
			FileHandle = CreateFileA(m_ParentDirectoryFileServer + i->FileNameData,
				GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
			if (FileHandle == INVALID_HANDLE_VALUE)
			{
				continue;
			}
			m_CListCtrl_File.SetItemText(ItemCount, 1, 0);   //文件大小
			m_CListCtrl_File.SetItemText(ItemCount, 2, "文件夹");


			FILETIME LastWriteTime = { 0 };
			GetFileTime(FileHandle, NULL, NULL, &LastWriteTime);
			m_CListCtrl_File.SetItemText(ItemCount, 3, CTime(LastWriteTime).Format("%Y-%m-%d %H:%M"));

			m_CListCtrl_File.SetItemData(ItemCount, 1);      //不显示隐式数据 

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
