//
//  AliRPCValueVoltage.h
//  Aliroot project
//
//  Created by Gabriele Gaetano Fronzé on 20/11/15.
//  Copyright © 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#ifndef AliRPCValueVoltage_h
#define AliRPCValueVoltage_h

#include <stdio.h>
#include "AliRPCValueDCS.h"

class AliRPCValueVoltage : public AliRPCValueDCS{
public:
    AliRPCValueVoltage();
    AliRPCValueVoltage(const AliRPCValueVoltage &obj)  : AliRPCValueDCS(obj){};
    AliRPCValueVoltage(UInt_t runNumber,ULong64_t timeStamp,Double_t vSupp,Bool_t isCalib);
    ~AliRPCValueVoltage(){};
    inline Double_t GetVSupp() const { return fVSupp; };
    inline void SetVSupp(Double_t vSupp) { fVSupp=vSupp; };
    
private:
    Double_t fVSupp;
    
    ClassDef(AliRPCValueVoltage,1);
};

#endif /* AliRPCValueVoltage_h */
