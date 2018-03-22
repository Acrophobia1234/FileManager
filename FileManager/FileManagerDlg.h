
// FileManagerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include<iostream>
#include<Windows.h>
#include <string>
#include <vector>
#include <WinSock2.h>
#include <Mstcpip.h>
#include "_CVolume.h"
using namespace std;

// CFileManagerDlg �Ի���
class CFileManagerDlg : public CDialogEx
{
// ����
public:
	CFileManagerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILEMANAGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	//void OnReceiveComplete(void);
	BOOL CFileManagerDlg::InitServerUSN();
	BOOL CFileManagerDlg::GetVolumeFileSystem(CHAR * VolumeName, CString& FileSystem);
	static UINT ThreadProcedure(LPVOID ParameterData);
	UINT CFileManagerDlg::RealThreadProcedure(LPVOID ParameterData);
	VOID CFileManagerDlg::FixedVolumeInfo(); 
	VOID CFileManagerDlg::FixedFileInfo(CString FileDirectory = "");
	int	CFileManagerDlg::GetServerIconInfo(LPCTSTR VolumeValue, LPCTSTR FileFullPathData,
		DWORD FileAttributes);
	BOOL CFileManagerDlg::GetParentDirectory(CString& DestinationString,
		CString& SourceString);
public:
	CListCtrl		m_CListCtrl_File;
	CStatusBar		m_StatusBar_File; // ����������״̬��
	CProgressCtrl*	m_ProgressCtrl_File;

	CImageList*		m_CImageList_Large;
	CImageList*		m_CImageList_Small;

	vector<_CVolume> m_Volumes;
	vector<_CVolume>::iterator m_Volume;	// ������

private:
	HANDLE m_MonitorThreadHandle[26];
	CString m_ParentDirectoryFileServer;
	ULONG64 m_CurrentPathReferenceFileServer;			    // ��¼��ǰĿ¼������
	CString m_ParentDirectoryFileClient;
public:
	afx_msg void OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	afx_msg LRESULT OnFixFileInfo(WPARAM wParam, LPARAM  lParam);
};

static DWORD MonitorProcedure(LPVOID ParameterData);