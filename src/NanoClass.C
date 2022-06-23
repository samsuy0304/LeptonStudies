#define NanoClass_cxx
#include "NanoClass.h"
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <string>


void NanoClass::PlotHist(TH1F hist, std::string sample_name, std::string plot_dir, std::string plot_name, std::string variable)
{
    printf("Plotting %s\n", plot_name.c_str());

    // canvas
    TCanvas c = TCanvas("c", "c", 800, 800);
    c.SetLeftMargin(0.15);

    // draw
    hist.Draw("hist error same");
    
    // save plot
    std::string output_name = plot_dir + "/" + sample_name + "_" + plot_name; 
    std::string output_name_pdf = output_name + ".pdf";
    c.Update();
    c.SaveAs(output_name_pdf.c_str());

}

void NanoClass::Loop()
{

    // In a ROOT session, you can do:
    //      root> .L NanoClass.C
    //      root> NanoClass t
    //      root> t.GetEntry(12); // Fill t data members with entry number 12
    //      root> t.Show();       // Show values of entry 12
    //      root> t.Show(16);     // Read and show values of entry 16
    //      root> t.Loop();       // Loop on all entries
    //
    //  This is the loop skeleton where:
    //    jentry is the global entry number in the chain
    //    ientry is the entry number in the current Tree
    //  Note that the argument to GetEntry must be:
    //    jentry for TChain::GetEntry
    //    ientry for TTree::GetEntry and TBranch::GetEntry
    //
    // To read only selected branches, Insert statements like:
    // METHOD1:
    //    fChain->SetBranchStatus("*",0);  // disable all branches
    //    fChain->SetBranchStatus("branchname",1);  // activate branchname
    // METHOD2: replace line
    //    fChain->GetEntry(jentry);       //read all branches
    // by  b_branchname->GetEntry(ientry); //read only this branch
    
    gROOT->SetBatch(kTRUE);
    
    if (fChain == 0)
    {
        return;
    }

    std::string plot_dir = "macro_plots";
    std::string sample = "SMS-T2-4bd_genMET-80_mStop-500_mLSP-490";
    printf("Running over %s\n", sample.c_str());

    Long64_t nentries = fChain->GetEntriesFast();
    Long64_t nbytes = 0, nb = 0;
    
    // histograms
    TH1F h_nLowPtElectron            = TH1F("h_nLowPtElectron",             "h_nLowPtElectron",              6,    0.0,  6.0);
    
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);
        nbytes += nb;
        // if (Cut(ientry) < 0) continue;
        if (jentry % 1000 == 0)
        {
            std::cout << "Event " << jentry << std::endl;
        }
        // fill histograms
        h_nLowPtElectron.Fill(nLowPtElectron);
    }
    PlotHist(h_nLowPtElectron, sample, plot_dir, "h_nLowPtElectron", "nElectrons");
}
