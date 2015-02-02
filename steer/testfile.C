#if ! defined __CINT__ || defined __MAKECINT__
#include <TFile.h>
#endif

void testfile(const char *filename, const char *treename){
  TFile *in = TFile::Open(filename);

  TTree *totest(0x0);
  TString tnstring(treename);
  if(tnstring.Contains("/")){
    TObjArray *tokens = tnstring.Tokenize("/");
    int ntok(0);
    TIter tokIter(tokens);
    TObjString *mystr(0x0);
    while((mystr = dynamic_cast<TObjString *>(tokIter))){
      if(ntok < tokens->GetEntries()-1){
        if(ntok == 0)
          gFile->cd(mystr->String().Data());
        else
          gDirectory->cd(mystr->String().Data());
      } else {
        totest = gDirectory->Get(mystr->String().Data());
      }
      ntok++;
    }
  } else {
    totest = (TTree *)in->Get(treename);
  }

  // Loop over all entries, see whether it crashes
  HighPtTracks::AliReducedJetEvent *myevent;
  totest->SetBranchAddress("JetEvent", &myevent);
  for(int ievent = 0; ievent  < totest->GetEntries(); ievent++){
    totest->GetEntry(ievent);
  }
}
