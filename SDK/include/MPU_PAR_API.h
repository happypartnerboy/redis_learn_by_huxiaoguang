
/*!
 *\file		MPU_PAR_API.h
 *\brief	MPUģ�飬����������ؽӿڶ���
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

#ifndef _INC_MPU_PAR_API_H_
#define _INC_MPU_PAR_API_H_

//!\brief   ��ʼ������������
//!\return  0��ʾ�ɹ�������Ϊ������
int MPU_PAR_Init();

//!\brief   ���浱ǰ���ò������ļ�
//!\return  0��ʾ�ɹ�������Ϊ������
int MPU_PAR_SaveConfig();
// ����ͬMPU_PAR_SaveConfig ��ͬ����ʱ����
int MPU_PAR_SaveConfigReal();

//!\brief   ���������ļ�
//!\param   szFullFileName  �����ļ�������·����
//!\return  0��ʾ�ɹ�������Ϊ������
int MPU_PAR_LoadConfig(const char *pszFullFileName);


//!\brief   ��ȡ���ò���
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   nDefault        ȱʡֵ
//!\return  �������β���ֵ
int MPU_PAR_GetValue(const char *pszSection, const char *pszKey, int nDefault);

//!\brief   �������ò���
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   nValue          ���β���ֵ
//!\return  0��ʾ�ɹ�������Ϊ������
int MPU_PAR_SetValue(const char *pszSection, const char *pszKey, int nValue);

//!\brief   ��ȡ���ò���
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pszDefault      ȱʡֵ
//!\param   pValue          �����ַ����Ͳ���ֵ
//!\param   nSize           buf�ռ��С
//!\return  ���ӿ��ַ����Ͳ���ֵ��pValueָ���buf��������pValue
char *MPU_PAR_GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize);

//!\brief   �������ò���
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pValue          �ַ����Ͳ���ֵ
//!\return  0��ʾ�ɹ�������Ϊ������
int MPU_PAR_SetValue(const char *pszSection, const char *pszKey, const char *pValue);

//fan-2014-09-25
//!\brief   ��ȡ�豸�ͺ���
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pszDefault      ȱʡֵ
//!\param   pValue          �����ַ����Ͳ���ֵ
//!\param   nSize           buf�ռ��С
//!\return  ���ӿ��ַ����Ͳ���ֵ��pValueָ���buf��������pValue
char *MPU_PAR_DevType_GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize);

//!\brief   ��ȡ�汾��ʶ��Э�����
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pszDefault      ȱʡֵ
//!\return  ��ȡֵ
int MPU_PAR_CustomizedInfo_GetValue(const char *pszSection, const char *pszKey, int nDefault);

//!\brief   ��ȡ�汾��ʶ��Э�����
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pszDefault      ȱʡֵ
//!\param   pValue          �����ַ����Ͳ���ֵ
//!\param   nSize           buf�ռ��С
//!\return  ���ӿ��ַ����Ͳ���ֵ��pValueָ���buf��������pValue
int MPU_PAR_CustomizedInfo_GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize);

//!\brief   ��ȡϵͳ����
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pszDefault      ȱʡֵ
//!\return  ��ȡֵ
int MPU_PAR_Fixed_GetValue(const char *pszSection, const char *pszKey, int nDefault);

//!\brief   ����ϵͳ����
//!\param   pszSection      ������
//!\param   pszKey          �ؼ�����
//!\param   pValue          �ַ����Ͳ���ֵ
//!\return  0��ʾ�ɹ�������Ϊ������
int MPU_PAR_Fixed_SetValue(const char *pszSection, const char *pszKey, int nValue);

#endif							//_INC_MPU_PAR_API_H_
