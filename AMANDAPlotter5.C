//
// AMANDAPlotter.C
// 
//   Created by Gabriele Gaetano Fronzé
//   Copyright © 2016 Gabriele Gaetano Fronzé. All rights reserved.
//

#include <stdio.h>
#include "TString.h"
#include "TMap.h"
#include "TObjArray.h"
#include "TSortedList.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TMultiGraph.h"
#include "TFile.h"
#include "AliRPCValueDCS.h"
#include "AliRPCValueCurrent.h"
#include "AliRPCValueVoltage.h"
#include "AliRPCValueScaler.h"
#include "AliRPCRunStatistics.h"
#include "AliRPCOverflowStatistics.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

void AMANDAPlotter5(TString inputFileName){

    TFile *inputFile=new TFile(inputFileName.Data());

	//definizione degli array usati per la nomenclatura
    const Int_t nSides=2;
    TString sides[2];
    sides[0]="INSIDE";
    sides[1]="OUTSIDE";

    const Int_t nPlanes=4;
    Int_t planes[4];
    planes[0]=11;
    planes[1]=12;
    planes[2]=21;
    planes[3]=22;
    Int_t mts[23];
    mts[11]=0;
    mts[12]=1;
    mts[21]=2;
    mts[22]=3;
    
    const Int_t nRPC=9;

    const Int_t nLocalBoards=234;

    const Int_t nCathodes=2;
    TString cathodes[nCathodes];
    cathodes[0]="BENDING";
    cathodes[1]="NOT_BENDING";

    Int_t RPCIndexes[18]={5,6,7,8,9,9,8,7,6,5,4,3,2,1,1,2,3,4};
    Int_t RPCSides[18]=  {0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0};

    Int_t color[nRPC] = {kBlack,kRed,kGreen,kBlue,kYellow-7,kMagenta,kCyan-7,kGray,kOrange};

    Double_t Areas[nRPC][nPlanes];
    for(Int_t iRPC=0;iRPC<nRPC;iRPC++){
        for(Int_t iPlane=0;iPlane<nPlanes;iPlane++){
            if(iRPC==5){
                if(iPlane<2){
                    Areas[iRPC][iPlane]=16056.;
                } else {
                    Areas[iRPC][iPlane]=18176.;
                }
            }else if(iRPC==4 || iRPC==6){
                if(iPlane<2){
                    Areas[iRPC][iPlane]=19728./28.*27.;
                } else {
                    Areas[iRPC][iPlane]=22338./28.*27.;
                }
            }else {
                if(iPlane<2){
                    Areas[iRPC][iPlane]=19728.;
                } else {
                    Areas[iRPC][iPlane]=22338.;
                }
            }
        }
    }

    ULong64_t epochStart=1262300400;
    ULong64_t epochStop=1451602800;

    Long64_t epoch2010=1262300400;
    Long64_t epoch2011=1293836400;
    Long64_t epoch2012=1325372400;
    Long64_t epoch2013=1356994800;
    Long64_t epoch2014=1388530800;
    Long64_t epoch2015=1420070400;
    Long64_t epoch2016=1451602800;
    Long64_t epoch2017=1483225200;

    ULong64_t bins=(epochStop-epochStart);
    Int_t rebinEntity=6000;

    Double_t meanDarkCurrent[nPlanes][7]={{1.333333333,1.068055556,0.971296296,0.916666667,2.494444444,1.930769231,2.005555556},{0.661111111,0.765277778,0.609259259,0.605555556,1.488888889,1.285185185,1.783333333},{1.494444444,1.529166667,1.294444444,1.733333333,2.941666667,2.374074074,3.388888889},{1.638888889,2.00125,1.877777778,2.483333333,4.294285714,3.446296296,5.027777778}};

    TList *data[nSides][nPlanes][nRPC];
    for(Int_t iSide=0;iSide<nSides;iSide++){
        for(Int_t iPlane=0;iPlane<nPlanes;iPlane++){
            for(Int_t iRPC=0;iRPC<nRPC;iRPC++){
                //cout<<iRPC<<endl;
                inputFile->GetObject(Form("AMANDA_MTR_%s_MT%d_RPC%d",(sides[iSide]).Data(),planes[iPlane],iRPC+1), data[iSide][iPlane][iRPC]);
                data[iSide][iPlane][iRPC]->Sort();
            }
        }
    }

    Double_t charge[nSides][nPlanes][nRPC];

    for (Int_t side=0; side<nSides; side++) {
        cout<<side<<endl;
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                cout<<"RPC "<<RPC<<endl;
                charge[side][plane][RPC-1]=0.;

                ULong64_t actualTimestamp=0;
                ULong64_t followingTimestamp=0;
                Int_t nEntries=data[side][plane][RPC-1]->GetEntries();
                Double_t binContentBuffer=0.;
                Int_t binContentsCounter=0;
                Int_t pointCounter1=0;
                Int_t pointCounter2=0;

                Bool_t singletonZero=kTRUE;
                Int_t yearIndex=0;

                TIter iter(data[side][plane][RPC-1]);
                AliRPCValueCurrent *current=(AliRPCValueCurrent*)iter();
                AliRPCValueCurrent *currentFollowing;
                while(currentFollowing=(AliRPCValueCurrent*)iter()){
                	//if(iEntry%10000==0)cout<<iEntry<<"/"<<nEntries<<endl;
                	//current=(AliRPCValueCurrent*)(data[side][plane][RPC-1]->At(iEntry));
                    //currentFollowing=(AliRPCValueCurrent*)(data[side][plane][RPC-1]->At(iEntry+1));
                    //if(current->GetITot()==0.) continue;
                	actualTimestamp=current->GetTimeStamp();
                    followingTimestamp=currentFollowing->GetTimeStamp();

                    if(actualTimestamp>epoch2016) yearIndex=6;
                    else if(actualTimestamp>epoch2015) yearIndex=5;
                    else if(actualTimestamp>epoch2014) yearIndex=4;
                    else if(actualTimestamp>epoch2013) yearIndex=3;
                    else if(actualTimestamp>epoch2012) yearIndex=2;
                    else if(actualTimestamp>epoch2011) yearIndex=1;
                    else if(actualTimestamp>epoch2010) yearIndex=0;

                    if(current->GetITot()>meanDarkCurrent[plane][yearIndex])current->SetIDark(meanDarkCurrent[plane][yearIndex]);
            		if(actualTimestamp>epoch2010)charge[side][plane][RPC-1]+=current->GetINet()*(Double_t)(followingTimestamp-actualTimestamp);
            		actualTimestamp=current->GetTimeStamp();

                    if(plane==1 && side==1 && RPC==2 && actualTimestamp>epoch2015 && singletonZero) {
                        charge[side][plane][RPC-1]=0.;
                        singletonZero=kFALSE;
                    }

                	current=currentFollowing;
                    currentFollowing=0x0;
                }
                delete data[side][plane][RPC-1];
            }
        }
    }

    for (Int_t side=0; side<nSides; side++) {
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                printf("%s %d %d %f\n",sides[side].Data(),planes[plane],RPC,charge[side][plane][RPC-1]);
            }
        }
    }
}