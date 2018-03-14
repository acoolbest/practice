#ifndef _INI_RW_H_

#define _INI_RW_H_


#ifdef __cplusplus

extern "C" {

#endif


//����ini�ļ����ڴ�

int iniFileLoad(const char *filename);

//�ͷ�ini�ļ���ռ��Դ

void iniFileFree();


//��ȡ�ַ�������������

int iniGetString(const char *section, const char *key, char *value, int size, const char *defvalue);

//��ȡ����ֵ

int iniGetInt(const char *section, const char *key, int defvalue);

//��ȡ������

double iniGetDouble(const char *section, const char *key, double defvalue);


//�����ַ�������valueΪNULL����ɾ����key�����У�����ע��

int iniSetString(const char *section, const char *key, const char *value);

//��������ֵ��baseȡֵ10��16��8���ֱ��ʾ10��16��8���ƣ�ȱʡΪ10����

int iniSetInt(const char *section, const char *key, int value, int base);


#ifdef __cplusplus

}

#endif


#endif