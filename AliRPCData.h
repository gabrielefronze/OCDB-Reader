//
// AliRPCData.h
// 
//   Created by Gabriele Gaetano Fronzé
//   Copyright © 2016 Gabriele Gaetano Fronzé. All rights reserved.

#ifndef AliRPCData_h
#define AliRPCData_h

#include <stdio.h>
#include "TObject.h"
#include "TString.h"
#include "TSortedList.h"
#include "AliRPCRunStatistics.h"

class AliRPCData : public TObject{
public:
	AliRPCData();
	AliRPCData(const AliRPCData &obj)  : TObject(obj){};
	~AliRPCData();
	Bool_t AddRunStatistics(Int_t plane, Int_t side, Int_t RPC, AliRPCRunStatistics *stats);
	Double_t GetMeanDarkCurrent(UInt_t runNumber, Bool_t normalizeToArea=kFALSE);
    Double_t GetMeanTotalCurrent(UInt_t runNumber, Bool_t normalizeToArea=kFALSE);
    Double_t GetMeanNetCurrent(UInt_t runNumber, Bool_t normalizeToArea=kFALSE);
    Double_t GetMeanHV(UInt_t runNumber);
    Double_t GetTotalScalerCountsBending(UInt_t runNumber, Bool_t normalizeToArea=kFALSE);
    Double_t GetTotalScalerCountsNotBending(UInt_t runNumber, Bool_t normalizeToArea=kFALSE);
    Double_t GetMeanRateBending(UInt_t runNumber, Bool_t normalizeToArea=kFALSE);
    Double_t GetMeanRateNotBending(UInt_t runNumber, Bool_t normalizeToArea=kFALSE);
    Double_t GetMeanIntegratedCharge(UInt_t runNumber, Bool_t normalizeToArea=kFALSE);
    Double_t GetMeanTimeStampStart(UInt_t runNumber, Bool_t normalizeToArea=kFALSE);
    TSortedList* operator()(Int_t iPlane,Int_t iSides,Int_t iRPC){return fRunStatistics[iPlane][iSides][iRPC];};

private:
	const static Int_t fNSides=2;

    const static Int_t fNPlanes=4;
    
    const static Int_t fNRPC=9;

    const static Int_t fNLocalBoards=234;

    const static Int_t fNCathodes=2;

    Double_t fAreas[fNRPC][fNPlanes];

    TSortedList *fRunStatistics[fNPlanes][fNSides][fNRPC];

    Bool_t fIsDarkRun;

    Bool_t IsThereThisRun(Int_t plane, Int_t side, Int_t RPC, UInt_t runNumber, Int_t &index);
    Double_t GetMeanSomething(UInt_t runNumber, Bool_t normalizeToArea,Double_t (AliRPCRunStatistics::*funky)() const);

    ClassDef(AliRPCData,2);
};

#endif