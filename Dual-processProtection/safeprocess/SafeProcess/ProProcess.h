#pragma once

class CProProcess
{
public:
	static bool StartProtect();
	static bool StopProtect();
	static bool IsProtect();
	static void InitProProcess(CString sProcess = _T(""));//CString sProcess要保护的进程名
	static void CleanProProcess();
	static CString GetProcessName();
protected:
	static void InitLogFile();
	static void SetProfileSetting(bool bProtect);
	static bool GetProfileSetting(); 
private:
	static HANDLE m_hEvent;
	static HANDLE m_hMemMap;
	static LPVOID m_pMemPointer;
	static CWinThread *m_thTimeThread; 
	static bool  m_bIsProtect;
	static CString m_sProcess;
};

class CLogFile
{
public:
	CString m_strFile;
public:
	CLogFile()
	{
		m_strFile.Empty();
	};
	~CLogFile(){};
	void SetFileName(CString strFile)
	{
		m_strFile = strFile;
	};
	void Init()
	{
		if(m_strFile.IsEmpty())
			return;
		CStdioFile file;
		if (file.Open(m_strFile, CFile::modeCreate)) 
		{
			file.Close();
		}
	};
	void WriteLine(CString strline)
	{
		//if(m_strFile.IsEmpty())
			return;

		CStdioFile file;
		if (file.Open(m_strFile, CFile::modeWrite))
		{
			file.SeekToEnd();
			file.WriteString(strline);
			file.Close();
		}
	};
};
