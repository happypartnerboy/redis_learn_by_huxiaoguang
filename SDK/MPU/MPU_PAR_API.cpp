
/*!
 *\file		MPU_PAR_API.cpp
 *\brief	MPUģ�飬����������ؽӿ�ʵ��
 *\author	����
 *\date		2009/03/20

���ܣ�
-#	�������
-#	�ָ�Ĭ�ϲ���
-#	���뵼������

˵����
-#	��ģ����������ɸ���ģ�鸺��
-#	���뵼��ʱ�������ϲ�GUIͨ��BKDģ��ӿڻ�ȡU�̹��ص�󣬼���Ŀ¼�����������ļ�ȫ·����
 */


#include "MPU_global.h"
#include "mysystem.h"

/**************************************************************************
 *                                                       ����ֲ���                                                            *
 **************************************************************************/

//�������ù�����ִ�ж���
#define MPU_PAR_CALL(x)															\
do																				\
{																				\
	int nRet;																		\
																				\
	CConfig *pConfig = CConfig::GetInstance();											\
																				\
	if (NULL != pConfig)															\
	{																			\
		nRet = pConfig->x;															\
		if (IDVR_OK != nRet)														\
		{																		\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);									\
		}																		\
	}																			\
	else																			\
	{																			\
		MPU_ERR("Get CConfig instance fail!\n");										\
		MPU_CHK(NULL != pConfig);													\
		nRet = IDVR_MPU_NO_CCONFIG_INSTANCE;										\
	}																			\
																				\
	return nRet;																	\
}																				\
while (0)

/**************************************************************************
 *                     ģ���ڲ�ȫ��                                      *
 **************************************************************************/
//fan-201409-25
static CConfig *pDevTypeConfig = NULL;	
//////////////

//������Ϣ�����ļ�
static CConfig *pCustomizedInfoConfig = NULL;

//Fixed �����ļ�
static CConfig *pFixedConfig = NULL;

/**************************************************************************
 *                     ģ���ڲ��ӿ�                                       *
 **************************************************************************/
//!\brief   ��ʼ������������
//!\return  0��ʾ�ɹ�������Ϊ������
int MPU_PAR_Init()
{
	int nRet;
	BOOL bNeedSave = FALSE;

	//���뵱ǰ�����ļ�
	nRet = MPU_PAR_LoadConfig(IDVR_CURRENT_CONFIG_FILE_PATH);

	//fan-2014-09-25
	//�����豸���������ļ�
	if (NULL == pDevTypeConfig)
	{
		pDevTypeConfig = CConfig::NewInstance();
		if (NULL == pDevTypeConfig)
		{
			nRet = IDVR_ERROR_NEW_MEMERROR;
		}
		else
		{
			nRet = pDevTypeConfig->LoadFile(IDVR_DEVICE_TYPE_FILE_PATH);
		}
	}
	/////////////////
	
	//����Э��֧�������ļ�
	if (NULL == pCustomizedInfoConfig)
	{
		pCustomizedInfoConfig = CConfig::NewInstance();
		if (NULL == pCustomizedInfoConfig)
		{
			nRet = IDVR_ERROR_NEW_MEMERROR;
		}
		else
		{
			nRet = pCustomizedInfoConfig->LoadFile(IDVR_CUSTOMIZEDINFO_CONFIG_FILE_PATH);
		}
	}
	
	//���� fixed �����ļ�
	if (NULL == pFixedConfig)
	{
		pFixedConfig = CConfig::NewInstance();
		if (NULL == pFixedConfig)
		{
			nRet = IDVR_ERROR_NEW_MEMERROR;
		}
		else
		{
			nRet = pFixedConfig->LoadFile(IDVR_FIXED_CONFIG_FILE_PATH);
		}
	}

	return nRet;
}

//!\brief   ���ʼ������������
//!\return  0��ʾ�ɹ�������Ϊ������
int PAR_Uninit()
{
	//��������
	MPU_PAR_SaveConfigReal();		//������ͬ������

	//�ͷſռ�
	MPU_PAR_CALL(UnloadFile());

	//fan-2014-09-25
	if (NULL != pDevTypeConfig)
	{
		pDevTypeConfig->ReleaseInstance(pDevTypeConfig);
		pDevTypeConfig = NULL;
	}
	
	if (NULL != pCustomizedInfoConfig)
	{
		pCustomizedInfoConfig->ReleaseInstance(pCustomizedInfoConfig);
		pCustomizedInfoConfig = NULL;
	}

	if (NULL != pFixedConfig)
	{
		pFixedConfig->ReleaseInstance(pFixedConfig);
		pFixedConfig = NULL;
	}
	
	return 0;
}


/**************************************************************************
 *                     ��������                                           *
 **************************************************************************/

//!\brief       ���浱ǰ���ò������ļ����첽��ʽ
//!\return  0��ʾ�ɹ�������Ϊ������
int MPU_PAR_SaveConfig()
{
	//�������ù�����ִ�ж���
	MPU_PAR_CALL(SaveFile());
}
// ����ͬMPU_PAR_SaveConfig ��ͬ����ʱ����
int MPU_PAR_SaveConfigReal()
{
	MPU_PAR_CALL(SaveFileReal());
}
//!\brief       �ָ�Ĭ�����ã�������ֻ���������ļ���������������µĻָ��������ָ�Ĭ��ǰ�ᱸ����ǰ���ص������ļ�,�����ļ���Ϊ�ļ���+-bak
//!\param   pszDefFileName      Ĭ�������ļ����ƣ���·����
//!\return  0��ʾ�ɹ�������Ϊ������ -3 ����У�����
int MPU_PAR_ResumeDefaultConfig(const char *pszDefFileName, const char *pszFlagFile)
{
	//�������ù�����ִ�ж���
	MPU_PAR_CALL(ResumeDefCfg(pszDefFileName,pszFlagFile));
}

//!\brief   ���������ļ� ���������ļ�ǰ�ᱸ����ǰ���ص������ļ��������ļ���Ϊ�ļ���+-bak
//!\param   pszNewCfgFileName �����������ļ�����
//!\param   pszFlagFile ��Ǹ��µ�����ļ�,ΪNULL���������ļ��е�ֵ��ӵ�ԭ�����ļ���
//!\return  0��ʾ�ɹ�,����Ϊ������
int MPU_PAR_UpdateConfig(const char *pszNewCfgFileName, const char *pszFlagFileName)
{
	MPU_PAR_CALL(UpdateCfg(pszNewCfgFileName, pszFlagFileName));
}

//!\brief   ���������ļ�
//!\param   szFullFileName  �����ļ�������·����
//!\return  0��ʾ�ɹ�������Ϊ������
int MPU_PAR_LoadConfig(const char *pszFullFileName)
{
	//�������ù�����ִ�ж���
	MPU_PAR_CALL(LoadFile(pszFullFileName));
}
//!\brief   ��ȡ���ò���
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   nDefault        ȱʡֵ
//!\return  �������β���ֵ
int MPU_PAR_GetValue(const char *pszSection, const char *pszKey, int nDefault)
{
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		return pConfig->GetValue(pszSection, pszKey, nDefault);
	}
	else
	{
		MPU_ERR("Get CConfig instance fail!\n");
		MPU_CHK(NULL != pConfig);
		return nDefault;
	}
}

//!\brief   �������ò���
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   nValue          ���β���ֵ
//!\return  0��ʾ�ɹ�������Ϊ������
int MPU_PAR_SetValue(const char *pszSection, const char *pszKey, int nValue)
{
	//�������ù�����ִ�ж���
	MPU_PAR_CALL(SetValue(pszSection, pszKey, nValue));
}

//!\brief   ��ȡ���ò���
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pszDefault      ȱʡֵ
//!\param   pValue          �����ַ����Ͳ���ֵ
//!\param   nSize           buf�ռ��С
//!\return  ���ӿ��ַ����Ͳ���ֵ��pValueָ���buf��������pValue
char *MPU_PAR_GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize)
{
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		return pConfig->GetValue(pszSection, pszKey, pszDefault, pValue, nSize);
	}
	else
	{
		MPU_ERR("Get CConfig instance fail!\n");
		MPU_CHK(NULL != pConfig);

		if (NULL == pValue)
		{
			MPU_ERR("NULL == pValue\n");
			MPU_CHK(NULL != pValue);
			return NULL;
		}
		else if (0 >= nSize)
		{
			MPU_ERR("0 >= nSize\n");
			MPU_CHK(nSize > 0);
			return NULL;
		}
		else if (NULL == pszDefault)
		{
			pValue[0] = 0;
		}
		else
		{
			strncpy(pValue, pszDefault, nSize - 1);
			pValue[nSize - 1] = 0;
		}

		return pValue;
	}
}

//!\brief   �������ò���
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pValue          �ַ����Ͳ���ֵ
//!\return  0��ʾ�ɹ�������Ϊ������
int MPU_PAR_SetValue(const char *pszSection, const char *pszKey, const char *pValue)
{
	//�������ù�����ִ�ж���
	MPU_PAR_CALL(SetValue(pszSection, pszKey, pValue));
}

//fan-2014-09-25
//!\brief   ��ȡ�豸�ͺ���
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pszDefault      ȱʡֵ
//!\param   pValue          �����ַ����Ͳ���ֵ
//!\param   nSize           buf�ռ��С
//!\return  ���ӿ��ַ����Ͳ���ֵ��pValueָ���buf��������pValue
char *MPU_PAR_DevType_GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize)
{
	if (NULL != pDevTypeConfig)
	{
		return pDevTypeConfig->GetValue(pszSection, pszKey, pszDefault, pValue, nSize);
	}
	else
	{
		MPU_ERR("DevType CConfig instance is NULL!\n");
		MPU_CHK(NULL != pDevTypeConfig);

		if (NULL == pValue)
		{
			MPU_ERR("NULL == pValue\n");
			MPU_CHK(NULL != pValue);
			return NULL;
		}
		else if (0 >= nSize)
		{
			MPU_ERR("0 >= nSize\n");
			MPU_CHK(nSize > 0);
			return NULL;
		}
		else if (NULL == pszDefault)
		{
			pValue[0] = 0;
		}
		else
		{
			strncpy(pValue, pszDefault, nSize - 1);
			pValue[nSize - 1] = 0;
		}

		return pValue;
	}
}

//!\brief   ��ȡ�汾��ʶ��Э�����
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pszDefault      ȱʡֵ
//!\return  ��ȡֵ
int MPU_PAR_CustomizedInfo_GetValue(const char *pszSection, const char *pszKey, int nDefault)
{
	if (NULL != pCustomizedInfoConfig)
	{
		return pCustomizedInfoConfig->GetValue(pszSection, pszKey, nDefault);
	}
	else
	{
		if (NULL == pCustomizedInfoConfig)
		{
			MPU_ERR("ProtocolAlias CConfig instance is NULL!\n");
			MPU_CHK(NULL != pCustomizedInfoConfig);
		}
	}

	return 0;
}

//BEN-2016-09-28
//!\brief   ��ȡ�汾��ʶ��Э�����
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pszDefault      ȱʡֵ
//!\param   pValue          �����ַ����Ͳ���ֵ
//!\param   nSize           buf�ռ��С
//!\return  ���ӿ��ַ����Ͳ���ֵ��pValueָ���buf��������pValue
int MPU_PAR_CustomizedInfo_GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize)
{
	if (NULL != pCustomizedInfoConfig)
	{
		pCustomizedInfoConfig->GetValue(pszSection, pszKey, pszDefault, pValue, nSize);
	}
	else
	{
		if (NULL == pCustomizedInfoConfig)
		{
			MPU_ERR("ProtocolAlias CConfig instance is NULL!\n");
			MPU_CHK(NULL != pCustomizedInfoConfig);
		}

		if (NULL == pValue)
		{
			MPU_ERR("NULL == pValue\n");
			MPU_CHK(NULL != pValue);
			return NULL;
		}
		else if (0 >= nSize)
		{
			MPU_ERR("0 >= nSize\n");
			MPU_CHK(nSize > 0);
			return NULL;
		}
		else if (NULL == pszDefault)
		{
			pValue[0] = 0;
		}
		else
		{
			strncpy(pValue, pszDefault, nSize - 1);
			pValue[nSize - 1] = 0;
		}
	}

	return 0;
}

//!\brief   ��ȡϵͳ����
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pszDefault      ȱʡֵ
//!\return  ��ȡֵ
int MPU_PAR_Fixed_GetValue(const char *pszSection, const char *pszKey, int nDefault)
{
	if (NULL != pFixedConfig)
	{
		return pFixedConfig->GetValue(pszSection, pszKey, nDefault);
	}
	else
	{
		if (NULL == pFixedConfig)
		{
			MPU_ERR("ProtocolAlias CConfig instance is NULL!\n");
			MPU_CHK(NULL != pFixedConfig);
		}
	}

	return 0;
}

//!\brief   ����ϵͳ����
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pValue          �ַ����Ͳ���ֵ
//!\return  0��ʾ�ɹ�������Ϊ������
int MPU_PAR_Fixed_SetValue(const char *pszSection, const char *pszKey, int nValue)
{
	//�������ù�����ִ�ж���
	if (0 == pFixedConfig->SetValue(pszSection, pszKey, nValue))
	{
		pFixedConfig->SaveFile();//�ɹ�����    ����
	}

	return 0;
}

/**************************************************************************
 *                                                        ע���ֲ���                                                           *
 **************************************************************************/
#undef MPU_PAR_CALL				//�������ù�����ִ�ж���
