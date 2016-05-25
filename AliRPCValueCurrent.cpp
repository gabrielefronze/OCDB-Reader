//
//  AliRPCValueCurrent.C
//  Aliroot project
//
//  Created by Gabriele Gaetano Fronzé on 20/11/15.
//  Copyright © 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#include "AliRPCValueCurrent.h"

AliRPCValueCurrent::AliRPCValueCurrent() : AliRPCValueDCS(){
	fCalibRunNumber=0;
    fITot=0.;
    fIDark=0.;
};

AliRPCValueCurrent::AliRPCValueCurrent(UInt_t runNumber,ULong64_t timeStamp,Double_t iTot,Double_t iDark,Bool_t isCalib,UInt_t calibRunNumber) : AliRPCValueDCS(runNumber,timeStamp,isCalib){
    fCalibRunNumber=calibRunNumber;
    fITot=iTot;
    fIDark=iDark;
};