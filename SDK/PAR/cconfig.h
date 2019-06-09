#ifndef _CONFIG_H
#define _CONFIG_H
class CConfig
{
public:
	int LoadIniFile(const char *pszFileName);
	struct tagSection;
private:
	//!\brief 读取一个section
	//!\param fp 文件指针
	//!\param pSect section指针
	//!\return 0表示读取正确，其他表示错误码
	//!\sa ENCONFIG 错误码
	int ReadIniSection(FILE * fp, tagSection * pSect);

	//!\brief 从配置文件的一行中获取section名称
	//!\param pszSectionName section名称缓冲区指针
	//!\param szBuf 一行字符串
	//!\return 0表示获取成功,其他表示获取失败
	int GetIniSectionName(char **strSectionName, char *szBuf);
	
	//!\brief 从配置文件的一行获取key名称和值
	//!\param szBuf 一行字符串
	//!\param pszKey key名字符串缓冲区指针
	//!\param pszValue key值字符串缓冲区指针
	//!\return 0表示获取成功 其他表示获取失败
	int GetIniKey(char *szBuf, char **pszKeyName, char **pszKeyValue);

private:
	int m_nCurrentLine;

};

#endif