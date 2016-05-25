//
//  OCDBFindCalib.cpp
//  Aliroot project
//
//  Created by Gabriele Gaetano Fronzé on 03/12/15.
//  Copyright © 2015 Gabriele Gaetano Fronzé. All rights reserved.
//

#include <stdio.h>
#include "TString.h"
#include "TMap.h"
#include "TObjArray.h"
#include "TSortedList.h"
//#include "AliCDBManager.h"
//#include "AliCDBEntry.h"
//#include "AliDCSValue.h"
#include "AliRPCDarkCurrent.h"
#include <vector>
#include <fstream>
#include <iostream>

void OCDBFindCalib(Bool_t file=kFALSE, TString filename=""){
    
    Int_t year=2015;
    vector<Int_t> runList;
    
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
    
    AliCDBManager *manager = AliCDBManager::Instance();
    manager->SetDefaultStorage(Form("alien://folder=/alice/data/%d/OCDB",2015));
    AliCDBManager *manager2 = AliCDBManager::Instance();
    manager2->SetDefaultStorage(Form("alien://folder=/alice/data/%d/OCDB",2015));
    AliCDBEntry *entry;
    AliGRPObject* grpData;
    AliCDBEntry *entry2;
    for (std::vector<Int_t>::iterator runIterator = runList.begin(); runIterator != runList.end(); ++runIterator) {
        manager->SetRun(*runIterator);
        entry = (AliCDBEntry*)(manager->Get("MUON/Calib/TriggerDCS"));
        
        if(!entry) {
            printf("\nproblem with entry\n");
            break;
        }
        
        manager2->SetRun(*runIterator);
        entry2 = (AliCDBEntry*)(manager->Get("GRP/GRP/Data"));
        if(!entry2) {
            printf("\nproblem with entry2\n");
            break;
        }
        
        AliGRPObject *grpObj=(AliGRPObject*)entry2->GetObject();
        TString *runType=new TString(grpObj->GetRunType());
        
        if(runType->Contains("PHYSICS")){
            Printf("\n\nRun %d is physics\n",*runIterator);
        } else if(runType->Contains("CALIBRATION")){
            Printf("\n\nRun %d is calib\n",*runIterator);
        } else {
            Printf("\n\nRun %d is %s\n",runType->Data());
        }
        entry=0x0;
        entry2=0x0;
        grpData=0x0;
        grpObj=0x0;
    }
    return;
}