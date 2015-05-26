void runAnalysisLocalHighPtTrees(const char *filelist, const char *selectorname){
  gROOT->Macro("LoadLibs.C");
  gROOT->LoadMacro("MakeChain.C");
  gSystem->AddIncludePath(Form("-I%s/include",gSystem->Getenv("ALICE_PHYSICS")));
  TChain *particleChain = MakeChain(filelist, "ReducedEvent");
  particleChain->Process(Form("%s++", selectorname),"", 123456789);
}
