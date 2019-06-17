
/*!
 *\file		MPU_PAR_API.cpp
 *\brief	MPU模块，参数管理相关接口实现
 *\author	林云
 *\date		2009/03/20

功能：
-#	保存参数
-#	恢复默认参数
-#	导入导出参数

说明：
-#	各模块参数配置由各子模块负责
-#	导入导出时，都由上层GUI通过BKD模块接口获取U盘挂载点后，检索目录，生成配置文件全路径名
 */


#include "MPU_global.h"
#include "mysystem.h"

/**************************************************************************
 *                                                       定义局部宏                                                            *
 **************************************************************************/

//调用配置管理器执行动作
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
 *                     模块内部全局                                      *
 **************************************************************************/
//fan-201409-25
static CConfig *pDevTypeConfig = NULL;	
//////////////

//定制信息配置文件
static CConfig *pCustomizedInfoConfig = NULL;

//Fixed 配置文件
static CConfig *pFixedConfig = NULL;

/**************************************************************************
 *                     模块内部接口                                       *
 **************************************************************************/
//!\brief   初始化参数管理器
//!\return  0表示成功，其它为错误码
int MPU_PAR_Init()
{
	int nRet;
	BOOL bNeedSave = FALSE;

	//导入当前配置文件
	nRet = MPU_PAR_LoadConfig(IDVR_CURRENT_CONFIG_FILE_PATH);

	//fan-2014-09-25
	//加载设备类型配置文件
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
	
	//加载协议支持配置文件
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
	
	//加载 fixed 配置文件
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

//!\brief   逆初始化参数管理器
//!\return  0表示成功，其它为错误码
int PAR_Uninit()
{
	//保存配置
	MPU_PAR_SaveConfigReal();		//最后进行同步工作

	//释放空间
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
 *                     参数管理                                           *
 **************************************************************************/

//!\brief       保存当前配置参数到文件，异步方式
//!\return  0表示成功，其它为错误码
int MPU_PAR_SaveConfig()
{
	//调用配置管理器执行动作
	MPU_PAR_CALL(SaveFile());
}
// 功能同MPU_PAR_SaveConfig ，同步即时保存
int MPU_PAR_SaveConfigReal()
{
	MPU_PAR_CALL(SaveFileReal());
}
//!\brief       恢复默认配置，本函数只用于配置文件已正常加载情况下的恢复参数。恢复默认前会备份先前加载的配置文件,备份文件名为文件名+-bak
//!\param   pszDefFileName      默认配置文件名称（含路径）
//!\return  0表示成功，其它为错误码 -3 板型校验错误
int MPU_PAR_ResumeDefaultConfig(const char *pszDefFileName, const char *pszFlagFile)
{
	//调用配置管理器执行动作
	MPU_PAR_CALL(ResumeDefCfg(pszDefFileName,pszFlagFile));
}

//!\brief   升级配置文件 升级配置文件前会备份先前加载的配置文件，备份文件名为文件名+-bak
//!\param   pszNewCfgFileName 升级的配置文件名称
//!\param   pszFlagFile 标记更新的项的文件,为NULL则将新配置文件中的值添加到原配置文件中
//!\return  0表示成功,其他为错误码
int MPU_PAR_UpdateConfig(const char *pszNewCfgFileName, const char *pszFlagFileName)
{
	MPU_PAR_CALL(UpdateCfg(pszNewCfgFileName, pszFlagFileName));
}

//!\brief   导入配置文件
//!\param   szFullFileName  配置文件名（含路径）
//!\return  0表示成功，其它为错误码
int MPU_PAR_LoadConfig(const char *pszFullFileName)
{
	//调用配置管理器执行动作
	MPU_PAR_CALL(LoadFile(pszFullFileName));
}
//!\brief   获取配置参数
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   nDefault        缺省值
//!\return  返回整形参数值
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

//!\brief   设置配置参数
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   nValue          整形参数值
//!\return  0表示成功，其它为错误码
int MPU_PAR_SetValue(const char *pszSection, const char *pszKey, int nValue)
{
	//调用配置管理器执行动作
	MPU_PAR_CALL(SetValue(pszSection, pszKey, nValue));
}

//!\brief   获取配置参数
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pszDefault      缺省值
//!\param   pValue          返回字符串型参数值
//!\param   nSize           buf空间大小
//!\return  本接口字符串型参数值到pValue指向的buf，并返回pValue
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

//!\brief   设置配置参数
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pValue          字符串型参数值
//!\return  0表示成功，其它为错误码
int MPU_PAR_SetValue(const char *pszSection, const char *pszKey, const char *pValue)
{
	//调用配置管理器执行动作
	MPU_PAR_CALL(SetValue(pszSection, pszKey, pValue));
}

//fan-2014-09-25
//!\brief   获取设备型号名
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pszDefault      缺省值
//!\param   pValue          返回字符串型参数值
//!\param   nSize           buf空间大小
//!\return  本接口字符串型参数值到pValue指向的buf，并返回pValue
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

//!\brief   获取版本标识或协议别名
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pszDefault      缺省值
//!\return  获取值
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
//!\brief   获取版本标识或协议别名
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pszDefault      缺省值
//!\param   pValue          返回字符串型参数值
//!\param   nSize           buf空间大小
//!\return  本接口字符串型参数值到pValue指向的buf，并返回pValue
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

//!\brief   获取系统语言
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pszDefault      缺省值
//!\return  获取值
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

//!\brief   设置系统语言
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pValue          字符串型参数值
//!\return  0表示成功，其它为错误码
int MPU_PAR_Fixed_SetValue(const char *pszSection, const char *pszKey, int nValue)
{
	//调用配置管理器执行动作
	if (0 == pFixedConfig->SetValue(pszSection, pszKey, nValue))
	{
		pFixedConfig->SaveFile();//成功设置    保存
	}

	return 0;
}

/**************************************************************************
 *                                                        注销局部宏                                                           *
 **************************************************************************/
#undef MPU_PAR_CALL				//调用配置管理器执行动作
