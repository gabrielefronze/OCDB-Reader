//
//  AliRPCValueDCS.h
//  Aliroot project
//
//  Created by Gabriele Gaetano Fronzé on 20/11/15.
//  Copyright © 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#ifndef AliRPCValueDCS_h
#define AliRPCValueDCS_h

#include <stdio.h>
#include "TObject.h"
#include "TString.h"

class AliRPCValueDCS : public TObject{
public:
    AliRPCValueDCS();
    AliRPCValueDCS(const AliRPCValueDCS &obj)  : TObject(obj){};
    AliRPCValueDCS(UInt_t runNumber,ULong64_t timeStamp,Bool_t isCalib);
    ~AliRPCValueDCS(){};
    inline UInt_t GetRunNumber() const { return fRunNumber; };
    inline ULong64_t GetTimeStamp() const { return fTimeStamp; };
    inline Bool_t GetIsCalib() const { return fIsCalib; };
    Bool_t IsEqual (const TObject *obj) const;
    Int_t Compare(const TObject *obj) const;
    Bool_t IsSortable() const { return kTRUE; };
    TString *WhatIsThis();
    inline Bool_t IsCurrent(){return (TString*)(this->WhatIsThis())->Contains("current");};
    inline Bool_t IsVoltage(){return (TString*)(this->WhatIsThis())->Contains("voltage");};
    inline Bool_t IsScaler(){return (TString*)(this->WhatIsThis())->Contains("scaler");};
    inline Bool_t IsOverflow(){return (TString*)(this->WhatIsThis())->Contains("overflow");};
    
private:
    UInt_t fRunNumber;
    ULong64_t fTimeStamp;
    Bool_t fIsCalib;
    
    ClassDef(AliRPCValueDCS,1);
};

#endif /* AliRPCValueDCS_h */
