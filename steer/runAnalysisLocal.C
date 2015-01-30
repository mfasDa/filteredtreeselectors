void runAnalysisLocal(const char *filelist, const char *selectorname){
  gROOT->LoadMacro("MakeChain.C");
  TChain *particleChain = MakeChain(filelist, "ParticlesInCone");
  particleChain->Process(Form("%s++", selectorname),"", 123456789);
}
