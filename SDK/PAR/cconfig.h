#ifndef _CONFIG_H
#define _CONFIG_H
#include<pthread.h>
#include<unistd.h>
typedef void *HANDLE;
class CConfig
{
public:
	int LoadIniFile(const char *pszFileName);
	CConfig();
	struct tagSection;

	//!\brief   获取配置参数
	//!\param   pszSection      区段名
	//!\param   pszKey          关键字名
	//!\param   nDefault        缺省值
	//!\return  返回整形参数值
	int GetValue(const char *pszSection, const char *pszKey, int nDefault);

	//直接进行移植
	//获取字符串
	int GetValue(const char *pszSection, const char *pszKey, char* pStr, int strLen);

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

	//!\brief 添加新的section,内容为空
	tagSection *AddSection();

	//!\brief   获取配置参数
	//!\param   pszSection      区段名
	//!\param   pszKey          关键字名
	//!\param   pszDefault      缺省值
	//!\param   pValue          返回字符串型参数值
	//!\param   nSize           buf空间大小
	//!\return  
	//本接口字符串型参数值到pValue指向的buf，并返回pValue
	char *GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize);


	
	//!\brief   获取配置参数
	//!\param   pszSection      区段名
	//!\param   pszKey          关键字名
	//!\param   pszDefault      缺省值
	//!\param   pValue          返回字符串型参数值
	//!\param   nSize           buf空间大小
	//!\return  
	//本接口字符串型参数值到pValue指向的buf，并返回pValue
	char *InternalGetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize);

	//!\brief 查找一个section
	//!\param pszSection section名
	//!\return NULL表示查找失败 其他表示section指针
	tagSection *SearchSection(const char *pszSection);

	tagSection *InsertSection();
	static int Lock();

	static int Unlock();

private:
	HANDLE hCfg;
	int m_nCurrentLine;				//当前行号
	char * m_strFile  ;				//文件名
	tagSection *m_pSection;			//section 
	int m_nSectionCount;			//section 大小
	int m_nSectionAlloced;			//已经分配的内存空间
	int m_iCfgDataChanged;		//保存到文件中
	static pthread_mutex_t m_mtxConfig; 	//互斥锁
	
};
#endif