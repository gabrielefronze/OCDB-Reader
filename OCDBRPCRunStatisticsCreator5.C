//
// OCDBRPCRunStatisticsCreator2.cpp
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
#include "AliRPCOverflowStatistics.h"
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

void OCDBRPCRunStatisticsCreator5(TString inputFileName, Bool_t isDark){
    TFile *inputFile=new TFile(inputFileName.Data());

    inputFileName.ReplaceAll(".txt", "");
    TFile *output=new TFile(Form("Run_Statistics__allamax_%s.root",inputFileName.Data()),"RECREATE");

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
    
    Double_t RPCAreas[nRPC][nPlanes];
    for(Int_t iRPC=0;iRPC<nRPC;iRPC++){
        for(Int_t iPlane=0;iPlane<nPlanes;iPlane++){
            if(iRPC==5){
                if(iPlane<2){
                    RPCAreas[iRPC][iPlane]=16056.;
                } else {
                    RPCAreas[iRPC][iPlane]=18176.;
                }
            }else if(iRPC==4 || iRPC==6){
                if(iPlane<2){
                    RPCAreas[iRPC][iPlane]=19728./28.*27.;
                } else {
                    RPCAreas[iRPC][iPlane]=22338./28.*27.;
                }
            }else {
                if(iPlane<2){
                    RPCAreas[iRPC][iPlane]=19728.;
                } else {
                    RPCAreas[iRPC][iPlane]=22338.;
                }
            }
        }
    }

    const ULong64_t fullScale=65535;

    //array per la conversione di iRPC={0,17} in iRPC={1,9}x{inside,outside}
    Int_t RPCIndexes[18]={5,6,7,8,9,9,8,7,6,5,4,3,2,1,1,2,3,4};
    Int_t RPCSides[18]=  {0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0};
    Int_t LBToRPC[nLocalBoards][3]={{1,1,0},{2,2,0},{3,2,0},{4,3,0},{5,3,0},{6,4,0},{7,4,0},{8,4,0},{9,6,0},{10,6,0},{11,6,0},{12,7,0},{13,7,0},{14,8,0},{15,8,0},{16,9,0},{17,1,0},{18,2,0},{19,2,0},{20,3,0},{21,3,0},{22,4,0},{23,4,0},{24,4,0},{25,4,0},{26,5,0},{27,5,0},{28,5,0},{29,5,0},{30,6,0},{31,6,0},{32,6,0},{33,6,0},{34,7,0},{35,7,0},{36,8,0},{37,8,0},{38,9,0},{39,1,0},{40,2,0},{41,2,0},{42,3,0},{43,3,0},{44,4,0},{45,4,0},{46,4,0},{47,4,0},{48,5,0},{49,5,0},{50,5,0},{51,5,0},{52,6,0},{53,6,0},{54,6,0},{55,6,0},{56,7,0},{57,7,0},{58,8,0},{59,8,0},{60,9,0},{61,1,0},{62,2,0},{63,2,0},{64,3,0},{65,3,0},{66,4,0},{67,4,0},{68,5,0},{69,5,0},{70,6,0},{71,6,0},{72,7,0},{73,7,0},{74,8,0},{75,8,0},{76,9,0},{77,1,0},{78,2,0},{79,2,0},{80,3,0},{81,3,0},{82,4,0},{83,4,0},{84,5,0},{85,5,0},{86,6,0},{87,6,0},{88,7,0},{89,7,0},{90,8,0},{91,8,0},{92,9,0},{93,1,0},{94,2,0},{95,2,0},{96,3,0},{97,3,0},{98,4,0},{99,4,0},{100,5,0},{101,5,0},{102,6,0},{103,6,0},{104,7,0},{105,7,0},{106,8,0},{107,8,0},{108,9,0},{109,1,0},{110,2,0},{111,3,0},{112,4,0},{113,5,0},{114,6,0},{115,7,0},{116,8,0},{117,9,0},{118,1,1},{119,2,1},{120,2,1},{121,3,1},{122,3,1},{123,4,1},{124,4,1},{125,4,1},{126,6,1},{127,6,1},{128,6,1},{129,7,1},{130,7,1},{131,8,1},{132,8,1},{133,9,1},{134,1,1},{135,2,1},{136,2,1},{137,3,1},{138,3,1},{139,4,1},{140,4,1},{141,4,1},{142,4,1},{143,5,1},{144,5,1},{145,5,1},{146,5,1},{147,6,1},{148,6,1},{149,6,1},{150,6,1},{151,7,1},{152,7,1},{153,8,1},{154,8,1},{155,9,1},{156,1,1},{157,2,1},{158,2,1},{159,3,1},{160,3,1},{161,4,1},{162,4,1},{163,4,1},{164,4,1},{165,5,1},{166,5,1},{167,5,1},{168,5,1},{169,6,1},{170,6,1},{171,6,1},{172,6,1},{173,7,1},{174,7,1},{175,8,1},{176,8,1},{177,9,1},{178,1,1},{179,2,1},{180,2,1},{181,3,1},{182,3,1},{183,4,1},{184,4,1},{185,5,1},{186,5,1},{187,6,1},{188,6,1},{189,7,1},{190,7,1},{191,8,1},{192,8,1},{193,9,1},{194,1,1},{195,2,1},{196,2,1},{197,3,1},{198,3,1},{199,4,1},{200,4,1},{201,5,1},{202,5,1},{203,6,1},{204,6,1},{205,7,1},{206,7,1},{207,8,1},{208,8,1},{209,9,1},{210,1,1},{211,2,1},{212,2,1},{213,3,1},{214,3,1},{215,4,1},{216,4,1},{217,5,1},{218,5,1},{219,6,1},{220,6,1},{221,7,1},{222,7,1},{223,8,1},{224,8,1},{225,9,1},{226,1,1},{227,2,1},{228,3,1},{229,4,1},{230,5,1},{231,6,1},{232,7,1},{233,8,1},{234,9,1}};

	TList *sortedListData[nSides][nPlanes][nRPC];
	TList *sortedListScalers[nSides][nPlanes][nRPC][nCathodes];
    TSortedList *scalersLocalBoardList[nCathodes][nPlanes][nLocalBoards];

    for (Int_t side=0; side<nSides; side++) {
    	cout<<sides[side].Data()<<endl;
        for (Int_t plane=0; plane<nPlanes; plane++) {
        	cout<<"\t"<<planes[plane]<<endl;
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
            	cout<<"\t\t"<<RPC<<endl;
            	sortedListData[side][plane][RPC-1]=new TList();
				inputFile->GetObject(Form("Data_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC),sortedListData[side][plane][RPC-1]);
				if(!sortedListData[side][plane][RPC-1])cout<<"PROBLEM"<<endl;
            	for(Int_t cathode=0;cathode<nCathodes;cathode++){
            		cout<<"\t\t\t"<<cathodes[cathode].Data()<<endl;
            		sortedListScalers[side][plane][RPC-1][cathode]=new TList();
            		inputFile->GetObject(Form("Scalers_MTR_%s_%s_MT%d_RPC%d",(sides[side]).Data(),(cathodes[cathode]).Data(),planes[plane],RPC),sortedListScalers[side][plane][RPC-1][cathode]);
            		if(!sortedListScalers[side][plane][RPC-1][cathode])cout<<"PROBLEM"<<endl;
            	}
            }
        }
    }

    for(Int_t cathode=0;cathode<nCathodes;cathode++){
        for(Int_t plane=0;plane<nPlanes;plane++){
            for(Int_t local=0;local<nLocalBoards;local++){
            	scalersLocalBoardList[cathode][plane][local]=new TSortedList();
            	inputFile->GetObject(Form("Scalers_MTR_%s_MT%d_LB%d",(cathodes[cathode]).Data(),planes[plane],local+1),scalersLocalBoardList[cathode][plane][local]);
            	if(!scalersLocalBoardList[cathode][plane][local])cout<<"PROBLEM"<<endl;
            }
        }
    }

    TList *sortedListRunStatistics[nSides][nPlanes][nRPC];

    for (Int_t side=0; side<nSides; side++) {
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
            	sortedListRunStatistics[side][plane][RPC-1]=new TList();
                sortedListRunStatistics[side][plane][RPC-1]->SetName(Form("Run_Stati_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC));
                printf("getting Data_MTR_%s_MT%d_RPC%d\n",(sides[side]).Data(),planes[plane],RPC);
   				//TList *sortedListData[side][plane][RPC-1];
   				//cout<<sortedListData[side][plane][RPC-1]->GetEntries()<<endl;
                //sortedListData[side][plane][RPC-1]->Sort();

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

			    for(Int_t iDataList=0; iDataList<sortedListData[side][plane][RPC-1]->GetEntries(); iDataList++){
			    	//generica entry della sorted list
                    AliRPCValueDCS *valueDCS = ((AliRPCValueDCS*)sortedListData[side][plane][RPC-1]->At(iDataList));
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
	                        timeStampStop=valueDCS->GetTimeStamp();
	                    } else continue;                   	
                    } else if(actualRunNumber<previousRunNumber) continue;
                    else {
						Int_t nAccepted[2]={0,0};
						Int_t nRejected[2]={0,0};
						Int_t totalScalers[2]={0,0};
						Double_t rates[2]={0.,0.};

                    	for (Int_t cathode=0; cathode<nCathodes; cathode++) {
	                    	for (Int_t localBoard=1; localBoard<=nLocalBoards; localBoard++) {
		                        Int_t iRPC09=LBToRPC[localBoard-1][1];
		                        Int_t iSide=LBToRPC[localBoard-1][2];
		                        if(iSide == side && iRPC09==RPC){
		                        	for(Int_t iScaler=scalersIndex[cathode];iScaler<scalersLocalBoardList[cathode][plane][localBoard-1]->GetEntries();iScaler++){
		                        		AliRPCValueScaler *valueScaler=((AliRPCValueScaler*)scalersLocalBoardList[cathode][plane][localBoard-1]->At(iScaler));

		                        		if(valueScaler->GetRunNumber()<previousRunNumber) continue;
		                        		else if(valueScaler->GetRunNumber()>previousRunNumber) {
		                        			scalersIndex[cathode]=iScaler;
		                        			break;
		                        		}
		                        		else{
		                        			if(valueScaler->GetHasOverflow()==kTRUE){
			                        			nRejected[cathode]++;
			                        			continue;
			                        		} else {
			                        			nAccepted[cathode]++;
			                        			totalScalers[cathode]+=valueScaler->GetScalerCounts();
			                        		}
		                        		}
		                        	}
		                        }
		                    }
		                    if(nAccepted[cathode]!=nRejected[cathode]) rates[cathode]=1./630.*1/((Double_t)(nAccepted[cathode]-nRejected[cathode]))*(Double_t)totalScalers[cathode];
		                    else rates[cathode]=0.;
		                    totalScalerCounts[cathode]=(ULong64_t)(rates[cathode]*(Double_t)(timeStampStop-timeStampStart));
                            cout<<cathode<<": "<<totalScalerCounts[cathode]<<endl<<endl;
		                    //cout<<rates[cathode]<<" "<<totalScalerCounts[cathode]<<endl;
		                }



			            if(nDarkCurrent!=0)meanDarkCurrent=meanDarkCurrent/(Double_t)nDarkCurrent;
			            if(nTotalCurrent!=0)meanTotalCurrent=meanTotalCurrent/(Double_t)nTotalCurrent;
			            if(nHV!=0)meanHV=meanHV/(Double_t)nHV;
			            //cout<<"setting "<<previousRunNumber<<" complete"<<endl;
			            //cout<<totalScalerCounts[0]<<"\t"<<totalScalerCounts[1]<<endl;
			            AliRPCRunStatistics *runStats=new AliRPCRunStatistics(previousRunNumber, timeStampStart, timeStampStop, isCalib, isDark, meanDarkCurrent, meanTotalCurrent, meanHV, totalScalerCounts[0], totalScalerCounts[1]);
			            sortedListRunStatistics[side][plane][RPC-1]->Add(runStats);
			            //cout<<(ULong64_t)runStats->GetTimeStampStart()<<"\t"<<(ULong64_t)runStats->GetTimeStampStop()<<endl;
			            //cout<<(ULong64_t)runStats->GetTotalScalerCountsBending()<<"\t"<<(ULong64_t)runStats->GetTotalScalerCountsNotBending()<<endl;
			            //cout<<runStats->GetMeanRateBending()<<"\t"<<runStats->GetMeanRateNotBending()<<endl<<endl;

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
                    	nAccepted[0]=0;
						nRejected[0]=0;
						totalScalers[0]=0;
						rates[0]=0;
                    	nAccepted[1]=0;
						nRejected[1]=0;
						totalScalers[1]=0;
						rates[1]=0;
                    }  
			    }
			    output->cd();
			    cout<<sortedListRunStatistics[side][plane][RPC-1]->GetEntries()<<endl;
			    sortedListRunStatistics[side][plane][RPC-1]->Sort();
			    sortedListRunStatistics[side][plane][RPC-1]->Write(Form("Run_Stati_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC),TObject::kSingleKey);
            }
        }
    }


}