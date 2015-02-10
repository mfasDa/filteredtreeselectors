//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Jan 30 06:15:28 2015 by ROOT version 5.34/08
// from TTree JetEvent/A tree with jet information
// found on file: root_archive.zip#AnalysisResults.root
//////////////////////////////////////////////////////////

#ifndef SelectorTrackEffPID_h
#define SelectorTrackEffPID_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <THnSparse.h>
#include <TSelector.h>

// Header file for the classes stored in the TTree if any.
#include "AliReducedJetEvent.h"

class TArrayD;
class TH1;
class TProfile;
class TString;

namespace HighPtTracks{
class AliReducedJetParticle;
};

class TrackHistogram : public TNamed{
public:
	TrackHistogram():
		TNamed(),
		fHistogram()
	{
		CreateDefaultBinning();
	}
	TrackHistogram(const char *name, const char *title):
		TNamed(name, title),
		fHistogram()
	{
		CreateDefaultBinning();
	}
	TrackHistogram(const TrackHistogram &ref):
		TNamed(ref),
		fHistogram(ref.fHistogram)
	{
		for(int ib = 0; ib < 5; ++ib) fBinnings[ib] = ref.fBinnings[ib];
	}
	TrackHistogram &operator=(const TrackHistogram &ref){
		TNamed::operator=(ref);
		if(this != &ref){
			fHistogram = ref.fHistogram;
			for(int ib = 0; ib < 5; ++ib) fBinnings[ib] = ref.fBinnings[ib];
		}
		return *this;
	}
	~TrackHistogram() { }

	void SetPtBinninng(const TArrayD &binning) { fBinnings[kTrackPt] = binning; }
	void SetJetPtBinning(const TArrayD &binnig) { fBinnings[kJetPt] = binnig; }
	void SetRadiusBinning(const TArrayD &binning) { fBinnings[kRadius] = binning; }
	void SetNContribBinning(const TArrayD &binning) { fBinnings[kNcontrib] = binning; }
	void Create();
	void Fill(double *values, double weight = 1.) { fHistogram->Fill(values,weight); }
	THnSparse *GetHistogram() const { return fHistogram; }
private:
	enum Axis_t{
		kJetPt,
		kTrackPt,
		kPID,
		kNcontrib,
		kRadius
	};
	void CreateDefaultBinning() {
		double min[5] = {0., 0., -0.5, -0.5, 0.5},
				max[5] = {200., 100., 4.5, 100.5, 0.5};
		int nbins[5] = {100, 1000, 5, 101, 100};
		for(int ib = 0; ib < 5; ib++) MakeLinearBinning(fBinnings[ib], nbins[ib], min[ib], max[ib]);
	}
   void DefineAxis(TAxis *axis, TString name, TString title, const TArrayD &binning) const;
   void MakeLinearBinning(TArrayD &array, int nbins, double min, double max) const;
	THnSparseD 			*fHistogram;
	TArrayD				fBinnings[5];

	ClassDef(TrackHistogram, 1);
};

// Fixed size dimensions of array or collections stored in the TTree if any.

class SelectorTrackEffPID : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   HighPtTracks::AliReducedJetEvent *JetEvent;

   // List of branches
   TBranch        *b_JetEvent;   //!

   // List of output histograms
   TrackHistogram   *fGen;                    //! particles at generation level
   TrackHistogram   *fRec;                    //! particles at reconstruction level
   TProfile         *fCrossSection;           //! Cross section histogram
   TH1              *fTrials;                 //! Number of trials histogram

   SelectorTrackEffPID(TTree * /*tree*/ =0) : fChain(0), fGen(NULL), fRec(NULL), fCrossSection(NULL), fTrials(NULL) { }
   virtual ~SelectorTrackEffPID() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

protected:
   unsigned int GetParticleType(const HighPtTracks::AliReducedJetParticle &recparticle) const;
   void CreateDefaultPtBinning(TArrayD &binning) const;
   void MakeLinearBinning(TArrayD &array, int nbins, double min, double max) const;
   unsigned int GetChargedContributors(const HighPtTracks::AliReducedJetInfo *recjet) const;
   void FillParticleHistos(const HighPtTracks::AliReducedJetParticle &part, double jetPt, unsigned int ncontrib, double weight);

   ClassDef(SelectorTrackEffPID,0);
};

#endif

#ifdef SelectorTrackEffPID_cxx
void SelectorTrackEffPID::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   JetEvent = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("JetEvent", &JetEvent, &b_JetEvent);
}

Bool_t SelectorTrackEffPID::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef SelectorTrackEffPID_cxx
