//
//  AliRPCDarkCurrent.hpp
//  Aliroot project
//
//  Created by Gabriele Gaetano Fronzé on 20/11/15.
//  Copyright © 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#ifndef AliRPCDarkCurrent_h
#define AliRPCDarkCurrent_h

#include <stdio.h>
#include "TObject.h"

class AliRPCDarkCurrent : public TObject{
public:
    AliRPCDarkCurrent();
    AliRPCDarkCurrent(const AliRPCDarkCurrent &obj)  : TObject(obj){};
    AliRPCDarkCurrent(UInt_t runNumber,Int_t plane,ULong64_t timeStamp,Double_t iTot,Double_t iDark,Bool_t isCalib);
    ~AliRPCDarkCurrent(){};
    inline UInt_t GetRunNumber() const { return fRunNumber; };
    inline Int_t GetPlane() const { return fPlane; };
    inline ULong64_t GetTimeStamp() const { return fTimeStamp; };
    inline Double_t GetITot() const { return fITot; };
    inline void SetITot(Double_t iTot) { fITot=iTot; };
    inline Double_t GetIDark() const { return fIDark; };
    inline void SetIDark(Double_t iDark) { fIDark=iDark; };
    inline Bool_t GetIsCalib() const { return fIsCalib; };
    Bool_t IsEqual (const TObject *obj) const;
    Int_t Compare(const TObject *obj) const;
    Bool_t IsSortable() const { return kTRUE; };
    void PrintData();
    
private:
    UInt_t fRunNumber;
    Int_t fPlane;
    ULong64_t fTimeStamp;
    Double_t fITot;
    Double_t fIDark;
    Bool_t fIsCalib;
    
    ClassDef(AliRPCDarkCurrent,1);
};

#endif /* AliRPCDarkCurrent_hpp */
