#ifndef COMMON_H
#define COMMON_H

#include <QTimer>


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


////////////////////////////结果信息/////////////////////////
struct RawTestInfo{
    //测试一面积1
    long m_nTest1Area1;
    //测试一面积2
    long m_nTest1Area2;
    //测试一比值
    long m_nTest1Ratio;
    //测试二面积1
    long m_nTest2Area1;
    //测试二面积2
    long m_nTest2Area2;
    //测试二比值
    long m_nTest2Ratio;
    //结果
    double m_fTestResult;
    //计算方案
    quint8 m_nComputeMothed;
    //扫描起始点
    quint8 m_nScanStart;
    //质控峰积分宽度
    quint8 m_nQCIntegralBreadth;
    //检测峰积分宽度
    quint8 m_nCheckIntegralBreadt;
    //放大参数
    quint8 m_nAmpParam;
    //质控峰高度
    quint8 m_nQCMinHeightValue;
    //测试通道号
    quint8 m_nChannel;
    //波峰位置1
    quint16 m_nCrestPos1;
    //波谷位置左1
    quint16 m_nTroughPosLeft1;
    //波谷位置右1
    quint16 m_nTroughPosRight1;
    //波峰位置2
    quint16 m_nCrestPos2;
    //波谷位置左2
    quint16 m_nTroughPosLeft2;
    //波谷位置右2
    quint16 m_nTroughPosRight2;
    //
    RawTestInfo(){
        Clear();
    }

    void Clear(){
        //测试一面积1
        m_nTest1Area1 = 0;
        //测试一面积2
        m_nTest1Area1 = 0;
        //测试一比值
        m_nTest1Ratio = 0;
        //测试二面积1
        m_nTest2Area1 = 0;
        //测试二面积2
        m_nTest2Area2 = 0;
        //测试二比值
        m_nTest2Ratio = 0;
        //结果
        m_fTestResult = 0;
        //计算方案
        m_nComputeMothed = 0;
        //扫描起始点
        m_nScanStart = 0;
        //质控峰积分宽度
        m_nQCIntegralBreadth = 0;
        //检测峰积分宽度
        m_nCheckIntegralBreadt = 0;
        //放大参数
        m_nAmpParam = 0;
        //质控峰高度
        m_nQCMinHeightValue = 0;
        //测试通道号
        m_nChannel = 0;
        //波峰位置1
        m_nCrestPos1 = 0;
        //波谷位置左1
        m_nTroughPosLeft1 = 0;
        //波谷位置右1
        m_nTroughPosRight1 = 0;
        //波峰位置2
        m_nCrestPos2 = 0;
        //波谷位置左2
        m_nTroughPosLeft2 = 0;
        //波谷位置右2
        m_nTroughPosRight2 = 0;
    }
};


struct ResultDataInfo{
    //本次测试的样本编号
    quint64 m_nNumberID;
    //姓名
    QString m_strName;
    //年龄
    QString m_strAge;
    //性别
    QString m_strSex;
    //测试数据的同步字编号
    quint8 m_nSyncID;
    //测试数据的长度-1280
    quint32 m_nDataLen;
    //测试数据
    QByteArray m_byteCanData;
    //测试条码号
    QString m_strIDCardBarCode;
    //测试ID卡信息
    QString m_strIDMessage;
    //新旧卡标志
    quint8 m_nCardFlag;
    //测试名称
    QString m_strTestName;
    //测试单位
    QString m_strTestUnit;
    //测试结果
    QString m_strResult;
    //反应时间
    qint32 m_nReactionTime;
    //试剂有效期
    QString m_strValidDate;
    //测试信息
    RawTestInfo m_RawTestInfo;
    //初始化
    ResultDataInfo(){
        m_RawTestInfo.Clear();
        m_byteCanData.clear();
        m_nDataLen = 0;
        m_nCardFlag = 0;
        m_nSyncID = 0;
        m_strIDCardBarCode = "";
        m_strIDMessage = "";
        m_strResult = "";
        m_strTestName = "";
        m_strTestUnit = "";
        m_nNumberID = 0;
        m_nReactionTime = 0;
        m_strValidDate = "";
        m_strName = "";
        m_strAge = "";
        m_strSex = "";
    }
};
////////////////////////////////////////////////////////////

////////////////////////////条码信息/////////////////////////

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
