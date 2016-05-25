//
// AliRPCRunStatistics.cpp
// 
//   Created by Gabriele Gaetano Fronzé
//   Copyright © 2016 Gabriele Gaetano Fronzé. All rights reserved.
//

#include "AliRPCRunStatistics.h"
#include "TClass.h"

AliRPCRunStatistics::AliRPCRunStatistics() : TObject(){
    fRunNumber=0;
    fTimeStampStart=0;
    fTimeStampStop=0;
    fIsCalib=kFALSE;
    fIsDark=kFALSE;
    fMeanDarkCurrent=0.;
    fMeanTotalCurrent=0.;
    fMeanHV=0.;
    fTotalScalerCountsBending=0.;
    fTotalScalerCountsNotBending=0.;
};

AliRPCRunStatistics::AliRPCRunStatistics(UInt_t runNumber,ULong64_t timeStampStart,ULong64_t timeStampStop,Bool_t isCalib,Bool_t isDark,Double_t meanDarkCurrent,Double_t meanTotalCurrent,Double_t meanHV,ULong64_t totalScalerCountsBending,ULong64_t totalScalerCountsNotBending) : TObject(){
    fRunNumber=runNumber;
    fTimeStampStart=timeStampStart;
    fTimeStampStop=timeStampStop;
    fIsCalib=isCalib;
    fIsDark=isDark;
    fMeanDarkCurrent=meanDarkCurrent;
    fMeanTotalCurrent=meanTotalCurrent;
    fMeanHV=meanHV;
    fTotalScalerCountsBending=totalScalerCountsBending;
    fTotalScalerCountsNotBending=totalScalerCountsNotBending;
};

Bool_t AliRPCRunStatistics::IsEqual (const TObject *obj) const {
    return fTimeStampStart == ((AliRPCRunStatistics*)obj)->GetTimeStampStart();
};

Int_t AliRPCRunStatistics::Compare(const TObject *obj) const {
    if ( fTimeStampStart <  ((AliRPCRunStatistics*)obj)->GetTimeStampStart() ) return -1;
    if ( fTimeStampStart == ((AliRPCRunStatistics*)obj)->GetTimeStampStart() ) return 0;
    else return 1;
};