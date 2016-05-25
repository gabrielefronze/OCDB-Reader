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
#include "TFile.h"
//#include "AliCDBManager.h"
//#include "AliCDBEntry.h"
//#include "AliDCSValue.h"
#include "AliRPCValueDCS.h"
#include "AliRPCValueCurrent.h"
#include "AliRPCValueVoltage.h"
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

void OCDBReader2(Bool_t file=kFALSE, TString filename=""){
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
            printf("Problems getting mapCurrent\n");
            break;
        }
        
        AliCDBEntry *entryVoltage = managerVoltage->Get("MUON/Calib/TriggerDCS");
        if(!entryVoltage) {
            printf("\nproblem with entryCurrent\n");
            break;
        }
        
        TMap *mapVoltage = (TMap*)entryVoltage->GetObject();
        if(!mapVoltage) {
            printf("Problems getting mapVoltage\n");
            break;
        }
        
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t side=0; side<nSides; side++) {
                for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                    TObjArray *dataArrayCurrents;
                    //printf(Form("MTR_%s_MT%d_RPC%d_HV.actual.iMon\n",sides[side].Data(),planes[plane],RPC));
                    dataArrayCurrents=(TObjArray*)(mapCurrent->GetValue(Form("MTR_%s_MT%d_RPC%d_HV.actual.iMon",sides[side].Data(),planes[plane],RPC)));
                    if(!dataArrayCurrents) {
                        printf(" Problems getting dataArrayCurrents\n");
                        break;
                    }
                    for (Int_t arrayIndex=0; arrayIndex<(dataArrayCurrents->GetEntries()); arrayIndex++) {
                        AliDCSValue *value = (AliDCSValue*)dataArrayCurrents->At(arrayIndex);
                        if (isCalib) {
                            dataList[side][plane][RPC-1].Add(new AliRPCValueCurrent(*runIterator,value->GetTimeStamp(),value->GetFloat(),value->GetFloat(),isCalib));
                            //printf("CA %f\n",value->GetFloat());
                        } else {
                            dataList[side][plane][RPC-1].Add(new AliRPCValueCurrent(*runIterator,value->GetTimeStamp(),value->GetFloat(),0.,isCalib));
                            //printf("PH %f\n",value->GetFloat());
                        }
                        
                        //((AliRPCValueCurrent*)dataList[side][plane][RPC-1].Last())->AliRPCValueCurrent::PrintData();
                        delete value;
                    }
                    
                    TObjArray *dataArrayVoltage;
                    //printf(Form("MTR_%s_MT%d_RPC%d_HV.actual.iMon\n",sides[side].Data(),planes[plane],RPC));
                    dataArrayVoltage=(TObjArray*)(mapCurrent->GetValue(Form("MTR_%s_MT%d_RPC%d_HV.vEff",sides[side].Data(),planes[plane],RPC)));
                    if(!dataArrayVoltage) {
                        printf(" Problems getting dataArrayVoltage\n");
                        break;
                    }
                    for (Int_t arrayIndex=0; arrayIndex<(dataArrayVoltage->GetEntries()); arrayIndex++) {
                        AliDCSValue *value = (AliDCSValue*)dataArrayVoltage->At(arrayIndex);
                        if (isCalib) {
                            dataList[side][plane][RPC-1].Add(new AliRPCValueVoltage(*runIterator,value->GetTimeStamp(),value->GetFloat(),isCalib));
                            printf("CA %f\n",value->GetFloat());
                        } else {
                            dataList[side][plane][RPC-1].Add(new AliRPCValueVoltage(*runIterator,value->GetTimeStamp(),value->GetFloat(),isCalib));
                            printf("PH %f\n",value->GetFloat());
                        }
                        
                        //((AliRPCValueCurrent*)dataList[side][plane][RPC-1].Last())->AliRPCValueCurrent::PrintData();
                        delete value;
                    }
                    //dataList[side][plane][RPC-1].Sort();
                }
            }
        }
    }
    
    printf("\n\n\nData retrieving complete\n\n\n");
    
    for (Int_t plane=0; plane<nPlanes; plane++) {
        for (Int_t side=0; side<nSides; side++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                TSortedList *sortedList=&(dataList[side][plane][RPC-1]);
                for(Int_t iList=0; iList<sortedList->GetEntries(); iList++){
                    Double_t iDarkCalib=0.;
                    AliRPCValueDCS *valueDCS=(AliRPCValueDCS*)(sortedList->At(iList));
                    if (valueDCS->IsCurrent()) {
                        //cout<<"current";
                        AliRPCValueCurrent* valueCurrent=(AliRPCValueCurrent*)valueDCS;
                        //printf("RPC %d MT %d isCalib %s TotCurrent= %f DarkCurrent=%f\n",side*9+RPC,planes[plane],"false\0true"+7*(valueCurrent->GetIsCalib()),valueCurrent->GetITot(),valueCurrent->GetIDark());
                        if (valueCurrent->GetIsCalib()==kTRUE) {
                            //cout<<" calib"<<endl;
                            iDarkCalib=valueCurrent->GetITot();
                            for (Int_t iList2=iList+1; iList2<sortedList->GetEntries(); iList2++) {
                                AliRPCValueDCS* valueDCS2=(AliRPCValueDCS*)(sortedList->At(iList2));
                                if (valueDCS2->IsCurrent()){
                                    AliRPCValueCurrent* valueCurrent2=(AliRPCValueCurrent*)valueDCS2;
                                    if (valueCurrent2->GetIsCalib()==kTRUE) {
                                        //cout<<"new calib"<<endl;
                                        //printf("RPC %d MT %d isCalib %s TotCurrent= %f DarkCurrent=%f\n",side*9+RPC,planes[plane],"false\0true"+7*(valueCurrent2->GetIsCalib()),valueCurrent2->GetITot(),valueCurrent2->GetIDark());
                                        iList=iList2-1;
                                        valueCurrent2=0x0;
                                        break;
                                    } else {
                                        //cout<<"setting dark current"<<endl;
                                        valueCurrent2->SetIDark(iDarkCalib);
                                        //printf("RPC %d MT %d isCalib %s TotCurrent= %f DarkCurrent=%f\n",side*9+RPC,planes[plane],"false\0true"+7*(valueCurrent2->GetIsCalib()),valueCurrent2->GetITot(),valueCurrent2->GetIDark());
                                        valueCurrent2=0x0;
                                    }
                                }
                            }
                        } //else cout<<" phys"<<endl;
                    }
//                    } else if (valueDCS->IsVoltage()){
//                        cout<<"voltage"<<endl;
//                        AliRPCValueVoltage* valueVoltage=(AliRPCValueVoltage*)valueDCS;
//                        if (valueVoltage->GetVSupp()<=8500.) {
//                            for (Int_t iList2=iList+1; iList2<sortedList->GetEntries(); iList2++) {
//                                AliRPCValueDCS *valueDCS2=(AliRPCValueDCS*)(sortedList->At(iList2));
//                                if (valueDCS2->IsVoltage()) {
//                                    iList=iList2-1;
//                                    break;
//                                } else {
//                                    valueCurrent->SetIDark(iDarkCalib);
//                                }
//                            }
//                        }
//                    }
                    valueDCS=0x0;
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
                    AliRPCValueDCS *valueDCS3 = ((AliRPCValueDCS*)dataList[side][plane][RPC-1].At(iList));

                    if (valueDCS3->IsVoltage()){
                        AliRPCValueVoltage *voltageBuffer=(AliRPCValueVoltage*)valueDCS3;
                        if (voltageBuffer->GetVSupp()<8500.) {
                            for(Int_t iList2=iList+1; iList2<(dataList[side][plane][RPC-1]).GetEntries(); iList2++){
                                AliRPCValueDCS *valueDCS4 = ((AliRPCValueDCS*)dataList[side][plane][RPC-1].At(iList2));
                                if (valueDCS4->IsCurrent()) {
                                    AliRPCValueCurrent *currentBuffer2=(AliRPCValueCurrent*)valueDCS4;
                                    if (currentBuffer2->GetIsCalib()==kFALSE) {
                                        //printf("RPC %d MT %d isCalib %s TotCurrent= %f DarkCurrent=%f\n",side*9+RPC,planes[plane],"false\0true"+7*(currentBuffer->GetIsCalib()),currentBuffer->GetITot(),currentBuffer->GetIDark());
                                        graphTot[side][plane][RPC-1]->SetPoint(dataCount, (Double_t)currentBuffer2->GetTimeStamp(), (Double_t)currentBuffer2->GetITot());
                                        graphDark[side][plane][RPC-1]->SetPoint(dataCount, (Double_t)currentBuffer2->GetTimeStamp(), (Double_t)currentBuffer2->GetIDark());
                                        dataCount++;
                                    }
                                } else if (valueDCS4->IsVoltage()) {
                                    iList=iList2-1;
                                    continue;
                                } else {
                                    continue;
                                }
                            }
                        } else {
                            cout<<"skipping because Vsupp too low"<<endl;
                            continue;
                        }
                        
                    } else {
                        continue;
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
                    graphDark[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,20.);
                    graphDark[side][plane][RPC-1]->Draw("alp");
                } else {
                    canv[2+side]->cd(plane+1);
                    graphDark[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,20.);
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
    
    TFile *output=new TFile(Form("PbPb_%d_currents.root",year),"RECREATE");
    for (Int_t iCanv=0; iCanv<nSides*2; iCanv++) {
        canv[iCanv]->SetName(Form("%d",iCanv));
        output->cd();
        canv[iCanv]->Write();
    }
    
    output->Close();
//    TCanvas *canvMT11 = new TCanvas("canvMT11","canvMT11");
    
}