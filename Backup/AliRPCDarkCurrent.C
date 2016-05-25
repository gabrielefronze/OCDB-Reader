//
//  AliRPCDarkCurrent.cpp
//  Aliroot project
//
//  Created by Gabriele Gaetano Fronzé on 20/11/15.
//  Copyright © 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#include "AliRPCDarkCurrent.h"

AliRPCDarkCurrent::AliRPCDarkCurrent() : TObject(){
    fRunNumber=0;
    fTimeStamp=0;
    fITot=0;
    fIDark=0;
    fIsCalib=kFALSE;
};

AliRPCDarkCurrent::AliRPCDarkCurrent(UInt_t runNumber,Int_t plane,ULong64_t timeStamp,Double_t iTot,Double_t iDark,Bool_t isCalib) : TObject(){
    fRunNumber=runNumber;
    fPlane=plane;
    fTimeStamp=timeStamp;
    fITot=iTot;
    fIDark=iDark;
    fIsCalib=isCalib;
};

Bool_t AliRPCDarkCurrent::IsEqual (const TObject *obj) const {
    return fTimeStamp == ((AliRPCDarkCurrent*)obj)->GetTimeStamp();
};

Int_t AliRPCDarkCurrent::Compare(const TObject *obj) const {
    if ( fTimeStamp <  ((AliRPCDarkCurrent*)obj)->GetTimeStamp() ) return -1;
    if ( fTimeStamp == ((AliRPCDarkCurrent*)obj)->GetTimeStamp() ) return 0;
    else return 1;
};

void AliRPCDarkCurrent::PrintData(){
    printf("Run #=%d\nTime=%llu\nitot=%f\nidark%f\nIs calibration=%d\n",fRunNumber,fTimeStamp,fITot,fIDark,fIsCalib);
}