//
// AMANDATxtToCParser.C
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

void AMANDATxtToCParser(TString inputFileName){

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

	TList *data[nSides][nPlanes][nRPC];
    for(Int_t iSide=0;iSide<nSides;iSide++){
        for(Int_t iPlane=0;iPlane<nPlanes;iPlane++){
            for(Int_t iRPC=0;iRPC<nRPC;iRPC++){
                data[iSide][iPlane][iRPC]=new TList();
            }
        }
    }

	Double_t timeStamp;
    Int_t side;
    Int_t mt;
    Int_t rpc;
    Double_t current;

	//1264586062.764;MTR_INSIDE_MT11_RPC1_HV.actual.iMon;0.

    Bool_t isZero=kFALSE;

	ifstream fin;
    fin.open(inputFileName.Data());
    char line[256];
    while(!fin.eof()){
        if(fin.eof())break;
        fin.getline(line,256);
        sscanf(line,"%lf %d %d %d %lf",&timeStamp,&side,&mt,&rpc,&current);

        if(current!=0. || (current==0. && !isZero)){
            //cout<<timeStamp<<" ";
            //cout<<side<<" ";
            //cout<<mt<<" ";
            //cout<<rpc<<" ";
            //cout<<current<<endl;

            data[side][mts[mt]][rpc-1]->Add(new AliRPCValueCurrent(0, timeStamp, current, 0., kFALSE, 0));
        } //else cout<<"#### skip ###"<<endl;

        isZero=(current==0.);


    }
    Printf("\t\t Parsing ended.\n");
    fin.close();

	inputFileName.ReplaceAll(".txt", "");
	TFile *outputFile= new TFile(Form("AMANDA_%s.root",inputFileName.Data()),"RECREATE");

    for(Int_t iSide=0;iSide<nSides;iSide++){
        for(Int_t iPlane=0;iPlane<nPlanes;iPlane++){
            for(Int_t iRPC=0;iRPC<nRPC;iRPC++){
                data[iSide][iPlane][iRPC]->Write(Form("AMANDA_MTR_%s_MT%d_RPC%d",(sides[iSide]).Data(),planes[iPlane],iRPC+1),TObject::kSingleKey);
            }
        }
    }


}