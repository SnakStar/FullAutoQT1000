#ifndef COMMON_H
#define COMMON_H


//响应返回帧
#define PL_FUN_ACK                          0x0001
//仪器报警
#define PL_FUN_ALARM                        0x0002

////////////////////////////////////////
//开机自检
#define PL_FUN_AM_HARDWEAR_CHECK            0x0010
//新建测试
#define PL_FUN_AM_NEW_TEST                  0x0011
//扫码结果
#define PL_FUN_AM_SCAN_RESULT               0x0012
//开始加样
#define PL_FUN_AM_START_SAMPLE              0x0013
//加样完成
#define PL_FUN_AM_SAMPLE_COMPLETE           0x0014
//准备测试
#define PL_FUN_AM_PREPARE_TEST              0x0015
//测试准备完成
#define PL_FUN_AM_PREPARE_TEST_COMPLETE     0x0016
//开始测试
#define PL_FUN_AM_START_TEST                0x0017
//测试结果
#define PL_FUN_AM_TEST_RESULT               0x0018
//准备卸卡
#define PL_FUN_AM_PREPARE_UNSTALL           0x0019
//卸卡准备完成
#define PL_FUN_AM_PREPARE_UNSTALL_COMPLETE  0x001A
//开始卸卡
#define PL_FUN_AM_START_UNSTALL             0x001B
//卸卡完成
#define PL_FUN_AM_START_UNSTALL_COMPLETE    0x001C

//备用
//#define PL_FUN_AM_PREPARE_UNSTALL           0x0015
//#define PL_FUN_AM_START_UNSTALL             0x0016

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

typedef struct _SamplePos{
    //样本位置
    quint32 m_nSamplePos;
    //试剂盘位置
    quint32 m_nReagentPos;
}SamplePos;



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
}SampleInfo;
////////////////////////////////////////////////////////////


////////////////////////////条码信息/////////////////////////

////////////////////////////////////////////////////////////

////////////////////////////条码信息/////////////////////////

////////////////////////////////////////////////////////////

////////////////////////////条码信息/////////////////////////

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
        m_frameHeader = 0xA5A11A5A;
        m_ProductID   = 0;
        m_DeviceAdd   = 0;
        m_FuncIndex   = 0;
        m_DataLength  = 0;
        m_Data.clear();
        m_CRC         = 0;
        m_frameEnd    = 0x5A1AA1A5;
    }
    void Clear(){
        MsgData();
    }
};

struct TestInfo{
    SampleInfo m_si;
    //项目名称
    QString  m_strItemName;
    //是否测试
    bool     m_bIsTest;
    //是否急诊
    bool m_bIsEmerg;
    //定时器
    QTimer *m_timer;
    //是否开始计时
    bool m_bIsTime;
    //总测试时间
    quint16 m_nCountTime;
    //剩余测试时间
    quint16 m_nRemainTime;
    //数据结果
    QByteArray m_byarrData;
    TestInfo(){
        m_bIsTime  = false;
        m_bIsTest  = false;
        m_bIsEmerg = false;
        m_strItemName = "";
        m_byarrData.clear();
        m_nCountTime  = -1;
        m_nRemainTime = -1;
    }
};


#ifndef MAKEWORD
#define MAKEWORD(low,high)		    (((quint8)(low)) | (((quint8)(high)) << 8))
#endif

#ifndef MAKEFOURWORD
#define MAKEFOURWORD(A,B,C,D)		(((quint8)(A)) | (((quint8)(B)) << 8) | (((quint8)(C)) << 16) | (((quint8)(D)) << 24) )
#endif

#endif // COMMON_H
