﻿#ifndef PARAMDEF_H
#define PARAMDEF_H

#define SCANMODE  "GeneralSet/ScanMode"
#define AUTOLOGIN "GeneralSet/AutoLogin"
#define LISMODE   "GeneralSet/LisMode"
//0血清，1全血
//#define BLOODTYPE "GeneralSet/BloodType"

//调试界面-仪器调试参数设置
#define DEBUGMODE        "DebugParamSet/DebugMode"
#define CONNPCMODE       "DebugParamSet/ConnPCMode"
#define TESTCARDMODE     "DebugParamSet/TestCardMode"
#define LOGDEBUGMODE     "DebugParamSet/LogDebugMode"

#define DILUENT          "DebugParamSet/Diluent"
#define SAMPLESIZE       "DebugParamSet/SampleSize"

#define DEBUGSCANSTART   "DebugParamSet/ScanStart"
#define DEBUGCALCMETHOD  "DebugParamSet/CalcMethod"
#define DEBUGAMPPARAM    "DebugParamSet/AmpParam"
#define DEBUGTESTTIME    "DebugParamSet/TestTime"
#define DEBUGRESULTVALUE "DebugParamSet/ResultValue"

//调试界面-仪器参数设置
#define SAMPLEOFFSETCONST  "DebugDeviceSet/SampleOffsetConst"
#define SAMPLEOFFSETCOEFF  "DebugDeviceSet/SampleOffsetCoeff"
#define DILUENTOFFSETCONST "DebugDeviceSet/DiluentOffsetConst"
#define DILUENTOFFSETCOEFF "DebugDeviceSet/DiluentOffsetCoeff"
#define RESULTRATIOCOEFF   "DebugDeviceSet/ResultRatioCoeff"
#define RESULTRATIOCONSTA  "DebugDeviceSet/ResultRatioConst"
#define CLEANOUTMAXVALUE   "DebugDeviceSet/CleanoutEffluentMaxValue"
#define CLEANOUTCURVALUE   "DebugDeviceSet/CleanoutCurrentValue"
#define EFFLUENTCURVALUE   "DebugDeviceSet/EffluentCurrentValue"

//调试界面-硬件参数设置
//退卡电机
#define DEBUGHWARMEXITCARDMOTORSIZE   "DebugHardwareSet/ExitCardMotorSize"
//转盘
#define DEBUGHWARMDIALZEROSTEP        "DebugHardwareSet/DialZeroStep"
#define DEBUGHWARMDIALOFFSET          "DebugHardwareSet/DialOffset"
//加样位
#define DEBUGHWARMSAMPLEAddX          "DebugHardwareSet/SampleAddX"
#define DEBUGHWARMSAMPLEAddY          "DebugHardwareSet/SampleAddY"
//混匀位
#define DEBUGHWARMMIXINGX              "DebugHardwareSet/MixingX"
#define DEBUGHWARMMIXINGYMAXDEPTH      "DebugHardwareSet/MixingYMaxDEPTH"
#define DEBUGHWARMMIXINGANDBUFDEPTH    "DebugHardwareSet/MixingAddBuffDEPTH"
#define DEBUGHWARMMIXINGTOUCHDEPTH     "DebugHardwareSet/MixingTouchDEPTH"
#define DEBUGHWARMMIXINGCLEAREOUTCOUNT "DebugHardwareSet/MixingCleareOutCount"
#define DEBUGHWARMMIXINGWATERCOUNT     "DebugHardwareSet/MixingWaterCount"
#define DEBUGHWARMMIXINGDWELLTIME      "DebugHardwareSet/MixingDwellTime"
#define DEBUGHWARMMIXINGWATERINTIME    "DebugHardwareSet/MixingWaterInTime"
//清洗位
#define DEBUGHWARMCLEAREOUTX                "DebugHardwareSet/CleareOutX"
#define DEBUGHWARMCLEAREOUTY                "DebugHardwareSet/CleareOutY"
#define DEBUGHWARMCLEAREOUTCOUNT            "DebugHardwareSet/CleareOutCount"
#define DEBUGHWARMCLEAREOUTWATERCOUNT       "DebugHardwareSet/CleareOutWaterCount"
#define DEBUGHWARMCLEAREOUTWATERINTIME      "DebugHardwareSet/CleareOutWaterInTime"
//缓冲液位
#define DEBUGHWARMBUFFERX         "DebugHardwareSet/BufferX"
#define DEBUGHWARMBUFFERY         "DebugHardwareSet/BufferY"
#define DEBUGHWARMBUFFERDEPTH     "DebugHardwareSet/BufferDepth"
#define DEBUGHWARMBUFFEROFFSET    "DebugHardwareSet/BufferOffset"
//吸样位
#define DEBUGHWARMSUCTIONX        "DebugHardwareSet/SuctionX"
#define DEBUGHWARMSUCTIONY        "DebugHardwareSet/SuctionY"
#define DEBUGHWARMSUCTIONDEPTH    "DebugHardwareSet/SuctionDepth"
#define DEBUGHWARMSUCTIONOFFSET   "DebugHardwareSet/SuctionOffset"
//激光头偏移量位置
#define DEBUGHWARSCANOFFSET       "DebugHardwareSet/ScanOffset"
//吸混匀液大小
#define DEBUGHWARSUCKBUFFSIZE       "DebugHardwareSet/SuckBuffSize"
//吐混匀液大小
#define DEBUGHWARSPITBUFFSIZE       "DebugHardwareSet/SpitBuffSize"


#endif // PARAMDEF_H
