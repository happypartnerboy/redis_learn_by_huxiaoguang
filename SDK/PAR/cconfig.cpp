#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cconfig.h"
#include <pthread.h>

#define PAR_DBG(x...)
#define PAR_ERROR(x...)
//!\brief 第一次分配section的个数
#define DEF_SEC_NUM (15)
//!\brief 递增的section的个数
#define ADD_SEC_NUM (5)
//!\brief 第一次分配的key的个数
#define DEF_KEY_NUM (5)
//!\brief 递增的key的个数
#define ADD_KEY_NUM (5)
//!\brief ini文件每行的字符数
#define INI_LINE_LENGTH (320)

//!\brief ini section 名称最大长度
#define INI_MAX_SECTION_NAME_LENGTH (48)

//!\brief ini key 名称最大长度
#define INI_MAX_KEY_NAME_LENGTH (48)

//!\brief ini key 值最大长度
#define INI_MAX_KEY_VALUE_LENGTH (255)


//#error
#define IDVR_ERROR_ALLOC (-1)
#define IDVR_PAR_ERROR_INVALID_FORMAT (-1) 



pthread_mutex_t CConfig::m_mtxConfig = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
pthread_t CConfig::m_SaveFileThread = (pthread_t)NULL;
pthread_mutex_t CConfig::m_mtxSaveFile =  PTHREAD_MUTEX_INITIALIZER;

struct CConfig::tagSection
{
  public:
	struct tagKey
	{
	  public:
        char *m_strName;
        char *m_strValue;
        tagKey():m_strName(NULL), m_strValue(NULL)
		{
		};

		 ~tagKey()
		{
			//释放字符串所占内存
			if (m_strName != NULL)
			{
				free(m_strName);
				m_strName = NULL;
			}

			if (m_strValue != NULL)
			{
				free(m_strValue);
				m_strValue = NULL;
			}
		}

		void Copy(tagKey &newKey)
		{
			newKey.~tagKey();
			if (m_strName != NULL)
			{
				newKey.m_strName = strdup(m_strName);
			}
			if (m_strValue != NULL)
			{
				newKey.m_strValue = strdup(m_strValue);
			}
		}
	};

	tagSection():m_strName(NULL), m_pKeyHead(NULL), m_nKeyCount(0), m_nKeyAlloced(0)
	{
	};

	tagKey *SearchKey(const char *pszKey)
	{
		int i = 0;
		tagKey *pKey = NULL;

		for (i = 0; i < m_nKeyCount; i++)
		{
			//找到key
			if (0 == strcmp(m_pKeyHead[i].m_strName, pszKey))
			{
				pKey = m_pKeyHead + i;
				break;
			}
		}
		return pKey;
	};

	tagKey *AddKey()
	{
		int max_key_nr = DEF_KEY_NUM;

		if (m_nKeyCount != 0)
		{
			if ((m_nKeyCount+1) > m_nKeyAlloced)
			{
				max_key_nr = m_nKeyAlloced + ADD_KEY_NUM;

				//需要重新分配内存
				m_pKeyHead = (tagSection::tagKey *) realloc(m_pKeyHead, sizeof (tagKey) * max_key_nr);
				m_nKeyAlloced = max_key_nr;
			}
		}
		else
		{
			//分配初始内存
			m_pKeyHead = (tagSection::tagKey *) malloc(sizeof (tagKey) * max_key_nr);
			m_nKeyAlloced = max_key_nr;
		}

		//内存分配失败
		if (NULL == m_pKeyHead)
		{
			return NULL;
		}

		//获取新添加的key指针
		tagKey *pKey = m_pKeyHead + m_nKeyCount;

		//初始化
		pKey->m_strName = NULL;
		pKey->m_strValue = NULL;

		//增加key数目
		m_nKeyCount++;

		return pKey;
	}

	void Copy(tagSection &newSection)
	{
		newSection.~tagSection();
		if (m_nKeyCount && m_pKeyHead)
		{
			newSection.m_pKeyHead = (tagKey *)malloc(m_nKeyAlloced * sizeof(tagKey));
			memset(newSection.m_pKeyHead, 0, m_nKeyAlloced * sizeof(tagKey));
			for (int i = 0; i < m_nKeyCount; i++)
			{
				m_pKeyHead[i].Copy(newSection.m_pKeyHead[i]);
			}
			newSection.m_nKeyCount = m_nKeyCount;
			newSection.m_nKeyAlloced = m_nKeyAlloced;
		}
		if (m_strName != NULL)
		{
			newSection.m_strName = strdup(m_strName);
		}
	}

	~tagSection()
	{
		//删除所有key,释放内存
		if (m_pKeyHead != NULL)
		{
			int i = 0;
			tagKey *pKey = m_pKeyHead;

			for (i = 0; i < m_nKeyCount; i++)
			{
				//析构
				pKey->~tagKey();
				pKey++;
			}
			//释放数组所占内存
			free(m_pKeyHead);
			m_nKeyCount = 0;
			m_nKeyAlloced = 0;
			m_pKeyHead = NULL;
		}
		if (m_strName)
		{
			free(m_strName);
			m_strName = NULL;
		}
	};

	char *m_strName;                //段名称
	tagKey *m_pKeyHead;             //节点          
	int m_nKeyCount;                //大小
	int m_nKeyAlloced;              //已经分配的大小
};

int CConfig::LoadIniFile(const char *pszFileName)
{
    //加载数据
    FILE *fp;
    int ret = -1;
	if (NULL == pszFileName)
	{
		//写debug日志,错误的参数
		PAR_ERROR("[%s]ERR:Bad Parameters", __FUNCTION__);
		return -1;
	}
	m_nCurrentLine = 0;
	fp = fopen(pszFileName, "r+");
	if (NULL == fp)
	{
		fp = fopen(pszFileName, "w+");
		if (NULL == fp)
		{
			fp = fopen(pszFileName, "r");
			if (NULL == fp)
			{
				//写系统日志,配置文件不存在并且创建失败
				PAR_ERROR("[%s]ERR:No Cfg File %s", __FUNCTION__, pszFileName);
				return -1;
			}
		}
	}
	//创建文件
    m_nSectionCount = 0;
	m_pSection = NULL;
	tagSection tSect;
    
    //Need to do  读取一段
    while (0 == (ret = ReadIniSection(fp, &tSect)))
    {
		//读取数据
        if (tSect.m_strName != NULL)
		{
			//添加新的section，或者指向所需要的位置
			tagSection *pCurSection = AddSection();
			//内存分配失败
			if (pCurSection == NULL)
			{
				if (m_pSection != NULL)
				{
					free(m_pSection);
					m_pSection = NULL;
				}
				PAR_ERROR("Add Section Failed, Can not Allocate Memory\n");
				fclose(fp);
				return IDVR_ERROR_ALLOC;
			}
			m_nSectionCount++;
			pCurSection->m_strName = tSect.m_strName;
			pCurSection->m_pKeyHead = tSect.m_pKeyHead;
			pCurSection->m_nKeyCount = tSect.m_nKeyCount;
			pCurSection->m_nKeyAlloced = tSect.m_nKeyAlloced;
			tSect.m_strName = NULL;
			tSect.m_pKeyHead = NULL;
			tSect.m_nKeyCount = 0;
			tSect.m_nKeyAlloced = 0;


		}
		if (feof(fp))	//文件达到末尾
		{
			break;
		}
    }

	if (m_strFile != NULL)
	{
		free(m_strFile);
		m_strFile = NULL;
	}

	if ((0 == ret))
	{
		m_strFile = strdup(pszFileName);
	}
	
	fclose(fp);
	#if 0
	printf("***************start*************\n");
	printf("m_strFile = %s\n",m_strFile);
	for (int i = 0; i < m_nSectionCount; i++)
	{
		tagSection *p = m_pSection + i;
		printf("************config start p->m_strName=%s******\n",p->m_strName);
		for(int j = 0; j < p->m_nKeyCount;j++)
		{
			CConfig::tagSection::tagKey*temp =  p->m_pKeyHead + j;
			
			printf("temp->m_strName = %s temp->m_strValue = %s\n",temp->m_strName,temp->m_strValue);
		}
		printf("************config start p->m_strName=%s******\n",p->m_strName);
	}	
	printf("****************end*********\n");
	#endif
    return 0;
}

/*
    读取一段
*/
int CConfig::ReadIniSection(FILE * fp, tagSection * pSect)
{
    if ((NULL == fp) || (NULL == pSect))
	{
		return -1;	//参数错误
	}
	//cout << "Read Section" << endl;
	char szBuff[INI_LINE_LENGTH + 1 + 1]; //换行符和结束符

	while (1)
	{
		int bufflen;

		if (!fgets(szBuff, INI_LINE_LENGTH, fp))
		{
			if (feof(fp))	//光标向后看
			{
				PAR_DBG("End of File\n");
				return 0;		//读取文件结束
			}
			else				//文件IO错误
			{
				PAR_ERROR("ERR: IO Fault\n");	//文件没有结束,但读不到数据
				return -1;
			}
		}
		m_nCurrentLine++;
		bufflen = strlen(szBuff);

		if (NULL == pSect->m_strName)	//读取section
		{
			int ret = GetIniSectionName(&(pSect->m_strName), szBuff);
			if (ret != 0)
			{
				return ret;
			}
			if (NULL == pSect->m_strName)	//空行或者注释行
			{
				continue;
			}
		}
		else					//读取key
		{
			int ret;
			char *key, *value;

			ret = GetIniKey(szBuff, &key, &value);
			if (ret < 0)		//没有key
			{
				continue;
			}
			else if (1 == ret)	//发现新的section
			{
				m_nCurrentLine--;
				fseek(fp, -bufflen, SEEK_CUR);
				break;
			}
			else if (0 != ret)
			{
				return ret;
			}

			char *pKeyName = strdup(key);	//需要重新分配空间

			if (NULL == pKeyName)
			{
				PAR_ERROR("Add Key Failed, Can not Allocate Memory\n");
				return IDVR_ERROR_ALLOC;	//分配内存失败
			}
			char *pKeyValue = strdup(value);

			if (NULL == pKeyValue)
			{
				free(pKeyName);
				pKeyName = NULL;
				PAR_ERROR("Add Key Failed, Can not Allocate Memory\n");
				return IDVR_ERROR_ALLOC;	//分配内存失败
			}
			//添加新的key
			CConfig::tagSection::tagKey * pKey = pSect->AddKey();
			if (NULL == pKey)
			{
				//写系统日志,分配内存失败
				free(pKeyName);
				free(pKeyValue);
				pKeyName = NULL;
				pKeyValue = NULL;
				PAR_ERROR("Add Key Failed, Can not Allocate Memory\n");
				return -1;
			}
			pKey->m_strName = pKeyName; //pKeyName  pKeyValue 都已经分配了内存
			pKey->m_strValue = pKeyValue;
		}
	}
	return 0;
}

//读取名称
int CConfig::GetIniSectionName(char **strSectionName, char *szBuf)
{
    char *current = NULL;
	char *tail = NULL;
	char *name = NULL;
	if (!szBuf) 
		return 0;

	current = szBuf;

	//忽略空白字符
	while (*current == ' ' || *current == '\t')
	{
		current++;
	}

	//忽略注释
	if (*current == ';' || *current == '#')
	{
		return 0;
	}

	//碰到section名称开始符
	if (*current++ == '[')
	{
		while (*current == ' ' || *current == '\t')
		{
			current++;
		}
	}
	//检测是否是空白行
	else if (('\0' == *current) || ('\n' == *current) || ('\r' == *current))
	{
		return 0;
	}
	else
	{
		PAR_ERROR("Invalid File Format, \"[\" Not Found In Section Line, Error Line = %d\n", m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;	//文件格式不对
	}

	//获取section名称
	name = tail = current;
	while (*tail != ']' && *tail != '\n' && *tail != ';' && *tail != '#' && *tail != '\0')
	{
		tail++;
	}
	//section名称没有以]结尾
	if (*tail != ']')
	{
		PAR_ERROR("Invalid File Format, No \"]\" Follow SectionName, Error Line = %d\n", m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}

	*tail = '\0';

	char *pValid = (++tail);
	while (*pValid != '\0' && *pValid != ';' && *pValid != '#')
	{		
		if (*pValid != ' ' && *pValid != '\t' && *pValid != '\n' && *pValid != '\r')
		{
			PAR_ERROR("Invalid File Format, Invalid Character 0x%x Follow \"]\", Error Line = %d\n", *pValid, m_nCurrentLine);
			return IDVR_PAR_ERROR_INVALID_FORMAT;
		}
		pValid++;
	}	
	
	//获取section名成功
	*tail = '\0';
	tail--;
	while (*tail == ' ' || *tail == '\t')
	{
		*tail = '\0';
		tail--;
	}

	//section名称为空
	int len = (tail - name + 1);
	if ((len <= 0) || (len > INI_MAX_SECTION_NAME_LENGTH))
	{
		PAR_ERROR("Invalid File Format, SectionName Length > %d or = 0, Error Line = %d\n", INI_MAX_SECTION_NAME_LENGTH, m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}
	
	*strSectionName = strdup(name);
	if (NULL == *strSectionName)
	{
		return IDVR_ERROR_ALLOC;
	}
    return 0;
}

//读取key 和 val
int CConfig::GetIniKey(char *szBuf, char **pszKeyName, char **pszKeyValue)
{
    char *current = NULL;
	char *tail = NULL;
	char *value = NULL;

	if (!szBuf)
		return -1;

	current = szBuf;

	//忽略空白字符
	while (*current == ' ' || *current == '\t')
	{
		current++;
	}

	//注释
	if (*current == ';' || *current == '#')
	{
		return -1;
	}

	//遇到新的section
	if (*current == '[')
	{
		return 1;
	}

	//空白行
	if (*current == '\n' || *current == '\r' || *current == '\0')
	{
		return -1;
	}

	//key名称为空
	if (*current == '=')
	{
		PAR_ERROR("Invalid File Format, KeyName Is NULL, Error Line = %d\n", m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}

	tail = current;
	while (*tail != '=' && *tail != '\r' && *tail != '\n' && *tail != ';' && *tail != '#' && *tail != '\0')
	{
		tail++;
	}

	if (*tail != '=')
	{
		PAR_ERROR("Invalid File Format, No \"=\" operator Follow KeyName, Error Line = %d\n", m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}
	
	value = tail + 1;

	//去掉key名称后面的空白字符
	*tail-- = '\0';
	while (*tail == ' ' || *tail == '\t')
	{
		*tail = '\0';
		tail--;
	}

	int len = (tail - current + 1);
	if ((len > INI_MAX_KEY_NAME_LENGTH) || (len <= 0))
	{
		PAR_ERROR("Invalid File Format, KeyName Lenth > %d, Error Line = %d\n", INI_MAX_KEY_NAME_LENGTH, m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}

	//获取key值
	tail = value;
	while (*tail != '\r' && *tail != '\n' && *tail != '\0')
	{
		tail++;
	}
	*tail = '\0';

	len = (tail - value);
	if ((len > INI_MAX_KEY_VALUE_LENGTH) || (len < 0))
	{
		PAR_ERROR("Invalid File Format, KeyValue Lenth > %d, Error Line = %d\n", INI_MAX_KEY_VALUE_LENGTH, m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}

	if (pszKeyName)
	{
		*pszKeyName = current;
	}
	if (pszKeyValue)
	{
		*pszKeyValue = value;
	}
    return 0;
}

CConfig::tagSection * CConfig::AddSection()
{
	int max_sect_nr = DEF_SEC_NUM;

	if (m_nSectionCount != 0)
	{
		if ((m_nSectionCount+1) > m_nSectionAlloced)
		{
			max_sect_nr = m_nSectionAlloced + ADD_SEC_NUM;	//需要重新分配内存
			m_pSection = (tagSection *) realloc(m_pSection, sizeof (tagSection) * max_sect_nr);
			m_nSectionAlloced = max_sect_nr;
		}
		else
		{
			// 无需分配内存
		}
	}
	else						//空section
	{
		//一般应为NULL
		if (m_pSection != NULL)
		{
			free(m_pSection);
			m_pSection = NULL;
		}
		//初始分配的DEF_SEC_NUM个section的空间
		m_pSection = (tagSection *) malloc(sizeof (tagSection) * max_sect_nr /*DEF_SEC_NUM */ );
		m_nSectionAlloced = max_sect_nr;
	}
	if (NULL == m_pSection)//分配空间失败
	{
		return NULL;
	}
	tagSection *pSection = m_pSection + m_nSectionCount;

	//设置初始值,避免错误的读写内存
	pSection->m_strName = NULL;
	pSection->m_nKeyCount = 0;
	pSection->m_pKeyHead = NULL;
	//cout << "Section Count " << m_nSectionCount << endl;
	return pSection;
}

int CConfig::GetValue(const char *pszSection, const char *pszKey, int nDefault)
{
//	TODO: Add your specialized code here.
	char *ret;
	char szBuff[51];
	char szDefault[51];
	int nResult = nDefault;

	sprintf(szDefault, "%d", nDefault);

	//获取key的字符串值
	ret = GetValue(pszSection, pszKey, szDefault, szBuff, 50);
	if (ret == NULL)
	{
		return nResult;			//nDefault
	}
	//转换成整型值
	nResult = strtol(szBuff, NULL, 0);
	//检查是否有错误
	//但是如果字符串为"abcd" nResult = 0检查不出错误
	return (int) nResult;

}

int CConfig::SetValue(const char *pszSection, const char *pszKey, int nValue)
{
	// TODO: Add your specialized code here.
	char szBuf[100];

	sprintf(szBuf, "%d", nValue);
	return SetValue(pszSection, pszKey, szBuf);
}

int CConfig::SetValue(const char *pszSection, const char *pszKey, const char *pValue)
{
	int nResult = 0;
	Lock();
	nResult = InternalSetValue(pszSection, pszKey, pValue);
	Unlock();
	return nResult;
}

int CConfig::InternalSetValue(const char *pszSection, const char *pszKey, const char *pValue)
{
	if ((NULL == pValue) || (NULL == pszSection) || (0 == strlen(pszSection)) || (NULL == pszKey) || (0 == strlen(pszKey)))
	{
		return -1;
	}

	CConfig::tagSection::tagKey * pKey = NULL;

	//查找section
	tagSection *pSection = SearchSection(pszSection);

	//没找到section,添加一个section
	if (pSection == NULL)
	{
		pSection = InsertSection();
		if (NULL == pSection)
		{
			return -1;
		}
		pSection->m_strName = strdup(pszSection);
		m_nSectionCount++;
		m_iCfgDataChanged = 1;	//添加 section 已成功
	}

	pKey = pSection->SearchKey(pszKey);
	//没找到key添加一个key
	char *pKeyName = NULL;
	char *pKeyValue = NULL;

	if (NULL == pKey)
	{
		pKeyName = strdup(pszKey);
		if (NULL == pKeyName)
		{
			return -1;	//内存不足
		}

		pKey = pSection->AddKey();
		
		if (NULL == pKey)			//添加key失败
		{
			free(pKeyName);
			pKeyName = NULL;
			return -1;
		}
		pKey->m_strName = pKeyName;
		//成功添加 key & keyName
	}
	pKeyValue = strdup(pValue);
	if (NULL == pKeyValue)
	{
		if (pKeyName != NULL)
		{
			free(pKeyName);
			pKeyName = NULL;
		}
		return -1;	//释放 keyName
	}
	if (pKey->m_strValue != NULL)	//原key有值
	{
		if ( (strlen(pKey->m_strValue) == strlen(pKeyValue)) 
			&& (0 == memcmp(pKey->m_strValue, pKeyValue, strlen(pKeyValue))) ) //内容一致
		{
			free(pKeyValue);
			pKeyValue = NULL;
			return 0;
		}
		//释放key值原占有的内存
		free(pKey->m_strValue);
		pKey->m_strValue = NULL;
	}
	pKey->m_strValue = pKeyValue;
	m_iCfgDataChanged = 1;
	return (int) 0;
}

int CConfig::GetValue(const char *pszSection, const char *pszKey, char* pStr, int strLen)
{
	char *ret;
	char szBuff[51];
	char szDefault[51];
	int nResult = 0;
	int len = strLen > 50 ? 50 : strLen;
	sprintf(szDefault, "%d", nResult);
	//获取key的字符串值
	ret = GetValue(pszSection, pszKey, szDefault, szBuff, 50);
	if (ret == NULL)
	{
		return nResult;			//nDefault
	}
	strncpy(pStr, szBuff, len);
	return nResult;
}

char *CConfig::GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize)
{
	char *pResult = NULL;
	Lock();
	pResult = InternalGetValue(pszSection, pszKey, pszDefault, pValue, nSize);
	Unlock();
	return pResult;
}

CConfig::CConfig():hCfg(NULL), m_strFile(NULL), m_pSection(NULL), m_nSectionCount(0), m_nSectionAlloced(0)
{
	m_iCfgDataChanged = 0;
	//创建线程
	if(!m_SaveFileThread)
	{
		int ret = 0;
		ret = pthread_create(&m_SaveFileThread, NULL, Fnx_SaveFileThread, this);
		if (ret < 0)
		{
			PAR_ERROR("pthread_create<Fnx_SaveFileThread> failed(%d)\n", ret);
		}
	}
}

void *CConfig::Fnx_SaveFileThread(void *lParam)
{
	int ret = 0;
	CConfig *pNTP = (CConfig *) lParam;
	while (1)
	{
		pNTP->CheckIfNeedSaveFile();
		sleep(1);				//一秒检测一次
	}
	return NULL;
}

void CConfig::CheckIfNeedSaveFile()
{
	int ret = 0;
	int ifSaveFile = 0;
	LockSave();
	ifSaveFile = m_nNeedSaveFileFlag; 	// 是否需要保存(save的时候)
	if (!m_iCfgDataChanged)
	{
		ifSaveFile = 0;
	}
	m_nNeedSaveFileFlag = 0; 	// 是否需要保存(save的时候)

	UnlockSave();
	
	if(ifSaveFile)
	{
		ret = SaveFileReal();
		if(ret)
		{
			printf("%s:%d, SaveFileReal fail ret=%d!\n", __FUNCTION__, 
			__LINE__, ret);
		}
	}

}

int CConfig::SaveFile()
{
	// TODO: Add your specialized code here.
	int nResult = -1;
	LockSave();
	m_nNeedSaveFileFlag = 1;	//异步的方式
	UnlockSave();
	nResult = 0;
	return nResult;
}

//存在不安全的现象
int CConfig::SaveFileReal()
{
	// TODO: Add your specialized code here.
	int nResult = -1;
	LockSave();
	nResult = SaveIniFile(m_strFile);
	if (0 == nResult)
	{
		m_iCfgDataChanged = 0;	//保存成功就清零
	}
	UnlockSave();

	return (int) nResult;
}


int CConfig::SaveIniFile(const char *pszFileName)
{
	if ((NULL == pszFileName) || (0 == strlen(pszFileName)))
	{
		//写调试日志,错误的参数
		PAR_ERROR("Invalid Parameter, pszFileName = NULL");
		return -1;
	}	
	int i = 0;
	FILE *fp = NULL;
	int ret = 0;

	fp = fopen(pszFileName, "w+");
	if (NULL == fp)
	{
		//写系统日志,文件打开失败
		PAR_ERROR("Open File %s Failed\n", pszFileName);
		return -1;
	}
	for (i = 0; i < m_nSectionCount; i++)
	{
		tagSection *pSect = m_pSection + i;
		int j;

		ret = fprintf(fp, "\n[%s]\n", pSect->m_strName);
		if (ret < 0)
		{
			fclose(fp);
			unlink(pszFileName);
			return -1;
		}

		for (j = 0; j < pSect->m_nKeyCount; j++)
		{
			ret = fprintf(fp, "%s=%s\n", pSect->m_pKeyHead[j].m_strName, pSect->m_pKeyHead[j].m_strValue);
			if (ret < 0)
			{
				fclose(fp);
				unlink(pszFileName);
				return -1;
			}
		}
	}
	//同步刷新
	fflush(fp);
	fclose(fp);
	return (int) 0;
}


//获取 pValue 已经拷贝了
char *CConfig::InternalGetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize)
{
	if ((NULL == pValue) || (nSize <= 0) || (NULL == pszSection) || (0 == strlen(pszSection)) || (NULL == pszKey) || (0 == strlen(pszKey)))
	{
		return NULL;
	}

	char *pResult = NULL;

	CConfig::tagSection::tagKey * pKey = NULL;

	tagSection *pSection = SearchSection(pszSection);

	//没找到section,添加一个section
	if (pSection == NULL)
	{
		pSection = InsertSection();
		if (NULL == pSection)
		{
			return NULL;
		}
		pSection->m_strName = strdup(pszSection);//拷贝了内存
		m_nSectionCount++;
		m_iCfgDataChanged = 1;
	}
	
	pKey = pSection->SearchKey(pszKey);

	//没找到key添加一个keyss
	char *pKeyName = NULL;
	char *pKeyValue = NULL;

	if (NULL == pKey)		//没有则创建一个
	{
		pKeyName = strdup(pszKey);
		if (NULL == pKeyName)
		{
			return NULL;	//内存不足
		}

		pKey = pSection->AddKey();
		if (NULL == pKey)			//添加key失败
		{
			free(pKeyName);
			pKeyName = NULL;
			return NULL;
		}
		pKey->m_strName = pKeyName;

		if (pszDefault != NULL)
		{
			pKeyValue = strdup(pszDefault);
		}
		else
		{
			pKeyValue = strdup("");
		}
		
		if (NULL == pKeyValue)
		{
			if (pKeyName != NULL)
			{
				free(pKeyName);
				pKeyName = NULL;
			}
			return NULL;
		}

		pKey->m_strValue = pKeyValue;
		m_iCfgDataChanged = 1;		//保存到文件中
	}
		
	if (nSize > 0)
	{
		//PAR_DBG("Get the Key %s\n", pKey->m_strName);
		strncpy(pValue, pKey->m_strValue, nSize - 1);
		pValue[nSize - 1] = '\0';

		pResult = pValue;
	}
	//PAR_DBG("Get pResult:%s\n", pResult);
	return pResult;
}

CConfig::tagSection * CConfig::SearchSection(const char *pszSection)
{
	tagSection *pSect = NULL;
	int i = 0;

	if (NULL == pszSection)
	{
		return NULL;
	}
	for (i = 0; i < m_nSectionCount; i++)
	{
		pSect = m_pSection + i;
		if (0 == strcmp(pSect->m_strName, pszSection))
		{
			break;
		}
	}
	if (i >= m_nSectionCount)
	{
		pSect = NULL;
	}
	return pSect;
}

CConfig::tagSection * CConfig::InsertSection()
{
	int max_sect_nr = DEF_SEC_NUM;

	if (m_nSectionCount != 0)
	{
		//计算是否需要增加内存空间
		if ((m_nSectionCount+1) > m_nSectionAlloced)
		{
			max_sect_nr = m_nSectionAlloced + ADD_SEC_NUM;	//需要重新分配内存
			m_pSection = (tagSection *) realloc(m_pSection, sizeof (tagSection) * max_sect_nr);
			m_nSectionAlloced = max_sect_nr;
		}
		else
		{
			// 无需分配内存
		}
	}
	else//没有起始节点，出错
	{
		return NULL;
	}

	tagSection *pEndSection = m_pSection + m_nSectionCount;
	tagSection *pSection = m_pSection + m_nSectionCount - 1;

	//设置初始值,避免错误的读写内存
	pEndSection->m_strName = pSection->m_strName;		
	pEndSection->m_nKeyCount = pSection->m_nKeyCount;	
	pEndSection->m_pKeyHead = pSection->m_pKeyHead;			
	pEndSection->m_nKeyAlloced = pSection->m_nKeyAlloced;		

	pSection->m_strName = NULL;
	pSection->m_nKeyCount = 0;
	pSection->m_pKeyHead = NULL;
	pSection->m_nKeyAlloced = 0;
	return pSection;
} 

inline int CConfig::LockSave()
{
	return pthread_mutex_lock(&m_mtxSaveFile);
}

inline int CConfig::UnlockSave()
{
	return pthread_mutex_unlock(&m_mtxSaveFile);
}

inline int CConfig::Lock()
{
	return pthread_mutex_lock(&m_mtxConfig);
}

inline int CConfig::Unlock()
{
	return pthread_mutex_unlock(&m_mtxConfig);
}
