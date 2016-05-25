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
#include "AliRPCValueDCS.h"
#include "AliRPCValueCurrent.h"
#include "AliRPCValueVoltage.h"
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

void OCDBReader3(Bool_t file=kFALSE, TString filename=""){   
    
    //anno da analizzare
    Int_t year=2015;
    
    //lista dei run numbers
    vector<Int_t> runList;
    
    //inserimento dei run automatico (da file)
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
    //inserimento manuale dei run
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
    
    //flag che è kTRUE se l'evento è di calibrazione
    Bool_t isCalib=kFALSE;
    
    //manager per interfacciarsi con gli OCDB
    AliCDBManager *managerCurrent = AliCDBManager::Instance();
    AliCDBManager *managerVoltage = AliCDBManager::Instance();
    AliCDBManager *managerRunType = AliCDBManager::Instance();
    
    //inizializzazione dei manager
    managerCurrent->SetDefaultStorage(Form("alien://folder=/alice/data/%d/OCDB",year));
    managerVoltage->SetDefaultStorage(Form("alien://folder=/alice/data/%d/OCDB",year));
    managerRunType->SetDefaultStorage(Form("alien://folder=/alice/data/%d/OCDB",year));
    
    //array 3D di liste di dati. le TSortedList sono già ordinate dopo ogni inserimento
    TSortedList dataList[nSides][nPlanes][nRPC];
    
    //loop sui run inseriti
    for (std::vector<Int_t>::iterator runIterator = runList.begin(); runIterator != runList.end(); ++runIterator) {
        //i manager puntano al run desiderato
        managerCurrent->SetRun(*runIterator);
        managerVoltage->SetRun(*runIterator);
        managerRunType->SetRun(*runIterator);
        
        //inizializzazione dell'entry contente il runtype
        AliCDBEntry *entryRunType = managerRunType->Get("GRP/GRP/Data");
        if(!entryRunType) {
            printf("\nproblem with entryRunType\n");
            break;
        }
        
        //retrievering delle informazioni sul run
        AliGRPObject *grpObj=(AliGRPObject*)entryRunType->GetObject();
        TString *runType=new TString(grpObj->GetRunType());
        
        //settaggio del flag isCalib
        if(runType->Contains("PHYSICS")){
            isCalib=kFALSE;
        } else if(runType->Contains("CALIBRATION")){
            isCalib=kTRUE;
        } else {
            break;
        }

        //inizializzazione dell'entry contente i valori di corrente
        AliCDBEntry *entryCurrent = managerCurrent->Get("MUON/Calib/TriggerDCS");
        if(!entryCurrent) {
            printf("\nproblem with entryCurrent\n");
            break;
        }
        
        //mappa delle correnti
        TMap *mapCurrent = (TMap*)entryCurrent->GetObject();
        if(!mapCurrent) {
            printf("Problems getting mapCurrent\n");
            break;
        }
        
        //inizializzazione dell'entry contente i valori di tensione
        AliCDBEntry *entryVoltage = managerVoltage->Get("MUON/Calib/TriggerDCS");
        if(!entryVoltage) {
            printf("\nproblem with entryCurrent\n");
            break;
        }
        
        //mappa delle tensioni
        TMap *mapVoltage = (TMap*)entryVoltage->GetObject();
        if(!mapVoltage) {
            printf("Problems getting mapVoltage\n");
            break;
        }
        
        //loop sui piani, i lati (inside e outside) e le RPC (9 per side)
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t side=0; side<nSides; side++) {
                for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                    //creazione di un pointer all'elemento della mappa delle correnti
                    TObjArray *dataArrayCurrents;
                    dataArrayCurrents=(TObjArray*)(mapCurrent->GetValue(Form("MTR_%s_MT%d_RPC%d_HV.actual.iMon",sides[side].Data(),planes[plane],RPC)));
                    if(!dataArrayCurrents) {
                        printf(" Problems getting dataArrayCurrents\n");
                        break;
                    }
                    
                    //loop sulle entry del vettore di misure di corrente
                    for (Int_t arrayIndex=0; arrayIndex<(dataArrayCurrents->GetEntries()); arrayIndex++) {
                        //puntatore all'elemento i-esimo
                        AliDCSValue *value = (AliDCSValue*)dataArrayCurrents->At(arrayIndex);
                        //se il run è di calibrazione corrente e corrente di buoio coincidono
                        if (isCalib) {
                            dataList[side][plane][RPC-1].Add(new AliRPCValueCurrent(*runIterator,value->GetTimeStamp(),value->GetFloat(),value->GetFloat(),isCalib));
                        //altrimenti imposto la corrente di buio a 0 (la cambio dopo)
                        } else {
                            dataList[side][plane][RPC-1].Add(new AliRPCValueCurrent(*runIterator,value->GetTimeStamp(),value->GetFloat(),0.,isCalib));
                        }
                        delete value;
                    }
                    
                    //creazione di un pointer all'elemento della mappa delle tensioni
                    TObjArray *dataArrayVoltage;
                    dataArrayVoltage=(TObjArray*)(mapCurrent->GetValue(Form("MTR_%s_MT%d_RPC%d_HV.vEff",sides[side].Data(),planes[plane],RPC)));
                    if(!dataArrayVoltage) {
                        printf(" Problems getting dataArrayVoltage\n");
                        break;
                    }
                    
                    //loop sulle entry del vettore di misure di tensione
                    for (Int_t arrayIndex=0; arrayIndex<(dataArrayVoltage->GetEntries()); arrayIndex++) {
                        AliDCSValue *value = (AliDCSValue*)dataArrayVoltage->At(arrayIndex);
                        dataList[side][plane][RPC-1].Add(new AliRPCValueVoltage(*runIterator,value->GetTimeStamp(),value->GetFloat(),isCalib));
                        delete value;
                    }
                }
            }
        }
    }
    
    printf("\n\n\nData retrieving complete\n\n\n");
    
    //loop sui piani, i lati (inside e outside) e le RPC (9 per side)
    for (Int_t plane=0; plane<nPlanes; plane++) {
        for (Int_t side=0; side<nSides; side++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                //alias all'elemento dell'array 3D di TSortedLists*
                TSortedList *sortedList=&(dataList[side][plane][RPC-1]);
                
                //contenitore per il valore di corrente di buio misurata
                Double_t iDarkCalib=0.;
                
                //flag positivo se la precedente misura di tensione è >=8500.V
                Bool_t voltageOkFlag=kFALSE;
                
                //loop sulle entries della lisa di dati
                for(Int_t iList=0; iList<sortedList->GetEntries(); iList++){
                    //L'elemento può essere una tensione o una corrente
                    AliRPCValueDCS *valueDCS=(AliRPCValueDCS*)(sortedList->At(iList));
                    //se è una tensione
                    if (valueDCS->IsVoltage()) {
                        //cast a tensione
                        AliRPCValueVoltage* valueVoltage=(AliRPCValueVoltage*)valueDCS;
                        //settaggio del flag
                        voltageOkFlag=(Bool_t)(valueVoltage->GetVSupp()>=8500.);
                        valueVoltage=0x0;
                    //se è una corrente
                    } else if (valueDCS->IsCurrent()) {
                        //cast a corrente
                        AliRPCValueCurrent* valueCurrent=(AliRPCValueCurrent*)valueDCS;
                        //se è un run di calibrazione fatto a tensione di lavoro
                        if (valueCurrent->GetIsCalib()==kTRUE && voltageOkFlag==kTRUE) {
                            //rimangono alcune letture a 0.0A, così si tolgono ###GIUSTO?###
                            if(valueCurrent->GetIDark()!=0.)iDarkCalib=valueCurrent->GetIDark();
                        //se è un run di fisica setto la dark current prendendola dal precedente run di calibrazione eseguito in condizioni di lavoro
                        } else {
                            valueCurrent->SetIDark(iDarkCalib);
                        }
                        valueCurrent=0x0;
                    }
                    valueDCS=0x0;
                }
            }
        }
    }
    
    printf("\n\n\nDark currents setting complete\n\n\n");

    
    TGraph *graphTot[nSides][nPlanes][nRPC];
    TGraph *graphDark[nSides][nPlanes][nRPC];
    TCanvas *canv[2*nSides+1];
    TLegend *leg[nSides];
    Int_t color[nRPC] = {kBlack,kRed,kGreen,kBlue,kYellow,kMagenta,kCyan,kGray,kOrange};
    
    for (Int_t side=0; side<nSides; side++) {
        canv[side]=new TCanvas(Form("canv_%s_iTot",sides[side].Data()),Form("canv_%s_iTot",sides[side].Data()));
        canv[side+2]=new TCanvas(Form("canv_%s_iDark",sides[side].Data()),Form("canv_%s_iDark",sides[side].Data()));
        canv[side]->Divide(2,2,0,0);
        canv[side+2]->Divide(2,2,0,0);
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
                
                //contatore incrementale per inserire i punti nei grafici
                Int_t dataCount=0;
                //flag per escludere l'inserimento di misure di corrente eseguite non a tensione di lavoro
                Bool_t drawFlag=kFALSE;
                
                //loop sulle entries
                for(Int_t iList=0; iList<(dataList[side][plane][RPC-1]).GetEntries(); iList++){
                    //generica entry della sorted list
                    AliRPCValueDCS *valueDCS3 = ((AliRPCValueDCS*)dataList[side][plane][RPC-1].At(iList));
                    
                    //se è una tensione
                    if (valueDCS3->IsVoltage()){
                        //cast a tensione
                        AliRPCValueVoltage *voltageBuffer=(AliRPCValueVoltage*)valueDCS3;
                        //se supera la soglia posso disegnare
                        if (voltageBuffer->GetVSupp()>=8500.) {
                            printf("VOLTAGE: \tRPC %d MT %d isCalib %s HV= %f\n",side*9+RPC,planes[plane],"false\0true"+7*(voltageBuffer->GetIsCalib()),voltageBuffer->GetVSupp());
                            drawFlag=kTRUE;
                            continue;
                        //altrimenti non disegno fino a quando non trovo una misura di tensione di lavoro
                        } else {
                            printf("VOLTAGE: \tRPC %d MT %d isCalib %s HV= %f\n",side*9+RPC,planes[plane],"false\0true"+7*(voltageBuffer->GetIsCalib()),voltageBuffer->GetVSupp());
                            drawFlag=kFALSE;
                            continue;
                        }
                    //se è una corrente e posso disegnare
                    } else if(valueDCS3->IsCurrent() && drawFlag==kTRUE){
                        //cast a corrente
                        AliRPCValueCurrent *currentBuffer=(AliRPCValueCurrent*)valueDCS3;
                        //disegno solo i punti relativi a run di fisica
                        if (currentBuffer->GetIsCalib()==kFALSE) {
                            printf("Drawing: \tRPC %d MT %d isCalib %s TotCurrent= %f DarkCurrent=%f\n",side*9+RPC,planes[plane],"false\0true"+7*(currentBuffer->GetIsCalib()),currentBuffer->GetITot(),currentBuffer->GetIDark());
                            graphTot[side][plane][RPC-1]->SetPoint(dataCount, (Double_t)currentBuffer->GetTimeStamp(), (Double_t)currentBuffer->GetITot());
                            graphDark[side][plane][RPC-1]->SetPoint(dataCount, (Double_t)currentBuffer->GetTimeStamp(), (Double_t)currentBuffer->GetIDark());
                            dataCount++;
                        //scarto i run di calibrazione
                        } else {
                            continue;
                        }
                    //se è una corrente ma non posso disegnare
                    } else if(valueDCS3->IsCurrent() && drawFlag==kFALSE){
                        //cast a corrente
                        AliRPCValueCurrent *currentBuffer=(AliRPCValueCurrent*)valueDCS3;
                        printf("Skipping: \tRPC %d MT %d isCalib %s TotCurrent= %f DarkCurrent=%f\n",side*9+RPC,planes[plane],"false\0true"+7*(currentBuffer->GetIsCalib()),currentBuffer->GetITot(),currentBuffer->GetIDark());
                    } else continue;
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
                    leg[side]->AddEntry(graphTot[side][plane][RPC-1],Form("RPC %d",RPC),"lp");
                }
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