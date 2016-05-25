//
// AliRPCData.cpp
// 
//   Created by Gabriele Gaetano Fronzé
//   Copyright © 2016 Gabriele Gaetano Fronzé. All rights reserved.
//

#include "AliRPCData.h"
#include "TClass.h"
#include "TList.h"

AliRPCData::AliRPCData() : TObject(){
    for(Int_t iRPC=0;iRPC<fNRPC;iRPC++){
        for(Int_t iPlane=0;iPlane<fNPlanes;iPlane++){
            if(iRPC==5){
                if(iPlane<2){
                    fAreas[iRPC][iPlane]=16056.;
                } else {
                    fAreas[iRPC][iPlane]=18176.;
                }
            }else if(iRPC==4 || iRPC==6){
                if(iPlane<2){
                    fAreas[iRPC][iPlane]=19728./28.*27.;
                } else {
                    fAreas[iRPC][iPlane]=22338./28.*27.;
                }
            }else {
                if(iPlane<2){
                    fAreas[iRPC][iPlane]=19728.;
                } else {
                    fAreas[iRPC][iPlane]=22338.;
                }
            }
        }
    }

    for(Int_t iPlane=0;iPlane<fNPlanes;iPlane++){
        for(Int_t iSide=0;iSide<fNSides;iSide++){
            for(Int_t iRPC=0;iRPC<fNRPC;iRPC++){
                fRunStatistics[iPlane][iSide][iRPC]=new TSortedList();
            }
        }
    }
};

AliRPCData::~AliRPCData(){
    for(Int_t iPlane=0;iPlane<fNPlanes;iPlane++){
        for(Int_t iSide=0;iSide<fNSides;iSide++){
            for(Int_t iRPC=0;iRPC<fNRPC;iRPC++){
                delete fRunStatistics[iPlane][iSide][iRPC];
            }
        }
    }
}

Bool_t AliRPCData::AddRunStatistics(Int_t plane, Int_t side, Int_t RPC, AliRPCRunStatistics *stats){
    if(plane>=fNPlanes) return kFALSE;
    if(side>=fNSides) return kFALSE;
    if(RPC>=fNRPC) return kFALSE;
    if(!stats) return kFALSE;
    fRunStatistics[plane][side][RPC]->Add(stats);
    return kTRUE;
};

Double_t AliRPCData::GetMeanSomething(UInt_t runNumber, Bool_t normalizeToArea,Double_t (AliRPCRunStatistics::*funky)() const){
    Int_t count=0;
    Double_t cumulus=0.;
    for(Int_t iPlane=0;iPlane<fNPlanes;iPlane++){
        for(Int_t iSide=0;iSide<fNSides;iSide++){
            for(Int_t iRPC=0;iRPC<fNRPC;iRPC++){
                Int_t index=0;
                Double_t buffer=0.;
                if(IsThereThisRun(iPlane, iSide, iRPC, runNumber, index)){
                    buffer=(((AliRPCRunStatistics*)fRunStatistics[iPlane][iSide][iRPC]->At(index))->*funky)();
                    if(normalizeToArea==kTRUE) buffer/=fAreas[iRPC][iPlane];
                    cumulus+=buffer;
                    count++;
                }
            }
        }
    }
    if(count!=0) return cumulus/(Double_t)count;
    else return 0.;
};

Double_t AliRPCData::GetMeanDarkCurrent(UInt_t runNumber, Bool_t normalizeToArea){
    return AliRPCData::GetMeanSomething(runNumber,normalizeToArea,&AliRPCRunStatistics::GetMeanDarkCurrent);
};

Double_t AliRPCData::GetMeanTotalCurrent(UInt_t runNumber, Bool_t normalizeToArea){
    return AliRPCData::GetMeanSomething(runNumber,normalizeToArea,&AliRPCRunStatistics::GetMeanTotalCurrent);
};

Double_t AliRPCData::GetMeanNetCurrent(UInt_t runNumber, Bool_t normalizeToArea){
    return AliRPCData::GetMeanSomething(runNumber,normalizeToArea,&AliRPCRunStatistics::GetMeanNetCurrent);
};

Double_t AliRPCData::GetMeanHV(UInt_t runNumber){
    return AliRPCData::GetMeanSomething(runNumber,kFALSE,&AliRPCRunStatistics::GetMeanHV);
};

Double_t AliRPCData::GetTotalScalerCountsBending(UInt_t runNumber, Bool_t normalizeToArea){
    return AliRPCData::GetMeanSomething(runNumber,normalizeToArea,&AliRPCRunStatistics::GetTotalScalerCountsBending);
};

Double_t AliRPCData::GetTotalScalerCountsNotBending(UInt_t runNumber, Bool_t normalizeToArea){
    return AliRPCData::GetMeanSomething(runNumber,normalizeToArea,&AliRPCRunStatistics::GetTotalScalerCountsNotBending);
};

Double_t AliRPCData::GetMeanRateBending(UInt_t runNumber, Bool_t normalizeToArea){
    return AliRPCData::GetMeanSomething(runNumber,normalizeToArea,&AliRPCRunStatistics::GetMeanRateBending);
};

Double_t AliRPCData::GetMeanRateNotBending(UInt_t runNumber, Bool_t normalizeToArea){
    return AliRPCData::GetMeanSomething(runNumber,normalizeToArea,&AliRPCRunStatistics::GetMeanRateNotBending);
};

Double_t AliRPCData::GetMeanIntegratedCharge(UInt_t runNumber, Bool_t normalizeToArea){
    return AliRPCData::GetMeanSomething(runNumber,normalizeToArea,&AliRPCRunStatistics::GetIntegratedCharge);
};

Double_t AliRPCData::GetMeanTimeStampStart(UInt_t runNumber, Bool_t normalizeToArea){
    return AliRPCData::GetMeanSomething(runNumber,normalizeToArea,&AliRPCRunStatistics::GetTimeStampStart);
};

Bool_t AliRPCData::IsThereThisRun(Int_t plane, Int_t side, Int_t RPC, UInt_t runNumber,  Int_t &index){
    for(Int_t iEntries=0;iEntries<fRunStatistics[plane][side][RPC]->GetEntries();iEntries++){
        if(((AliRPCRunStatistics*)fRunStatistics[plane][side][RPC]->At(iEntries))->GetRunNumber()==runNumber){
            index=iEntries;
            return kTRUE;
        }
    }
    index=-1;
    return kFALSE;
};