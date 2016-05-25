//
// OCDBRunStatisticsPlotter.cpp
// 
//   Created by Gabriele Gaetano Fronzé
//   Copyright © 2016 Gabriele Gaetano Fronzé. All rights reserved.
//

#include <stdio.h>
#include "TString.h"
#include "TMap.h"
#include "TObjArray.h"
#include "TList.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TFile.h"
#include "AliRPCValueDCS.h"
#include "AliRPCValueCurrent.h"
#include "AliRPCValueVoltage.h"
#include "AliRPCValueScaler.h"
#include "AliRPCRunStatistics.h"
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

void OCDBRunStatisticsPlotter2(TString inputFileName){
	TFile *inputFile=new TFile(inputFileName.Data());

	inputFileName.ReplaceAll(".root", "");
    TFile *output=new TFile(Form("Run_Plots_%s.root",inputFileName.Data()),"RECREATE");

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

    //primo run PbPb
    Int_t firstPbPbRun=244913;

    //run number dei run dei quali plottare la distribuzione di corrente
    Int_t ppRunForCurrentDistribution=244411;
    Int_t pPbRunForCurrentDistribution=0;
    Int_t PbPbRunForCurrentDistribution=245949;

    //run number dei run dei quali plottare la distribuzione di dark rate e dark current
    Int_t startRunDarkDistribution=245729;
	Int_t stopRunDarkDistribution=245833;

	//run number dei run dei quali plottare la correlazione ∆iDark/iDark vs Dark rate e carica integrata
    Int_t startRunDarkCurrentCorrelation=245729;
	Int_t stopRunDarkCurrentCorrelation=245833;

	//run number dei run dei quali plottare la correlazione ∆darkRate/darkRate vs carica integrata
    Int_t startRunDarkRateCorrelation=245729;
	Int_t stopRunDarkRateCorrelation=245833;

    Int_t color[nRPC] = {kBlack,kRed,kGreen,kBlue,kYellow,kMagenta,kCyan,kGray,kOrange};
    Int_t markerStyles[nPlanes]={20,24,21,25};

	// instanze di canvas e grafici per l'output
	TCanvas *canvCurrentDistribution=new TCanvas("Current_distributions","Current_distributions");
	if(pPbRunForCurrentDistribution!=0) canvCurrentDistribution->Divide(3);
    else canvCurrentDistribution->Divide(2);
	TH1D *ppCurrentDistribution=new TH1D("pp_current_distribution", "pp_current_distribution", 24, 0., .0015);
	if(pPbRunForCurrentDistribution!=0) TH1D *pPbCurrentDistribution=new TH1D("pPb_current_distribution", "pPb_current_distribution", 24, 0., .0015);
	TH1D *PbPbCurrentDistribution=new TH1D("PbPb_current_distribution", "PbPb_current_distribution", 24, 0., .0015);

	TCanvas *canvppCurrentVsRate[nSides];
	TGraph *ppAverageCurrentVsRate[nSides][nPlanes][nRPC];

	TCanvas *canvPbPbCurrentVsRate[nSides];
	TGraph *PbPbAverageCurrentVsRate[nSides][nPlanes][nRPC];

	TCanvas *canvIntegratedCharge[nSides];
	TGraph *integratedCharge[nSides][nPlanes][nRPC];

	TCanvas *canvRateVsTime=new TCanvas("Rate_vs_time","Rate_vs_time");
	TGraph *darkRateVsTime=new TGraph();
    darkRateVsTime->SetName("Dark rate vs time");
    darkRateVsTime->SetTitle("Dark rate vs time");
    darkRateVsTime->SetLineColor(kPink);
    darkRateVsTime->SetMarkerColor(kPink);
    darkRateVsTime->SetMarkerStyle(20);
    darkRateVsTime->SetMarkerSize(0.27);

	TCanvas *canvDarkRateDistribution=new TCanvas("Dark_rate_distributions","Dark_rate_distributions");
	canvDarkRateDistribution->Divide(2);
	TH1D *beginningDarkRateDistribution=new TH1D("beginning_dark_rate_distribution", "beginning_dark_rate_distribution", 25, 0., 2.);
	TH1D *endDarkRateDistribution=new TH1D("end_dark_rate_distribution", "end_dark_rate_distribution", 25, 0., 2.);

	TCanvas *canvDarkCurrentVsTime=new TCanvas("dark_current_vs_time","dark_current_vs_time");
	TGraph *darkCurrentVsTime=new TGraph();
    darkCurrentVsTime->SetName("Dark current vs time");
    darkCurrentVsTime->SetTitle("Dark current vs time");
    darkCurrentVsTime->SetLineColor(kPink);
    darkCurrentVsTime->SetMarkerColor(kPink);
    darkCurrentVsTime->SetMarkerStyle(20);
    darkCurrentVsTime->SetMarkerSize(0.27);

	TCanvas *canvDarkCurrentDistribution=new TCanvas("Dark_current_distributions","Dark_current_distributions");
	canvDarkCurrentDistribution->Divide(2);
	TH1D *beginningDarkCurrentDistribution=new TH1D("beginning_dark_current_distribution", "beginning_dark_current_distribution", 32, 0., 8.);
	TH1D *endDarkCurrentDistribution=new TH1D("end_dark_current_distribution", "end_dark_current_distribution", 32, 0., 8.);

	TCanvas *canvVariations=new TCanvas("Variations","Variations");
    //canvVariations->Divide(3);
	TGraph *darkCurrentVariationVsCharge[nSides][nPlanes][nRPC];
    TGraph *darkRateVariationVsCharge[nSides][nPlanes][nRPC];
	TGraph *darkCurrentVariationVsDarkRateVariation[nSides][nPlanes][nRPC];
    TMultiGraph *darkCurrentVariationVsChargeMulti=new TMultiGraph("variations", "variations");

	TList *sortedListRunStatistics[nSides][nPlanes][nRPC];
	Double_t beginningDarkCurrents[nSides][nPlanes][nRPC];
	Double_t deltaDarkCurrents[nSides][nPlanes][nRPC];
	Double_t beginningDarkRates[nSides][nPlanes][nRPC];
	Double_t deltaDarkRates[nSides][nPlanes][nRPC];

    for (Int_t side=0; side<nSides; side++) {
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                sortedListRunStatistics[side][plane][RPC-1]=new TList();
            	inputFile->GetObject(Form("Run_Stati_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC),sortedListRunStatistics[side][plane][RPC-1]);
            	if(!sortedListRunStatistics[side][plane][RPC-1])continue;
                sortedListRunStatistics[side][plane][RPC-1]->Sort();
            }
        }
    }

    for (Int_t side=0; side<nSides; side++) {
    	canvppCurrentVsRate[side]=new TCanvas(Form("pp_Current_vs_Rate_%s",sides[side].Data()),Form("pp_Current_vs_Rate_%s",sides[side].Data()));
    	canvppCurrentVsRate[side]->Divide(2,2);

    	canvPbPbCurrentVsRate[side]=new TCanvas(Form("PbPb_Current_vs_Rate_%s",sides[side].Data()),Form("PbPb_Current_vs_Rate_%s",sides[side].Data()));
    	canvPbPbCurrentVsRate[side]->Divide(2,2);

    	canvIntegratedCharge[side]=new TCanvas(Form("Integrated_charge_%s",sides[side].Data()),Form("Integrated_charge_%s",sides[side].Data()));
    	canvIntegratedCharge[side]->Divide(2,2);

        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
            	ppAverageCurrentVsRate[side][plane][RPC-1]= new TGraph();
            	ppAverageCurrentVsRate[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                ppAverageCurrentVsRate[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                ppAverageCurrentVsRate[side][plane][RPC-1]->SetMarkerStyle(20);
                ppAverageCurrentVsRate[side][plane][RPC-1]->SetMarkerSize(0.27);
                ppAverageCurrentVsRate[side][plane][RPC-1]->SetTitle(Form("pp_average_current_vs_rate_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
            	PbPbAverageCurrentVsRate[side][plane][RPC-1]= new TGraph();
            	PbPbAverageCurrentVsRate[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                PbPbAverageCurrentVsRate[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                PbPbAverageCurrentVsRate[side][plane][RPC-1]->SetMarkerStyle(20);
                PbPbAverageCurrentVsRate[side][plane][RPC-1]->SetMarkerSize(0.27);
                PbPbAverageCurrentVsRate[side][plane][RPC-1]->SetTitle(Form("PbPb_average_current_vs_rate_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
            	integratedCharge[side][plane][RPC-1]= new TGraph();
            	integratedCharge[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                integratedCharge[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                integratedCharge[side][plane][RPC-1]->SetMarkerStyle(20);
                integratedCharge[side][plane][RPC-1]->SetMarkerSize(0.27);
                integratedCharge[side][plane][RPC-1]->SetTitle(Form("integrated_charge_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                integratedCharge[side][plane][RPC-1]->GetXaxis()->SetTitle("Epoch timestamp [s]");
                integratedCharge[side][plane][RPC-1]->GetYaxis()->SetTitle("Integrated charge [uC]");
                darkCurrentVariationVsCharge[side][plane][RPC-1]= new TGraph();
                darkCurrentVariationVsCharge[side][plane][RPC-1]->SetName(Form("Dark_current_variations_vs_integrated_charge_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                darkCurrentVariationVsCharge[side][plane][RPC-1]->SetTitle(Form("Dark_current_variations_vs_integrated_charge_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                darkCurrentVariationVsCharge[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                darkCurrentVariationVsCharge[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                darkCurrentVariationVsCharge[side][plane][RPC-1]->SetMarkerStyle(markerStyles[plane]);
                darkCurrentVariationVsCharge[side][plane][RPC-1]->SetMarkerSize(0.27);
                darkRateVariationVsCharge[side][plane][RPC-1]= new TGraph();
                darkRateVariationVsCharge[side][plane][RPC-1]->SetName(Form("Dark_rate_variations_vs_integrated_charge_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                darkRateVariationVsCharge[side][plane][RPC-1]->SetTitle(Form("Dark_current_variations_vs_integrated_charge_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                darkRateVariationVsCharge[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                darkRateVariationVsCharge[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                darkRateVariationVsCharge[side][plane][RPC-1]->SetMarkerStyle(markerStyles[plane]);
                darkRateVariationVsCharge[side][plane][RPC-1]->SetMarkerSize(0.27);
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]= new TGraph();
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetName(Form("Dark_current_variations_vs_Dark_rate_variation_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetTitle(Form("Dark_current_variations_vs_integrated_charge_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetMarkerStyle(markerStyles[plane]);
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetMarkerSize(0.27);

                Int_t ppCount=0;
                Int_t PbPbCount=0;
                Int_t integratedChargeCount=0;
                Int_t firstNotCalib=0;
                Double_t integratedChargeValue=0.;
                Int_t startingValues=4;
                Int_t controlStartingValues=0;
                beginningDarkCurrents[side][plane][RPC-1]=0.;
                beginningDarkRates[side][plane][RPC-1]=0.;
            	for(Int_t iList=0;iList<sortedListRunStatistics[side][plane][RPC-1]->GetEntries();iList++){
            		AliRPCRunStatistics *runStats=(AliRPCRunStatistics *)(sortedListRunStatistics[side][plane][RPC-1]->At(iList));

            		// cout<<runStats->GetRunNumber()<<endl;
            		// cout<<runStats->GetTimeStampStart()<<endl;
            		// cout<<runStats->GetTimeStampStop()<<endl;
            		// cout<<runStats->GetElapsedTime()<<endl;
            		// cout<<runStats->GetIsCalib()<<endl;
            		// cout<<runStats->GetMeanDarkCurrent()<<endl;
            		// cout<<runStats->GetMeanTotalCurrent()<<endl;
            		// cout<<runStats->GetMeanNetCurrent()<<endl;
            		// cout<<runStats->GetMeanHV()<<endl;
            		// cout<<runStats->GetTotalScalerCountsBending()<<endl;
            		// cout<<runStats->GetTotalScalerCountsNotBending()<<endl;
            		// cout<<runStats->GetMeanRateBending()<<endl;
            		// cout<<runStats->GetMeanRateNotBending()<<endl<<endl<<endl;

                    if(runStats->GetMeanDarkCurrent()==0.){
                        firstNotCalib++;
                        //continue;
                    }

                    //if(runStats->GetMeanRateBending()==0 || runStats->GetMeanRateNotBending()==0) continue;

            		if(runStats->GetRunNumber()<firstPbPbRun) ppAverageCurrentVsRate[side][plane][RPC-1]->SetPoint(ppCount++, runStats->GetMeanRateNotBending()/Areas[RPC-1][plane], runStats->GetMeanNetCurrent());
        			else PbPbAverageCurrentVsRate[side][plane][RPC-1]->SetPoint(ppCount++, runStats->GetMeanRateNotBending()/Areas[RPC-1][plane], runStats->GetMeanNetCurrent());
        			
                    integratedChargeValue+=runStats->GetElapsedTime()*runStats->GetMeanNetCurrent();
                    cout<<runStats->GetTimeStampStop()<<"\t\t"<<runStats->GetMeanDarkCurrent()<<"\t"<<integratedChargeValue<<"\t"<<runStats->GetRunNumber()<<endl;
        			integratedCharge[side][plane][RPC-1]->SetPoint(integratedChargeCount++, runStats->GetTimeStampStart(), integratedChargeValue);

            		if(iList==firstNotCalib && controlStartingValues<=startingValues){
                        //cout<<"setting beginning"<<endl;
            			beginningDarkCurrents[side][plane][RPC-1]+=runStats->GetMeanDarkCurrent();
            			beginningDarkRates[side][plane][RPC-1]+=runStats->GetMeanRateNotBending();
                        controlStartingValues++;
                        if(controlStartingValues==startingValues){
                            beginningDarkCurrents[side][plane][RPC-1]/=(Double_t)startingValues;
                            beginningDarkRates[side][plane][RPC-1]/=(Double_t)startingValues;
                        }
            		} else if(iList==(sortedListRunStatistics[side][plane][RPC-1]->GetEntries())-1){
                        //cout<<"setting deltas"<<endl;
            			deltaDarkCurrents[side][plane][RPC-1]=(runStats->GetMeanDarkCurrent()-beginningDarkCurrents[side][plane][RPC-1]);///Areas[RPC-1][plane];
            			deltaDarkRates[side][plane][RPC-1]=(runStats->GetMeanRateNotBending()-beginningDarkRates[side][plane][RPC-1]);///Areas[RPC-1][plane];
                        printf("RPC%d MT%d %s DR=%f DC=%f\n",RPC,plane,sides[side].Data(),deltaDarkRates[side][plane][RPC-1],deltaDarkCurrents[side][plane][RPC-1]);
            			darkCurrentVariationVsCharge[side][plane][RPC-1]->SetPoint(0, integratedChargeValue/Areas[RPC-1][plane], deltaDarkCurrents[side][plane][RPC-1]/beginningDarkCurrents[side][plane][RPC-1]*100.);
                        darkRateVariationVsCharge[side][plane][RPC-1]->SetPoint(0, integratedChargeValue/Areas[RPC-1][plane], deltaDarkRates[side][plane][RPC-1]/beginningDarkRates[side][plane][RPC-1]*100.);
            			darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetPoint(0, deltaDarkRates[side][plane][RPC-1]/beginningDarkRates[side][plane][RPC-1]*100., deltaDarkCurrents[side][plane][RPC-1]/beginningDarkCurrents[side][plane][RPC-1]*100.);
            		}

        			if(runStats->GetRunNumber()==ppRunForCurrentDistribution) ppCurrentDistribution->Fill(runStats->GetMeanNetCurrent()/Areas[RPC-1][plane]);
        			//if(iList==0) ppCurrentDistribution->Fill(runStats->GetMeanNetCurrent()/Areas[RPC-1][plane]);
        			if(runStats->GetRunNumber()==PbPbRunForCurrentDistribution) PbPbCurrentDistribution->Fill(runStats->GetMeanNetCurrent()/Areas[RPC-1][plane]);
        			if(runStats->GetRunNumber()==pPbRunForCurrentDistribution && pPbRunForCurrentDistribution!=0) pPbCurrentDistribution->Fill(runStats->GetMeanNetCurrent()/Areas[RPC-1][plane]);

        			if(runStats->GetRunNumber()==startRunDarkDistribution){
                        cout<<"start dark distribution"<<endl;
        				beginningDarkRateDistribution->Fill(runStats->GetMeanRateNotBending()/Areas[RPC-1][plane]);
        				beginningDarkCurrentDistribution->Fill(runStats->GetMeanDarkCurrent());
        			}

        			if(runStats->GetRunNumber()==stopRunDarkDistribution){
                        cout<<"stop dark distribution"<<endl;
        				endDarkRateDistribution->Fill(runStats->GetMeanRateNotBending()/Areas[RPC-1][plane]);
        				endDarkCurrentDistribution->Fill(runStats->GetMeanDarkCurrent());
        			}
            	}

            	if(RPC==1){
            		canvppCurrentVsRate[side]->cd(plane+1);
                    ppAverageCurrentVsRate[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0., 30.);
            		ppAverageCurrentVsRate[side][plane][RPC-1]->Draw("ap");
            		canvPbPbCurrentVsRate[side]->cd(plane+1);
            		PbPbAverageCurrentVsRate[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0., 30.);
                    PbPbAverageCurrentVsRate[side][plane][RPC-1]->Draw("ap");
            		canvIntegratedCharge[side]->cd(plane+1);
                    integratedCharge[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0., 1650000.);
            		integratedCharge[side][plane][RPC-1]->Draw("alp");
            	}else{
					canvppCurrentVsRate[side]->cd(plane+1);
            		ppAverageCurrentVsRate[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0., 30.);
                    ppAverageCurrentVsRate[side][plane][RPC-1]->Draw("p");
            		canvPbPbCurrentVsRate[side]->cd(plane+1);
            		PbPbAverageCurrentVsRate[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0., 30.);
                    PbPbAverageCurrentVsRate[side][plane][RPC-1]->Draw("p");
            		canvIntegratedCharge[side]->cd(plane+1);
                    integratedCharge[side][plane][RPC-1]->GetYaxis()->SetRangeUser(0., 1650000.);
            		integratedCharge[side][plane][RPC-1]->Draw("lp");
            	}

                //darkCurrentVariationVsCharge[side][plane][RPC-1]->GetXaxis()->SetTitle("Integrated charge [#muC/cm^{2}]");
                //darkCurrentVariationVsCharge[side][plane][RPC-1]->GetYaxis()->SetTitle("#frac{#Deltai_{Dark}}{i_{Dark_{0}}} [%]");
                //darkCurrentVariationVsCharge[side][plane][RPC-1]->GetXaxis()->SetRangeUser(0., 160.);
                //darkCurrentVariationVsCharge[side][plane][RPC-1]->GetYaxis()->SetRangeUser(-250., 2000.);
                darkCurrentVariationVsChargeMulti->Add(darkCurrentVariationVsCharge[side][plane][RPC-1]);
            }
        }
    }
    //canvVariations->Dump();
    //canvVariations->cd(2);
    //darkRateVariationVsCharge->Draw("ap");
    //canvVariations->cd(3);
    //darkCurrentVariationVsDarkRateVariation->Draw("ap");
    canvVariations->cd();
    darkCurrentVariationVsChargeMulti->Draw("AP");
    darkCurrentVariationVsChargeMulti->GetYaxis()->SetRangeUser(-250., 2000.);
    darkCurrentVariationVsChargeMulti->GetXaxis()->SetTitle("Integrated charge [#muC/cm^{2}]");
    darkCurrentVariationVsChargeMulti->GetYaxis()->SetTitle("#frac{#Deltai_{Dark}}{i_{Dark_{0}}} [%]");
    gPad->Modified();
    gPad->Update();

    if(pPbRunForCurrentDistribution!=0){
        canvCurrentDistribution->cd(1);
        ppCurrentDistribution->Draw();
        canvCurrentDistribution->cd(2);
        pPbCurrentDistribution->Draw();
        canvCurrentDistribution->cd(3);
        PbPbCurrentDistribution->Draw();        
    } else {
        canvCurrentDistribution->cd(1);
        ppCurrentDistribution->Draw();
        canvCurrentDistribution->cd(2);
        PbPbCurrentDistribution->Draw();
    }


    canvDarkRateDistribution->cd(1);
    beginningDarkRateDistribution->Draw();
    canvDarkRateDistribution->cd(2);
    endDarkRateDistribution->Draw();

    canvDarkCurrentDistribution->cd(1);
    beginningDarkCurrentDistribution->Draw();
    canvDarkCurrentDistribution->cd(2);
    endDarkCurrentDistribution->Draw();
}