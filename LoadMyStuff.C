#include "TString.h"

void LoadMyStuff(TString myopt="fast"){
	char * opt;
	if(myopt.Contains("force")){
	opt = "kfg";
	}
	else {
	opt = "kg";
	}
	gSystem->CompileMacro("AliRPCValueDCS.cpp",opt);
	gSystem->CompileMacro("AliRPCValueCurrent.cpp",opt);
	gSystem->CompileMacro("AliRPCValueVoltage.cpp",opt);
	gSystem->CompileMacro("AliRPCValueScaler.cpp",opt);
	gSystem->CompileMacro("AliRPCRunStatistics.cpp",opt);
	gSystem->CompileMacro("AliRPCOverflowStatistics.cpp",opt);
	gSystem->CompileMacro("AliRPCData.cpp",opt);
}