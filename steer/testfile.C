#if ! defined __CINT__ || defined __MAKECINT__
#include <TFile.h>
#endif

void testfile(const char *filename, const char *treename){
  TFile *in = TFile::Open(filename);

  TTree *totest = (TTree *)in->Get(treename);

  // Loop over all entries, see whether it crashes
  HighPtTracks::AliReducedJetEvent *myevent;
  totest->SetBranchAddress("JetEvent", &myevent);
  for(int ievent = 0; ievent  < totest->GetEntries(); ievent++){
    totest->GetEntry(ievent);
  }
}
