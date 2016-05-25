//
//  OCDBReader.cpp
//  Aliroot project
//
//  Created by Gabriele Gaetano Fronzé on 02/12/15.
//  Copyright © 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#include <stdio.h>
#include "TString.h"
#include "TMap.h"
#include "TObjArray.h"
#include "TSortedList.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraph.h"
//#include "AliCDBManager.h"
//#include "AliCDBEntry.h"
//#include "AliDCSValue.h"
#include "AliRPCDarkCurrent.h"
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

void OCDBReader(Bool_t file=kFALSE, TString filename=""){
    Int_t year=2015;
    vector<Int_t> runList;
    
    if (file) {
        ifstream fin;
        Int_t nRunBuffer=1;
        fin.open(filename.Data());
        while(!fin.eof()){
            fin >> nRunBuffer;
            if(fin.eof())break;
            runList.push_back(nRunBuffer);
            Printf("\t\t\t Added %d\n",nRunBuffer);
        }
        Printf("\t\t Run adding ended.\n");
        fin.close();
    } else {
        Int_t nRuns=0;
        Int_t nRunBuffer=1;
        cout<<"How many runs do you want to add?"<<endl;
        cin>>nRuns;
        for(Int_t runIndex=0;runIndex<nRuns;runIndex++){
            cout<<runIndex<<".\t";
            cin>>nRunBuffer;
            runList.push_back(nRunBuffer);
            cout<<"Added"<<endl;
            
        }
        Printf("\t\t Run adding ended.\n");
    }
    
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
    Bool_t isCalib=kFALSE;
    
    AliCDBManager *managerCurrent = AliCDBManager::Instance();
    AliCDBManager *managerVoltage = AliCDBManager::Instance();
    AliCDBManager *managerRunType = AliCDBManager::Instance();
    
    TSortedList dataList[nSides][nPlanes][nRPC];
    
    managerCurrent->SetDefaultStorage(Form("alien://folder=/alice/data/%d/OCDB",year));
    managerVoltage->SetDefaultStorage(Form("alien://folder=/alice/data/%d/OCDB",year));
    managerRunType->SetDefaultStorage(Form("alien://folder=/alice/data/%d/OCDB",year));
    
    for (std::vector<Int_t>::iterator runIterator = runList.begin(); runIterator != runList.end(); ++runIterator) {
        managerCurrent->SetRun(*runIterator);
        managerVoltage->SetRun(*runIterator);
        managerRunType->SetRun(*runIterator);
        
        AliCDBEntry *entryRunType = managerRunType->Get("GRP/GRP/Data");
        if(!entryRunType) {
            printf("\nproblem with entryRunType\n");
            break;
        }
        
        AliGRPObject *grpObj=(AliGRPObject*)entryRunType->GetObject();
        TString *runType=new TString(grpObj->GetRunType());
        
        if(runType->Contains("PHYSICS")){
            isCalib=kFALSE;
        } else if(runType->Contains("CALIBRATION")){
            isCalib=kTRUE;
        } else {
            break;
        }

        AliCDBEntry *entryCurrent = managerCurrent->Get("MUON/Calib/TriggerDCS");
        if(!entryCurrent) {
            printf("\nproblem with entryCurrent\n");
            break;
        }
        
        TMap *mapCurrent = (TMap*)entryCurrent->GetObject();
        if(!mapCurrent) {
            printf("Problems getting map\n");
            break;
        }
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t side=0; side<nSides; side++) {
                for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                    TObjArray *dataArray;
                    //printf(Form("MTR_%s_MT%d_RPC%d_HV.actual.iMon\n",sides[side].Data(),planes[plane],RPC));
                    dataArray=(TObjArray*)(mapCurrent->GetValue(Form("MTR_%s_MT%d_RPC%d_HV.actual.iMon",sides[side].Data(),planes[plane],RPC)));
                    if(!dataArray) {
                        printf(" Problems getting dataArray\n");
                        break;
                    }
                    for (Int_t arrayIndex=0; arrayIndex<(dataArray->GetEntries()); arrayIndex++) {
                        AliDCSValue *value = (AliDCSValue*)dataArray->At(arrayIndex);
                        if (isCalib) {
                            dataList[side][plane][RPC-1].Add(new AliRPCDarkCurrent(*runIterator,plane,value->GetTimeStamp(),value->GetFloat(),value->GetFloat(),isCalib));
                            //printf("CA %f\n",value->GetFloat());
                        } else {
                            dataList[side][plane][RPC-1].Add(new AliRPCDarkCurrent(*runIterator,plane,value->GetTimeStamp(),value->GetFloat(),0.,isCalib));
                            //printf("PH %f\n",value->GetFloat());
                        }
                        
                        //((AliRPCDarkCurrent*)dataList[side][plane][RPC-1].Last())->AliRPCDarkCurrent::PrintData();
                        delete value;
                    }
                    //dataList[side][plane][RPC-1].Sort();
                }
            }
        }
        
        AliCDBEntry *entryVoltage = managerCurrent->Get("MUON/Calib/TriggerDCS");
        if(!entryVoltage) {
            printf("\nproblem with entryVoltage\n");
            break;
        }
        
        TMap *mapVoltage = (TMap*)entryVoltage->GetObject();
        if(!mapVoltage) {
            printf("Problems getting map\n");
            break;
        }
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t side=0; side<nSides; side++) {
                for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                    TObjArray *dataArray;
                    dataArray=(TObjArray*)(mapVoltage->GetValue(Form("MTR_%s_MT%d_RPC%d_HV.actual.iMon",sides[side].Data(),planes[plane],RPC)));
                    if(!dataArray) {
                        printf(" Problems getting dataArray\n");
                        break;
                    }
                    for (Int_t arrayIndex=0; arrayIndex<(dataArray->GetEntries()); arrayIndex++) {
                        AliDCSValue *value = (AliDCSValue*)dataArray->At(arrayIndex);
                        if (isCalib) {
                            
                        } else {
                            
                        }
                        delete value;
                    }
                }
            }
        }

    }
    
    printf("\n\n\nData retrieving complete\n\n\n");
    
    for (Int_t plane=0; plane<nPlanes; plane++) {
        for (Int_t side=0; side<nSides; side++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                //dataList[side][plane][RPC-1].Sort();
                for(Int_t iList=0; iList<(dataList[side][plane][RPC-1]).GetEntries(); iList++){
                    Double_t iDarkCalib=0.;
                    
                    if (((AliRPCDarkCurrent*)dataList[side][plane][RPC-1].At(iList))->GetIsCalib()==kTRUE) {
                        //printf("CA %d. RPC %d MT %d DarkCurrent=%f\n",iList,side*9+RPC,planes[plane],((AliRPCDarkCurrent*)dataList[side][plane][RPC-1].At(iList))->GetIDark());
                        iDarkCalib=((AliRPCDarkCurrent*)dataList[side][plane][RPC-1].At(iList))->GetITot();
                        //printf("%d. RPC %d MT %d DarkCurrent=%f\n",iList,side*9+RPC,planes[plane],iDarkCalib);
                        for (Int_t iList2=iList+1; iList2<(dataList[side][plane][RPC-1]).GetEntries(); iList2++) {
                            if (((AliRPCDarkCurrent*)dataList[side][plane][RPC-1].At(iList2))->GetIsCalib()) {
                                iList=iList2-1;
                                break;
                            } else {
                                ((AliRPCDarkCurrent*)dataList[side][plane][RPC-1].At(iList2))->SetIDark(iDarkCalib);
                                //printf("PH %d. RPC %d MT %d DarkCurrent=%f\n",iList2,side*9+RPC,planes[plane],((AliRPCDarkCurrent*)dataList[side][plane][RPC-1].At(iList2))->GetIDark());
                            }
                        }
                    } else {
                        //printf("PH %d. RPC %d MT %d DarkCurrent=%f\n",iList,side*9+RPC,planes[plane],((AliRPCDarkCurrent*)dataList[side][plane][RPC-1].At(iList))->GetIDark());
                    }
                }
            }
        }
    }
    
    printf("\n\n\nDark currents setting complete\n\n\n");

    
    TGraph *graphTot[nSides][nPlanes][nRPC];
    TGraph *graphDark[nSides][nPlanes][nRPC];
    TCanvas *canv[2*nSides];
    TLegend *leg[nSides];
    Int_t color[nRPC] = {kBlack,kRed,kGreen,kBlue,kYellow,kMagenta,kCyan,kGray,kOrange};
    
    for (Int_t side=0; side<nSides; side++) {
        canv[side]=new TCanvas(Form("canv_%s_iTot",sides[side].Data()),Form("canv_%s_iTot",sides[side].Data()));
        canv[side+2]=new TCanvas(Form("canv_%s_iDark",sides[side].Data()),Form("canv_%s_iDark",sides[side].Data()));
        canv[side]->Divide(nPlanes+1);
        canv[side+2]->Divide(nPlanes+1);
        leg[side]=new TLegend(0.2,0.2,0.8,0.8);
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                graphTot[side][plane][RPC-1]=new TGraph();
                graphTot[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                graphTot[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                graphTot[side][plane][RPC-1]->SetMarkerStyle(20);
                graphTot[side][plane][RPC-1]->SetMarkerSize(0.20);
                graphTot[side][plane][RPC-1]->SetTitle(Form("iTot_MT%d_%s",planes[plane],sides[side].Data()));
                graphDark[side][plane][RPC-1]=new TGraph();
                graphDark[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                graphDark[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                graphDark[side][plane][RPC-1]->SetMarkerStyle(20);
                graphDark[side][plane][RPC-1]->SetMarkerSize(0.20);
                graphDark[side][plane][RPC-1]->SetTitle(Form("iDark_MT%d_%s",planes[plane],sides[side].Data()));
                Int_t dataCount=0;
                for(Int_t iList=0; iList<(dataList[side][plane][RPC-1]).GetEntries(); iList++){
                    AliRPCDarkCurrent *currentBuffer = ((AliRPCDarkCurrent*)dataList[side][plane][RPC-1].At(iList));
                    
                    if (currentBuffer->GetIsCalib()==kFALSE) {
                        printf("RPC %d MT %d isCalib %s TotCurrent= %f DarkCurrent=%f\n",side*9+RPC,planes[plane],"false\0true"+6*(currentBuffer->GetIsCalib()),currentBuffer->GetITot(),currentBuffer->GetIDark());
                        graphTot[side][plane][RPC-1]->SetPoint(dataCount, (Double_t)currentBuffer->GetTimeStamp(), (Double_t)currentBuffer->GetITot());
                        graphDark[side][plane][RPC-1]->SetPoint(dataCount, (Double_t)currentBuffer->GetTimeStamp(), (Double_t)currentBuffer->GetIDark());
                        dataCount++;
                    }
                    currentBuffer=0x0;
                }
                
                if (RPC==1) {
                    canv[side]->cd(plane+1);
                    graphTot[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,45.);
                    graphTot[side][plane][RPC-1]->Draw("alp");
                } else {
                    canv[side]->cd(plane+1);
                    graphTot[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,45.);
                    graphTot[side][plane][RPC-1]->Draw("lp");
                }
                
                if (RPC==1) {
                    canv[2+side]->cd(plane+1);
                    graphDark[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,10.);
                    graphDark[side][plane][RPC-1]->Draw("alp");
                } else {
                    canv[2+side]->cd(plane+1);
                    graphDark[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,10.);
                    graphDark[side][plane][RPC-1]->Draw("lp");
                }
                if (plane==0) {
                    leg[side]->AddEntry(graphTot[side][plane][RPC-1],Form("RPC %d %s",RPC,sides[side].Data()),"lp");
                }
            }
        }
        canv[side]->cd(nPlanes+1);
        leg[side]->SetTextSize(0.05);
        leg[side]->Draw();
        canv[side+2]->cd(nPlanes+1);
        leg[side]->SetTextSize(0.05);
        leg[side]->Draw();
    }
    
//    TCanvas *canvMT11 = new TCanvas("canvMT11","canvMT11");
    
}