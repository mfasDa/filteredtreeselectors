TChain *MakeChain(const char *filename, const char *treename){
  TChain *c = new TChain(treename, treename);
  ifstream in(filename);
  std::string fname;
  while(getline(in,fname)) c->AddFile(fname.c_str());
  return c;
}
