//
// OCDBRPCRunStatisticsCreator.cpp
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
#include "TGraph.h"
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

void OCDBRPCRunStatisticsCreator(TString inputFileName){
    TFile *inputFile=new TFile(inputFileName.Data());

    inputFileName.ReplaceAll(".root", "");
    TFile *output=new TFile(Form("Run_Statistics_%s.root",inputFileName.Data()),"RECREATE");

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

    TList *sortedListRunStatistics[nSides][nPlanes][nRPC];

    for (Int_t side=0; side<nSides; side++) {
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
            	sortedListRunStatistics[side][plane][RPC-1]=new TList();
                sortedListRunStatistics[side][plane][RPC-1]->SetName(Form("Run_Stati_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC));
   				TList *sortedListData;
   				printf("getting Data_MTR_%s_MT%d_RPC%d\n",(sides[side]).Data(),planes[plane],RPC);
                inputFile->GetObject(Form("Data_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC),sortedListData);
   				//cout<<sortedListData->GetEntries()<<endl;
                //sortedListData->Sort();

                UInt_t actualRunNumber=0;
                UInt_t previousRunNumber=0;
			    ULong64_t timeStampStart=0;
			    ULong64_t timeStampStop=0;
			    Bool_t isCalib=kFALSE;
			    Double_t meanDarkCurrent=0.;
			    UInt_t nDarkCurrent=0;
			    Double_t meanTotalCurrent=0.;
			    UInt_t nTotalCurrent=0;
			    Double_t meanHV=0.;
			    UInt_t nHV=0;
			    ULong64_t totalScalerCounts[2]={0,0};
			    Int_t scalersIndex[2]={0,0};

			    for(Int_t iDataList=0; iDataList<sortedListData->GetEntries(); iDataList++){
			    	//generica entry della sorted list
                    AliRPCValueDCS *valueDCS = ((AliRPCValueDCS*)sortedListData->At(iDataList));
                    if(previousRunNumber==0){
                    	previousRunNumber=valueDCS->GetRunNumber();
                    	timeStampStart=valueDCS->GetTimeStamp();
                    	isCalib=valueDCS->GetIsCalib();
                    }
                    actualRunNumber=valueDCS->GetRunNumber();
                    if(actualRunNumber==previousRunNumber){
	                    if (valueDCS->IsVoltage()){
	                        //cast a tensione
	                        AliRPCValueVoltage *voltageBuffer=(AliRPCValueVoltage*)valueDCS;
	                        meanHV+=voltageBuffer->GetVSupp();
	                        nHV++;
	                    } else if(valueDCS->IsCurrent()){
	                        //cast a corrente
	                        AliRPCValueCurrent *currentBuffer=(AliRPCValueCurrent*)valueDCS;
	                        meanTotalCurrent+=currentBuffer->GetITot();
	                        nTotalCurrent++;
	                        meanDarkCurrent+=currentBuffer->GetIDark();
	                        nDarkCurrent++;
	                    } else continue;                   	
                    } else {
                    	//cout<<"run data "<<previousRunNumber<<" ended."<<endl;
   						for(Int_t cathode=0;cathode<nCathodes;cathode++){
			                TList *sortedListScalers;
			                //printf("getting Scalers_MTR_%s_%s_MT%d_RPC%d\n",(sides[side]).Data(),(cathodes[cathode]).Data(),planes[plane],RPC);
			                inputFile->GetObject(Form("Scalers_MTR_%s_%s_MT%d_RPC%d",(sides[side]).Data(),(cathodes[cathode]).Data(),planes[plane],RPC),sortedListScalers);
			                //sortedListScalers->Sort();
			                //cout<<sortedListScalers->GetEntries()<<endl;
			                for(Int_t iScalersList=scalersIndex[cathode]; iScalersList<sortedListScalers->GetEntries(); iScalersList++){
			                	//cout<<iScalersList<<endl;
			                	AliRPCValueScaler* scalerData=(AliRPCValueScaler*)sortedListScalers->At(iScalersList);
			                	//skipping not interesting run numbers
			                	if(scalerData->GetRunNumber()<previousRunNumber) continue;
			                	//getting data from scalers belonging to the analyzed run number
			                	else if(scalerData->GetRunNumber()==previousRunNumber){
									totalScalerCounts[cathode]+=((AliRPCValueScaler*)sortedListScalers->At(iScalersList))->GetScalerCounts();
									timeStampStop=scalerData->GetTimeStamp();
								//once a new run is found break the cycle but keep track of the reached index (limits first if occurrences)
			                	} else {
			                		scalersIndex[cathode]=iScalersList;
			                		break;
			                	}
		                    }
		                    //cout<<"run scalers "<<previousRunNumber<<" ended."<<endl;
			            }
			            if(nDarkCurrent!=0)meanDarkCurrent=meanDarkCurrent/(Double_t)nDarkCurrent;
			            if(nTotalCurrent!=0)meanTotalCurrent=meanTotalCurrent/(Double_t)nTotalCurrent;
			            if(nHV!=0)meanHV=meanHV/(Double_t)nHV;
			            //cout<<"setting "<<previousRunNumber<<" complete"<<endl;
			            AliRPCRunStatistics *runStats=new AliRPCRunStatistics(previousRunNumber, timeStampStart, timeStampStop, isCalib, meanDarkCurrent, meanTotalCurrent, meanHV, totalScalerCounts[0], totalScalerCounts[1]);
			            sortedListRunStatistics[side][plane][RPC-1]->Add(runStats);

			            /*cout<<runStats->GetRunNumber()<<endl;
		        		cout<<runStats->GetTimeStampStart()<<endl;
		        		cout<<runStats->GetTimeStampStop()<<endl;
		        		cout<<runStats->GetElapsedTime()<<endl;
		        		cout<<runStats->GetIsCalib()<<endl;
		        		cout<<runStats->GetMeanDarkCurrent()<<endl;
		        		cout<<runStats->GetMeanTotalCurrent()<<endl;
		        		cout<<runStats->GetMeanNetCurrent()<<endl;
		        		cout<<runStats->GetMeanHV()<<endl;
		        		cout<<runStats->GetTotalScalerCountsBending()<<endl;
		        		cout<<runStats->GetTotalScalerCountsNotBending()<<endl;
		        		cout<<runStats->GetMeanRateBending()<<endl;
		        		cout<<runStats->GetMeanRateNotBending()<<endl<<endl<<endl;*/

			            //runStats->Dump();
			            //printf("Written run %d \tMTR %s \t\tMT%d \tRPC%d\n",previousRunNumber, (sides[side]).Data(),planes[plane],RPC);
			            actualRunNumber=0;
					    timeStampStart=0;
					    timeStampStop=0;
					    isCalib=kFALSE;
					    meanDarkCurrent=0.;
					    nDarkCurrent=0;
					    meanTotalCurrent=0.;
					    nTotalCurrent=0;
					    meanHV=0.;
					    nHV=0;
					    totalScalerCounts[0]=0;
					    totalScalerCounts[1]=0;
			            //runStats=0x0;
			            //force reset of previous and start timestamp
			            previousRunNumber=0;
			            previousRunNumber=valueDCS->GetRunNumber();
                    	timeStampStart=valueDCS->GetTimeStamp();
                    	isCalib=valueDCS->GetIsCalib();
                    }  
			    }
			    output->cd();
			    sortedListRunStatistics[side][plane][RPC-1]->Sort();
			    sortedListRunStatistics[side][plane][RPC-1]->Write(Form("Run_Stati_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC),TObject::kSingleKey);
            }
        }
    }


}