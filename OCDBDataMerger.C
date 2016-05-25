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

void OCDBDataMerger(TString listFileName,TString outputFileName){

	vector<TFile*> filesToMerge;

	ifstream fin;
    TString buffer;
    fin.open(listFileName.Data());
    while(!fin.eof()){
        fin >> buffer;
        if(fin.eof())break;
        filesToMerge.push_back(new TFile(buffer.Data()));
        Printf("\t\t\t Added %s\n",buffer.Data());
    }
    Printf("\t\t File adding ended.\n");
    fin.close();

    TFile *outputFile=new TFile(outputFileName.Data(),"RECREATE");

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
    
    const Int_t nRPC=9;

    const Int_t nLocalBoards=234;

    const Int_t nCathodes=2;
    TString cathodes[nCathodes];
    cathodes[0]="BENDING";
    cathodes[1]="NOT_BENDING";

    TList *mergedDataList[nSides][nPlanes][nRPC];
    TList *scalersMergedDataList[2][nSides][nPlanes][nRPC];
    TList *scalersLocalBoardMergedList[nCathodes][nPlanes][nLocalBoards];
    
    for (Int_t plane=0; plane<nPlanes; plane++) {
        for (Int_t side=0; side<nSides; side++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                mergedDataList[side][plane][RPC-1]=new TList();
                mergedDataList[side][plane][RPC-1]->SetName(Form("Data_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC));
                scalersMergedDataList[0][side][plane][RPC-1]=new TList();
                scalersMergedDataList[0][side][plane][RPC-1]->SetName(Form("Scalers_MTR_%s_%s_MT%d_RPC%d",(sides[side]).Data(),(cathodes[0]).Data(),planes[plane],RPC));
                scalersMergedDataList[1][side][plane][RPC-1]=new TList();
                scalersMergedDataList[1][side][plane][RPC-1]->SetName(Form("Scalers_MTR_%s_%s_MT%d_RPC%d",(sides[side]).Data(),(cathodes[1]).Data(),planes[plane],RPC));
            }
        }
    }

    for(Int_t cathode=0;cathode<nCathodes;cathode++){
        for(Int_t plane=0;plane<nPlanes;plane++){
            for(Int_t local=0;local<nLocalBoards;local++){
                scalersLocalBoardMergedList[cathode][plane][local]=new TList();
                scalersLocalBoardMergedList[cathode][plane][local]->SetName(Form("Scalers_MTR_%s_MT%d_LB%d",(cathodes[cathode]).Data(),planes[plane],local+1));
            }
        }
    }

    for (std::vector<TFile*>::iterator fileIterator = filesToMerge.begin(); fileIterator != filesToMerge.end(); ++fileIterator){
        TList *dataList[nSides][nPlanes][nRPC];
        TList *scalersDataList[2][nSides][nPlanes][nRPC];
        TList *scalersLocalBoardList[nCathodes][nPlanes][nLocalBoards];

        cout<<(*fileIterator)->GetName()<<" parsing started..."<<endl;

        cout<<"\t Data parsing started..."<<endl;
        for (Int_t side=0; side<nSides; side++) {
            for (Int_t plane=0; plane<nPlanes; plane++) {
                for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                    (*fileIterator)->GetObject(Form("Data_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC),dataList[side][plane][RPC-1]);
                    Int_t nEntries=dataList[side][plane][RPC-1]->GetEntries();
                    for(Int_t iEntries=0;iEntries<nEntries;iEntries++){
                        if(iEntries%10==0)cout<<iEntries<<"/"<<nEntries<<"\r";
                        mergedDataList[side][plane][RPC-1]->Add(dataList[side][plane][RPC-1]->At(iEntries));
                    }
                    cout<<endl;
                    dataList[side][plane][RPC-1]=0x0;
                }
            }
        }
        cout<<"\t Data parsing ended."<<endl;

        cout<<"\t RPC Scalers parsing started..."<<endl;
        for(Int_t cathode=0;cathode<nCathodes;cathode++){
            for (Int_t side=0; side<nSides; side++) {
                for (Int_t plane=0; plane<nPlanes; plane++) {
                    for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                        (*fileIterator)->GetObject(Form("Scalers_MTR_%s_%s_MT%d_RPC%d",(sides[side]).Data(),(cathodes[cathode]).Data(),planes[plane],RPC),scalersDataList[cathode][side][plane][RPC-1]);
                        Int_t nEntries=scalersDataList[cathode][side][plane][RPC-1]->GetEntries();
                        for(Int_t iEntries=0;iEntries<nEntries;iEntries++){
                            if(iEntries%10000==0)cout<<iEntries<<"/"<<nEntries<<"\r";
                            scalersMergedDataList[cathode][side][plane][RPC-1]->Add(scalersDataList[cathode][side][plane][RPC-1]->At(iEntries));
                        }
                        cout<<endl;
                        scalersDataList[cathode][side][plane][RPC-1]=0x0;
                    }
                }
            }
        }
        cout<<"\t RPC Scalers parsing ended."<<endl;
        
        cout<<"\t LB Scalers parsing started..."<<endl;
        for(Int_t cathode=0;cathode<nCathodes;cathode++){
            for(Int_t plane=0;plane<nPlanes;plane++){
                for(Int_t local=0;local<nLocalBoards;local++){
                    (*fileIterator)->GetObject(Form("Scalers_MTR_%s_MT%d_LB%d",(cathodes[cathode]).Data(),planes[plane],local+1),scalersLocalBoardList[cathode][plane][local]);
                    Int_t nEntries=scalersLocalBoardList[cathode][plane][local]->GetEntries();
                    for(Int_t iEntries=0;iEntries<nEntries;iEntries++){
                        if(iEntries%10000==0)cout<<iEntries<<"/"<<nEntries<<"\r";
                        scalersLocalBoardMergedList[cathode][plane][local]->Add(scalersLocalBoardList[cathode][plane][local]->At(iEntries));
                    }
                    cout<<endl;
                    scalersLocalBoardList[cathode][plane][local]=0x0;
                }
            }
        }
        cout<<"\t LB Scalers parsing ended."<<endl;

        cout<<(*fileIterator)->GetName()<<" parsing ended."<<endl;
        delete *fileIterator;
    }

    outputFile->cd();
    for (Int_t plane=0; plane<nPlanes; plane++) {
        for (Int_t side=0; side<nSides; side++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                mergedDataList[side][plane][RPC-1]->Sort();
                mergedDataList[side][plane][RPC-1]->Write(Form("Data_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC),TObject::kSingleKey);
                scalersMergedDataList[0][side][plane][RPC-1]->Sort();
                scalersMergedDataList[0][side][plane][RPC-1]->Write(Form("Scalers_MTR_%s_%s_MT%d_RPC%d",(sides[side]).Data(),(cathodes[0]).Data(),planes[plane],RPC),TObject::kSingleKey);
                scalersMergedDataList[1][side][plane][RPC-1]->Sort();
                scalersMergedDataList[1][side][plane][RPC-1]->Write(Form("Scalers_MTR_%s_%s_MT%d_RPC%d",(sides[side]).Data(),(cathodes[1]).Data(),planes[plane],RPC),TObject::kSingleKey);
            }
        }
    }

    for(Int_t cathode=0;cathode<nCathodes;cathode++){
        for(Int_t plane=0;plane<nPlanes;plane++){
            for(Int_t local=0;local<nLocalBoards;local++){
                scalersLocalBoardMergedList[cathode][plane][local]->Sort();
                scalersLocalBoardMergedList[cathode][plane][local]->Write(Form("Scalers_MTR_%s_MT%d_LB%d",(cathodes[cathode]).Data(),planes[plane],local+1),TObject::kSingleKey);
            }
        }
    }
}