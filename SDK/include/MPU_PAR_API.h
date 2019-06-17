
/*!
 *\file		MPU_PAR_API.h
 *\brief	MPU模块，参数管理相关接口定义
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

#ifndef _INC_MPU_PAR_API_H_
#define _INC_MPU_PAR_API_H_

//!\brief   初始化参数管理器
//!\return  0表示成功，其它为错误码
int MPU_PAR_Init();

//!\brief   保存当前配置参数到文件
//!\return  0表示成功，其它为错误码
int MPU_PAR_SaveConfig();
// 功能同MPU_PAR_SaveConfig ，同步即时保存
int MPU_PAR_SaveConfigReal();

//!\brief   导入配置文件
//!\param   szFullFileName  配置文件名（含路径）
//!\return  0表示成功，其它为错误码
int MPU_PAR_LoadConfig(const char *pszFullFileName);


//!\brief   获取配置参数
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   nDefault        缺省值
//!\return  返回整形参数值
int MPU_PAR_GetValue(const char *pszSection, const char *pszKey, int nDefault);

//!\brief   设置配置参数
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   nValue          整形参数值
//!\return  0表示成功，其它为错误码
int MPU_PAR_SetValue(const char *pszSection, const char *pszKey, int nValue);

//!\brief   获取配置参数
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pszDefault      缺省值
//!\param   pValue          返回字符串型参数值
//!\param   nSize           buf空间大小
//!\return  本接口字符串型参数值到pValue指向的buf，并返回pValue
char *MPU_PAR_GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize);

//!\brief   设置配置参数
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pValue          字符串型参数值
//!\return  0表示成功，其它为错误码
int MPU_PAR_SetValue(const char *pszSection, const char *pszKey, const char *pValue);

//fan-2014-09-25
//!\brief   获取设备型号名
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pszDefault      缺省值
//!\param   pValue          返回字符串型参数值
//!\param   nSize           buf空间大小
//!\return  本接口字符串型参数值到pValue指向的buf，并返回pValue
char *MPU_PAR_DevType_GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize);

//!\brief   获取版本标识或协议别名
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pszDefault      缺省值
//!\return  获取值
int MPU_PAR_CustomizedInfo_GetValue(const char *pszSection, const char *pszKey, int nDefault);

//!\brief   获取版本标识或协议别名
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pszDefault      缺省值
//!\param   pValue          返回字符串型参数值
//!\param   nSize           buf空间大小
//!\return  本接口字符串型参数值到pValue指向的buf，并返回pValue
int MPU_PAR_CustomizedInfo_GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize);

//!\brief   获取系统语言
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pszDefault      缺省值
//!\return  获取值
int MPU_PAR_Fixed_GetValue(const char *pszSection, const char *pszKey, int nDefault);

//!\brief   设置系统语言
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pValue          字符串型参数值
//!\return  0表示成功，其它为错误码
int MPU_PAR_Fixed_SetValue(const char *pszSection, const char *pszKey, int nValue);

#endif							//_INC_MPU_PAR_API_H_
