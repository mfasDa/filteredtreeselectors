void runAnalysisProofHighPtTrees(const char *filelist, const char *selectorname){
  TProof::Open("lite://");
  gROOT->Macro("LoadLibs.C");
  gProof->Exec("gROOT->Macro(\"LoadLibs.C\")");
  gROOT->LoadMacro("MakeChain.C");
  gSystem->AddIncludePath(Form("-I%s/include",gSystem->Getenv("ALICE_PHYSICS")));
  TChain *particleChain = MakeChain(filelist, "ReducedEvent");
  particleChain->SetProof();
  particleChain->Process(Form("%s++", selectorname),"", 123456789);
}
