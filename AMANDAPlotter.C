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

void AMANDAPlotter(TString inputFileName){

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

    Int_t color[nRPC] = {kBlack,kRed,kGreen,kBlue,kYellow,kMagenta,kCyan,kGray,kOrange};

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

    TCanvas *canviTot[nSides];
    TCanvas *canvCharge[nSides];
    TMultiGraph *graphTotMulti[nSides][nPlanes];
    TMultiGraph *graphChargeMulti[nSides][nPlanes];
    TGraph *graphTot[nSides][nPlanes][nRPC];
    TGraph *graphCharge[nSides][nPlanes][nRPC];
    TH1D *histoTot[nSides][nPlanes][nRPC];
    TH1D *histoCharge[nSides][nPlanes][nRPC];
    TH1D *histoTotCounts[nSides][nPlanes][nRPC];
    TH1D *histoChargeCounts[nSides][nPlanes][nRPC];
    TH1D *histoTotRatio[nSides][nPlanes][nRPC];
    TH1D *histoChargeRatio[nSides][nPlanes][nRPC];

    ULong64_t epochStart=1262300400;
    ULong64_t epochStop=1451602800;
    ULong64_t bins=(epochStop-epochStart);
    Int_t rebinEntity=1000;

    for (Int_t side=0; side<1; side++) {
        cout<<side<<endl;
        canviTot[side]=new TCanvas(Form("canv_%s_iTot",sides[side].Data()),Form("canv_%s_iTot",sides[side].Data()));
        canviTot[side]->Divide(2,2);
        canvCharge[side]=new TCanvas(Form("canv_%s_Charge",sides[side].Data()),Form("canv_%s_Charge",sides[side].Data()));
        canvCharge[side]->Divide(2,2);
        for (Int_t plane=0; plane<1; plane++) {
	        graphTotMulti[side][plane]=new TMultiGraph(Form("iTot %d %s",planes[plane],sides[side].Data()),Form("iTot %d %s",planes[plane],sides[side].Data()));
			graphChargeMulti[side][plane]=new TMultiGraph(Form("Charge %d %s",planes[plane],sides[side].Data()),Form("Charge %d %s",planes[plane],sides[side].Data()));
            for (Int_t RPC=1; RPC<=1; RPC++) {
                graphTot[side][plane][RPC-1]=new TGraph();
                graphTot[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                graphTot[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                graphTot[side][plane][RPC-1]->SetMarkerStyle(20);
                graphTot[side][plane][RPC-1]->SetMarkerSize(0.20);
                graphTot[side][plane][RPC-1]->SetTitle(Form("iTot_MT%d_%s",planes[plane],sides[side].Data()));
                graphCharge[side][plane][RPC-1]=new TGraph();
                graphCharge[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                graphCharge[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                graphCharge[side][plane][RPC-1]->SetMarkerStyle(20);
                graphCharge[side][plane][RPC-1]->SetMarkerSize(0.20);
                graphCharge[side][plane][RPC-1]->SetTitle(Form("Charge_MT%d_%s",planes[plane],sides[side].Data()));
                histoTot[side][plane][RPC-1]=new TH1D(Form("iTot_MT%d_RPC%d_%s",planes[plane],RPC,sides[side].Data()), Form("iTot_MT%d_RPC%d_%s",planes[plane],RPC,sides[side].Data()), (Int_t)bins, (Double_t)epochStart, (Double_t)epochStop);
                histoCharge[side][plane][RPC-1]=new TH1D(Form("Charge_MT%d_RPC%d_%s",planes[plane],RPC,sides[side].Data()), Form("Charge_MT%d_RPC%d_%s",planes[plane],RPC,sides[side].Data()), (Int_t)bins, (Double_t)epochStart, (Double_t)epochStop);
                histoTotCounts[side][plane][RPC-1]=new TH1D(Form("Counts_iTot_MT%d_RPC%d_%s",planes[plane],RPC,sides[side].Data()), Form("Counts_iTot_MT%d_RPC%d_%s",planes[plane],RPC,sides[side].Data()), (Int_t)bins, (Double_t)epochStart, (Double_t)epochStop);
                histoChargeCounts[side][plane][RPC-1]=new TH1D(Form("Counts_Charge_MT%d_RPC%d_%s",planes[plane],RPC,sides[side].Data()), Form("Counts_Charge_MT%d_RPC%d_%s",planes[plane],RPC,sides[side].Data()), (Int_t)bins, (Double_t)epochStart, (Double_t)epochStop);
                histoTotRatio[side][plane][RPC-1]=new TH1D(Form("Ratio_iTot_MT%d_RPC%d_%s",planes[plane],RPC,sides[side].Data()), Form("Ratio_iTot_MT%d_RPC%d_%s",planes[plane],RPC,sides[side].Data()), (Int_t)bins, (Double_t)epochStart, (Double_t)epochStop);
                histoChargeRatio[side][plane][RPC-1]=new TH1D(Form("Ratio_Charge_MT%d_RPC%d_%s",planes[plane],RPC,sides[side].Data()), Form("Ratio_Charge_MT%d_RPC%d_%s",planes[plane],RPC,sides[side].Data()), (Int_t)bins, (Double_t)epochStart, (Double_t)epochStop);


                AliRPCValueCurrent *current=(AliRPCValueCurrent*)(data[side][plane][RPC-1]->At(0));
                AliRPCValueCurrent *currentFollowing;
                ULong64_t actualTimestamp=0;
                ULong64_t followingTimestamp=0;
                Double_t charge=0.;
                Int_t nEntries=data[side][plane][RPC-1]->GetEntries();
                Double_t binContentBuffer=0.;
                Int_t binContentsCounter=0;
                Int_t pointCounter1=0;
                Int_t pointCounter2=0;

                for(Int_t iEntry=0;iEntry<nEntries-1;iEntry++){
                	if(iEntry%10000==0)cout<<iEntry<<"/"<<nEntries<<endl;
                	//current=(AliRPCValueCurrent*)(data[side][plane][RPC-1]->At(iEntry));
                    currentFollowing=(AliRPCValueCurrent*)(data[side][plane][RPC-1]->At(iEntry+1));
                    //if(current->GetITot()==0.) continue;
                	actualTimestamp=current->GetTimeStamp();
                    followingTimestamp=currentFollowing->GetTimeStamp();

            		charge+=current->GetITot()*(Double_t)(followingTimestamp-actualTimestamp);
            		actualTimestamp=current->GetTimeStamp();
                    //Double_t chargeBinContent=histoCharge[side][plane][RPC-1]->GetBinContent((Int_t)(followingTimestamp));

                    if((followingTimestamp-epochStart)%rebinEntity==0){
                        //cout<<"filling "<<followingTimestamp-epochStart<<" with "<<charge<<endl;
                        histoCharge[side][plane][RPC-1]->SetBinContent((Int_t)(followingTimestamp-epochStart), charge/Areas[RPC-1][plane]);
                        histoChargeCounts[side][plane][RPC-1]->Fill(followingTimestamp);
                        graphCharge[side][plane][RPC-1]->SetPoint(pointCounter1++, followingTimestamp, charge/Areas[RPC-1][plane]);
                        graphTot[side][plane][RPC-1]->SetPoint(pointCounter2++, current->GetTimeStamp(), current->GetITot()/Areas[RPC-1][plane]);

                        //Double_t iTotBinContent=histoTot[side][plane][RPC-1]->GetBinContent((Int_t)actualTimestamp);
                        histoTot[side][plane][RPC-1]->SetBinContent((Int_t)(actualTimestamp-epochStart), current->GetITot());
                        histoTotCounts[side][plane][RPC-1]->Fill(actualTimestamp);
                    }
                	current=currentFollowing;
                    currentFollowing=0x0;
                }
                graphTotMulti[side][plane]->Add(graphTot[side][plane][RPC-1]);
                graphChargeMulti[side][plane]->Add(graphCharge[side][plane][RPC-1]);
                histoCharge[side][plane][RPC-1]->Rebin(rebinEntity);
                histoChargeCounts[side][plane][RPC-1]->Rebin(rebinEntity);
                histoTot[side][plane][RPC-1]->Rebin(rebinEntity);
                histoTotCounts[side][plane][RPC-1]->Rebin(rebinEntity);
                //histoChargeRatio[side][plane][RPC-1]->Divide((TH1*)histoCharge[side][plane][RPC-1],(TH1*)histoChargeCounts[side][plane][RPC-1]);
                //histoTotRatio[side][plane][RPC-1]->Divide((TH1*)histoTot[side][plane][RPC-1],(TH1*)histoTotCounts[side][plane][RPC-1]);

                for(Int_t iBin=0;iBin<histoCharge[side][plane][RPC-1]->GetNbinsX();iBin++){
                    //cout<<iBin<<"/"<<histoCharge[side][plane][RPC-1]->GetNbinsX()<<endl;
                    if(histoChargeCounts[side][plane][RPC-1]->GetBinContent(iBin)!=0.)histoCharge[side][plane][RPC-1]->SetBinContent(iBin, histoCharge[side][plane][RPC-1]->GetBinContent(iBin)/(histoChargeCounts[side][plane][RPC-1]->GetBinContent(iBin)));
                }
                
                canviTot[side]->cd(plane+1);
                histoTot[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                if(RPC==1) histoTot[side][plane][RPC-1]->Draw();
                else histoTot[side][plane][RPC-1]->Draw("SAME");


                canvCharge[side]->cd(plane+1);
                histoCharge[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                if(RPC==1) histoCharge[side][plane][RPC-1]->Draw();
                else histoCharge[side][plane][RPC-1]->Draw("SAME");

                canvCharge[side]->cd(plane+2);
                histoChargeCounts[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                if(RPC==1) histoChargeCounts[side][plane][RPC-1]->Draw();
                else histoChargeCounts[side][plane][RPC-1]->Draw("SAME");

                // canvCharge[side]->cd(plane+1);
                // //graphCharge[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                // if(RPC==1) //graphCharge[side][plane][RPC-1]->Draw("AP");
                // else //graphCharge[side][plane][RPC-1]->Draw("P");
            }


			canvCharge[side]->cd(plane+3);
			graphTotMulti[side][plane]->Draw("AP");
			canvCharge[side]->cd(plane+4);
			graphChargeMulti[side][plane]->Draw("AP");
        }
    }
}