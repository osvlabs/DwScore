#include "globals.h"

void setResolution(int width, int height)
{
    DEVMODE lpDevMode;
    lpDevMode.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL|DM_DISPLAYFREQUENCY;
    LONG result;
    //�õ���ʾ�豸��һ��ͼ��ģʽ�豸��ͨ���Ըú���һϵ�еĵ��ÿ��Եõ���ʾ�豸���е�ͼ��ģʽ��Ϣ
    EnumDisplaySettings(NULL,ENUM_REGISTRY_SETTINGS,&lpDevMode);
    lpDevMode.dmPelsWidth = width ;//ˮƽ�ֱ���
    lpDevMode.dmPelsHeight = height;//��ֱ�ֱ���
    //����ʾ�豸��lpszDeviceName�����ж�������ã��ı�Ϊ��lpDevMode�����ж����ͼ��ģʽ
    //NULLֵ������ȱʡ����ʾ�豸��
    result = ChangeDisplaySettingsEx(NULL,&lpDevMode,NULL,0,NULL);
    if(result == DISP_CHANGE_SUCCESSFUL) //�ı�ֱ��ʳɹ�
    {
        //ʹ��CDS_UPDATEREGISTRY��ʾ�˴��޸��ǳ־õģ�����ע�����д������ص�����
        result = ChangeDisplaySettingsEx(NULL,&lpDevMode,NULL,CDS_UPDATEREGISTRY,NULL);
    }
}
