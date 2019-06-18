#ifndef RESULTSTRUCTDEF_H
#define RESULTSTRUCTDEF_H


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



#endif // RESULTSTRUCTDEF_H
