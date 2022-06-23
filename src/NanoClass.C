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

    // Int_t           LowPtElectron_genPartIdx[5];   //[nLowPtElectron]
    // UChar_t         LowPtElectron_genPartFlav[5];   //[nLowPtElectron]
    
    // histograms
    TH1F h_nLowPtElectron            = TH1F("h_nLowPtElectron",             "h_nLowPtElectron",              6,    0.0,  6.0);
    TH1F h_LowPtElectron_genPartFlav = TH1F("h_LowPtElectron_genPartFlav",  "h_LowPtElectron_genPartFlav",  30,      0,  30);
    TH1F h_LowPtElectron_dxy         = TH1F("h_LowPtElectron_dxy",          "h_LowPtElectron_dxy",          40,  -0.02,  0.02);
    TH1F h_LowPtElectron_dxyErr      = TH1F("h_LowPtElectron_dxyErr",       "h_LowPtElectron_dxyErr",       40,      0,  0.1);
    TH1F h_LowPtElectron_dxySig      = TH1F("h_LowPtElectron_dxySig",       "h_LowPtElectron_dxySig",       50,      0,  5.0);
    
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
        // loop over electrons
        for (int k = 0; k < nLowPtElectron; ++k)
        {
            float dxySig = -999;
            // avoid dividing by 0
            if (LowPtElectron_dxyErr[k] != 0)
            {
                dxySig = abs(LowPtElectron_dxy[k] / LowPtElectron_dxyErr[k]);
            }
            // fill histograms
            h_LowPtElectron_genPartFlav.Fill(LowPtElectron_genPartFlav[k]);
            h_LowPtElectron_dxy.Fill(LowPtElectron_dxy[k]);
            h_LowPtElectron_dxyErr.Fill(LowPtElectron_dxyErr[k]);
            h_LowPtElectron_dxySig.Fill(dxySig);
        }
    }
    // plot histograms
    PlotHist(h_nLowPtElectron,              sample, plot_dir, "h_nLowPtElectron",               "nElectrons");
    PlotHist(h_LowPtElectron_genPartFlav,   sample, plot_dir, "h_LowPtElectron_genPartFlav",    "genPartFlav");
    PlotHist(h_LowPtElectron_dxy,           sample, plot_dir, "h_LowPtElectron_dxy",            "dxy");
    PlotHist(h_LowPtElectron_dxyErr,        sample, plot_dir, "h_LowPtElectron_dxyErr",         "dxyErr");
    PlotHist(h_LowPtElectron_dxySig,        sample, plot_dir, "h_LowPtElectron_dxySig",         "dxySig");
}
