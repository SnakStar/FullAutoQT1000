﻿#ifndef COMMANDSTRUCTDEF_H
#define COMMANDSTRUCTDEF_H

#include <QTimer>


//条码长度
#define BARCODELENGTH 16

#define SAMPLECOUNT   16
////////////////////////////条码信息/////////////////////////
typedef struct _Scan_Info_t{

    unsigned char CODE[BARCODELENGTH];   //条码
    unsigned int  Scan_State; 	         //扫描状态

}Scan_Info_t;

typedef struct _Bar_Code_Info{
    Scan_Info_t scaninfo[SAMPLECOUNT];
}ScanBarCodeInfo;
////////////////////////////////////////////////////////////


////////////////////////////条码信息/////////////////////////



////////////////////////////ID卡信息/////////////////////////
typedef struct _ID_Card_Info{
    //ID卡数据
    QByteArray m_byarrIDCardData;
    //测试项目名称
    QString m_strTestItem;
    //测试单位
    QString m_strTestUnit;
    //剩余数量
    quint8 m_nRemaining;
}IDCardInfo;

////////////////////////////////////////////////////////////

////////////////////////////测试信息/////////////////////////

////////////////////////////////////////////////////////////

struct MsgData{
    quint32 m_frameHeader;
    quint32 m_ProductID;
    quint16 m_DeviceAdd;
    quint16 m_FuncIndex;
    quint32 m_DataLength;
    QByteArray m_Data;
    quint32 m_CRC;
    quint32 m_frameEnd;
    MsgData(){
        m_frameHeader = 0x5A1AA1A5;
        m_ProductID   = 0;
        m_DeviceAdd   = 0;
        m_FuncIndex   = 0;
        m_DataLength  = 0;
        m_Data.clear();
        m_CRC         = 0;
        m_frameEnd    = 0xA5A11A5A;
    }
    void Clear(){
        MsgData();
    }
};

//试剂类
typedef struct _Reagent_Obj{
    //试剂卡槽位置
    quint8 m_nReagentPos;
    //试剂单位
    QString m_strReagentUnit;
    //试剂有效期
    QString m_strReagentValidDate;
    //ID卡数据
    QByteArray m_byarrIDCardData;
    //试剂项目名称
    QString m_strReagentItemName;
    //试剂剩余数量
    quint8 m_nReagentRemaining;
}ReagentObj;

//用户类
typedef struct _User_Info{
public:
    QString m_strName;
    quint8  m_nAge;
    quint8  m_nSex;
    _User_Info(){
        m_strName = "";
        m_nAge = 0;
        m_nSex = 0;
    }
    void Clear(){
        _User_Info();
    }
}UserInfo;

//项目类
typedef struct _Test_Item{
    //项目名称
    QString m_strItemName;
    //项目反应总时间
    quint16 m_nItemCountTime;
    //项目反应位置
    quint8  m_nItemTestPos;
    //项目剩余时间
    quint16 m_nItemRemainTime;
    //项目是否发送
    //bool m_bIsSend;
    //项目是否为空
    bool m_bIsEmpty;
    //项目定时器;
    QTimer* m_timer;
    //项目结果
    QString m_strResult;
    //项目原始数据
    QByteArray m_byteDrawData;
    //试剂类对象
    ReagentObj m_ReagentObj;
    _Test_Item(){
        m_strItemName = "";
        m_nItemCountTime = 0;
        m_nItemTestPos = 0;
        m_nItemRemainTime = 0;
        //m_bIsSend = false;
        m_bIsEmpty = true;
        m_timer = NULL;
        m_strResult = "";
        m_byteDrawData.clear();
    }
    void Clear(){
        _Test_Item();
    }
}TestItem;

//样本对象类
typedef struct _Sample_Obj{
    //样本位置
    quint8 m_nSamplePos;
    //样本编号
    QString m_strSampleID;
    //用户信息
    UserInfo m_UserInfo;
    //项目信息
    QList<TestItem> m_listTestItem;
    //样本是否为空
    bool m_bIsEmpty;
    _Sample_Obj(){
        m_bIsEmpty = true;
        m_nSamplePos = 0;
        m_strSampleID = "";
        m_UserInfo.Clear();
        m_listTestItem.clear();
    }
    void Clear(){
        _Sample_Obj();
    }
}SampleObj;

typedef struct _Send_Queue{
    quint8 m_nSamplePos;
    QString m_strItemName;
    QByteArray m_byteContentData;
    _Send_Queue(){
        m_nSamplePos=0;
        m_strItemName.clear();
        m_byteContentData.clear();
    }
    void Clear(){
        _Send_Queue();
    }
}SendQueue;


typedef struct _SamplePos{
    //样本位置
    quint32 m_nSamplePos;
    //试剂盘位置
    quint32 m_nReagentPos;
}SamplePos;


/*
typedef struct _SampleInfo{
    //样本位置
    SamplePos m_SamPos;
    //样本编号
    QString  m_strSampleNo;
    _SampleInfo(){
        m_SamPos.m_nReagentPos = -1;
        m_SamPos.m_nSamplePos  = -1;
        m_strSampleNo = "";
    }
    void Clear(){
        _SampleInfo();
    }
}SampleInfo;
////////////////////////////////////////////////////////////


struct TestInfo{
    SampleInfo m_si;
    //项目名称
    QString  m_strItemName;
    //是否测试
    bool     m_bIsTest;
    //是否发送测试类型
    bool     m_bIsSend;
    //定时器
    QTimer*  m_timer;
    //是否开始计时
    bool m_bIsTime;
    //总测试时间
    quint16 m_nCountTime;
    //剩余测试时间
    quint16 m_nRemainTime;
    //数据结果
    QByteArray m_byarrData;
    //ID卡位置
    quint8 m_nIDCardIndex;
    //测试单位
    QString m_strUnit;
    //测试结果
    QString m_strResult;
    TestInfo(){
        m_bIsTime  = false;
        m_bIsTest  = false;
        m_bIsSend  = false;
        //m_bIsEmerg = false;
        m_nIDCardIndex = 0;
        m_strItemName = "";
        m_strResult = "";
        m_strUnit = "";
        m_byarrData.clear();
        m_nCountTime  = -1;
        m_nRemainTime = -1;
        m_si.Clear();
    }
    void Clear(){
        TestInfo();
    }
};*/





#endif // COMMANDSTRUCTDEF_H
