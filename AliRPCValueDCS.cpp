//
//  AliRPCValueDCS.C
//  Aliroot project
//
//  Created by Gabriele Gaetano Fronzé on 20/11/15.
//  Copyright © 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#include "AliRPCValueDCS.h"
#include "TClass.h"

AliRPCValueDCS::AliRPCValueDCS() : TObject(){
    fRunNumber=0;
    fTimeStamp=0;
    fIsCalib=kFALSE;
};

AliRPCValueDCS::AliRPCValueDCS(UInt_t runNumber,ULong64_t timeStamp,Bool_t isCalib) : TObject(){
    fRunNumber=runNumber;
    fTimeStamp=timeStamp;
    fIsCalib=isCalib;
};

Bool_t AliRPCValueDCS::IsEqual (const TObject *obj) const {
    return fTimeStamp == ((AliRPCValueDCS*)obj)->GetTimeStamp();
};

Int_t AliRPCValueDCS::Compare(const TObject *obj) const {
    if ( fTimeStamp <  ((AliRPCValueDCS*)obj)->GetTimeStamp() ) return -1;
    if ( fTimeStamp == ((AliRPCValueDCS*)obj)->GetTimeStamp() ) return 0;
    else return 1;
};

TString* AliRPCValueDCS::WhatIsThis(){
    TString *className =new TString(((TClass*)this->IsA())->GetName());
    if (className->Contains("AliRPCValueCurrent")) {
        delete className;
        return new TString("current");
    } else if (className->Contains("AliRPCValueVoltage")) {
        delete className;
        return new TString("voltage");
    } else if (className->Contains("AliRPCValueScaler")) {
        delete className;
        return new TString("scaler");
    } else if (className->Contains("AliRPCOverflowStatistics")) {
        delete className;
        return new TString("overflow");
    } else {
        delete className;
        return new TString("unknown");
    }
    
};