#include <stdio.h>
#include <string.h>

class CMyLogFile 
{
public:
	char *m_strFile;
public:
	CMyLogFile()
	{
		m_strFile = NULL;
	};
	~CMyLogFile(){if(m_strFile) delete m_strFile;};
	void SetFileName(char *strFile)
	{  
		if (m_strFile == NULL)
		{
			m_strFile = new char[strlen(strFile)+1];
			strcpy(m_strFile,strFile);
		}
	};
	void Init()
	{
		if(m_strFile == NULL)
			return;
		FILE *file = NULL;
		file = fopen(m_strFile,"w");
		if (file != NULL)
		{
			fclose(file);
		}
	};
	void WriteLine(char *strline)
	{
		if(m_strFile==NULL)
			return;

		FILE *file = NULL;
		file = fopen(m_strFile,"rt+");
		if (file != NULL)
		{
			fseek(file, 0L, SEEK_END);
			fwrite( strline, sizeof(char),strlen(strline)+1, file ); 
			fclose(file);
		}
	};
};