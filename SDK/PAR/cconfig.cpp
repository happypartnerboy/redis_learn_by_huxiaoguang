#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cconfig.h"
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
#if 0
    FILE *fp;
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
	/* we allocate 15 sections first */
	//m_pSection = new tagSection[max_sect_nr];
	tagSection tSect;
    
    //Need to do 
#endif
    return 0;
}

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