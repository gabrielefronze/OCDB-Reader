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
#include "AliRPCValueScaler.h"
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

void OCDBAnalyzerWithRates72Canvases(TString inputFileName){
    
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

    const Int_t nLocalBoards=234;

    const Int_t nCathodes=2;
    TString cathodes[nCathodes];
    cathodes[0]="BENDING";
    cathodes[1]="NOT_BENDING";
    
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

    TGraph *graphTot[nSides][nPlanes][nRPC];
    TGraph *graphDarkPhys[nSides][nPlanes][nRPC];
    TGraph *graphDarkCalib[nSides][nPlanes][nRPC];
    TGraph *graphNet[nSides][nPlanes][nRPC];
    TGraph *graphV[nSides][nPlanes][nRPC];
    TGraph *graphCharge[nSides][nPlanes][nRPC];
    TCanvas *canviTot[nSides];
    TCanvas *canviDark[nSides];
    TCanvas *canvHV[nSides];
    TCanvas *canviNet[nSides];
    TCanvas *canvCharge[nSides];
    TCanvas *canvLegend;
    TLegend *leg[nSides];

    Int_t color[nRPC] = {kBlack,kRed,kGreen,kBlue,kYellow,kMagenta,kCyan,kGray,kOrange};

    for (Int_t side=0; side<nSides; side++) {
        cout<<side<<endl;
        canviTot[side]=new TCanvas(Form("canv_%s_iTot",sides[side].Data()),Form("canv_%s_iTot",sides[side].Data()));
        canviTot[side]->Divide(2,2);
        
        canviDark[side]=new TCanvas(Form("canv_%s_iDark",sides[side].Data()),Form("canv_%s_iDark",sides[side].Data()));
        canviDark[side]->Divide(2,2);
        
        canvHV[side]=new TCanvas(Form("canv_%s_HV",sides[side].Data()),Form("canv_%s_HV",sides[side].Data()));
        canvHV[side]->Divide(2,2);
        
        canviNet[side]=new TCanvas(Form("canv_%s_iNet",sides[side].Data()),Form("canv_%s_iNet",sides[side].Data()));
        canviNet[side]->Divide(2,2);
        
        canvCharge[side]=new TCanvas(Form("canv_%s_Charge",sides[side].Data()),Form("canv_%s_Charge",sides[side].Data()));
        canvCharge[side]->Divide(2,2);
        
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
                    canviTot[side]->cd(plane+1);
                    graphTot[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,40.);
                    graphTot[side][plane][RPC-1]->Draw("ap");
                } else {
                    canviTot[side]->cd(plane+1);
                    graphTot[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,40.);
                    graphTot[side][plane][RPC-1]->Draw("p");
                }
                
                if (RPC==1) {
                    canviDark[side]->cd(plane+1);
                    graphDarkCalib[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,20.);
                    graphDarkCalib[side][plane][RPC-1]->Draw("ap");
                    graphDarkCalib[side][plane][RPC-1]->GetXaxis()->SetTitle("Epoch timestamp [s]");
                    graphDarkCalib[side][plane][RPC-1]->GetYaxis()->SetTitle("i_{Dark} [uA]");
                } else {
                    canviDark[side]->cd(plane+1);
                    graphDarkCalib[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,20.);
                    graphDarkCalib[side][plane][RPC-1]->Draw("p");
                    graphDarkCalib[side][plane][RPC-1]->GetXaxis()->SetTitle("Epoch timestamp [s]");
                    graphDarkCalib[side][plane][RPC-1]->GetYaxis()->SetTitle("i_{Dark} [uA]");
                }
                
                if (RPC==1) {
                    canvHV[side]->cd(plane+1);
                    graphV[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,11000.);
                    graphV[side][plane][RPC-1]->Draw("ap");
                } else {
                    canvHV[side]->cd(plane+1);
                    graphV[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,11000.);
                    graphV[side][plane][RPC-1]->Draw("p");
                }
                
                if (RPC==1) {
                    canviNet[side]->cd(plane+1);
                    graphNet[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,40.);
                    graphNet[side][plane][RPC-1]->Draw("ap");
                } else {
                    canviNet[side]->cd(plane+1);
                    graphNet[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,40.);
                    graphNet[side][plane][RPC-1]->Draw("p");
                }
                
                if (RPC==1) {
                    canvCharge[side]->cd(plane+1);
                    graphCharge[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,10.);
                    graphCharge[side][plane][RPC-1]->Draw("ap");
                } else {
                    canvCharge[side]->cd(plane+1);
                    graphCharge[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,10.);
                    graphCharge[side][plane][RPC-1]->Draw("p");
                }
                
                if (plane==0) {
                    leg[side]->AddEntry(graphTot[side][plane][RPC-1],Form("RPC %d",RPC),"p");
                }
                sortedList=0x0;
            }
        }
    }

    canvLegend=new TCanvas("RPC colors Legend","RPC colors Legend");
    canvLegend->cd();
    leg[0]->SetTextSize(0.05);
    leg[0]->Draw();
    
    inputFileName.ReplaceAll(".root", "");
    TFile *output=new TFile(Form("Analysis_results_%s.root",inputFileName.Data()),"RECREATE");
    for (Int_t iCanv=0; iCanv<nSides; iCanv++) {
        output->cd();
        canviTot[iCanv]->Write();   
        canviDark[iCanv]->Write();       
        canviNet[iCanv]->Write();        
        canvHV[iCanv]->Write();     
        canvCharge[iCanv]->Write();
    }
    
    output->cd();
    //canvas della legenda
    canvLegend->Write();
    
    TGraph *graphScalers[nCathodes][nSides][nPlanes][nRPC];
    TCanvas *canvScalers[nSides*2];
    canvScalers[0]=new TCanvas(Form("canv_%s_scalers_bend",sides[0].Data()),Form("canv_%s_scalers_bend",sides[0].Data()));
    canvScalers[1]=new TCanvas(Form("canv_%s_scalers_not_bend",sides[0].Data()),Form("canv_%s_scalers_not_bend",sides[0].Data()));
    canvScalers[2]=new TCanvas(Form("canv_%s_scalers_bend",sides[1].Data()),Form("canv_%s_scalers_bend",sides[1].Data()));
    canvScalers[3]=new TCanvas(Form("canv_%s_scalers_not_bend",sides[1].Data()),Form("canv_%s_scalers_not_bend",sides[1].Data()));
    canvScalers[0]->Divide(2,2);    
    canvScalers[1]->Divide(2,2);
    canvScalers[2]->Divide(2,2);
    canvScalers[3]->Divide(2,2);

    TGraph *graphRatesPerCm2[nCathodes][nSides][nPlanes][nRPC];
    TCanvas *canvRatesPerCm2[nSides*2];
    canvRatesPerCm2[0]=new TCanvas(Form("canv_%s_rates/cm^2_bend",sides[0].Data()),Form("canv_%s_rates/cm^2_bend",sides[0].Data()));
    canvRatesPerCm2[1]=new TCanvas(Form("canv_%s_rates/cm^2_not_bend",sides[0].Data()),Form("canv_%s_rates/cm^2_not_bend",sides[0].Data()));
    canvRatesPerCm2[2]=new TCanvas(Form("canv_%s_rates/cm^2_bend",sides[1].Data()),Form("canv_%s_rates/cm^2_bend",sides[1].Data()));
    canvRatesPerCm2[3]=new TCanvas(Form("canv_%s_rates/cm^2_not_bend",sides[1].Data()),Form("canv_%s_rates/cm^2_not_bend",sides[1].Data()));
    canvRatesPerCm2[0]->Divide(2,2);    
    canvRatesPerCm2[1]->Divide(2,2);
    canvRatesPerCm2[2]->Divide(2,2);
    canvRatesPerCm2[3]->Divide(2,2);

    TGraph *graphChargePerHit[nCathodes][nSides][nPlanes][nRPC];
    TCanvas *canvChargePerHit[nSides*2];
    canvChargePerHit[0]=new TCanvas(Form("canv_%s_charge_per_hit_bend",sides[0].Data()),Form("canv_%s_charge_per_hit_bend",sides[0].Data()));
    canvChargePerHit[1]=new TCanvas(Form("canv_%s_charge_per_hit_not_bend",sides[0].Data()),Form("canv_%s_charge_per_hit_not_bend",sides[0].Data()));
    canvChargePerHit[2]=new TCanvas(Form("canv_%s_charge_per_hit_bend",sides[1].Data()),Form("canv_%s_charge_per_hit_bend",sides[1].Data()));
    canvChargePerHit[3]=new TCanvas(Form("canv_%s_charge_per_hit_not_bend",sides[1].Data()),Form("canv_%s_charge_per_hit_not_bend",sides[1].Data()));
    canvChargePerHit[0]->Divide(2,2);    
    canvChargePerHit[1]->Divide(2,2);
    canvChargePerHit[2]->Divide(2,2);
    canvChargePerHit[3]->Divide(2,2);

    TGraph *graphRateCurrentCorrelation[nSides][nPlanes][nRPC];
    TCanvas *canvRateCurrentCorrelation[nSides][nPlanes][nRPC];

    TGraph *graphMeanRPCIDark[nSides][nPlanes][nRPC];
    TGraph *graphMeanRPCRate[nSides][nPlanes][nRPC];

    for(Int_t cathode=0;cathode<nCathodes;cathode++){
        for (Int_t side=0; side<nSides; side++) {
            for (Int_t plane=0; plane<nPlanes; plane++) {
                for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                    if(cathode==1){
                        canvRateCurrentCorrelation[side][plane][RPC-1]=new TCanvas(Form("canv_MT%d_%s_RPC_%d_rate_current_correlation",planes[plane],sides[side].Data(),RPC),Form("canv_MT_%d_%s_RPC_%d_rate_current_correlation",planes[plane],sides[side].Data(),RPC));
                        canvRateCurrentCorrelation[side][plane][RPC-1]->Divide(2);
                        graphRateCurrentCorrelation[side][plane][RPC-1]=new TGraph();
                        graphRateCurrentCorrelation[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                        graphRateCurrentCorrelation[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                        graphRateCurrentCorrelation[side][plane][RPC-1]->SetMarkerStyle(20);
                        graphRateCurrentCorrelation[side][plane][RPC-1]->SetMarkerSize(0.20);
                        graphRateCurrentCorrelation[side][plane][RPC-1]->SetTitle(Form("rate_current_correlation_MT%d_%s_%d",planes[plane],sides[side].Data(),RPC));

                        graphMeanRPCIDark[side][plane][RPC-1]=new TGraph();
                        graphMeanRPCIDark[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                        graphMeanRPCIDark[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                        graphMeanRPCIDark[side][plane][RPC-1]->SetMarkerStyle(20);
                        graphMeanRPCIDark[side][plane][RPC-1]->SetMarkerSize(0.20);
                        graphMeanRPCIDark[side][plane][RPC-1]->SetTitle(Form("mean_idark_MT%d_%s_%d",planes[plane],sides[side].Data(),RPC));

                        graphMeanRPCRate[side][plane][RPC-1]=new TGraph();
                        graphMeanRPCRate[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                        graphMeanRPCRate[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                        graphMeanRPCRate[side][plane][RPC-1]->SetMarkerStyle(20);
                        graphMeanRPCRate[side][plane][RPC-1]->SetMarkerSize(0.20);
                        graphMeanRPCRate[side][plane][RPC-1]->SetTitle(Form("mean_rate_MT%d_%s_%d",planes[plane],sides[side].Data(),RPC));
                    }

                    graphScalers[cathode][side][plane][RPC-1]=new TGraph();
                    graphScalers[cathode][side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                    graphScalers[cathode][side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                    graphScalers[cathode][side][plane][RPC-1]->SetMarkerStyle(20);
                    graphScalers[cathode][side][plane][RPC-1]->SetMarkerSize(0.20);
                    graphScalers[cathode][side][plane][RPC-1]->SetTitle(Form("Scalers_MT%d_%s_%s",planes[plane],sides[side].Data(),cathodes[cathode].Data()));

                    graphRatesPerCm2[cathode][side][plane][RPC-1]=new TGraph();
                    graphRatesPerCm2[cathode][side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                    graphRatesPerCm2[cathode][side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                    graphRatesPerCm2[cathode][side][plane][RPC-1]->SetMarkerStyle(20);
                    graphRatesPerCm2[cathode][side][plane][RPC-1]->SetMarkerSize(0.20);
                    graphRatesPerCm2[cathode][side][plane][RPC-1]->SetTitle(Form("Rates_MT%d_%s_%s",planes[plane],sides[side].Data(),cathodes[cathode].Data()));

                    graphChargePerHit[cathode][side][plane][RPC-1]=new TGraph();
                    graphChargePerHit[cathode][side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                    graphChargePerHit[cathode][side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                    graphChargePerHit[cathode][side][plane][RPC-1]->SetMarkerStyle(20);
                    graphChargePerHit[cathode][side][plane][RPC-1]->SetMarkerSize(0.20);
                    graphChargePerHit[cathode][side][plane][RPC-1]->SetTitle(Form("Charge_per_hit_MT%d_%s_%s",planes[plane],sides[side].Data(),cathodes[cathode].Data()));

                    TSortedList *sortedList;
                    inputFile->GetObject(Form("Data_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC),sortedList);
                    sortedList->Sort();

                    TSortedList *sortedListScalers;
                    printf("getting Scalers_MTR_%s_%s_MT%d_RPC%d\n",(sides[side]).Data(),(cathodes[cathode]).Data(),planes[plane],RPC);
                    inputFile->GetObject(Form("Scalers_MTR_%s_%s_MT%d_RPC%d",(sides[side]).Data(),(cathodes[cathode]).Data(),planes[plane],RPC),sortedListScalers);
                    ULong64_t totScalers=0;
                    for(Int_t iList=0; iList<sortedListScalers->GetEntries(); iList++){
                        totScalers+=((AliRPCValueScaler*)sortedListScalers->At(iList))->GetScalerCounts();
                        graphScalers[cathode][side][plane][RPC-1]->SetPoint(iList, ((AliRPCValueScaler*)sortedListScalers->At(iList))->GetTimeStamp(), totScalers);
                    }
                    totScalers=0;

                    Int_t currentRun=0;
                    ULong64_t runScalers=0;
                    ULong64_t startTime=0;
                    ULong64_t stopTime=0;
                    AliRPCValueScaler *scalerData;
                    Int_t runCount=0;
                    Int_t runCount2=0;
                    Int_t sortedListPreviousIndex=0;
                    for(Int_t iList=0; iList<sortedListScalers->GetEntries(); iList++){
                        scalerData=((AliRPCValueScaler*)sortedListScalers->At(iList));

                        if (currentRun==0){
                            currentRun=scalerData->GetRunNumber();
                            startTime=scalerData->GetTimeStamp();
                            //printf("elaborating run %d ",currentRun);
                        }

                        if (currentRun!=scalerData->GetRunNumber()){
                            Double_t elapsedTime=(Double_t)(stopTime-startTime);
                            if (elapsedTime!=0.){
                                Double_t currentRunAverageRate=((Double_t)runScalers)/elapsedTime;
                                graphRatesPerCm2[cathode][side][plane][RPC-1]->SetPoint(runCount, currentRun, currentRunAverageRate*2./Areas[RPC-1][plane]);
                                runCount++;

                                Double_t averageCurrent=0.;
                                Int_t currentMeasuresCount=0;
                                AliRPCValueDCS *value;
                                for (Int_t iList2=sortedListPreviousIndex; iList2<sortedList->GetEntries(); iList2++){
                                    value=(AliRPCValueDCS*)(sortedList->At(iList2));
                                    if(value->GetRunNumber()!=currentRun){
                                        if (currentMeasuresCount==0) continue; //se non ho ancora trovato misure devo ancora arrivare al run giusto
                                        else {  //altrimenti ho già superato la zona interessante, quindi posso breakare il loop (e forzarlo ad iniziare da dove aveva lasciato)
                                            sortedListPreviousIndex=iList2-1;
                                            break;
                                        }
                                    } else {
                                        if(value->IsCurrent()){
                                            AliRPCValueCurrent *valueCurrent=(AliRPCValueCurrent*)value;
                                            if(!(valueCurrent->GetITot()<0.))averageCurrent+=valueCurrent->GetITot();
                                            currentMeasuresCount++;
                                        }
                                        //cout<<"Right run number!!!!!!!!!!!"<<endl;
                                    }
                                }
                                value=0x0;

                                if(currentMeasuresCount!=0){
                                    averageCurrent=averageCurrent/(Double_t)currentMeasuresCount;
                                    if(averageCurrent<0){
                                        cout<<"less than 0 dark current in run "<<currentRun<<endl;
                                    } else {
                                        graphChargePerHit[cathode][side][plane][RPC-1]->SetPoint(runCount2, currentRun, averageCurrent/(currentRunAverageRate*2.));
                                        if (cathode==1){
                                            graphRateCurrentCorrelation[side][plane][RPC-1]->SetPoint(runCount2, currentRunAverageRate*2., averageCurrent);
                                            cout<<"setting:\t"<<RPC<<"\t"<<currentRun<<"\t"<<currentRunAverageRate<<"\t"<<averageCurrent<<endl;
                                            graphMeanRPCIDark[side][plane][RPC-1]->SetPoint(runCount2, currentRun, averageCurrent);
                                            graphMeanRPCRate[side][plane][RPC-1]->SetPoint(runCount2, currentRun, currentRunAverageRate);
                                        }
                                        runCount2++;   
                                    }
                                }
                                averageCurrent=0.;
                                currentMeasuresCount=0;
                                //printf("%f\n",((Double_t)(stopTime-startTime)));
                            }
                            runScalers=0;
                            currentRun=scalerData->GetRunNumber();
                            startTime=scalerData->GetTimeStamp();
                            //printf("elaborating run %d ",currentRun);
                        } else {
                            runScalers+=scalerData->GetScalerCounts();
                            stopTime=scalerData->GetTimeStamp();
                        }
                        scalerData=0x0;
                    }

                    sortedListScalers=0x0;

                    Int_t index=-1;
                    if (side==0){ //INSIDE
                        if (cathode==0) { //BENDING
                            index=0;
                        } else if (cathode==1){ //NON-BENDING
                            index=1;
                        }
                    } else if (side==1){ //OUTSIDE
                        if (cathode==0) { //BENDING
                            index=2;
                        } else if (cathode==1){ //NON-BENDING
                            index=3;
                        }
                    }

                    if (index==-1) {
                        printf("\n!!! Error !!!\n");
                        continue;
                    }

                    if (RPC==1) {
                        canvScalers[index]->cd(plane+1);
                        graphScalers[cathode][side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,3000000000.);
                        graphScalers[cathode][side][plane][RPC-1]->Draw("ap");
                        canvRatesPerCm2[index]->cd(plane+1);
                        graphRatesPerCm2[cathode][side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,160000.);
                        graphRatesPerCm2[cathode][side][plane][RPC-1]->Draw("ap");
                        canvChargePerHit[index]->cd(plane+1);
                        graphChargePerHit[cathode][side][plane][RPC-1]->GetYaxis()->SetRangeUser(-0.0003,0.0014);
                        graphChargePerHit[cathode][side][plane][RPC-1]->Draw("ap");
                    } else {
                        canvScalers[index]->cd(plane+1);
                        graphScalers[cathode][side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,3000000000.);
                        graphScalers[cathode][side][plane][RPC-1]->Draw("p");
                        canvRatesPerCm2[index]->cd(plane+1);
                        graphRatesPerCm2[cathode][side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,160000.);
                        graphRatesPerCm2[cathode][side][plane][RPC-1]->Draw("p");
                        canvChargePerHit[index]->cd(plane+1);
                        graphChargePerHit[cathode][side][plane][RPC-1]->GetYaxis()->SetRangeUser(-0.0003,0.0014);
                        graphChargePerHit[cathode][side][plane][RPC-1]->Draw("p");
                    }

                    if(cathode==1){
                        TVirtualPad *pad=canvRateCurrentCorrelation[side][plane][RPC-1]->cd(1);
                        pad->Divide(1,2);
                        pad->cd(1);
                        graphRatesPerCm2[cathode][side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,160000.);
                        graphRatesPerCm2[cathode][side][plane][RPC-1]->Draw("ap");
                        pad->cd(2);
                        graphNet[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0.,40.);
                        graphNet[side][plane][RPC-1]->Draw("ap");

                        canvRateCurrentCorrelation[side][plane][RPC-1]->cd(2);
                        graphRateCurrentCorrelation[side][plane][RPC-1]->GetXaxis()->SetRangeUser(0.,15000.);
                        graphRateCurrentCorrelation[side][plane][RPC-1]->Draw("ap");
                    }
                }
            }
        }
    }

    TCanvas *canvMeanValues[nSides][nPlanes];
    TGraph *graphMeanIDark[nSides][nPlanes];
    TGraph *graphMeanRate[nSides][nPlanes];
    for (Int_t side=0; side<nSides; side++) {
        for (Int_t plane=0; plane<nPlanes; plane++) {             
            canvMeanValues[side][plane]=new TCanvas(Form("canv_MT%d_%s_mean_values",planes[plane],sides[side].Data()));
            canvMeanValues[side][plane]->Divide(2);

            graphMeanIDark[side][plane]=new TGraph();
            graphMeanIDark[side][plane]->SetLineColor(color[0]);
            graphMeanIDark[side][plane]->SetMarkerColor(color[0]);
            graphMeanIDark[side][plane]->SetMarkerStyle(20);
            graphMeanIDark[side][plane]->SetMarkerSize(0.20);
            graphMeanIDark[side][plane]->SetTitle(Form("mean_idark_MT%d_%s_%d",planes[plane],sides[side].Data(),RPC));

            graphMeanRate[side][plane]=new TGraph();
            graphMeanRate[side][plane]->SetLineColor(color[0]);
            graphMeanRate[side][plane]->SetMarkerColor(color[0]);
            graphMeanRate[side][plane]->SetMarkerStyle(20);
            graphMeanRate[side][plane]->SetMarkerSize(0.20);
            graphMeanRate[side][plane]->SetTitle(Form("mean_rate_MT%d_%s_%d",planes[plane],sides[side].Data(),RPC));

            Int_t nPoints=graphMeanRPCIDark[side][plane][0]->GetN();
    cout<<"CIAO"<<endl;

            for(Int_t iPoint=0;iPoint<nPoints;iPoint++){
                Double_t meanIDark=0.;
                Double_t meanRate=0.;
                Double_t run=0.;
                for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                    Double_t iDark=0.;
                    Double_t rate=0.;
                    graphMeanRPCIDark[side][plane][RPC-1]->GetPoint(iPoint,run,iDark);
                    graphMeanRPCRate[side][plane][RPC-1]->GetPoint(iPoint,run,rate);
                    cout<<"reading:\t"<<RPC<<"\t"<<run<<"\t"<<rate<<"\t"<<iDark<<endl;
                    meanIDark+=iDark;
                    meanRate+=rate;
                }
                meanIDark/=72.;
                meanRate/=72.;
                graphMeanIDark[side][plane]->SetPoint(iPoint, run, meanIDark);
                graphMeanRate[side][plane]->SetPoint(iPoint, run, meanRate);
            }

            canvMeanValues[side][plane]->cd(1);
            graphMeanIDark[side][plane]->Draw("ap");
            canvMeanValues[side][plane]->cd(2);
            graphMeanRate[side][plane]->Draw("ap");

        }
    }

    output->cd();

    for(Int_t canvIndex=0;canvIndex<nSides*2;canvIndex++){
        canvScalers[canvIndex]->Write();
        canvRatesPerCm2[canvIndex]->Write();
        canvChargePerHit[canvIndex]->Write();
    }

    for (Int_t side=0; side<nSides; side++) {
        for (Int_t plane=0; plane<nPlanes; plane++) {
            canvMeanValues[side][plane]->Write();
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                canvRateCurrentCorrelation[side][plane][RPC-1]->Write();
            }
        }
    }

    output->Close();
}