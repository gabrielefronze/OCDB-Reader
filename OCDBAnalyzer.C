//
//  OCDBAnalyzer.cpp
//  Aliroot project
//
//  Created by Gabriele Gaetano Fronzé on 14/12/15.
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
#include "AliRPCValueDCS.h"
#include "AliRPCValueCurrent.h"
#include "AliRPCValueVoltage.h"
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

void OCDBAnalyzer(TString inputFileName){
    
    TFile *inputFile=new TFile(inputFileName.Data());
    
    //anno da analizzare
    Int_t year=2015;
    
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
    
    TGraph *graphTot[nSides][nPlanes][nRPC];
    TGraph *graphDarkPhys[nSides][nPlanes][nRPC];
    TGraph *graphDarkCalib[nSides][nPlanes][nRPC];
    TGraph *graphNet[nSides][nPlanes][nRPC];
    TGraph *graphV[nSides][nPlanes][nRPC];
    TGraph *graphCharge[nSides][nPlanes][nRPC];
    TCanvas *canv[6*nSides+1];
    TLegend *leg[nSides];

    Int_t color[nRPC] = {kBlack,kRed,kGreen,kBlue,kYellow,kMagenta,kCyan,kGray,kOrange};

    for (Int_t side=0; side<nSides; side++) {
        canv[side]=new TCanvas(Form("canv_%s_iTot",sides[side].Data()),Form("canv_%s_iTot",sides[side].Data()));
        canv[side]->Divide(2,2);
        
        canv[side+2]=new TCanvas(Form("canv_%s_iDark",sides[side].Data()),Form("canv_%s_iDark",sides[side].Data()));
        canv[side+2]->Divide(2,2);
        
        canv[side+3]=new TCanvas(Form("canv_%s_HV",sides[side].Data()),Form("canv_%s_HV",sides[side].Data()));
        canv[side+3]->Divide(2,2);
        
        canv[side+4]=new TCanvas(Form("canv_%s_iNet",sides[side].Data()),Form("canv_%s_iNet",sides[side].Data()));
        canv[side+4]->Divide(2,2);
        
        canv[side+5]=new TCanvas(Form("canv_%s_Charge",sides[side].Data()),Form("canv_%s_Charge",sides[side].Data()));
        canv[side+5]->Divide(2,2);
        
        leg[side]=new TLegend(0.2,0.2,0.8,0.8);
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                
                graphTot[side][plane][RPC-1]=new TGraph();
                graphTot[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                graphTot[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                graphTot[side][plane][RPC-1]->SetMarkerStyle(20);
                graphTot[side][plane][RPC-1]->SetMarkerSize(0.20);
                graphTot[side][plane][RPC-1]->SetTitle(Form("iTot_MT%d_%s",planes[plane],sides[side].Data()));
                
                graphDarkPhys[side][plane][RPC-1]=new TGraph();
                graphDarkPhys[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                graphDarkPhys[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                graphDarkPhys[side][plane][RPC-1]->SetMarkerStyle(20);
                graphDarkPhys[side][plane][RPC-1]->SetMarkerSize(0.20);
                graphDarkPhys[side][plane][RPC-1]->SetTitle(Form("iDark_MT%d_%s_Phys",planes[plane],sides[side].Data()));

                graphDarkCalib[side][plane][RPC-1]=new TGraph();
                graphDarkCalib[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                graphDarkCalib[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                graphDarkCalib[side][plane][RPC-1]->SetMarkerStyle(20);
                graphDarkCalib[side][plane][RPC-1]->SetMarkerSize(0.20);
                graphDarkCalib[side][plane][RPC-1]->SetTitle(Form("iDark_MT%d_%s_Calib",planes[plane],sides[side].Data()));
                
                graphNet[side][plane][RPC-1]=new TGraph();
                graphNet[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                graphNet[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                graphNet[side][plane][RPC-1]->SetMarkerStyle(20);
                graphNet[side][plane][RPC-1]->SetMarkerSize(0.20);
                graphNet[side][plane][RPC-1]->SetTitle(Form("iNet_MT%d_%s",planes[plane],sides[side].Data()));
                
                graphV[side][plane][RPC-1]=new TGraph();
                graphV[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                graphV[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                graphV[side][plane][RPC-1]->SetMarkerStyle(20);
                graphV[side][plane][RPC-1]->SetMarkerSize(0.20);
                graphV[side][plane][RPC-1]->SetTitle(Form("HV_MT%d_%s",planes[plane],sides[side].Data()));
                
                graphCharge[side][plane][RPC-1]=new TGraph();
                graphCharge[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                graphCharge[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                graphCharge[side][plane][RPC-1]->SetMarkerStyle(20);
                graphCharge[side][plane][RPC-1]->SetMarkerSize(0.20);
                graphCharge[side][plane][RPC-1]->SetTitle(Form("Charge_MT%d_%s",planes[plane],sides[side].Data()));
                
                //contatore incrementale per inserire i punti nei grafici
                Int_t dataCountCurrent=0;
                Int_t dataCountCurrent2=0;
                Int_t dataCountVoltage=0;
                //flag per escludere l'inserimento di misure di corrente eseguite non a tensione di lavoro
                Bool_t drawFlag=kFALSE;
                
                TSortedList *sortedList;
                inputFile->GetObject(Form("Data_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC),sortedList);
                sortedList->Sort();
                
                //loop sulle entries
                for(Int_t iList=0; iList<sortedList->GetEntries(); iList++){
                    //generica entry della sorted list
                    AliRPCValueDCS *valueDCS3 = ((AliRPCValueDCS*)sortedList->At(iList));
                    
                    //se è una tensione
                    if (valueDCS3->IsVoltage()){
                        //cast a tensione
                        AliRPCValueVoltage *voltageBuffer=(AliRPCValueVoltage*)valueDCS3;
                        //se supera la soglia posso disegnare
                        if (voltageBuffer->GetVSupp()>=8500.) {
                            //printf("VOLTAGE: \tRPC %d MT %d isCalib %s HV= %f\n",side*9+RPC,planes[plane],"false\0true"+7*(voltageBuffer->GetIsCalib()),voltageBuffer->GetVSupp());
                            drawFlag=kTRUE;
                            graphV[side][plane][RPC-1]->SetPoint(dataCountVoltage, (Double_t)voltageBuffer->GetTimeStamp(), (Double_t)voltageBuffer->GetVSupp());
                            dataCountVoltage++;
                            //altrimenti non disegno fino a quando non trovo una misura di tensione di lavoro
                        } else {
                            //printf("VOLTAGE: \tRPC %d MT %d isCalib %s HV= %f\n",side*9+RPC,planes[plane],"false\0true"+7*(voltageBuffer->GetIsCalib()),voltageBuffer->GetVSupp());
                            drawFlag=kFALSE;
                        }
                        //se è una corrente e posso disegnare
                    } else if(valueDCS3->IsCurrent() && drawFlag==kTRUE){
                        //cast a corrente
                        AliRPCValueCurrent *currentBuffer=(AliRPCValueCurrent*)valueDCS3;
                        //disegno solo i punti relativi a run di fisica
                        if (currentBuffer->GetIsCalib()==kFALSE) {
                            //printf("Drawing: \tRPC %d MT %d isCalib %s TotCurrent= %f DarkCurrent=%f\n",side*9+RPC,planes[plane],"false\0true"+7*(currentBuffer->GetIsCalib()),currentBuffer->GetITot(),currentBuffer->GetIDark());
                            Double_t darkCurrent=(Double_t)currentBuffer->GetIDark();
                            Double_t totalCurrent=(Double_t)currentBuffer->GetITot();
                            Double_t netCurrent=totalCurrent-darkCurrent;
                            ULong64_t timeStamp=currentBuffer->GetTimeStamp();
                            Int_t runNumber=currentBuffer->GetRunNumber();
                            
                            graphTot[side][plane][RPC-1]->SetPoint(dataCountCurrent, timeStamp, totalCurrent);
                            graphDarkPhys[side][plane][RPC-1]->SetPoint(dataCountCurrent, timeStamp, darkCurrent);
                            graphNet[side][plane][RPC-1]->SetPoint(dataCountCurrent, timeStamp, netCurrent);
                            dataCountCurrent++;
                            //scarto i run di calibrazione
                        } else {
                            graphDarkCalib[side][plane][RPC-1]->SetPoint(dataCountCurrent2, currentBuffer->GetTimeStamp(), currentBuffer->GetIDark());
                            dataCountCurrent2++;                            
                        }
                        //se è una corrente ma non posso disegnare
                    } else if(valueDCS3->IsCurrent() && drawFlag==kFALSE){
                        //cast a corrente
                        AliRPCValueCurrent *currentBuffer=(AliRPCValueCurrent*)valueDCS3;
                        //printf("Skipping: \tRPC %d MT %d isCalib %s TotCurrent= %f DarkCurrent=%f\n",side*9+RPC,planes[plane],"false\0true"+7*(currentBuffer->GetIsCalib()),currentBuffer->GetITot(),currentBuffer->GetIDark());
                    } else continue;
                }
                
                Double_t netCurrentStart=0.;
                Double_t netCurrentStop=0.;
                Double_t timeStampStart=0;
                Double_t timeStampStop=0;
                Double_t integratedCharge=0.;
                Int_t dataCount3=0;
                
                for (Int_t iNetGraph=0; iNetGraph<dataCountCurrent-1; iNetGraph++) {
                    graphNet[side][plane][RPC-1]->GetPoint(iNetGraph, timeStampStart, netCurrentStart);
                    graphNet[side][plane][RPC-1]->GetPoint(iNetGraph+1, timeStampStop, netCurrentStop);
                    integratedCharge+=(netCurrentStart+netCurrentStop)*(timeStampStop-timeStampStart)/2000.;
                    graphCharge[side][plane][RPC-1]->SetPoint(iNetGraph, timeStampStop, integratedCharge);
                    //cout<<integratedCharge<<endl;
                }
                
                if (RPC==1) {
                    canv[side]->cd(plane+1);
                    graphTot[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,40.);
                    graphTot[side][plane][RPC-1]->Draw("alp");
                } else {
                    canv[side]->cd(plane+1);
                    graphTot[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,40.);
                    graphTot[side][plane][RPC-1]->Draw("lp");
                }
                
                if (RPC==1) {
                    canv[2+side]->cd(plane+1);
                    graphDarkCalib[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,20.);
                    graphDarkCalib[side][plane][RPC-1]->Draw("alp");
                } else {
                    canv[2+side]->cd(plane+1);
                    graphDarkCalib[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,20.);
                    graphDarkCalib[side][plane][RPC-1]->Draw("lp");
                }
                
                if (RPC==1) {
                    canv[3+side]->cd(plane+1);
                    graphV[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,11000.);
                    graphV[side][plane][RPC-1]->Draw("alp");
                } else {
                    canv[3+side]->cd(plane+1);
                    graphV[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,11000.);
                    graphV[side][plane][RPC-1]->Draw("lp");
                }
                
                if (RPC==1) {
                    canv[4+side]->cd(plane+1);
                    graphNet[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,40.);
                    graphNet[side][plane][RPC-1]->Draw("alp");
                } else {
                    canv[4+side]->cd(plane+1);
                    graphNet[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,40.);
                    graphNet[side][plane][RPC-1]->Draw("lp");
                }
                
                if (RPC==1) {
                    canv[5+side]->cd(plane+1);
                    graphCharge[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,10000.);
                    graphCharge[side][plane][RPC-1]->Draw("alp");
                } else {
                    canv[5+side]->cd(plane+1);
                    graphCharge[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,10000.);
                    graphCharge[side][plane][RPC-1]->Draw("lp");
                }
                
                if (plane==0) {
                    leg[side]->AddEntry(graphTot[side][plane][RPC-1],Form("RPC %d",RPC),"lp");
                }
                sortedList=0x0;
            }
        }
    }
    
    canv[2*nSides]=new TCanvas("RPC colors Legend","RPC colors Legend");
    canv[2*nSides]->cd();
    leg[0]->SetTextSize(0.05);
    leg[0]->Draw();
    
    
    TFile *output=new TFile(Form("PbPb_%d_currents.root",year),"RECREATE");
    for (Int_t iCanv=0; iCanv<nSides*2; iCanv++) {
        output->cd();
        canv[iCanv]->Write();
    }
    output->cd();
    canv[2*nSides]->Write();
    
    output->Close();
}