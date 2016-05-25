//
//  AliRPCOverflowStatistics.h
//  Aliroot project
//
//  Created by Gabriele Gaetano Fronzé on 21/12/15.
//  Copyright © 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#ifndef AliRPCOverflowStatistics_h
#define AliRPCOverflowStatistics_h

#include <stdio.h>
#include "AliRPCValueDCS.h"

class AliRPCOverflowStatistics : public AliRPCValueDCS{
public:
	AliRPCOverflowStatistics();
    AliRPCOverflowStatistics(const AliRPCOverflowStatistics &obj)  : AliRPCValueDCS(obj){};
    AliRPCOverflowStatistics(UInt_t runNumber,ULong64_t timeStamp,UInt_t overflowLBCount,UInt_t underflowLBCount,Bool_t isCalib);
    ~AliRPCOverflowStatistics(){};
    inline UInt_t GetOverflowNumber(){return fOverflowLBCount;};
    inline UInt_t GetUnderNumber(){return fUnderflowLBCount;};


private:
	UInt_t fOverflowLBCount;
	UInt_t fUnderflowLBCount;

	ClassDef(AliRPCOverflowStatistics,1);
};

#endif