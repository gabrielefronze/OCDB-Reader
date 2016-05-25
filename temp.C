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

Bool_t AlreadyThere(Int_t runNumber, vector<Int_t> vect){
    for(std::vector<Int_t>::iterator it = vect.begin(); it != vect.end(); ++it){
        cout<<*it<<endl;
        if(*it==runNumber){
            cout<<"FOUND "<<runNumber<<endl;
            return kTRUE;
        }
    }
    return kFALSE;
}

void AddIfNotYet(Int_t runNumber, vector<Int_t> vect){
    if(!AlreadyThere(runNumber, vect)){
        vect.push_back(runNumber);
        cout<<"added "<<runNumber<<endl;
        return;
    }
}