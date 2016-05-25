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

void AMANDAPlotter3_furbo(TString inputFileName){

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

    Int_t color18[nRPC*nSides]={kCyan+2,kCyan,kCyan+3,kBlue,kBlue+3,kBlue-6,kMagenta,kMagenta+4,kMagenta-6,kRed,kRed+3,kRed-1,kYellow,kYellow+3,kYellow-5,kGreen,kGreen+3,kGreen+4};
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

    //TCanvas *canviTot[nSides];
    TCanvas *canvCharge[nSides];
    TGraph *graphTot[nSides][nPlanes][nRPC];
    TGraph *graphCharge[nSides][nPlanes][nRPC];
    TH1D *histoCharge[nSides][nPlanes];
    TH1D *histoChargeCounts[nSides][nPlanes];
    TGraph *graphChargeMean[nSides][nPlanes];
    TMultiGraph *multiGraph[nSides][nPlanes];

    TCanvas *canvPlanes[nPlanes];
    TMultiGraph *planesMultiGraph[nPlanes];
    for (Int_t plane=0; plane<nPlanes; plane++) {
        canvPlanes[plane]=new TCanvas(Form("MT%d",planes[plane]),Form("MT%d",planes[plane]));
        planesMultiGraph[plane]=new TMultiGraph(Form("Integrated charge MT%d",planes[plane]), Form("Integrated charge MT%d",planes[plane]));
    }

    Double_t maxCharge[nPlanes][nSides];
    Int_t maxChargeRPC[nPlanes][nSides];

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
    Int_t rebinEntity=1200;

    Double_t meanDarkCurrent[nPlanes][7]={{1.333333333,1.068055556,0.971296296,0.916666667,2.494444444,1.930769231,2.005555556},{0.661111111,0.765277778,0.609259259,0.605555556,1.488888889,1.285185185,1.783333333},{1.494444444,1.529166667,1.294444444,1.733333333,2.941666667,2.374074074,3.388888889},{1.638888889,2.00125,1.877777778,2.483333333,4.294285714,3.446296296,5.027777778}};

    // TLine *line2010 = new TLine(epoch2010,0.,epoch2010,14000.);
    // TLine *line2011 = new TLine(epoch2011,0.,epoch2011,14000.);
    // TLine *line2012 = new TLine(epoch2012,0.,epoch2012,14000.);
    // TLine *line2013 = new TLine(epoch2013,0.,epoch2013,14000.);
    // TLine *line2014 = new TLine(epoch2014,0.,epoch2014,14000.);
    // TLine *line2015 = new TLine(epoch2015,0.,epoch2015,14000.);

    // TPaveText *pt2010 = new TPaveText(epoch2010,6000.,epoch2010+25000000,7000.);
    // pt2010->SetLineColor(kWhite);
    // pt2010->SetFillColor(kWhite);
    // pt2010->SetTextSize(0.05);
    // pt2010->SetShadowColor(kWhite);
    // pt2010->AddText("2010");

    // TPaveText *pt2011 = new TPaveText(epoch2011,6000.,epoch2011+25000000,7000.);
    // pt2011->SetLineColor(kWhite);
    // pt2011->SetFillColor(kWhite);
    // pt2011->SetTextSize(0.05);
    // pt2011->SetShadowColor(kWhite);
    // pt2011->AddText("2011");

    // TPaveText *pt2012 = new TPaveText(epoch2012,6000.,epoch2012+25000000,7000.);
    // pt2012->SetLineColor(kWhite);
    // pt2012->SetFillColor(kWhite);
    // pt2012->SetTextSize(0.05);
    // pt2012->SetShadowColor(kWhite);
    // pt2012->AddText("2012");

    // TPaveText *pt2013 = new TPaveText(epoch2013,6000.,epoch2013+25000000,7000.);
    // pt2013->SetLineColor(kWhite);
    // pt2013->SetFillColor(kWhite);
    // pt2013->SetTextSize(0.05);
    // pt2013->SetShadowColor(kWhite);
    // pt2013->AddText("2013");

    // TPaveText *pt2014 = new TPaveText(epoch2014,6000.,epoch2014+25000000,7000.);
    // pt2014->SetLineColor(kWhite);
    // pt2014->SetFillColor(kWhite);
    // pt2014->SetTextSize(0.05);
    // pt2014->SetShadowColor(kWhite);
    // pt2014->AddText("2014");

    // TPaveText *pt2015 = new TPaveText(epoch2015,6000.,epoch2015+25000000,7000.);
    // pt2015->SetLineColor(kWhite);
    // pt2015->SetFillColor(kWhite);
    // pt2015->SetTextSize(0.05);
    // pt2015->SetShadowColor(kWhite);
    // pt2015->AddText("2015");


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

    for (Int_t side=0; side<nSides; side++) {
        cout<<side<<endl;
        // canviTot[side]=new TCanvas(Form("canv_%s_iTot",sides[side].Data()),Form("canv_%s_iTot",sides[side].Data()));
        // canviTot[side]->Divide(2,2);
        canvCharge[side]=new TCanvas(Form("canv_%s_Charge",sides[side].Data()),Form("canv_%s_Charge",sides[side].Data()));
        canvCharge[side]->Divide(2,2);
        for (Int_t plane=0; plane<nPlanes; plane++) {
            histoCharge[side][plane]=new TH1D(Form("Charge_MT%d_%s",planes[plane],sides[side].Data()), Form("Charge_MT%d_%s",planes[plane],sides[side].Data()), (Int_t)bins, (Double_t)epochStart, (Double_t)epochStop);
            histoChargeCounts[side][plane]=new TH1D(Form("Counts_Charge_MT%d_%s",planes[plane],sides[side].Data()), Form("Counts_Charge_MT%d_%s",planes[plane],sides[side].Data()), (Int_t)bins, (Double_t)epochStart, (Double_t)epochStop);
            graphChargeMean[side][plane]=new TGraph();
            graphChargeMean[side][plane]->SetLineColor(kBlack);
            graphChargeMean[side][plane]->SetMarkerColor(kBlack);
            graphChargeMean[side][plane]->SetMarkerStyle(24);
            graphChargeMean[side][plane]->SetMarkerSize(0.27);
            graphChargeMean[side][plane]->SetTitle(Form("Integrated charge MT%d %s",planes[plane],sides[side].Data()));
            maxCharge[plane][side]=0.;
            maxChargeRPC[plane][side]=0;
            multiGraph[side][plane]=new TMultiGraph(Form("Charge_MT%d_%s",planes[plane],sides[side].Data()), Form("Charge_MT%d_%s",planes[plane],sides[side].Data()));

            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                cout<<"RPC "<<RPC<<endl;
                graphTot[side][plane][RPC-1]=new TGraph();
                graphTot[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                graphTot[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                graphTot[side][plane][RPC-1]->SetMarkerStyle(20);
                graphTot[side][plane][RPC-1]->SetMarkerSize(0.27);
                graphTot[side][plane][RPC-1]->SetTitle(Form("iTot_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                graphCharge[side][plane][RPC-1]=new TGraph();
                graphCharge[side][plane][RPC-1]->SetLineColor(color18[side*(nRPC-1)+RPC-1]);
                graphCharge[side][plane][RPC-1]->SetMarkerColor(color18[side*(nRPC-1)+RPC-1]);
                graphCharge[side][plane][RPC-1]->SetMarkerStyle(20);
                graphCharge[side][plane][RPC-1]->SetMarkerSize(0.27);
                graphCharge[side][plane][RPC-1]->SetTitle(Form("Integrated charge MT%d %s %d",planes[plane],sides[side].Data(),RPC));

                ULong64_t actualTimestamp=0;
                ULong64_t followingTimestamp=0;
                Double_t charge=0.;
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
            		charge+=current->GetINet()*(Double_t)(followingTimestamp-actualTimestamp);
            		actualTimestamp=current->GetTimeStamp();

                    if(plane==1 && side==1 && RPC==2 && actualTimestamp>epoch2015 && singletonZero) {
                        charge=0.;
                        singletonZero=kFALSE;
                    }
                    //Double_t chargeBinContent=histoCharge[side][plane]->GetBinContent((Int_t)(followingTimestamp));

                    if(actualTimestamp%100==0){
                        //cout<<"filling "<<followingTimestamp-epochStart<<" with "<<charge<<endl;
                        if(!(plane==3 && side==0 && RPC==3)){
                            if(!(plane==1 && side==1 && RPC==2)){
                                histoCharge[side][plane]->AddBinContent((Int_t)(followingTimestamp-epochStart), charge/Areas[RPC-1][plane]);
                                histoChargeCounts[side][plane]->Fill(followingTimestamp);
                            } else cout<<"found the new RPC"<<endl;
                        } else cout<<"found the FEERIC RPC"<<endl;
                        //cout<<followingTimestamp<<" "<<charge/Areas[RPC-1][plane]<<endl;
                        graphCharge[side][plane][RPC-1]->SetPoint(pointCounter1++, (Double_t)followingTimestamp, (Double_t)(charge/Areas[RPC-1][plane]));
                        graphTot[side][plane][RPC-1]->SetPoint(pointCounter2++, (Double_t)current->GetTimeStamp(), (Double_t)(current->GetITot()/Areas[RPC-1][plane]));
                    }
                	current=currentFollowing;
                    currentFollowing=0x0;
                }
                delete data[side][plane][RPC-1];
                cout<<"max charge RPC "<<maxChargeRPC[plane][side]<<endl;
                if(charge/Areas[RPC-1][plane]>maxCharge[plane][side]){
                    maxChargeRPC[plane][side]=RPC;
                    maxCharge[plane][side]=charge/Areas[RPC-1][plane];
                }
                cout<<"max charge RPC "<<maxChargeRPC[plane][side]<<endl;

                //graphTotMulti[side][plane]->Add(graphTot[side][plane][RPC-1]);
                //graphChargeMulti[side][plane]->Add(graphCharge[side][plane][RPC-1]);
            }

            histoCharge[side][plane]->Rebin(rebinEntity);
            histoChargeCounts[side][plane]->Rebin(rebinEntity);

            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                if(RPC!=maxChargeRPC[plane][side]){
                    if(!(plane==3 && side==0 && RPC==3)){
                        if(!(plane==1 && side==1 && RPC==2)){
                            graphCharge[side][plane][RPC-1]->SetFillStyle(0);
                            graphCharge[side][plane][RPC-1]->SetFillColor(0);
                            planesMultiGraph[plane]->Add(graphCharge[side][plane][RPC-1]);
                        }
                    }
                }
            }

            // Int_t count=0;
            // Double_t previousValue=0.;
            // Double_t binWidth=histoCharge[side][plane]->GetBinWidth(1);
            // for(Int_t iBin=0;iBin<histoCharge[side][plane]->GetNbinsX();iBin++){
            //     //cout<<iBin<<"/"<<histoCharge[side][plane]->GetNbinsX()<<endl;
            //     //cout<<histoCharge[side][plane]->GetBinContent(iBin)<<" "<<histoChargeCounts[side][plane]->GetBinContent(iBin)<<endl;
            //     if(histoChargeCounts[side][plane]->GetBinContent(iBin+1)>=9.){
            //         //cout<<histoCharge[side][plane]->GetBinContent(iBin)<<endl;
            //         //cout<<binWidth<<endl;
            //         histoCharge[side][plane]->SetBinContent(iBin, histoCharge[side][plane]->GetBinContent(iBin)/(histoChargeCounts[side][plane]->GetBinContent(iBin+1)));
            //         Double_t actualValue=histoCharge[side][plane]->GetBinContent(iBin);
            //         if(actualValue>=previousValue){
            //             graphChargeMean[side][plane]->SetPoint(count++, iBin*binWidth+epochStart, actualValue);
            //             previousValue=actualValue; 
            //         } else {
            //             graphChargeMean[side][plane]->SetPoint(count++, iBin*binWidth+epochStart, previousValue);
            //         }                   
            //     }
            // }

            printf("Plane %d %s accomplished\n",planes[plane],sides[side].Data());

            delete histoCharge[side][plane];
            delete histoChargeCounts[side][plane];

            // graphChargeMean[side][plane]->SetMarkerColor(kViolet);
            // multiGraph[side][plane]->Add(graphChargeMean[side][plane]);
            // if(maxChargeRPC[plane][side]!=0)multiGraph[side][plane]->Add(graphCharge[side][plane][maxChargeRPC[plane][side]-1]);
            // if(plane==3 && side==0){
            //     graphCharge[side][plane][3-1]->SetTitle("Integrated charge MT2 INSIDE 3");
            //     multiGraph[side][plane]->Add(graphCharge[side][plane][3-1]);
            // }
            // if(plane==1 && side==1){
            //     multiGraph[side][plane]->Add(graphCharge[side][plane][2-1]);
            // }

            // canvCharge[side]->cd(plane+1);
            // multiGraph[side][plane]->Draw("ap");
            // // line2010->Draw();
            // // line2011->Draw();
            // // line2012->Draw();
            // // line2013->Draw();
            // // line2014->Draw();
            // // line2015->Draw();
            // // pt2010->Draw();
            // // pt2011->Draw();
            // // pt2012->Draw();
            // // pt2013->Draw();
            // // pt2014->Draw();
            // // pt2015->Draw();
            // multiGraph[side][plane]->GetYaxis()->SetTitle("Integrated current [#muC/cm^{2}]");
            // multiGraph[side][plane]->GetYaxis()->SetTitleOffset(2.);
            // multiGraph[side][plane]->GetYaxis()->SetRangeUser(0., 14000.);
            // multiGraph[side][plane]->GetXaxis()->SetTitle("Epoch timestamp [s]");
            // canvCharge[side]->cd(plane+1)->BuildLegend();
            // gPad->Update();
            // gPad->Modified();

        }
    }

    for (Int_t plane=0; plane<nPlanes; plane++) {    
        cout<<plane<<endl;
        canvPlanes[plane]->cd();
        planesMultiGraph[plane]->Draw("ap");
        planesMultiGraph[plane]->GetXaxis()->SetTitle("Timestamp [s]");
        planesMultiGraph[plane]->GetYaxis()->SetTitle("Integrated charge [#muC/cm^{2}]");
        canvPlanes[plane]->BuildLegend();
        gPad->Update();
        gPad->Modified();
    }
}