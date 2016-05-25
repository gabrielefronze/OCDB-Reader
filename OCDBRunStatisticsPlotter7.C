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
#include "AliRPCData.h"
#include <vector>
#include <iterator>
#include <fstream>
#include <iostream>

using namespace std;

bool AlreadyThere(Int_t runNumber, vector<Int_t> &vect){
    for(std::vector<Int_t>::iterator it = vect.begin(); it != vect.end(); ++it){
        if(*it==runNumber){
            //cout<<"FOUND "<<runNumber<<endl;
            return true;
        }
    }
    return false;
}

void AddIfNotYet(Int_t runNumber, vector<Int_t> &vect){

    if(!(AlreadyThere(runNumber, vect))){
        vect.push_back(runNumber);
        cout<<"added "<<runNumber<<endl;
        return;
    }
}

void OCDBRunStatisticsPlotter7(TString inputFileNamePhysRuns, TString inputFileNameCalibRuns, TString AMANDAChargesFile, TString PbPbCurrentsFile){
	TFile *inputFilePhysRuns=new TFile(inputFileNamePhysRuns.Data());
    TFile *inputFileCalibRuns=new TFile(inputFileNameCalibRuns.Data());

	inputFileNamePhysRuns.ReplaceAll(".root", "");
    inputFileNameCalibRuns.ReplaceAll(".root", "");
    TFile *output=new TFile(Form("Run_Plots_%s+%s4.root",inputFileNamePhysRuns.Data(),inputFileNameCalibRuns.Data()),"RECREATE");

    vector<Int_t> runNumbers;

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
    Int_t PbPbRunForCurrentDistribution=246541;

    //run number dei run dei quali plottare la distribuzione di dark current
    Int_t startRunDarkCurrentDistribution=223472;
	Int_t stopRunDarkCurrentDistribution=244217;

    //run number dei run dei quali plottare la distribuzione di dark rate
    Int_t startRunDarkRateDistribution=219635;
    Int_t stopRunDarkRateDistribution=244224;

    Int_t color[nRPC] = {kBlack,kRed,kGreen,kBlue,kYellow,kMagenta,kCyan,kGray,kOrange};
    Int_t markerStyles[nPlanes]={20,24,21,25};

    Double_t integratedCharges[nSides][nPlanes][nRPC];

    Int_t mts[23];
    mts[11]=0;
    mts[12]=1;
    mts[21]=2;
    mts[22]=3;

    ifstream fin;
    fin.open(AMANDAChargesFile.Data());
    char line[256];
    while(!fin.eof()){
        if(fin.eof())break;
        fin.getline(line,256);
        sscanf(line,"%d %d %d %lf",&AMANDAside,&AMANDAmt,&AMANDArpc,&AMANDAcharge);
        integratedCharges[AMANDAside][mts[AMANDAmt]][AMANDArpc-1]=AMANDAcharge;
        //printf("%d %d %d %lf\n",AMANDAside,AMANDAmt,AMANDArpc,AMANDAcharge);
    }
    Printf("\t\t Parsing ended.\n");
    fin.close();

    Double_t PbPbCurrents[nSides][nPlanes][nRPC];

	// instanze di canvas e grafici per l'output
	TCanvas *canvCurrentDistribution=new TCanvas("Current_distributions","Current_distributions");
	if(pPbRunForCurrentDistribution!=0) canvCurrentDistribution->Divide(3);
    else canvCurrentDistribution->Divide(2);
	TH1D *ppCurrentDistribution=new TH1D("pp_current_distribution", "pp_current_distribution", 24, 0., .0015);
	if(pPbRunForCurrentDistribution!=0) TH1D *pPbCurrentDistribution=new TH1D("pPb_current_distribution", "pPb_current_distribution", 24, 0., .0015);
	TH1D *PbPbCurrentDistribution=new TH1D("PbPb_current_distribution", "PbPb_current_distribution", 24, 0., .0015);

    ifstream fin2;
    fin2.open(PbPbCurrentsFile.Data());
    char line[256];
    while(!fin2.eof()){
        if(fin2.eof())break;
        fin2.getline(line,256);
        sscanf(line,"%d %d %d %lf",&AMANDAside,&AMANDAmt,&AMANDArpc,&AMANDAcharge);
        //PbPbCurrents[AMANDAside][mts[AMANDAmt]][AMANDArpc-1]=AMANDAcharge;
        //PbPbCurrentDistribution->Fill(PbPbCurrents[AMANDAside][mts[AMANDAmt]][AMANDArpc-1]);
        //printf("%d %d %d %lf\n",AMANDAside,AMANDAmt,AMANDArpc,AMANDAcharge);
    }
    Printf("\t\t Parsing ended.\n");
    fin2.close();

	TCanvas *canvppCurrentVsRate[nSides];
	TGraph *ppAverageCurrentVsRate[nSides][nPlanes][nRPC];
    TGraph *ppMeanAverageCurrentVsRate=new TGraph();
    ppMeanAverageCurrentVsRate->SetName("pp_mean_average_current_vs_rate");
    ppMeanAverageCurrentVsRate->SetTitle("Mean average net current vs rate - pp collisions");
    ppMeanAverageCurrentVsRate->SetLineColor(kPink);
    ppMeanAverageCurrentVsRate->SetMarkerColor(kPink);
    ppMeanAverageCurrentVsRate->SetMarkerStyle(21);
    ppMeanAverageCurrentVsRate->SetMarkerSize(0.3);
    TMultiGraph *ppAverageCurrentVsRateMulti[nSides][nPlanes];//=new TMultiGraph("pp_average_net_current_vs_rate","pp Average i_{Net} vs Rate");

	TCanvas *canvPbPbCurrentVsRate[nSides];
	TGraph *PbPbAverageCurrentVsRate[nSides][nPlanes][nRPC];
    TGraph *PbPbMeanAverageCurrentVsRate=new TGraph();
    PbPbMeanAverageCurrentVsRate->SetName("PbPb_mean_average_current_vs_rate");
    PbPbMeanAverageCurrentVsRate->SetTitle("Mean average net current vs rate - PbPb collisions");
    PbPbMeanAverageCurrentVsRate->SetLineColor(kPink);
    PbPbMeanAverageCurrentVsRate->SetMarkerColor(kPink);
    PbPbMeanAverageCurrentVsRate->SetMarkerStyle(21);
    PbPbMeanAverageCurrentVsRate->SetMarkerSize(0.3);
    TMultiGraph *PbPbAverageCurrentVsRateMulti[nSides][nPlanes];//=new TMultiGraph("PbPb_average_net_current_vs_rate","PbPb Average i_{Net} vs Rate");

    TCanvas *canvppDarkCurrentVsHV[nSides];
    TGraph *ppAverageDarkCurrentVsHV[nSides][nPlanes][nRPC];
    TGraph *ppMeanAverageDarkCurrentVsHV=new TGraph();
    ppMeanAverageDarkCurrentVsHV->SetName("pp mean average dark_current vs HV");
    ppMeanAverageDarkCurrentVsHV->SetTitle("pp_mean_average_dark_current_vs_HV");
    ppMeanAverageDarkCurrentVsHV->SetLineColor(kPink);
    ppMeanAverageDarkCurrentVsHV->SetMarkerColor(kPink);
    ppMeanAverageDarkCurrentVsHV->SetMarkerStyle(21);
    ppMeanAverageDarkCurrentVsHV->SetMarkerSize(0.3);
    TMultiGraph *ppAverageDarkCurrentVsHVMulti[nSides][nPlanes];//=new TMultiGraph("pp_average_dark_net_current_vs_HV","pp Average i_{Net} vs HV");

    TCanvas *canvPbPbDarkCurrentVsHV[nSides];
    TGraph *PbPbAverageDarkCurrentVsHV[nSides][nPlanes][nRPC];
    TGraph *PbPbMeanAverageDarkCurrentVsHV=new TGraph();
    PbPbMeanAverageDarkCurrentVsHV->SetName("PbPb mean average dark_current vs_HV");
    PbPbMeanAverageDarkCurrentVsHV->SetTitle("PbPb_mean_average_dark_current_vs_HV");
    PbPbMeanAverageDarkCurrentVsHV->SetLineColor(kPink);
    PbPbMeanAverageDarkCurrentVsHV->SetMarkerColor(kPink);
    PbPbMeanAverageDarkCurrentVsHV->SetMarkerStyle(21);
    PbPbMeanAverageDarkCurrentVsHV->SetMarkerSize(0.3);
    TMultiGraph *PbPbAverageDarkCurrentVsHVMulti[nSides][nPlanes];//=new TMultiGraph("PbPb_average_net_current_vs_rate","PbPb Average i_{Net} vs Rate");

	TCanvas *canvIntegratedCharge[nSides];
	TGraph *integratedCharge[nSides][nPlanes][nRPC];
    TGraph *meanIntegratedCharge=new TGraph();
    meanIntegratedCharge->SetName("mean integrated current vs time");
    meanIntegratedCharge->SetTitle("mean_integrated_current_vs_time");
    meanIntegratedCharge->SetLineColor(kPink);
    meanIntegratedCharge->SetMarkerColor(kPink);
    meanIntegratedCharge->SetMarkerStyle(21);
    meanIntegratedCharge->SetMarkerSize(0.3);
    TMultiGraph *integratedChargeMulti[nSides][nPlanes];//=new TMultiGraph("integrated_charge_vs_time","Integrated charge vs Time");

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
	TH1D *beginningDarkRateDistribution=new TH1D("beginning_dark_rate_distribution", "beginning_dark_rate_distribution", 25, 0., 1.5);
	TH1D *endDarkRateDistribution=new TH1D("end_dark_rate_distribution", "end_dark_rate_distribution", 25, 0., 1.5);

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
    canvVariations->Divide(3);
	TGraph *darkCurrentVariationVsCharge[nSides][nPlanes][nRPC];
    TMultiGraph *darkCurrentVariationVsChargeMulti=new TMultiGraph("variations 1", "#frac{#Deltai_{Dark}}{i_{Dark_{0}}} [%] vs Integrated charge");
    TGraph *darkRateVariationVsCharge[nSides][nPlanes][nRPC];
    TMultiGraph *darkRateVariationVsChargeMulti=new TMultiGraph("variations 2", "#frac{#Deltarate}{rate_{0}} [%] vs Integrated charge");
	TGraph *darkCurrentVariationVsDarkRateVariation[nSides][nPlanes][nRPC];
    TMultiGraph *darkCurrentVariationVsDarkRateVariationMulti=new TMultiGraph("variations 3", "#frac{#Deltarate}{rate_{0}} [%] vs #frac{#Deltai_{Dark}}{i_{Dark_{0}}} [%]");


	TList *sortedListRunStatistics[nSides][nPlanes][nRPC];
    AliRPCData *RPCData=new AliRPCData();

	Double_t beginningDarkCurrents[nSides][nPlanes][nRPC];
	Double_t deltaDarkCurrents[nSides][nPlanes][nRPC];
	Double_t beginningDarkRates[nSides][nPlanes][nRPC];
	Double_t deltaDarkRates[nSides][nPlanes][nRPC];

    for (Int_t side=0; side<nSides; side++) {
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                sortedListRunStatistics[side][plane][RPC-1]=new TList();
                TList *dummyListCalibData=new TList();
                //cout<<"ciao"<<endl;
            	inputFilePhysRuns->GetObject(Form("Run_Stati_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC),sortedListRunStatistics[side][plane][RPC-1]);
                if(!inputFileCalibRuns) continue;
                inputFileCalibRuns->GetObject(Form("Run_Stati_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC),dummyListCalibData);
            	if(!sortedListRunStatistics[side][plane][RPC-1]){cout<<"PROBLEM PHYS"<<endl; continue;}
                if(!dummyListCalibData){cout<<"PROBLEM CALIB"<<endl; continue;}

                for(Int_t iDummyList=0; iDummyList<dummyListCalibData->GetEntries();iDummyList++){
                    sortedListRunStatistics[side][plane][RPC-1]->Add((AliRPCRunStatistics*)dummyListCalibData->At(iDummyList));
                }

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
            ppAverageCurrentVsRateMulti[side][plane]= new TMultiGraph(Form("pp_average_net_current_vs_rate_MT%d_%s",planes[plane],sides[side].Data()),Form("pp Average i_{Net} vs Rate MT%d %s",planes[plane],sides[side].Data()));
            PbPbAverageCurrentVsRateMulti[side][plane]= new TMultiGraph(Form("PbPb_average_net_current_vs_rate_MT%d_%s",planes[plane],sides[side].Data()),Form("PbPb Average i_{Net} vs Rate MT%d %s",planes[plane],sides[side].Data()));
            ppAverageDarkCurrentVsHVMulti[side][plane]= new TMultiGraph(Form("pp_average_dark_current_vs_HV_MT%d_%s",planes[plane],sides[side].Data()),Form("pp average dark current vs HV MT%d %s",planes[plane],sides[side].Data()));
            PbPbAverageDarkCurrentVsHVMulti[side][plane]= new TMultiGraph(Form("PbPb_average_dark_current_vs_HV_MT%d_%s",planes[plane],sides[side].Data()),Form("PbPb average dark current vs HV MT%d %s",planes[plane],sides[side].Data()));
            integratedChargeMulti[side][plane]= new TMultiGraph(Form("integrated_charge_vs_time_MT%d_%s",planes[plane],sides[side].Data()),Form("Integrated charge vs Time MT%d %s",planes[plane],sides[side].Data()));
            
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                printf("%d %d %d\n",side,plane,RPC);
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

                ppAverageDarkCurrentVsHV[side][plane][RPC-1]= new TGraph();
                ppAverageDarkCurrentVsHV[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                ppAverageDarkCurrentVsHV[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                ppAverageDarkCurrentVsHV[side][plane][RPC-1]->SetMarkerStyle(20);
                ppAverageDarkCurrentVsHV[side][plane][RPC-1]->SetMarkerSize(0.27);
                ppAverageDarkCurrentVsHV[side][plane][RPC-1]->SetTitle(Form("pp_average_dark_current_vs_HV_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                PbPbAverageDarkCurrentVsHV[side][plane][RPC-1]= new TGraph();
                PbPbAverageDarkCurrentVsHV[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                PbPbAverageDarkCurrentVsHV[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                PbPbAverageDarkCurrentVsHV[side][plane][RPC-1]->SetMarkerStyle(20);
                PbPbAverageDarkCurrentVsHV[side][plane][RPC-1]->SetMarkerSize(0.27);
                PbPbAverageDarkCurrentVsHV[side][plane][RPC-1]->SetTitle(Form("PbPb_average_dark_current_vs_HV_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));

            	integratedCharge[side][plane][RPC-1]= new TGraph();
            	integratedCharge[side][plane][RPC-1]->SetLineColor(color[RPC-1]);
                integratedCharge[side][plane][RPC-1]->SetMarkerColor(color[RPC-1]);
                integratedCharge[side][plane][RPC-1]->SetMarkerStyle(20);
                integratedCharge[side][plane][RPC-1]->SetMarkerSize(0.4);
                integratedCharge[side][plane][RPC-1]->SetTitle(Form("integrated_charge_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                integratedCharge[side][plane][RPC-1]->GetXaxis()->SetTitle("Epoch timestamp [s]");
                integratedCharge[side][plane][RPC-1]->GetYaxis()->SetTitle("Integrated charge [uC]");
                darkCurrentVariationVsCharge[side][plane][RPC-1]= new TGraph();
                darkCurrentVariationVsCharge[side][plane][RPC-1]->SetName(Form("Dark_current_variations_vs_integrated_charge_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                darkCurrentVariationVsCharge[side][plane][RPC-1]->SetTitle(Form("Dark_current_variations_vs_integrated_charge_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                darkCurrentVariationVsCharge[side][plane][RPC-1]->SetLineColor(kBlue+2);
                darkCurrentVariationVsCharge[side][plane][RPC-1]->SetMarkerColor(kBlue+2);
                darkCurrentVariationVsCharge[side][plane][RPC-1]->SetMarkerStyle(20);
                darkCurrentVariationVsCharge[side][plane][RPC-1]->SetMarkerSize(0.4);
                darkRateVariationVsCharge[side][plane][RPC-1]= new TGraph();
                darkRateVariationVsCharge[side][plane][RPC-1]->SetName(Form("Dark_rate_variations_vs_integrated_charge_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                darkRateVariationVsCharge[side][plane][RPC-1]->SetTitle(Form("Dark_current_variations_vs_integrated_charge_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                darkRateVariationVsCharge[side][plane][RPC-1]->SetLineColor(kBlue+2);
                darkRateVariationVsCharge[side][plane][RPC-1]->SetMarkerColor(kBlue+2);
                darkRateVariationVsCharge[side][plane][RPC-1]->SetMarkerStyle(20);
                darkRateVariationVsCharge[side][plane][RPC-1]->SetMarkerSize(0.4);
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]= new TGraph();
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetName(Form("Dark_current_variations_vs_Dark_rate_variation_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetTitle(Form("Dark_current_variations_vs_integrated_charge_MT%d_%s_RPC%d",planes[plane],sides[side].Data(),RPC));
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetLineColor(kBlue+2);
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetMarkerColor(kBlue+2);
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetMarkerStyle(20);
                darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetMarkerSize(0.4);

                Int_t ppCount=0;
                Int_t PbPbCount=0;
                Int_t ppCount1=0;
                Int_t PbPbCount1=0;
                Int_t integratedChargeCount=0;
                Int_t firstNotCalib=0;
                Double_t integratedChargeValue=0.;
                Int_t valuesCurrent=4;
                Int_t valuesRate=4;
                Int_t controlStartingValuesCurrent=0;
                Int_t controlStartingValuesRate=0;
                beginningDarkCurrents[side][plane][RPC-1]=0.;
                beginningDarkRates[side][plane][RPC-1]=0.;
                TIter iterRunStats(sortedListRunStatistics[side][plane][RPC-1]);
                AliRPCRunStatistics *runStats;
                while(runStats=(AliRPCRunStatistics *)iterRunStats()){
            	//for(Int_t iList=0;iList<sortedListRunStatistics[side][plane][RPC-1]->GetEntries();iList++){
            		//AliRPCRunStatistics *runStats=(AliRPCRunStatistics *)(sortedListRunStatistics[side][plane][RPC-1]->At(iList));

                    if(runStats->GetRunNumber()==245829) continue;

                    AddIfNotYet(runStats->GetRunNumber(),runNumbers);
                    RPCData->AddRunStatistics(plane, side, RPC-1, runStats);

                    if(controlStartingValuesCurrent<=valuesCurrent || controlStartingValuesRate<=valuesRate){
                        //cout<<"setting beginning"<<endl;
                        if(controlStartingValuesCurrent==valuesCurrent){
                            beginningDarkCurrents[side][plane][RPC-1]/=(Double_t)valuesCurrent;
                            if(beginningDarkCurrents[side][plane][RPC-1]==0.){
                                cout<<"ERROR currents"<<endl;
                                controlStartingValuesCurrent=0;
                            }
                        }
                        if(controlStartingValuesRate==valuesRate){
                            beginningDarkRates[side][plane][RPC-1]/=(Double_t)valuesRate;
                            if(beginningDarkRates[side][plane][RPC-1]==0.){
                                cout<<"ERROR rates"<<endl;
                                controlStartingValuesRate=0;
                            }
                        }                        
                        if(runStats->GetMeanTotalCurrent()!=0.){
                            beginningDarkCurrents[side][plane][RPC-1]+=runStats->GetMeanTotalCurrent();
                            controlStartingValuesCurrent++;
                        }
                        if(runStats->GetMeanRateNotBending()!=0.){
                            beginningDarkRates[side][plane][RPC-1]+=runStats->GetMeanRateNotBending();
                            controlStartingValuesRate++;
                        }
                    }

                    if(plane==3 && side==0 && RPC==2) cout<<"beg: "<<beginningDarkCurrents[side][plane][RPC-1]<<endl;

                    // cout<<"####### "<<beginningDarkCurrents[side][plane][RPC-1]<<endl;
                    // cout<<"####### "<<beginningDarkRates[side][plane][RPC-1]<<endl;

                    if(runStats->GetMeanRateNotBending()!=0.){
                        if(runStats->GetRunNumber()<firstPbPbRun){
                            if(runStats->GetMeanRateNotBending()!=0.)ppAverageCurrentVsRate[side][plane][RPC-1]->SetPoint(ppCount++, runStats->GetMeanRateNotBending()/Areas[RPC-1][plane], runStats->GetMeanNetCurrent()/Areas[RPC-1][plane]);
                            if(runStats->GetMeanDarkCurrent()!=0.)ppAverageDarkCurrentVsHV[side][plane][RPC-1]->SetPoint(ppCount1++, runStats->GetMeanHV(), runStats->GetMeanDarkCurrent()/Areas[RPC-1][plane]);
                        }
                        else{
                            if(runStats->GetMeanRateNotBending()!=0.)PbPbAverageCurrentVsRate[side][plane][RPC-1]->SetPoint(PbPbCount++, runStats->GetMeanRateNotBending()/Areas[RPC-1][plane], runStats->GetMeanNetCurrent()/Areas[RPC-1][plane]);
                            if(runStats->GetMeanDarkCurrent()!=0.)PbPbAverageDarkCurrentVsHV[side][plane][RPC-1]->SetPoint(PbPbCount1++, runStats->GetMeanHV(), runStats->GetMeanDarkCurrent()/Areas[RPC-1][plane]);
                        }
                    
                    }

                    integratedChargeValue+=runStats->GetElapsedTime()*runStats->GetMeanNetCurrent();
                    //cout<<runStats->GetTimeStampStop()<<"\t\t"<<runStats->GetMeanDarkCurrent()<<"\t"<<integratedChargeValue<<"\t"<<runStats->GetRunNumber()<<endl;
        			integratedCharge[side][plane][RPC-1]->SetPoint(integratedChargeCount++, runStats->GetTimeStampStart(), integratedChargeValue/Areas[RPC-1][plane]);

        			if(runStats->GetRunNumber()==ppRunForCurrentDistribution) ppCurrentDistribution->Fill(runStats->GetMeanNetCurrent()/Areas[RPC-1][plane]);
        			if(runStats->GetRunNumber()==PbPbRunForCurrentDistribution) PbPbCurrentDistribution->Fill(runStats->GetMeanNetCurrent()/Areas[RPC-1][plane]);
        			if(runStats->GetRunNumber()==pPbRunForCurrentDistribution && pPbRunForCurrentDistribution!=0) pPbCurrentDistribution->Fill(runStats->GetMeanNetCurrent()/Areas[RPC-1][plane]);

        			if(runStats->GetRunNumber()==startRunDarkCurrentDistribution){
                        //cout<<"start dark distribution"<<endl;
        				beginningDarkCurrentDistribution->Fill(runStats->GetMeanDarkCurrent());
        			}

        			if(runStats->GetRunNumber()==stopRunDarkCurrentDistribution){
                        //cout<<"stop dark distribution"<<endl;
        				endDarkCurrentDistribution->Fill(runStats->GetMeanDarkCurrent());
        			}
                    //cout<<runStats->GetRunNumber()<<endl;
                    if(runStats->GetRunNumber()==startRunDarkRateDistribution){
                        cout<<"start dark distribution"<<endl;
                        beginningDarkRateDistribution->Fill(runStats->GetMeanRateNotBending()/Areas[RPC-1][plane]);
                    }

                    if(runStats->GetRunNumber()==stopRunDarkRateDistribution){
                        cout<<"stop dark distribution"<<endl;
                        endDarkRateDistribution->Fill(runStats->GetMeanRateNotBending()/Areas[RPC-1][plane]);
                    }
            	}

                Int_t controlEndingValuesCurrent=0;
                Int_t controlEndingValuesRate=0;
                deltaDarkCurrents[side][plane][RPC-1]=0.;
                deltaDarkRates[side][plane][RPC-1]=0.;
                for(Int_t iList2=sortedListRunStatistics[side][plane][RPC-1]->GetEntries()-1;iList2>0;--iList2){
                    AliRPCRunStatistics *runStats2=(AliRPCRunStatistics *)(sortedListRunStatistics[side][plane][RPC-1]->At(iList2));

                    if(controlEndingValuesCurrent<=valuesCurrent || controlEndingValuesRate<=valuesRate){
                        //cout<<"setting beginning"<<endl;
                        if(runStats2->GetMeanTotalCurrent()!=0.){
                            deltaDarkCurrents[side][plane][RPC-1]+=runStats2->GetMeanTotalCurrent();
                            controlEndingValuesCurrent++;
                        }
                        if(runStats2->GetMeanRateNotBending()!=0.){
                            deltaDarkRates[side][plane][RPC-1]+=runStats2->GetMeanRateNotBending();
                            controlEndingValuesRate++;
                        }                        
                        if(controlEndingValuesCurrent==valuesCurrent){
                            cout<<"ok1"<<endl;
                            deltaDarkCurrents[side][plane][RPC-1]/=(Double_t)valuesCurrent;
                            if(deltaDarkCurrents[side][plane][RPC-1]==0)cout<<"ERROR currents"<<endl;
                        }
                        if(controlEndingValuesCurrent==valuesRate){
                            cout<<"ok2"<<endl;
                            deltaDarkRates[side][plane][RPC-1]/=(Double_t)valuesRate;
                            if(deltaDarkRates[side][plane][RPC-1]==0)cout<<"ERROR rates"<<endl;
                        }
                    }
                }

                if(plane==3 && side==0 && RPC==2) deltaDarkCurrents[side][plane][RPC-1]=9.;

                deltaDarkCurrents[side][plane][RPC-1]-=beginningDarkCurrents[side][plane][RPC-1];
                deltaDarkRates[side][plane][RPC-1]-=beginningDarkRates[side][plane][RPC-1];
                //cout<<beginningDarkCurrents[side][plane][RPC-1]<<"\t"<<beginningDarkRates[side][plane][RPC-1]<<endl;
                //cout<<deltaDarkCurrents[side][plane][RPC-1]<<"\t"<<deltaDarkRates[side][plane][RPC-1]<<endl;
                //cout<<integratedChargeValue/Areas[RPC-1][plane]<<"\t"<<deltaDarkCurrents[side][plane][RPC-1]/beginningDarkCurrents[side][plane][RPC-1]*100.;
                //cout<<integratedChargeValue/Areas[RPC-1][plane]<<"\t"<<deltaDarkRates[side][plane][RPC-1]/beginningDarkRates[side][plane][RPC-1]*100.;
                //cout<<deltaDarkRates[side][plane][RPC-1]/beginningDarkRates[side][plane][RPC-1]*100.<<"\t"<<deltaDarkCurrents[side][plane][RPC-1]/beginningDarkCurrents[side][plane][RPC-1]*100.;

                if(beginningDarkCurrents[side][plane][RPC-1]!=0.)darkCurrentVariationVsCharge[side][plane][RPC-1]->SetPoint(0, integratedCharges[side][plane][RPC-1]/Areas[RPC-1][plane], deltaDarkCurrents[side][plane][RPC-1]/beginningDarkCurrents[side][plane][RPC-1]*100.);
                if(beginningDarkRates[side][plane][RPC-1]!=0.)darkRateVariationVsCharge[side][plane][RPC-1]->SetPoint(0, integratedCharges[side][plane][RPC-1]/Areas[RPC-1][plane], deltaDarkRates[side][plane][RPC-1]/beginningDarkRates[side][plane][RPC-1]*100.);
                if(beginningDarkRates[side][plane][RPC-1]!=0. && beginningDarkCurrents[side][plane][RPC-1]!=0.)darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]->SetPoint(0, deltaDarkRates[side][plane][RPC-1]/beginningDarkRates[side][plane][RPC-1]*100., deltaDarkCurrents[side][plane][RPC-1]/beginningDarkCurrents[side][plane][RPC-1]*100.);

                ppAverageCurrentVsRateMulti[side][plane]->Add(ppAverageCurrentVsRate[side][plane][RPC-1]);
                PbPbAverageCurrentVsRateMulti[side][plane]->Add(PbPbAverageCurrentVsRate[side][plane][RPC-1]);
                integratedChargeMulti[side][plane]->Add(integratedCharge[side][plane][RPC-1]);
                ppAverageDarkCurrentVsHVMulti[side][plane]->Add(ppAverageDarkCurrentVsHV);
                PbPbAverageDarkCurrentVsHVMulti[side][plane]->Add(PbPbAverageDarkCurrentVsHV);

                darkCurrentVariationVsChargeMulti->Add(darkCurrentVariationVsCharge[side][plane][RPC-1]);
                darkRateVariationVsChargeMulti->Add(darkRateVariationVsCharge[side][plane][RPC-1]);
                darkCurrentVariationVsDarkRateVariationMulti->Add(darkCurrentVariationVsDarkRateVariation[side][plane][RPC-1]);
            }
            canvppCurrentVsRate[side]->cd(plane+1);
            ppAverageCurrentVsRateMulti[side][plane]->Draw("AP");
            ppAverageCurrentVsRateMulti[side][plane]->GetYaxis()->SetTitle("i_{Net} [#muA/cm^{2}]");
            ppAverageCurrentVsRateMulti[side][plane]->GetXaxis()->SetTitle("Rate [Hz/cm^{2}]");
            gPad->Modified();
            gPad->Update();

            canvPbPbCurrentVsRate[side]->cd(plane+1);
            PbPbAverageCurrentVsRateMulti[side][plane]->Draw("AP");
            PbPbAverageCurrentVsRateMulti[side][plane]->GetYaxis()->SetTitle("i_{Net} [#muA/cm^{2}]");
            PbPbAverageCurrentVsRateMulti[side][plane]->GetXaxis()->SetTitle("Rate [Hz/cm^{2}]");
            gPad->Modified();
            gPad->Update();         

            canvIntegratedCharge[side]->cd(plane+1);
            integratedChargeMulti[side][plane]->Draw("AP");
            integratedChargeMulti[side][plane]->GetYaxis()->SetTitle("Integrated charge [#muC/cm^{2}]");
            integratedChargeMulti[side][plane]->GetXaxis()->SetTitle("Epoch timestamp [s]");
            gPad->Modified();
            gPad->Update();
        }
        output->cd();
        canvppCurrentVsRate[side]->Write();
        canvPbPbCurrentVsRate[side]->Write();
        canvIntegratedCharge[side]->Write();  
    }

    Double_t intCharge=0.;
    Int_t count1=0;
    Int_t count2=0;
    Int_t count=0;
    for(std::vector<Int_t>::iterator runNumberIt = runNumbers.begin(); runNumberIt != runNumbers.end(); ++runNumberIt){
        if(*runNumberIt<firstPbPbRun){
            ppMeanAverageCurrentVsRate->SetPoint(count1, RPCData->GetMeanRateNotBending(*runNumberIt,kTRUE), RPCData->GetMeanNetCurrent(*runNumberIt,kTRUE));
            ppMeanAverageDarkCurrentVsHV->SetPoint(count1, RPCData->GetMeanHV(*runNumberIt), RPCData->GetMeanDarkCurrent(*runNumberIt));
            count1++;
        }
        else {
            PbPbMeanAverageCurrentVsRate->SetPoint(count2, RPCData->GetMeanRateNotBending(*runNumberIt,kTRUE), RPCData->GetMeanNetCurrent(*runNumberIt,kTRUE));
            PbPbMeanAverageDarkCurrentVsHV->SetPoint(count2, RPCData->GetMeanHV(*runNumberIt), RPCData->GetMeanDarkCurrent(*runNumberIt));
            count2++;
        }
        intCharge+=RPCData->GetMeanIntegratedCharge(*runNumberIt);
        meanIntegratedCharge->SetPoint(count, RPCData->GetMeanTimeStampStart(*runNumberIt), intCharge);
        if(RPCData->GetMeanRateNotBending(*runNumberIt,kTRUE)!=0.)darkRateVsTime->SetPoint(count, RPCData->GetMeanTimeStampStart(*runNumberIt), RPCData->GetMeanRateNotBending(*runNumberIt,kTRUE));
        if(RPCData->GetMeanTotalCurrent(*runNumberIt,kTRUE)!=0.)darkCurrentVsTime->SetPoint(count, RPCData->GetMeanTimeStampStart(*runNumberIt), RPCData->GetMeanTotalCurrent(*runNumberIt,kTRUE));
        count++;
    }


    TCanvas *canv=new TCanvas("means");
    canv->Divide(3,2);
    canv->cd(1);
    ppMeanAverageCurrentVsRate->Draw("AP");
    ppMeanAverageCurrentVsRate->SetMarkerColor(kOrange+1);
    ppMeanAverageCurrentVsRate->SetMarkerSize(0.3);
    ppMeanAverageCurrentVsRate->GetXaxis()->SetTitle("Rate [Hz/cm^{2}]");
    ppMeanAverageCurrentVsRate->GetYaxis()->SetTitle("i_{Net} [#muA]");
    canv->cd(2);
    PbPbMeanAverageCurrentVsRate->Draw("AP");
    PbPbMeanAverageCurrentVsRate->SetMarkerColor(kOrange+1);
    PbPbMeanAverageCurrentVsRate->SetMarkerSize(0.3);
    PbPbMeanAverageCurrentVsRate->GetXaxis()->SetTitle("Rate [Hz/cm^{2}]");
    PbPbMeanAverageCurrentVsRate->GetYaxis()->SetTitle("i_{Net} [#muA]");
    canv->cd(3);
    meanIntegratedCharge->Draw("AP");
    canv->cd(4);
    ppMeanAverageDarkCurrentVsHV->Draw("AP");
    canv->cd(5);
    PbPbMeanAverageDarkCurrentVsHV->Draw("AP");
    gPad->Modified();
    gPad->Update();

    output->cd();
    canv->Write();

    //canvVariations->Dump();
    canvVariations->cd(1);
    darkCurrentVariationVsChargeMulti->Draw("AP");
    //darkCurrentVariationVsChargeMulti->GetYaxis()->SetRangeUser(-250., 2000.);
    darkCurrentVariationVsChargeMulti->GetXaxis()->SetTitle("Integrated charge [#muC/cm^{2}]");
    darkCurrentVariationVsChargeMulti->GetYaxis()->SetTitle("#frac{#Deltai_{Dark}}{i_{Dark_{0}}} [%]");
    darkCurrentVariationVsChargeMulti->GetYaxis()->SetTitleOffset(2);
    canvVariations->cd(2);
    darkRateVariationVsChargeMulti->Draw("ap");
    darkRateVariationVsChargeMulti->GetXaxis()->SetTitle("Integrated charge [#muC/cm^{2}]");
    darkRateVariationVsChargeMulti->GetYaxis()->SetTitle("#frac{#Deltarate}{rate_{0}} [%]");
    darkRateVariationVsChargeMulti->GetYaxis()->SetTitleOffset(2);
    canvVariations->cd(3);
    darkCurrentVariationVsDarkRateVariationMulti->Draw("ap");
    darkCurrentVariationVsDarkRateVariationMulti->GetXaxis()->SetTitle("#frac{#Deltarate}{rate_{0}} [%]");
    darkCurrentVariationVsDarkRateVariationMulti->GetYaxis()->SetTitle("#frac{#Deltai_{Dark}}{i_{Dark_{0}}} [%]");
    darkCurrentVariationVsDarkRateVariationMulti->GetYaxis()->SetTitleOffset(2);
    gPad->Modified();
    gPad->Update();

    output->cd();
    canvVariations->Write();

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
    output->cd();
    canvCurrentDistribution->Write();


    canvDarkRateDistribution->cd(1);
    beginningDarkRateDistribution->Draw();
    canvDarkRateDistribution->cd(2);
    endDarkRateDistribution->Draw();
    output->cd();
    canvDarkRateDistribution->Write();

    canvDarkCurrentDistribution->cd(1);
    beginningDarkCurrentDistribution->Draw();
    canvDarkCurrentDistribution->cd(2);
    endDarkCurrentDistribution->Draw();
    output->cd();
    canvDarkCurrentDistribution->Write();

    canvRateVsTime->cd();
    darkRateVsTime->Draw("ap");
    canvDarkCurrentVsTime->cd();
    darkCurrentVsTime->Draw("ap");
    output->cd();
    canvRateVsTime->Write();
    canvDarkCurrentVsTime->Write();
}