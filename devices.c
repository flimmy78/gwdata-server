#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <endian.h>

#include "devices.h"
#include "debug.h"
#include "protocal_208.h"


#define ARRAY_SIZE(a)   (sizeof(a)/sizeof(a[0]))

static cJSON *temp_v2json(int id,const char *data,int len)
{
        int v = data[1];
        char buf[128] = {0};
        snprintf(buf,sizeof(buf),"%d",v);
        return cJSON_CreateString(buf);
}

static int temp_v2chararray(int id,cJSON *value,char *buf,int len)
{
        return protocal208_sd2data(id,buf,len);
}

static int temp_v2cloud(int id,cJSON *value,char *buf,int len)
{
        int v = atoi(value->valuestring);
        buf[0] = v;
        return 1;
}

static cJSON *light_v2json(int id,const char *data,int len)
{
        int16_t v = *(int16_t*)data;
        v = le16toh(v);
        char buf[128] = {0};
        snprintf(buf,sizeof(buf),"%d",v);
        return cJSON_CreateString(buf);
}

static int light_v2chararray(int id,cJSON *value,char *buf,int len)  //NOTICE  p208 need it
{
        return protocal208_sd2data(id,buf,len);
}

static int light_v2cloud(int id,cJSON *value,char *buf,int len)
{
        uint16_t v = atoi(value->valuestring);
        v = htobe16(v);
        memcpy(buf,&v,sizeof(v));

        return sizeof(v);
}

static cJSON *led_v2json(int id,const char *data,int len)
{
        int v = data[0];
        if(v == 1){
                return cJSON_CreateString("true");
        }else{
                return cJSON_CreateString("false");
        }
}

static int led_v2chararray(int id,cJSON *value,char *buf,int len)
{
        switch(value->type){
                case cJSON_False:
                        buf[0] = 3;
                        break;
                case cJSON_True:
                        buf[0] = 1;
                        break;
                case cJSON_String:
                        if(strcasecmp(value->valuestring,"true") == 0)
                                buf[0] = 1;
                        else
                                buf[0] = 3;
                        break;
                default:
                        break;
        }
        return 8;
}

static int led_v2cloud(int id,cJSON *value,char *buf,int len)
{
        switch(value->type){
                case cJSON_False:
                        buf[0] = 3;
                        break;
                case cJSON_True:
                        buf[0] = 1;
                        break;
                case cJSON_String:
                        if(strcasecmp(value->valuestring,"true"))
                                buf[0] = 1;
                        else
                                buf[0] = 3;
                        break;
                default:
                        break;
        }
        return 1;
}

static cJSON *acceleration_v2json(int id,const char *data,int len)
{
         signed short int Ax = data[0]|data[1]<<8;
         signed short int Ay = data[2]|data[3]<<8;
         signed short int Az = data[4]|data[5]<<8;

         signed short int Gx = data[6]|data[7]<<8;
         signed short int Gy = data[8]|data[9]<<8;
         signed short int Gz = data[10]|data[11]<<8;

         char buf[128] = {0};
         snprintf(buf,sizeof(buf),"Ax:%d,Ay:%d,Az:%d,Gx:%d,Gy:%d,Gz:%d",Ax,Ay,Az,Gx,Gy,Gz);

         return cJSON_CreateString(buf);
}

static int acceleration_v2chararray(int id,cJSON *value,char *buf,int len)
{
        return protocal208_sd2data(id,buf,len);
}

static int acceleration_v2cloud(int id,cJSON *value,char *buf,int len)
{
        return 0;
}


static cJSON *magnetic_v2json(int id,const char *data,int len)
{
        signed short int X = data[0]|data[1]<<8;
        signed short int Y = data[2]|data[3]<<8;
        signed short int Z = data[4]|data[5]<<8;

        char buf[128] = {0};
        snprintf(buf,sizeof(buf),"X:%d,Y:%d,Z:%d",X,Y,Z);

        return cJSON_CreateString(buf);
}

static int magnetic_v2chararray(int id,cJSON *value,char *buf,int len)
{
        return protocal208_sd2data(id,buf,len);
}

static int magnetic_v2cloud(int id,cJSON *value,char *buf,int len)
{
        return -1;
}

cJSON *rfid_v2json(int id,const char *data,int len)
{
        int32_t value = *(int32_t*)data; 
        value = le32toh(value); 
        return cJSON_CreateNumber(value);
}

static int rfid_v2chararray(int id,cJSON *value,char *buf,int len)
{
        return protocal208_sd2data(id,buf,len);
}
static int rfid_v2cloud(int id,cJSON *value,char *buf,int len)
{
        int32_t v = atoi(value->valuestring);
        v = htole32(v);
        memcpy(buf,&v,sizeof(v));
        return sizeof(v);
}
static cJSON *temp_and_humi_v2json(int id,const char *data,int len)
{
        int humi = (unsigned char)data[0];
        int temp = (unsigned char)data[1];
        cJSON *value = cJSON_CreateObject();
        cJSON_AddNumberToObject(value,"humidity",humi);
        cJSON_AddNumberToObject(value,"temperature",temp);


        return value;
}

static int temp_and_humi_v2chararray(int id,cJSON *value,char *buf,int len)
{
        return -1;
}

static int temp_and_humi_v2cloud(int id,cJSON *value,char *buf,int len)
{
        return -1;
}

static cJSON *closet_v2json(int id,const char *data,int len)
{
        int v = data[0];
        switch(v){
                case 1:
                        return cJSON_CreateString("left");
                case 2:
                        return cJSON_CreateString("right");
                case 3:
                        return cJSON_CreateString("stop");
                default:
                        return cJSON_CreateString("closet unknown");
        }
}
static int closet_v2chararray(int id,cJSON *value,char *buf,int len)
{
        if(strcasecmp(value->valuestring,"left") == 0){
                buf[0] = 1;
        }else if(strcasecmp(value->valuestring,"right") == 0){
                buf[0] = 2;
        }else{
                buf[0] = 3;
        }
        return 8;
}
static int closet_v2cloud(int id,cJSON *value,char *buf,int len)
{
        if(strcasecmp(value->valuestring,"left") == 0){
                buf[0] = 1;
        }else if(strcasecmp(value->valuestring,"right") == 0){
                buf[0] = 2;
        }else{
                buf[0] = 3;
        }
        return 1;
}

static cJSON *heart_v2json(int id,const char *data,int len)
{
        int status = data[0];
        int freq = data[1];

        if(status == 2)
                return cJSON_CreateString("testing");
        char buf[100] = {0};
        snprintf(buf,sizeof(buf),"%d",freq);
        return cJSON_CreateString(buf);
}

static int heart_v2chararay(int id,cJSON *value,char *buf,int len)
{
        return -1;
}

static int heart_v2cloud(int id,cJSON *value,char *buf,int len)
{
        return -1;
}

static cJSON *tiwen_v2json(int id,const char *data,int len)
{
        if(data[0] == 0xA0){
                int v = data[1]*256 + data[2];
                cJSON *value = cJSON_CreateObject();
                cJSON_AddNumberToObject(value,"TW",v);
                return value;
        }else if(data[0] == 0xA1){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddStringToObject(value,"status","stop");
                return value;
        }else if(data[0] == 0xA2){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddNumberToObject(value,"SN0",data[1]);
                cJSON_AddNumberToObject(value,"SN1",data[2]);
                cJSON_AddNumberToObject(value,"SN2",data[3]);
                cJSON_AddNumberToObject(value,"SN3",data[4]);
                return value;
        }else if(data[0] == 0xA3){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddNumberToObject(value,"T1",data[1]);
                cJSON_AddNumberToObject(value,"T2",data[2]);
                cJSON_AddNumberToObject(value,"T3",data[3]);
                cJSON_AddNumberToObject(value,"T4",data[4]);
        }else if(data[0] == 0xCA){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddStringToObject(value,"status","adjust up success");
                return value;
        }else if(data[0] == 0xCD){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddStringToObject(value,"status","adjust down success");
                return value;
        }else if(data[0] == 0x5A){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddStringToObject(value,"status","online");
                return value;
        }
        cJSON *value = cJSON_CreateObject();
        cJSON_AddStringToObject(value,"status","error");
        return value;
}

static int tiwen_v2chararray(int id,cJSON *value,char *buf,int len)
{
        const char *str = value->valuestring;
        if(strcasecmp(str,"start") == 0){
                buf[0] = 0xA0;
                return 1;
        }else if(strcasecmp(str,"stop") == 0){
                buf[0] = 0xA1;
                return 1;
        }else if(strcasecmp(str,"read_id") == 0){
                buf[0] = 0xA2;
                return 1;
        }else if(strcasecmp(str,"read_date") == 0){
                buf[0] = 0xA3;
                return 1;
        }else if(strstr(str,"adjust_up") != NULL){
                char *start = strchr(str,':') + 1;
                int x = atoi(start);
                buf[0] = 0xA4;
                buf[1] = x;
                return 2;
        }else if(strstr(str,"adjust_down") != NULL){
                char *start = strchr(str,':') + 1;
                int x = atoi(start);
                buf[0] = 0xA4;
                buf[1] = x;
                return 2;
        }else{
                buf[0] = 0xAA;
                return 1;
        }
        

}

static int tiwen_v2cloud(int id,cJSON *value,char *buf,int len)
{
        buf[0] = 0;
        return 1;
}

static cJSON *xueyang_v2json(int id,const char *data,int size)
{
        if(data[0] == 0xA0){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddNumberToObject(value,"MB",data[1]);
                cJSON_AddNumberToObject(value,"XY",data[2]);
                cJSON_AddNumberToObject(value,"XL",data[3]);
                return value;
        }else if(data[0] == 0xA1){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddStringToObject(value,"status","stop");
                return value;
        }else if(data[0] == 0xA2){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddNumberToObject(value,"SN0",data[1]);
                cJSON_AddNumberToObject(value,"SN1",data[2]);
                cJSON_AddNumberToObject(value,"SN2",data[3]);
                cJSON_AddNumberToObject(value,"SN3",data[4]);
                return value;
        }else if(data[0] == 0xA3){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddNumberToObject(value,"T1",data[1]);
                cJSON_AddNumberToObject(value,"T2",data[2]);
                cJSON_AddNumberToObject(value,"T3",data[3]);
                cJSON_AddNumberToObject(value,"T4",data[4]);
                return value;
        }else if(data[0] == 0x5A){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddStringToObject(value,"status","online");
                return value;
        }else{
                cJSON *value = cJSON_CreateObject();
                cJSON_AddStringToObject(value,"status","error");
                return value;
        }
}

static int xueyang_v2chararray(int id,cJSON *value,char *buf,int len)
{
        const char *str = value->valuestring;
        if(strcasecmp(str,"start") == 0){
                buf[0] = 0xA0;
                return 1;
        }else if(strcasecmp(str,"stop") == 0){
                buf[0] = 0xA1;
                return 1;
        }else if(strcasecmp(str,"read_id") == 0){
                buf[0] = 0xA2;
                return 1;
        }else if(strcasecmp(str,"read_date") == 0){
                buf[0] = 0xA3;
                return 1;
        }else{
                buf[0] = 0xAA;
                return 1;
        }
        
}

static int xueyang_v2cloud(int id,cJSON *value,char *buf,int len)
{
        buf[0] = 0;
        return 1;
}

static cJSON *xueya_v2json(int id,const char *data,int len)
{
        if(data[0] == 0x5B){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddStringToObject(value,"status","stop");
                return value;
        }else if(data[0] == 0x5A){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddStringToObject(value,"status","online");
                return value;
        }else if(data[0] == 0x54){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddNumberToObject(value,"QYH",data[1]);
                cJSON_AddNumberToObject(value,"QYL",data[2]);
                return value;
        }else if(data[0] == 0x55){
                cJSON *value = cJSON_CreateObject();
                int shuzhang = data[3]*256 + data[4];
                int shousuo  = (data[1]&0x7f)*256 + data[2];
                cJSON_AddNumberToObject(value,"shuzhang",shuzhang);
                cJSON_AddNumberToObject(value,"shousuo",shousuo);
                cJSON_AddNumberToObject(value,"XL",data[5]);
                return value;
        }else if(data[0] == 0x56){
                cJSON *value = cJSON_CreateObject();
                cJSON_AddNumberToObject(value,"X",data[1]);
                return value;
        }
        cJSON *value = cJSON_CreateObject();
        cJSON_AddStringToObject(value,"status","error");
        return value;
}

static int xueya_v2chararray(int id,cJSON *value,char *buf,int len)
{
        const char *str = value->valuestring;
        if(strcasecmp(str,"sleep") == 0){
                buf[0] = 0xAB;
                return 1;
        }else if(strcasecmp(str,"wakeup") == 0){
                buf[0] = 0xAA;
                return 1;
        }else if(strcasecmp(str,"start") == 0){
                buf[0] = 0xA0;
                return 1;
        }else if(strcasecmp(str,"stop") == 0){
                buf[0] = 0xA3;
                return 1;
        }else{
                buf[0] = 0xAA;
                return 1;
        }
}

static int xueya_v2cloud(int id,cJSON *value,char *buf,int len)
{
        buf[0] = 0;
        return 1;
}

struct devices
{
        int type; 
        cJSON* (*v2json)(int id,const char *data,int len);
        int (*v2chararray)(int id,cJSON *value,char *buf,int size);
        int (*v2cloud)(int id,cJSON *value,char *buf,int len);
};


static struct  devices devices[] = {
        {0x10,temp_v2json,temp_v2chararray,temp_v2cloud}, //�¶�
        {0x11,temp_v2json,temp_v2chararray,temp_v2cloud}, //ʪ��
        {0x12,light_v2json,light_v2chararray,light_v2cloud}, //����
        {0x16,light_v2json,light_v2chararray,light_v2cloud}, //��ѹ
        {0x13,light_v2json,light_v2chararray,light_v2cloud}, //��ȼ����
        {0x1d,light_v2json,light_v2chararray,light_v2cloud}, //����
        {0x1a,light_v2json,light_v2chararray,light_v2cloud}, //������̼
        {0x18,led_v2json,led_v2chararray,led_v2cloud},     //�̵���
        {0x14,led_v2json,led_v2chararray,led_v2cloud},     //�������
        {0x22,led_v2json,led_v2chararray,led_v2cloud},     //���ⷴ��
        {0x23,led_v2json,led_v2chararray,led_v2cloud},     //��������
        {0x24,led_v2json,led_v2chararray,led_v2cloud},     //����
        {0x25,led_v2json,led_v2chararray,led_v2cloud},     //���
        {0x26,led_v2json,led_v2chararray,led_v2cloud},     //����
        {0x27,led_v2json,led_v2chararray,led_v2cloud},     //��
        {0x29,rfid_v2json,rfid_v2chararray,rfid_v2cloud},     //��
        {0x15,acceleration_v2json,acceleration_v2chararray,acceleration_v2cloud}, //���ٶ�
        {0x20,magnetic_v2json,magnetic_v2chararray,magnetic_v2cloud},  //�ų�
        {0x29,rfid_v2json,rfid_v2chararray,rfid_v2cloud},
        {0x41,temp_and_humi_v2json,temp_and_humi_v2chararray,temp_and_humi_v2cloud},
	{0x43,light_v2json,light_v2chararray,light_v2cloud}, //ph
        {0x2A,closet_v2json,closet_v2chararray,closet_v2cloud},//�����¹�
        {0x3A,heart_v2json,heart_v2chararay,heart_v2cloud},// ���ʴ�����
        {0x44,tiwen_v2json,tiwen_v2chararray,tiwen_v2cloud},//����
        {0x42,xueyang_v2json,xueyang_v2chararray,xueyang_v2cloud},//Ѫ��
        {0x45,xueya_v2json,xueya_v2chararray,xueya_v2cloud},//Ѫѹ
        
};

static struct devices *find_device(int device_type)
{
        int i;
        for(i=0;i<ARRAY_SIZE(devices);i++){
                if(device_type == devices[i].type)
                        return &devices[i];
        }
        return NULL;
}

cJSON* device_v2json(int id,int type,const char *data,int len)
{
        struct devices *d = find_device(type);
        if(d == NULL){
                return NULL;
        }

        return d->v2json(id,data,len);
}

int device_v2chararray(int id,int type,cJSON *value,char *buf,int size)
{
        struct devices *d = find_device(type);

        if(d == NULL)
                return -1;

        return d->v2chararray(id,value,buf,size);
}
int device_v2cloud(int id,int type,cJSON *value,char *buf,int size)
{
        struct devices *d = find_device(type);
        if(d == NULL)
                return -1;
        return d->v2cloud(id,value,buf,size);
}
