//
// OCDBParserWithScalersTree.cpp
// 
//   Created by Gabriele Gaetano Fronzé
//   Copyright © 2016 Gabriele Gaetano Fronzé. All rights reserved.
//

#include <stdio.h>
#include "TString.h"
#include "TMap.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TList.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TFile.h"
//#include "AliGRPObject.h"
//#include "AliCDBEntry.h"
//#include "AliMUONTriggerScalers.h"
//#include "AliMpDDLStore.h"
#include "AliRPCValueDCS.h"
#include "AliRPCValueCurrent.h"
#include "AliRPCValueVoltage.h"
#include "AliRPCValueScaler.h"
#include "AliRPCOverflowStatistics.h"
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

void CheckPointer(TNamed *pointer){
    if(!pointer) {
        printf("\nproblem with %s\n",pointer->GetName());
        continue;
    }
}

void OCDBParserWithScalersTree(Bool_t file=kFALSE, TString filename="",Int_t analyzedYear){
    
    //anno da analizzare
    Int_t year=analyzedYear;
    
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
        //cout<<"How many runs do you want to add?"<<endl;
        cin>>nRuns;
        for(Int_t runIndex=0;runIndex<nRuns;runIndex++){
            //cout<<runIndex<<".\t";
            cin>>nRunBuffer;
            runList.push_back(nRunBuffer);
            //cout<<"Added"<<endl;
            
        }
        Printf("\t\t Run adding ended.\n");
    }
    
    //definizione degli array usati per la nomenclatura
    const Int_t nSides=2;
    TString sides[nSides];
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
    
    //flag che è kTRUE se l'evento è di calibrazione
    Bool_t isCalib=kFALSE;

    //array per la conversione di iRPC={0,17} in iRPC={1,9}x{inside,outside}
    Int_t RPCIndexes[18]={5,6,7,8,9,9,8,7,6,5,4,3,2,1,1,2,3,4};
    Int_t RPCSides[18]=  {0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0};

    //manager per interfacciarsi con gli OCDB
    AliCDBManager *managerCurrent = AliCDBManager::Instance();
    AliCDBManager *managerVoltage = AliCDBManager::Instance();
    AliCDBManager *managerRunType = AliCDBManager::Instance();
    AliCDBManager *managerScaler  = AliCDBManager::Instance();
    
    //inizializzazione dei manager
    managerCurrent->SetDefaultStorage(Form("alien://folder=/alice/data/%d/OCDB",year));
    managerVoltage->SetDefaultStorage(Form("alien://folder=/alice/data/%d/OCDB",year));
    managerRunType->SetDefaultStorage(Form("alien://folder=/alice/data/%d/OCDB",year));
    managerScaler->SetDefaultStorage(Form("alien://folder=/alice/data/%d/OCDB",year));
    
    //array 3D di liste di dati. le TList sono già ordinate dopo ogni inserimento
    TList *dataList[nSides][nPlanes][nRPC];
    TList *scalersDataList[2][nSides][nPlanes][nRPC];
    TList *scalersLocalBoardList[nCathodes][nPlanes][nLocalBoards];
    
    for (Int_t plane=0; plane<nPlanes; plane++) {
        for (Int_t side=0; side<nSides; side++) {
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                dataList[side][plane][RPC-1]=new TList();
                dataList[side][plane][RPC-1]->SetName(Form("Data_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC));
                scalersDataList[0][side][plane][RPC-1]=new TList();
                scalersDataList[0][side][plane][RPC-1]->SetName(Form("Scalers_MTR_%s_%s_MT%d_RPC%d",(sides[side]).Data(),(cathodes[0]).Data(),planes[plane],RPC));
                scalersDataList[1][side][plane][RPC-1]=new TList();
                scalersDataList[1][side][plane][RPC-1]->SetName(Form("Scalers_MTR_%s_%s_MT%d_RPC%d",(sides[side]).Data(),(cathodes[1]).Data(),planes[plane],RPC));
            }
        }
    }

    for(Int_t cathode=0;cathode<nCathodes;cathode++){
        for(Int_t plane=0;plane<nPlanes;plane++){
            for(Int_t local=0;local<nLocalBoards;local++){
                scalersLocalBoardList[cathode][plane][local]=new TList();
                scalersLocalBoardList[cathode][plane][local]->SetName(Form("Scalers_MTR_%s_MT%d_LB%d",(cathodes[cathode]).Data(),planes[plane],local+1));
            }
        }
    }

    //loop sui run inseriti
    AliCDBEntry *entryCurrent;
    TMap *mapCurrent;
    AliCDBEntry *entryVoltage;
    TMap *mapVoltage;
    AliCDBEntry *entryScalers;
    TClonesArray *arrayScalers;
    for (std::vector<Int_t>::iterator runIterator = runList.begin(); runIterator != runList.end(); ++runIterator) {
        //i manager puntano al run desiderato
        managerCurrent->SetRun(*runIterator);
        managerVoltage->SetRun(*runIterator);
        managerRunType->SetRun(*runIterator);

        AliCDBStorage *defStorage = managerCurrent->GetDefaultStorage();
        defStorage->QueryCDB(*runIterator);
        TObjArray* arrCDBID = defStorage->GetQueryCDBList();
        TIter nxt(arrCDBID);
        AliCDBId* cdbID = 0;
        Bool_t hasGRP = kFALSE;
        while ((cdbID=(AliCDBId*)nxt())) {
            if (cdbID->GetPath() == "GRP/GRP/Data") {hasGRP = kTRUE; break;}
        }
        if(!hasGRP){
            printf("\n\nSkipping run %d\n\n",*runIterator);
            continue;
        }
        cout<<"1"<<endl;
        //if(defStorage) delete defStorage;
        //if(arrCDBID) delete arrCDBID;
        //if(cdbID) delete cdbID;


        if(!AliMpCDB::LoadDDLStore()) continue;
        AliMpDDLStore *ddlStore=AliMpDDLStore::Instance();
        
        //inizializzazione dell'entry contente il runtype
        AliCDBEntry *entryRunType = managerRunType->Get("GRP/GRP/Data");
        CheckPointer((TNamed*)entryRunType);
        
        //retrievering delle informazioni sul run
        AliGRPObject *grpObj=(AliGRPObject*)entryRunType->GetObject();
        CheckPointer((TNamed*)grpObj);
        TString *runType=new TString(grpObj->GetRunType());
        Long64_t SOR=(Long64_t)grpObj->GetTimeStart();
        Long64_t EOR=(Long64_t)grpObj->GetTimeEnd();
        //if(entryRunType) delete entryRunType;
        //if(grpObj) delete grpObj;
        // cout<<"2"<<endl;
        
        //settaggio del flag isCalib
        if(runType->Contains("PHYSICS")){
            isCalib=kFALSE;
            //cout<<*runIterator<<" is phys"<<endl;
        } else if(runType->Contains("CALIBRATION")){
            isCalib=kTRUE;
            //cout<<*runIterator<<" is calib"<<endl;
        } else {
            continue;
        }
        //if(runType) delete runType;
        cout<<"3"<<endl;
        
        //cout<<isCalib<<endl;
        
        //inizializzazione dell'entry contente i valori di corrente
        entryCurrent=0x0;
        entryCurrent = managerCurrent->Get("MUON/Calib/TriggerDCS");
        CheckPointer((TNamed*)entryCurrent);
        
        //mappa delle correnti
        mapCurrent=0x0;
        mapCurrent = (TMap*)entryCurrent->GetObject();
        CheckPointer((TNamed*)mapCurrent);
        
        //inizializzazione dell'entry contente i valori di tensione
        entryVoltage=0x0;
        entryVoltage = managerVoltage->Get("MUON/Calib/TriggerDCS");
        CheckPointer((TNamed*)entryVoltage);
        
        //mappa delle tensioni
        mapVoltage=0x0;
        mapVoltage = (TMap*)entryVoltage->GetObject();
        CheckPointer((TNamed*)mapVoltage);

        
        //loop sui piani, i lati (inside e outside) e le RPC (9 per side)
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t side=0; side<nSides; side++) {
                TObjArray *dataArrayVoltage;
                TObjArray *dataArrayCurrents;
                for (Int_t RPC=1; RPC<=nRPC; RPC++) {                    
                    //creazione di un pointer all'elemento della mappa delle tensioni
                    dataArrayVoltage=(TObjArray*)(mapCurrent->GetValue(Form("MTR_%s_MT%d_RPC%d_HV.vEff",sides[side].Data(),planes[plane],RPC)));
                    
                    if(!dataArrayVoltage) {
                        printf(" Problems getting dataArrayVoltage\n");
                        break;
                    }

                    //cout<<"N voltage entries= "<<dataArrayVoltage->GetEntries()<<endl;
                    
                    Bool_t isVoltageOk=kTRUE;

                    //loop sulle entry del vettore di misure di tensione
                    AliDCSValue *value;
                    for (Int_t arrayIndex=0; arrayIndex<(dataArrayVoltage->GetEntries()); arrayIndex++) {
                        value = (AliDCSValue*)dataArrayVoltage->At(arrayIndex);
                        if(value->GetFloat()<8500.){
                            isVoltageOk=kFALSE;
                            //cout<<"\t"<<value->GetFloat()<<"\tBAD"<<endl;
                            break;
                        } else {
                            //cout<<"\t"<<value->GetFloat()<<endl;
                            dataList[side][plane][RPC-1]->Add(new AliRPCValueVoltage(*runIterator,value->GetTimeStamp(),value->GetFloat(),isCalib));
                        }
                        //cout<<"\t"<<value->GetFloat()<<endl;
                        dataList[side][plane][RPC-1]->Add(new AliRPCValueVoltage(*runIterator,value->GetTimeStamp(),value->GetFloat(),isCalib));
                        value=0x0;
                    }

                    if (isVoltageOk==kFALSE) break; 

                    //creazione di un pointer all'elemento della mappa delle correnti
                    dataArrayCurrents=(TObjArray*)(mapCurrent->GetValue(Form("MTR_%s_MT%d_RPC%d_HV.actual.iMon",sides[side].Data(),planes[plane],RPC)));
                    if(!dataArrayCurrents) {
                        printf(" Problems getting dataArrayCurrents\n");
                        break;
                    }
                    //printf("%d-%d-%d\n",plane,side,RPC);
                    //loop sulle entry del vettore di misure di corrente
                    //cout<<"N current entries= "<<dataArrayCurrents->GetEntries()<<endl;
                    for (Int_t arrayIndex=0; arrayIndex<(dataArrayCurrents->GetEntries()); arrayIndex++) {
                        //puntatore all'elemento i-esimo
                        value = (AliDCSValue*)dataArrayCurrents->At(arrayIndex);
                        //se il run è di calibrazione corrente e corrente di buio coincidono
                        if (isCalib) {
                            dataList[side][plane][RPC-1]->Add(new AliRPCValueCurrent(*runIterator,value->GetTimeStamp(),value->GetFloat(),value->GetFloat(),isCalib,0));
                            //altrimenti imposto la corrente di buio a 0 (la cambio dopo)
                        } else {
                            dataList[side][plane][RPC-1]->Add(new AliRPCValueCurrent(*runIterator,value->GetTimeStamp(),value->GetFloat(),0.,isCalib,0));
                        }
                        //cout<<"\t"<<value->GetFloat()<<"   "<<value->GetTimeStamp()<<endl;
                        value=0x0;
                    }
                    //Form("Data_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC)
                    // dataArrayVoltage->SetOwner();
                    // dataArrayVoltage->Clear();
                    // dataArrayVoltage->Delete();
                    // dataArrayCurrents->SetOwner();
                    // dataArrayCurrents->Clear();
                    // dataArrayCurrents->Delete();
                    //if(value) delete value;
                }
                //if(dataArrayVoltage) delete dataArrayVoltage;
                //if(dataArrayCurrents) delete dataArrayCurrents;
                cout<<"4"<<endl;
            }
        }

        managerScaler->SetRun(*runIterator);
        //inizializzazone dell'entry contenente le letture degli scalers
        entryScalers=0x0;
        entryScalers = managerScaler->Get("MUON/Calib/TriggerScalers");
        CheckPointer((TNamed*)entryScalers);

        //array delle letture
        arrayScalers=0x0;
        arrayScalers = (TClonesArray*)entryScalers->GetObject();
        CheckPointer((TNamed*)arrayScalers);

        if(arrayScalers)printf("\n\n\n##### Scalers %d ######\n\n\n",arrayScalers->GetEntries());

        ULong64_t elapsedTime=0;
        UInt_t readLB[nCathodes][nSides][nPlanes][nRPC];
        UInt_t overflowLB[nCathodes][nSides][nPlanes][nRPC];
        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t side=0; side<nSides; side++) {
                for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                    for (Int_t cathode=0; cathode<nCathodes; cathode++) {
                        readLB[cathode][side][plane][RPC-1]=0;
                        overflowLB[cathode][side][plane][RPC-1]=0;
                    }
                }
            }
        }                        
        //loop sulle entries, sui piani, i catodi (bending e non bending) e le Local Boards (234 per piano)
        cout<<"entries: "<<arrayScalers->GetEntries()<<endl;
        AliMUONTriggerScalers *scalersData;
        AliRPCValueScaler *buffer;
        for (Int_t scalerEntry=0; scalerEntry<arrayScalers->GetEntries() && isCalib==kFALSE; scalerEntry++){
            scalersData=(AliMUONTriggerScalers*)arrayScalers->At(scalerEntry);
            //cout<<"deltat: "<<scalersData->GetDeltaT()<<endl;
            Int_t arrayScalersEntries=arrayScalers->GetEntries();
            if(scalerEntry%100==0)cout<<scalerEntry<<"/"<<arrayScalersEntries<<endl;
            elapsedTime+=scalersData->GetDeltaT();
            for (Int_t plane=0; plane<nPlanes; plane++) {
                for (Int_t cathode=0; cathode<nCathodes; cathode++) {
                    //cout<<"{";
                    for (Int_t localBoard=0; localBoard<nLocalBoards; localBoard++) {
                        Int_t iRPC017=(ddlStore->GetDEfromLocalBoard(localBoard+1, plane+10))-(plane+1+10)*100;
                        Int_t iRPC09=RPCIndexes[iRPC017];
                        Int_t iSide=RPCSides[iRPC017];

                        //cout<<"{"<<localBoard<<","<<iRPC09<<","<<iSide<<"},";

                        // se c'è overflow scarto il dato
                        readLB[cathode][iSide][plane][iRPC09-1]++;
                        Bool_t isOverflow=kFALSE;
                        if (scalersData->GetLocScalStripOver(cathode, plane, localBoard)!=0. ){//| scalersData->GetLocScalStrip(cathode, plane, localBoard)==0.) {
                            //printf(" MTR %d cathode %d LB %d RPC %d or %d_%s timestamp %lu data %d OVERFLOW\n",planes[plane],cathode,localBoard,iRPC017,iRPC09,(sides[iSide]).Data(),SOR+elapsedTime,scalersData->GetLocScalStrip(cathode, plane, localBoard));
                            overflowLB[cathode][iSide][plane][iRPC09-1]++;
                            readLB[cathode][iSide][plane][iRPC09-1]--;
                            isOverflow=kTRUE;
                        }
                        // se la lettura non è quella a fine run immagazzino il dato con timestamp pari a SOR+DeltaT
                        if(scalerEntry!=arrayScalersEntries-1){
                            buffer=new AliRPCValueScaler(*runIterator, SOR+elapsedTime, scalersData->GetLocScalStrip(cathode, plane, localBoard), isCalib, scalersData->GetDeltaT(), isOverflow);
                            scalersDataList[cathode][iSide][plane][iRPC09-1]->Add(buffer);
                            scalersLocalBoardList[cathode][plane][localBoard]->Add(buffer);
                            delete buffer;
                        }
                        // altrimenti il timestamp è pari all'EOR
                        else {
                            buffer=new AliRPCValueScaler(*runIterator, EOR, scalersData->GetLocScalStrip(cathode, plane, localBoard), isCalib, scalersData->GetDeltaT(), isOverflow);
                            scalersDataList[cathode][iSide][plane][iRPC09-1]->Add(buffer);
                            scalersLocalBoardList[cathode][plane][localBoard]->Add(buffer);
                            delete buffer;
                        }
                        //delete buffer;
                        //printf(" MTR %d cathode %d LB %d RPC %d or %d_%s timestamp %lu data %d\n",planes[plane],cathode,localBoard,iRPC017,iRPC09,(sides[iSide]).Data(),SOR+elapsedTime,scalersData->GetLocScalStrip(cathode, plane, localBoard));
                    }
                    //cout<<"}"<<endl;
                }
            }
        }
        arrayScalers->SetOwner();
        arrayScalers->Clear();
        arrayScalers->Delete();
        //delete scalersData;
        //delete buffer;

        printf("scalers reading complete.\n");

        for (Int_t plane=0; plane<nPlanes; plane++) {
            for (Int_t side=0; side<nSides; side++) {
                for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                    for (Int_t cathode=0; cathode<nCathodes; cathode++) {
                        scalersDataList[cathode][side][plane][RPC-1]->Add(new AliRPCOverflowStatistics(*runIterator, EOR, overflowLB[cathode][side][plane][RPC-1], readLB[cathode][side][plane][RPC-1], isCalib));
                        //cout<<"RUN"<<*runIterator<<" side:"<<side<<" plane "<<plane<<" RPC "<<RPC<<" cathode "<<cathode<<" READ="<<readLB[cathode][side][plane][RPC-1]<<" OVERFLOW="<<overflowLB[cathode][side][plane][RPC-1]<<endl<<endl;
                    }
                }
            }
        }
    }

    //cout<<"5.0"<<endl;
    //if(entryCurrent) delete entryCurrent;
    //cout<<"5.1"<<endl;
    //if(mapCurrent) delete mapCurrent;
    //cout<<"5.2"<<endl;
    //if(entryVoltage) delete entryVoltage;
    //cout<<"5.3"<<endl;
    //if(mapVoltage) delete mapVoltage;
    //cout<<"5"<<endl;

    //cout<<"6.1"<<endl;
    //if(scalersData) delete scalersData;
    //cout<<"6.2"<<endl;
    //if(entryScalers) delete entryScalers;
    //cout<<"6.3"<<endl;
    //if(arrayScalers) delete arrayScalers;
    //cout<<"6"<<endl;
    
    printf("\n\n\nData retrieving complete\n\n\n");

    filename.ReplaceAll(".txt", "");
    TFile *outputFile=new TFile(Form("OCDB_data_%s_overflow_count.root",filename.Data()),"RECREATE");

    //loop sui piani, i lati (inside e outside) e le RPC (9 per side)
    for (Int_t plane=0; plane<nPlanes; plane++) {
        for (Int_t side=0; side<nSides; side++) {
            TList *sortedList=new TList();
            for (Int_t RPC=1; RPC<=nRPC; RPC++) {
                //alias all'elemento dell'array 3D di TLists*
                sortedList=0x0;
                sortedList=dataList[side][plane][RPC-1];
                sortedList->Sort();
                
                //contenitore per il run number del run dal quale deriva la misura di dark current
                UInt_t calibRunNumber=0;

                //contenitore per il valore di corrente di buio misurata
                Double_t iDarkCalib=0.;
                
                //flag positivo se la precedente misura di tensione è >=8500.V
                Bool_t voltageOkFlag=kFALSE;
                
                printf("MT%d_%s_%d\n",planes[plane],sides[side].Data(),RPC);

                //loop sulle entries della lisa di dati
                AliRPCValueDCS *valueDCS;
                AliRPCValueVoltage* valueVoltage;
                AliRPCValueCurrent* valueCurrent;
                for(Int_t iList=0; iList<sortedList->GetEntries(); iList++){
                    cout<<iList<<"/"<<sortedList->GetEntries()<<endl;
                    //L'elemento può essere una tensione o una corrente
                    valueDCS=(AliRPCValueDCS*)(sortedList->At(iList));
                    //se è una tensione
                    if (valueDCS->IsVoltage()) {
                        //cast a tensione
                        valueVoltage=(AliRPCValueVoltage*)valueDCS;
                        //settaggio del flag
                        voltageOkFlag=(Bool_t)(valueVoltage->GetVSupp()>=8500.);
                        valueVoltage=0x0;
                        //se è una corrente
                    } else if (valueDCS->IsCurrent()) {
                        //cast a corrente
                        valueCurrent=(AliRPCValueCurrent*)valueDCS;
                        //se è un run di calibrazione fatto a tensione di lavoro
                        if (valueCurrent->GetIsCalib()==kTRUE && voltageOkFlag==kTRUE) {
                            //rimangono alcune letture a 0.0A, così si tolgono ###GIUSTO?###
                            if(valueCurrent->GetIDark()!=0.){
                                iDarkCalib=valueCurrent->GetIDark();
                                calibRunNumber=valueCurrent->GetCalibRunNumber();
                            }
                            //se è un run di fisica setto la dark current prendendola dal precedente run di calibrazione eseguito in condizioni di lavoro
                        } else {
                            valueCurrent->SetIDark(iDarkCalib);
                            valueCurrent->SetCalibRunNumber(calibRunNumber);
                            //cout<<valueCurrent<<"\t"<<calibRunNumber<<endl;
                        }
                        valueCurrent=0x0;
                    }
                    //cout<<valueDCS->GetIsCalib()<<endl;
                    valueDCS=0x0;
                }
                // cout<<"7.0"<<endl;
                // if(valueDCS) delete valueDCS;
                // cout<<"7.1"<<endl;
                // if(valueVoltage) delete valueVoltage;
                // cout<<"7.2"<<endl;
                // if(valueCurrent) delete valueCurrent;
                // cout<<"7"<<endl;

                outputFile->cd();
                sortedList->Sort();
                cout<<scalersDataList[0][side][plane][RPC-1]->At(1)<<endl;
                cout<<scalersDataList[1][side][plane][RPC-1]->At(1)<<endl;
                scalersDataList[0][side][plane][RPC-1]->Sort();
                scalersDataList[1][side][plane][RPC-1]->Sort();
                sortedList->Write(Form("Data_MTR_%s_MT%d_RPC%d",(sides[side]).Data(),planes[plane],RPC),TObject::kSingleKey);
                scalersDataList[0][side][plane][RPC-1]->Write(Form("Scalers_MTR_%s_%s_MT%d_RPC%d",(sides[side]).Data(),(cathodes[0]).Data(),planes[plane],RPC),TObject::kSingleKey);
                scalersDataList[1][side][plane][RPC-1]->Write(Form("Scalers_MTR_%s_%s_MT%d_RPC%d",(sides[side]).Data(),(cathodes[1]).Data(),planes[plane],RPC),TObject::kSingleKey);
                sortedList->Delete();
            }
            if(sortedList) delete sortedList;
            cout<<"8"<<endl;
        }
    }    

    for(Int_t cathode=0;cathode<nCathodes;cathode++){
        for(Int_t plane=0;plane<nPlanes;plane++){
            for(Int_t local=0;local<nLocalBoards;local++){
                outputFile->cd();
                scalersLocalBoardList[cathode][plane][local]->Sort();
                scalersLocalBoardList[cathode][plane][local]->Write(Form("Scalers_MTR_%s_MT%d_LB%d",(cathodes[cathode]).Data(),planes[plane],local+1),TObject::kSingleKey);
                printf("Scalers_MTR_%s_MT%d_LB%d\n",(cathodes[cathode]).Data(),planes[plane],local+1);
            }
        }
    }

    printf("\n\n\nDark currents setting complete\n\n\n");
}