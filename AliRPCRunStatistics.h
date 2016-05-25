//
// AliRunRPCStatistics.h
// 
//   Created by Gabriele Gaetano Fronzé
//   Copyright © 2016 Gabriele Gaetano Fronzé. All rights reserved.
//

#ifndef AliRPCRunStatistics_h
#define AliRPCRunStatistics_h

#include <stdio.h>
#include "TObject.h"
#include "TString.h"

class AliRPCRunStatistics : public TObject{
public:
    AliRPCRunStatistics();
    AliRPCRunStatistics(const AliRPCRunStatistics &obj)  : TObject(obj){}; 
    AliRPCRunStatistics(UInt_t runNumber,ULong64_t timeStampStart,ULong64_t timeStampStop,Bool_t isCalib,Bool_t isDark,Double_t meanDarkCurrent,Double_t meanTotalCurrent,Double_t meanHV,ULong64_t totalScalerCountsBending,ULong64_t totalScalerCountsNotBending);
    ~AliRPCRunStatistics(){};
    inline UInt_t GetRunNumber() const { return fRunNumber; };
    inline Double_t GetTimeStampStart() const { return (Double_t)fTimeStampStart; };
    inline Double_t GetTimeStampStop() const { return (Double_t)fTimeStampStop; };
    inline Double_t GetElapsedTime() const { if(fTimeStampStop==0 || fTimeStampStart==0) return 0; else return (Double_t)(fTimeStampStop-fTimeStampStart); };
    inline Bool_t GetIsCalib() const { return fIsCalib; };
    inline Bool_t GetIsDark() const { return fIsDark; };
    inline Double_t GetMeanDarkCurrent() const { return fMeanDarkCurrent; };
    inline Double_t GetMeanTotalCurrent() const { return fMeanTotalCurrent; };
    inline Double_t GetMeanNetCurrent() const { return fMeanTotalCurrent-fMeanDarkCurrent; };
    inline Double_t GetMeanHV() const { return fMeanHV; };
    inline Double_t GetTotalScalerCountsBending() const { return (Double_t)fTotalScalerCountsBending; };
    inline Double_t GetTotalScalerCountsNotBending() const { return (Double_t)fTotalScalerCountsNotBending; };
    inline Double_t GetMeanRateBending() const { return (Double_t)fTotalScalerCountsBending/(Double_t)(fTimeStampStop-fTimeStampStart); };
    inline Double_t GetMeanRateNotBending() const { return (Double_t)fTotalScalerCountsNotBending/(Double_t)(fTimeStampStop-fTimeStampStart); };
    inline Double_t GetIntegratedCharge() const { if(fTimeStampStop==0 || fTimeStampStart==0) return 0; else return (fMeanTotalCurrent-fMeanDarkCurrent)*(Double_t)(fTimeStampStop-fTimeStampStart); };

    Bool_t IsEqual (const TObject *obj) const;
    Int_t Compare(const TObject *obj) const;
    Bool_t IsSortable() const { return kTRUE; };
    
private:
    UInt_t fRunNumber;
    ULong64_t fTimeStampStart;
    ULong64_t fTimeStampStop;
    Bool_t fIsCalib;
    Bool_t fIsDark;
    Double_t fMeanDarkCurrent;
    Double_t fMeanTotalCurrent;
    Double_t fMeanHV;
    ULong64_t fTotalScalerCountsBending;
    ULong64_t fTotalScalerCountsNotBending;
    
    ClassDef(AliRPCRunStatistics,3);
};

#endif /* AliRPCRunStatistics_h */