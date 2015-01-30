void runAnalysisLocal(const char *filelist, const char *selectorname){
  gROOT->Macro("LoadLibs.C");
  gROOT->LoadMacro("MakeChain.C");
  TChain *particleChain = MakeChain(filelist, "ParticleTree/JetEvent");
  particleChain->Process(Form("%s++", selectorname),"", 123456789);
}
