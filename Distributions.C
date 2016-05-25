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

void Distributions(TString beginningFile, TString endFile){

    const Int_t nPlanes=4;
    
    const Int_t nRPC=9; 

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

	TCanvas *canvDarkCurrentDistribution=new TCanvas("Dark_current_distributions","Dark_current_distributions");
	canvDarkCurrentDistribution->Divide(2);

	TH1D *beginningDarkCurrentDistribution=new TH1D("beginning_dark_current_distribution", "", 12, 0., 1.2);
	TH1D *endDarkCurrentDistribution=new TH1D("end_dark_current_distribution", "", 12, 0., 1.2);
    beginningDarkCurrentDistribution->GetXaxis()->SetTitle("Dark current [nA/cm^{2}]");
    endDarkCurrentDistribution->GetXaxis()->SetTitle("Dark current [nA/cm^{2}]");
    beginningDarkCurrentDistribution->GetYaxis()->SetTitle("Entries");
    endDarkCurrentDistribution->GetYaxis()->SetTitle("Entries");

    Int_t AMANDAside=-1;
    Int_t AMANDAmt=-1;
    Int_t AMANDArpc=-1;
    Double_t AMANDAcharge=-1.;

	ifstream fin;
    fin.open(beginningFile.Data());
    char line[256];
    while(!fin.eof()){
        if(fin.eof())break;
        fin.getline(line,256);
        sscanf(line,"%d %d %d %lf",&AMANDAside,&AMANDAmt,&AMANDArpc,&AMANDAcharge);
        beginningDarkCurrentDistribution->Fill(1000*AMANDAcharge/Areas[AMANDArpc-1][AMANDAmt]);
        //printf("%d %d %d %lf\n",AMANDAside,AMANDAmt,AMANDArpc,AMANDAcharge);
    }
    Printf("\t\t Parsing ended.\n");
    fin.close();

	ifstream fin2;
    fin2.open(endFile.Data());
    char line2[256];
    while(!fin2.eof()){
        if(fin2.eof())break;
        fin2.getline(line2,256);
        sscanf(line2,"%d %d %d %lf",&AMANDAside,&AMANDAmt,&AMANDArpc,&AMANDAcharge);
        endDarkCurrentDistribution->Fill(1000*AMANDAcharge/Areas[AMANDArpc-1][AMANDAmt]);
        //printf("%d %d %d %lf\n",AMANDAside,AMANDAmt,AMANDArpc,AMANDAcharge);
    }
    Printf("\t\t Parsing ended.\n");
    fin2.close();

    canvDarkCurrentDistribution->cd(1);
    beginningDarkCurrentDistribution->Draw();
    canvDarkCurrentDistribution->cd(2);
	endDarkCurrentDistribution->Draw();


}