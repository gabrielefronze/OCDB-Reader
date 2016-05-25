//
//  AliRPCValueScaler.C
//  Aliroot project
//
//  Created by Gabriele Gaetano Fronzé on 21/12/15.
//  Copyright © 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#include "AliRPCValueScaler.h"

AliRPCValueScaler::AliRPCValueScaler() : AliRPCValueDCS(){
    fScalerCounts=0.;
    fOverflow=kFALSE;
    fDeltaT=-1.;
};

AliRPCValueScaler::AliRPCValueScaler(UInt_t runNumber,ULong64_t timeStamp, Long64_t scalerCounts,Bool_t isCalib,Double_t deltaT,Bool_t overflow) : AliRPCValueDCS(runNumber,timeStamp,isCalib){
    fScalerCounts=scalerCounts;
    fOverflow=overflow;
    fDeltaT=deltaT;
};