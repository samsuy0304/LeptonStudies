#define KUSU_cxx
#include "KUSU.h"
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <string>
#include <cmath>
#include <map>


std::string KUSU::GetLabel(std::string variable)
{
    std::map<std::string, std::string> labels {
        // variables
        {"nElectrons",  "n_{e}"},
        {"pt",          "p_{T} [GeV]"},
        {"eta",         "#eta"},
        {"phi",         "#phi"},
        {"mass",        "m [GeV]"},
        {"genPartIdx",  "Gen Part Idx"},
        {"genPartFlav", "Gen Part Flav"},
        {"dxy",         "d_{xy}"},
        {"dxyErr",      "d_{xy} err"},
        {"dxySig",      "d_{xy} sig"},
        {"EMID",        "EmbeddedID"},
        {"dz",         "d_{z}"},
        {"dzErr",      "d_{z} err"},
        {"dzSig",      "d_{z} sig"},
        
        {"ISO",         "ISO"},
        {"CONV",      "Conveto"},
        {"IpSig",      "d_{IP} sig"},
        
    };
    std::string label = "";
    // check if variable exists in labels
    if (labels.find(variable) == labels.end())
    {
        // variable does not exist
        printf("ERROR: the variable '%s' does not exist in labels.\n", variable.c_str());
    }
    else
    {
        // variable exists
        label = labels[variable];
    }
    return labels[variable];
}

void KUSU::SetupHist(TH1F &hist, std::string title, std::string x_title, std::string y_title, int color, int line_width)
{
    hist.SetStats(kFALSE);
    
    TAxis* x_axis = hist.GetXaxis();
    TAxis* y_axis = hist.GetYaxis();
    
    hist.SetTitle(title.c_str());
    x_axis->SetTitle(x_title.c_str());
    y_axis->SetTitle(y_title.c_str());
    hist.SetLineColor(color);
    hist.SetLineWidth(line_width);
}

void KUSU::PlotHist(TH1F &hist, std::string sample_name, std::string plot_dir, std::string plot_name, std::string variable)
{
    printf("Plotting %s\n", plot_name.c_str());

    // canvas
    TCanvas c = TCanvas("c", "c", 800, 800);
    c.SetLeftMargin(0.15);

    // setup histogram
    std::string title   = plot_name;
    std::string x_title = GetLabel(variable);
    std::string y_title = "Entries";
    int color           = kBlack;
    int line_width      = 1;
    SetupHist(hist, title, x_title, y_title, color, line_width);

    // draw
    hist.Draw("hist error same");
    
    // save plot
    std::string output_name = plot_dir + "/" + sample_name + "_" + plot_name; 
    std::string output_name_pdf = output_name + ".pdf";
    c.Update();
    c.SaveAs(output_name_pdf.c_str());
}

void KUSU::Loop()
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
    
    
    // LowPtElectron_genPartFlav == 0
    TH1F LIG0_EMID       = TH1F("Iron_Flav0_EMID","Iron_Flav0_EMID",     45,-1.0,8.0);
    TH1F LILG0_EMID    = TH1F("IronLong_Flav0_EMID","IronLong_Flav0_EMID",  45,-1.0,8.0);
    TH1F LIFG0_EMID    = TH1F("IronFake_Flav0_EMID","IronFake_Flav0_EMID",  45,-1.0,8.0);
    // LowPtElectron_genPartFlav == 1
    TH1F LIG1_EMID       = TH1F("Iron_Flav1_EMID","Iron_Flav1_EMID",     45,-1.0,8.0);
    TH1F LILG1_EMID    = TH1F("IronLong_Flav1_EMID","IronLong_Flav1_EMID",  45,-1.0,8.0);
    TH1F LIFG1_EMID    = TH1F("IronFake_Flav1_EMID","IronFake_Flav1_EMID",  45,-1.0,8.0);
    // LowPtElectron_genPartFlav == 5
    TH1F LIG5_EMID      = TH1F("Iron_Flav5_EMID","Iron_Flav5_EMID",     45,-1.0,8.0);
    TH1F LILG5_EMID    = TH1F("IronLong_Flav5_EMID","IronLong_Flav5_EMID",  45,-1.0,8.0);
    TH1F LIFG5_EMID    = TH1F("IronFake_Flav5_EMID","IronFake_Flav5_EMID",  45,-1.0,8.0);
    
    
    
    
    ///PT
    
     // LowPtElectron_genPartFlav == 0
    TH1F LIG0_pt       = TH1F("Iron_Flav0_pt","Iron_Flav0_pt",     20,0.0,20.0);
    TH1F LILG0_pt    = TH1F("IronLong_Flav0_pt","IronLong_Flav0_pt",  20,0.0,20.0);
    TH1F LIFG0_pt    = TH1F("IronFake_Flav0_pt","IronFake_Flav0_pt",  20,0.0,20.0);
    // LowPtElectron_genPartFlav == 1
    TH1F LIG1_pt       = TH1F("Iron_Flav1_pt","Iron_Flav1_pt",     20,0.0,20.0);
    TH1F LILG1_pt    = TH1F("IronLong_Flav1_pt","IronLong_Flav1_pt",  20,0.0,20.0);
    TH1F LIFG1_pt    = TH1F("IronFake_Flav1_pt","IronFake_Flav1_pt",  20,0.0,20.0);
    // LowPtElectron_genPartFlav == 5
    TH1F LIG5_pt      = TH1F("Iron_Flav5_pt","Iron_Flav5_pt",     20,0.0,20.0);
    TH1F LILG5_pt    = TH1F("IronLong_Flav5_pt","IronLong_Flav5_pt",  20,0.0,20.0);
    TH1F LIFG5_pt    = TH1F("IronFake_Flav5_pt","IronFake_Flav5_pt",  20,0.0,20.0);
    
    
    
    
    //// Eta
    
    
    // LowetaElectron_genPartFlav == 0
    TH1F LIG0_eta       = TH1F("Iron_Flav0_eta","Iron_Flav0_eta",     30,-3,3);
    TH1F LILG0_eta    = TH1F("IronLong_Flav0_eta","IronLong_Flav0_eta",  30,-3,3);
    TH1F LIFG0_eta    = TH1F("IronFake_Flav0_eta","IronFake_Flav0_eta",  30,-3,3);
    // LowetaElectron_genPartFlav == 1
    TH1F LIG1_eta       = TH1F("Iron_Flav1_eta","Iron_Flav1_eta",     30,-3,3);
    TH1F LILG1_eta    = TH1F("IronLong_Flav1_eta","IronLong_Flav1_eta",  30,-3,3);
    TH1F LIFG1_eta    = TH1F("IronFake_Flav1_eta","IronFake_Flav1_eta",  30,-3,3);
    // LowetaElectron_genPartFlav == 5
    TH1F LIG5_eta      = TH1F("Iron_Flav5_eta","Iron_Flav5_eta",     30,-3,3);
    TH1F LILG5_eta    = TH1F("IronLong_Flav5_eta","IronLong_Flav5_eta",  30,-3,3);
    TH1F LIFG5_eta    = TH1F("IronFake_Flav5_eta","IronFake_Flav5_eta",  30,-3,3);
    
    
    ///DXY
    
    

 // LowdxyElectron_genPartFlav == 0
    TH1F LIG0_dxy       = TH1F("Iron_Flav0_dxy","Iron_Flav0_dxy",     50,-0.2,0.2);
    TH1F LILG0_dxy    = TH1F("IronLong_Flav0_dxy","IronLong_Flav0_dxy",  50,-0.2,0.2);
    TH1F LIFG0_dxy    = TH1F("IronFake_Flav0_dxy","IronFake_Flav0_dxy",  50,-0.2,0.2);
    // LowdxyElectron_genPartFlav == 1
    TH1F LIG1_dxy       = TH1F("Iron_Flav1_dxy","Iron_Flav1_dxy",     50,-0.2,0.2);
    TH1F LILG1_dxy    = TH1F("IronLong_Flav1_dxy","IronLong_Flav1_dxy",  50,-0.2,0.2);
    TH1F LIFG1_dxy    = TH1F("IronFake_Flav1_dxy","IronFake_Flav1_dxy",  50,-0.2,0.2);
    // LowdxyElectron_genPartFlav == 5
    TH1F LIG5_dxy      = TH1F("Iron_Flav5_dxy","Iron_Flav5_dxy",     50,-0.2,0.2);
    TH1F LILG5_dxy    = TH1F("IronLong_Flav5_dxy","IronLong_Flav5_dxy",  50,-0.2,0.2);
    TH1F LIFG5_dxy    = TH1F("IronFake_Flav5_dxy","IronFake_Flav5_dxy",  50,-0.2,0.2);
    
    
    // Dz
    
    
     // LowdzElectron_genPartFlav == 0
    TH1F LIG0_dz       = TH1F("Iron_Flav0_dz","Iron_Flav0_dz",     50,-0.2,0.2);
    TH1F LILG0_dz    = TH1F("IronLong_Flav0_dz","IronLong_Flav0_dz",  50,-0.2,0.2);
    TH1F LIFG0_dz    = TH1F("IronFake_Flav0_dz","IronFake_Flav0_dz",  50,-0.2,0.2);
    // LowdzElectron_genPartFlav == 1
    TH1F LIG1_dz       = TH1F("Iron_Flav1_dz","Iron_Flav1_dz",     50,-0.2,0.2);
    TH1F LILG1_dz    = TH1F("IronLong_Flav1_dz","IronLong_Flav1_dz",  50,-0.2,0.2);
    TH1F LIFG1_dz    = TH1F("IronFake_Flav1_dz","IronFake_Flav1_dz",  50,-0.2,0.2);
    // LowdzElectron_genPartFlav == 5
    TH1F LIG5_dz      = TH1F("Iron_Flav5_dz","Iron_Flav5_dz",     50,-0.2,0.2);
    TH1F LILG5_dz    = TH1F("IronLong_Flav5_dz","IronLong_Flav5_dz",  50,-0.2,0.2);
    TH1F LIFG5_dz    = TH1F("IronFake_Flav5_dz","IronFake_Flav5_dz",  50,-0.2,0.2);
    
    
    
    // dz Err
    
    // LowdzErrElectron_genPartFlav == 0
    TH1F LIG0_dzErr       = TH1F("Iron_Flav0_dzErr","Iron_Flav0_dzErr",     50,0.0,0.2);
    TH1F LILG0_dzErr    = TH1F("IronLong_Flav0_dzErr","IronLong_Flav0_dzErr",  50,0.0,0.2);
    TH1F LIFG0_dzErr    = TH1F("IronFake_Flav0_dzErr","IronFake_Flav0_dzErr",  50,0.0,0.2);
    // LowdzErrElectron_genPartFlav == 1
    TH1F LIG1_dzErr       = TH1F("Iron_Flav1_dzErr","Iron_Flav1_dzErr",     50,0.0,0.2);
    TH1F LILG1_dzErr    = TH1F("IronLong_Flav1_dzErr","IronLong_Flav1_dzErr",  50,0.0,0.2);
    TH1F LIFG1_dzErr    = TH1F("IronFake_Flav1_dzErr","IronFake_Flav1_dzErr",  50,0.0,0.2);
    // LowdzErrElectron_genPartFlav == 5
    TH1F LIG5_dzErr      = TH1F("Iron_Flav5_dzErr","Iron_Flav5_dzErr",     50,0.0,0.2);
    TH1F LILG5_dzErr    = TH1F("IronLong_Flav5_dzErr","IronLong_Flav5_dzErr",  50,0.0,0.2);
    TH1F LIFG5_dzErr    = TH1F("IronFake_Flav5_dzErr","IronFake_Flav5_dzErr",  50,0.0,0.2);
    
    
    // dxy ERr
    

 // LowdxyErrElectron_genPartFlav == 0
    TH1F LIG0_dxyErr       = TH1F("Iron_Flav0_dxyErr","Iron_Flav0_dxyErr",     50,0.0,0.2);
    TH1F LILG0_dxyErr    = TH1F("IronLong_Flav0_dxyErr","IronLong_Flav0_dxyErr",  50,0.0,0.2);
    TH1F LIFG0_dxyErr    = TH1F("IronFake_Flav0_dxyErr","IronFake_Flav0_dxyErr",  50,0.0,0.2);
    // LowdxyErrElectron_genPartFlav == 1
    TH1F LIG1_dxyErr       = TH1F("Iron_Flav1_dxyErr","Iron_Flav1_dxyErr",     50,0.0,0.2);
    TH1F LILG1_dxyErr    = TH1F("IronLong_Flav1_dxyErr","IronLong_Flav1_dxyErr",  50,0.0,0.2);
    TH1F LIFG1_dxyErr    = TH1F("IronFake_Flav1_dxyErr","IronFake_Flav1_dxyErr",  50,0.0,0.2);
    // LowdxyErrElectron_genPartFlav == 5
    TH1F LIG5_dxyErr      = TH1F("Iron_Flav5_dxyErr","Iron_Flav5_dxyErr",     50,0.0,0.2);
    TH1F LILG5_dxyErr    = TH1F("IronLong_Flav5_dxyErr","IronLong_Flav5_dxyErr",  50,0.0,0.2);
    TH1F LIFG5_dxyErr    = TH1F("IronFake_Flav5_dxyErr","IronFake_Flav5_dxyErr",  50,0.0,0.2);
    
    
    
    // ISO
    
    

 // LowISOElectron_genPartFlav == 0
    TH1F LIG0_ISO       = TH1F("Iron_Flav0_ISO","Iron_Flav0_ISO",     50,0.0,20);
    TH1F LILG0_ISO    = TH1F("IronLong_Flav0_ISO","IronLong_Flav0_ISO",  50,0.0,20);
    TH1F LIFG0_ISO    = TH1F("IronFake_Flav0_ISO","IronFake_Flav0_ISO",  50,0.0,20);
    // LowISOElectron_genPartFlav == 1
    TH1F LIG1_ISO       = TH1F("Iron_Flav1_ISO","Iron_Flav1_ISO",     50,0.0,20);
    TH1F LILG1_ISO    = TH1F("IronLong_Flav1_ISO","IronLong_Flav1_ISO",  50,0.0,20);
    TH1F LIFG1_ISO    = TH1F("IronFake_Flav1_ISO","IronFake_Flav1_ISO",  50,0.0,20);
    // LowISOElectron_genPartFlav == 5
    TH1F LIG5_ISO      = TH1F("Iron_Flav5_ISO","Iron_Flav5_ISO",     50,0.0,20);
    TH1F LILG5_ISO    = TH1F("IronLong_Flav5_ISO","IronLong_Flav5_ISO",  50,0.0,20);
    TH1F LIFG5_ISO    = TH1F("IronFake_Flav5_ISO","IronFake_Flav5_ISO",  50,0.0,20);
    
    
    
    // Conveto
    
    

 // LowCONVElectron_genPartFlav == 0
    TH1F LIG0_CONV       = TH1F("Iron_Flav0_CONV","Iron_Flav0_CONV",     2,0.0,2.0);
    TH1F LILG0_CONV    = TH1F("IronLong_Flav0_CONV","IronLong_Flav0_CONV",  2,0.0,2.0);
    TH1F LIFG0_CONV    = TH1F("IronFake_Flav0_CONV","IronFake_Flav0_CONV",  2,0.0,2.0);
    // LowCONVElectron_genPartFlav == 1
    TH1F LIG1_CONV       = TH1F("Iron_Flav1_CONV","Iron_Flav1_CONV",     2,0.0,2.0);
    TH1F LILG1_CONV    = TH1F("IronLong_Flav1_CONV","IronLong_Flav1_CONV",  2,0.0,2.0);
    TH1F LIFG1_CONV    = TH1F("IronFake_Flav1_CONV","IronFake_Flav1_CONV",  2,0.0,2.0);
    // LowCONVElectron_genPartFlav == 5
    TH1F LIG5_CONV      = TH1F("Iron_Flav5_CONV","Iron_Flav5_CONV",     2,0.0,2.0);
    TH1F LILG5_CONV    = TH1F("IronLong_Flav5_CONV","IronLong_Flav5_CONV",  2,0.0,2.0);
    TH1F LIFG5_CONV    = TH1F("IronFake_Flav5_CONV","IronFake_Flav5_CONV",  2,0.0,2.0);
    
    
    
    
    //dxy Sig
    
    

 // LowdxySigElectron_genPartFlav == 0
    TH1F LIG0_dxySig       = TH1F("Iron_Flav0_dxySig","Iron_Flav0_dxySig",     50,-5.0,5.0);
    TH1F LILG0_dxySig    = TH1F("IronLong_Flav0_dxySig","IronLong_Flav0_dxySig",  50,-5.0,5.0);
    TH1F LIFG0_dxySig    = TH1F("IronFake_Flav0_dxySig","IronFake_Flav0_dxySig",  50,-5.0,5.0);
    // LowdxySigElectron_genPartFlav == 1
    TH1F LIG1_dxySig       = TH1F("Iron_Flav1_dxySig","Iron_Flav1_dxySig",     50,-5.0,5.0);
    TH1F LILG1_dxySig    = TH1F("IronLong_Flav1_dxySig","IronLong_Flav1_dxySig",  50,-5.0,5.0);
    TH1F LIFG1_dxySig    = TH1F("IronFake_Flav1_dxySig","IronFake_Flav1_dxySig",  50,-5.0,5.0);
    // LowdxySigElectron_genPartFlav == 5
    TH1F LIG5_dxySig      = TH1F("Iron_Flav5_dxySig","Iron_Flav5_dxySig",     50,-5.0,5.0);
    TH1F LILG5_dxySig    = TH1F("IronLong_Flav5_dxySig","IronLong_Flav5_dxySig",  50,-5.0,5.0);
    TH1F LIFG5_dxySig    = TH1F("IronFake_Flav5_dxySig","IronFake_Flav5_dxySig",  50,-5.0,5.0);
    
    
    //dz Sig
    
    

 // LowdzSigElectron_genPartFlav == 0
    TH1F LIG0_dzSig       = TH1F("Iron_Flav0_dzSig","Iron_Flav0_dzSig",     50,-5.0,5.0);
    TH1F LILG0_dzSig    = TH1F("IronLong_Flav0_dzSig","IronLong_Flav0_dzSig",  50,-5.0,5.0);
    TH1F LIFG0_dzSig    = TH1F("IronFake_Flav0_dzSig","IronFake_Flav0_dzSig",  50,-5.0,5.0);
    // LowdzSigElectron_genPartFlav == 1
    TH1F LIG1_dzSig       = TH1F("Iron_Flav1_dzSig","Iron_Flav1_dzSig",     50,-5.0,5.0);
    TH1F LILG1_dzSig    = TH1F("IronLong_Flav1_dzSig","IronLong_Flav1_dzSig",  50,-5.0,5.0);
    TH1F LIFG1_dzSig    = TH1F("IronFake_Flav1_dzSig","IronFake_Flav1_dzSig",  50,-5.0,5.0);
    // LowdzSigElectron_genPartFlav == 5
    TH1F LIG5_dzSig      = TH1F("Iron_Flav5_dzSig","Iron_Flav5_dzSig",     50,-5.0,5.0);
    TH1F LILG5_dzSig    = TH1F("IronLong_Flav5_dzSig","IronLong_Flav5_dzSig",  50,-5.0,5.0);
    TH1F LIFG5_dzSig    = TH1F("IronFake_Flav5_dzSig","IronFake_Flav5_dzSig",  50,-5.0,5.0);
    
    
    // Convlusion
    
    

 // LowIpSigElectron_genPartFlav == 0
    TH1F LIG0_IpSig       = TH1F("Iron_Flav0_IpSig","Iron_Flav0_IpSig",     50,0.0,5.0);
    TH1F LILG0_IpSig    = TH1F("IronLong_Flav0_IpSig","IronLong_Flav0_IpSig",  50,0.0,5.0);
    TH1F LIFG0_IpSig    = TH1F("IronFake_Flav0_IpSig","IronFake_Flav0_IpSig",  50,0.0,5.0);
    // LowIpSigElectron_genPartFlav == 1
    TH1F LIG1_IpSig       = TH1F("Iron_Flav1_IpSig","Iron_Flav1_IpSig",     50,0.0,5.0);
    TH1F LILG1_IpSig    = TH1F("IronLong_Flav1_IpSig","IronLong_Flav1_IpSig",  50,0.0,5.0);
    TH1F LIFG1_IpSig    = TH1F("IronFake_Flav1_IpSig","IronFake_Flav1_IpSig",  50,0.0,5.0);
    // LowIpSigElectron_genPartFlav == 5
    TH1F LIG5_IpSig      = TH1F("Iron_Flav5_IpSig","Iron_Flav5_IpSig",     50,0.0,5.0);
    TH1F LILG5_IpSig    = TH1F("IronLong_Flav5_IpSig","IronLong_Flav5_IpSig",  50,0.0,5.0);
    TH1F LIFG5_IpSig    = TH1F("IronFake_Flav5_IpSig","IronFake_Flav5_IpSig",  50,0.0,5.0);
    
    
    
    
    
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
        
        // loop over electrons
        for (int k = 0; k < nLowPtElectron; ++k)
        {
            float dxySig = -999;
            // avoid dividing by 0
            if (LowPtElectron_dxyErr[k] != 0)
            {
                dxySig = abs(LowPtElectron_dxy[k] / LowPtElectron_dxyErr[k]);
            }
            
            
            float dzSig = -999;
            
            if (LowPtElectron_dxyErr[k] != 0)
            {
                dzSig = abs(LowPtElectron_dz[k] / LowPtElectron_dzErr[k]);
            }
                
            
            float IPsig =-999;
            if (LowPtElectron_dxyErr[k] != 0)
            {
                IPsig = sqrt(dxySig*dxySig + dzSig*dzSig);
        
            }
    
               
           
            
            
            if (LowPtElectron_eta[k] <2.4 && LowPtElectron_embeddedID[k]>=1.5)
            {
                if (LowPtElectron_miniPFRelIso_all[k] <4 && LowPtElectron_dxy[k]<0.05 && LowPtElectron_dz[k]<0.1 && IPsig<2)
                {
                    //////////////////////////////////////////////////
                    //Iron
                    
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {
                        LIG0_EMID.Fill(LowPtElectron_embeddedID[k]);
                        LIG0_eta.Fill(LowPtElectron_eta[k]);
                        LIG0_pt.Fill(LowPtElectron_pt[k]);
                        LIG0_dxy.Fill(LowPtElectron_dxy[k]);
                        LIG0_dz.Fill(LowPtElectron_dz[k]);
                        LIG0_dzErr.Fill(LowPtElectron_dzErr[k]);
                        LIG0_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                   
                        LIG0_dzSig.Fill(dzSig);
                        LIG0_dxySig.Fill(dxySig);
                        LIG0_IpSig.Fill(IPsig);
                        LIG0_CONV.Fill(LowPtElectron_convVeto[k]);
                        LIG0_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                    }
                    // LowPtElectron_genPartFlav == 1
                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        LIG1_EMID.Fill(LowPtElectron_embeddedID[k]);
                        LIG1_eta.Fill(LowPtElectron_eta[k]);
                        LIG1_pt.Fill(LowPtElectron_pt[k]);
                        LIG1_dxy.Fill(LowPtElectron_dxy[k]);
                        LIG1_dz.Fill(LowPtElectron_dz[k]);
                        LIG1_dzErr.Fill(LowPtElectron_dzErr[k]);
                        LIG1_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                       
                        LIG1_dzSig.Fill(dzSig);
                        LIG1_dxySig.Fill(dxySig);
                        LIG1_IpSig.Fill(IPsig);
                        LIG1_CONV.Fill(LowPtElectron_convVeto[k]);
                        LIG1_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                    }
                    // LowPtElectron_genPartFlav == 5
                    if (LowPtElectron_genPartFlav[k] == 5)
                    {
                        LIG5_EMID.Fill(LowPtElectron_embeddedID[k]);
                        LIG5_eta.Fill(LowPtElectron_eta[k]);
                        LIG5_pt.Fill(LowPtElectron_pt[k]);
                        LIG5_dxy.Fill(LowPtElectron_dxy[k]);
                        LIG5_dz.Fill(LowPtElectron_dz[k]);
                        LIG5_dzErr.Fill(LowPtElectron_dzErr[k]);
                        LIG5_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                      
                        LIG5_dzSig.Fill(dzSig);
                        LIG5_dxySig.Fill(dxySig);
                        LIG5_IpSig.Fill(IPsig);
                        LIG5_CONV.Fill(LowPtElectron_convVeto[k]);
                        LIG5_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                    }

                }
                
                
                
                
                
                if (LowPtElectron_miniPFRelIso_all[k] <4 &&  IPsig>=2)
                {
                    //////////////////////////////////////////////////
                    //Irom Long
                    
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {
                        LILG0_EMID.Fill(LowPtElectron_embeddedID[k]);
                        LILG0_eta.Fill(LowPtElectron_eta[k]);
                        LILG0_pt.Fill(LowPtElectron_pt[k]);
                        LILG0_dxy.Fill(LowPtElectron_dxy[k]);
                        LILG0_dz.Fill(LowPtElectron_dz[k]);
                        LILG0_dzErr.Fill(LowPtElectron_dzErr[k]);
                        LILG0_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                       
                        LILG0_dzSig.Fill(dzSig);
                        LILG0_dxySig.Fill(dxySig);
                        LILG0_IpSig.Fill(IPsig);
                        LILG0_CONV.Fill(LowPtElectron_convVeto[k]);
                        LILG0_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                    }
                    // LowPtElectron_genPartFlav == 1
                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        LILG1_EMID.Fill(LowPtElectron_embeddedID[k]);
                        LILG1_eta.Fill(LowPtElectron_eta[k]);
                        LILG1_pt.Fill(LowPtElectron_pt[k]);
                        LILG1_dxy.Fill(LowPtElectron_dxy[k]);
                        LILG1_dz.Fill(LowPtElectron_dz[k]);
                        LILG1_dzErr.Fill(LowPtElectron_dzErr[k]);
                        LILG1_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                     
                        LILG1_dzSig.Fill(dzSig);
                        LILG1_dxySig.Fill(dxySig);
                        LILG1_IpSig.Fill(IPsig);
                        LILG1_CONV.Fill(LowPtElectron_convVeto[k]);
                        LILG1_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                    }
                    // LowPtElectron_genPartFlav == 5
                    if (LowPtElectron_genPartFlav[k] == 5)
                    {
                        LILG5_EMID.Fill(LowPtElectron_embeddedID[k]);
                        LILG5_eta.Fill(LowPtElectron_eta[k]);
                        LILG5_pt.Fill(LowPtElectron_pt[k]);
                        LILG5_dxy.Fill(LowPtElectron_dxy[k]);
                        LILG5_dz.Fill(LowPtElectron_dz[k]);
                        LILG5_dzErr.Fill(LowPtElectron_dzErr[k]);
                        LILG5_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                   
                        LILG5_dzSig.Fill(dzSig);
                        LILG5_dxySig.Fill(dxySig);
                        LILG5_IpSig.Fill(IPsig);
                        LILG5_CONV.Fill(LowPtElectron_convVeto[k]);
                        LILG5_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                    }

                }
                
                
                
                
                
                if (LowPtElectron_miniPFRelIso_all[k] <4 && LowPtElectron_dxy[k]<0.05 && LowPtElectron_dz[k]<0.1 && IPsig<2)
                {
                    //////////////////////////////////////////////////
               
                    //Iron Fake
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {
                        LIFG0_EMID.Fill(LowPtElectron_embeddedID[k]);
                        LIFG0_eta.Fill(LowPtElectron_eta[k]);
                        LIFG0_pt.Fill(LowPtElectron_pt[k]);
                        LIFG0_dxy.Fill(LowPtElectron_dxy[k]);
                        LIFG0_dz.Fill(LowPtElectron_dz[k]);
                        LIFG0_dzErr.Fill(LowPtElectron_dzErr[k]);
                        LIFG0_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                       
                        LIFG0_dzSig.Fill(dzSig);
                        LIFG0_dxySig.Fill(dxySig);
                        LIFG0_IpSig.Fill(IPsig);
                        LIFG0_CONV.Fill(LowPtElectron_convVeto[k]);
                        LIFG0_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                    }
                    // LowPtElectron_genPartFlav == 1
                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        LIFG1_EMID.Fill(LowPtElectron_embeddedID[k]);
                        LIFG1_eta.Fill(LowPtElectron_eta[k]);
                        LIFG1_pt.Fill(LowPtElectron_pt[k]);
                        LIFG1_dxy.Fill(LowPtElectron_dxy[k]);
                        LIFG1_dz.Fill(LowPtElectron_dz[k]);
                        LIFG1_dzErr.Fill(LowPtElectron_dzErr[k]);
                        LIFG1_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                   
                        LIFG1_dzSig.Fill(dzSig);
                        LIFG1_dxySig.Fill(dxySig);
                        LIFG1_IpSig.Fill(IPsig);
                        LIFG1_CONV.Fill(LowPtElectron_convVeto[k]);
                        LIFG1_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                    }
                    // LowPtElectron_genPartFlav == 5
                    if (LowPtElectron_genPartFlav[k] == 5)
                    {
                        LIFG1_EMID.Fill(LowPtElectron_embeddedID[k]);
                        LIFG1_eta.Fill(LowPtElectron_eta[k]);
                        LIFG1_pt.Fill(LowPtElectron_pt[k]);
                        LIFG1_dxy.Fill(LowPtElectron_dxy[k]);
                        LIFG1_dz.Fill(LowPtElectron_dz[k]);
                        LIFG1_dzErr.Fill(LowPtElectron_dzErr[k]);
                        LIFG1_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                       
                        LIFG1_dzSig.Fill(dzSig);
                        LIFG1_dxySig.Fill(dxySig);
                        LIFG1_IpSig.Fill(IPsig);
                        LIFG1_CONV.Fill(LowPtElectron_convVeto[k]);
                        LIFG1_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                    }

                }
            }   
        }
    }

    
    
    PlotHist(LIG0_EMID,      sample, plot_dir, "EMID_Iron_Flav0", "EMID");
    PlotHist(LILG0_EMID,      sample, plot_dir, "EMID_IronLong_Flav0","EMID");
    PlotHist(LIFG0_EMID,      sample, plot_dir, "EMID_IronFake_Flav0", "EMID");
    
    PlotHist(LIG1_EMID,      sample, plot_dir, "EMID_Iron_Flav1", "EMID");
    PlotHist(LILG1_EMID,      sample, plot_dir, "EMID_IronLong_Flav1", "EMID");
    PlotHist(LIFG1_EMID,      sample, plot_dir, "EMID_IronFake_Flav1", "EMID");
    
    PlotHist(LIG5_EMID,      sample, plot_dir, "EMID_Iron_Flav5", "EMID");
    PlotHist(LILG5_EMID,      sample, plot_dir, "EMID_IronLong_Flav5", "EMID");
    PlotHist(LIFG5_EMID,      sample, plot_dir, "EMID_IronFake_Flav5", "EMID");
    
    PlotHist(LIG0_pt,      sample, plot_dir, "pt_Iron_Flav0", "pt");
    PlotHist(LILG0_pt,      sample, plot_dir, "pt_pt_IronLong_Flav0","pt");
    PlotHist(LIFG0_pt,      sample, plot_dir, "pt_IronFake_Flav0", "pt");
    
    PlotHist(LIG1_pt,      sample, plot_dir, "pt_Iron_Flav1", "pt");
    PlotHist(LILG1_pt,      sample, plot_dir, "pt_IronLong_Flav1", "pt");
    PlotHist(LIFG1_pt,      sample, plot_dir, "pt_IronFake_Flav1", "pt");
    
    PlotHist(LIG5_pt,      sample, plot_dir, "pt_Iron_Flav5", "pt");
    PlotHist(LILG5_pt,      sample, plot_dir, "pt_IronLong_Flav5", "pt");
    PlotHist(LIFG5_pt,      sample, plot_dir, "pt_IronFake_Flav5", "pt");
    
    
    PlotHist(LIG0_eta,      sample, plot_dir, "eta_Iron_Flav0", "eta");
    PlotHist(LILG0_eta,      sample, plot_dir, "eta_IronLong_Flav0","eta");
    PlotHist(LIFG0_eta,      sample, plot_dir, "eta_IronFake_Flav0", "eta");
    
    PlotHist(LIG1_eta,      sample, plot_dir, "eta_Iron_Flav1", "eta");
    PlotHist(LILG1_eta,      sample, plot_dir, "eta_IronLong_Flav1", "eta");
    PlotHist(LIFG1_eta,      sample, plot_dir, "eta_IronFake_Flav1", "eta");
    
    PlotHist(LIG5_eta,      sample, plot_dir, "eta_Iron_Flav5", "eta");
    PlotHist(LILG5_eta,      sample, plot_dir, "eta_IronLong_Flav5", "eta");
    PlotHist(LIFG5_eta,      sample, plot_dir, "eta_IronFake_Flav5", "eta");
    
    PlotHist(LIG0_dxy,      sample, plot_dir, "dxy_Iron_Flav0", "dxy");
    PlotHist(LILG0_dxy,      sample, plot_dir, "dxy_IronLong_Flav0","dxy");
    PlotHist(LIFG0_dxy,      sample, plot_dir, "dxy_IronFake_Flav0", "dxy");
    
    PlotHist(LIG1_dxy,      sample, plot_dir, "dxy_Iron_Flav1", "dxy");
    PlotHist(LILG1_dxy,      sample, plot_dir, "dxy_IronLong_Flav1", "dxy");
    PlotHist(LIFG1_dxy,      sample, plot_dir, "dxy_IronFake_Flav1", "dxy");
    
    PlotHist(LIG5_dxy,      sample, plot_dir, "dxy_Iron_Flav5", "dxy");
    PlotHist(LILG5_dxy,      sample, plot_dir, "dxy_IronLong_Flav5", "dxy");
    PlotHist(LIFG5_dxy,      sample, plot_dir, "dxy_IronFake_Flav5", "dxy");
    
    PlotHist(LIG0_dxyErr,      sample, plot_dir, "dxyErr_Iron_Flav0", "dxyErr");
    PlotHist(LILG0_dxyErr,      sample, plot_dir, "dxyErr_IronLong_Flav0","dxyErr");
    PlotHist(LIFG0_dxyErr,      sample, plot_dir, "dxyErr_IronFake_Flav0", "dxyErr");
    
    PlotHist(LIG1_dxyErr,      sample, plot_dir, "dxyErr_Iron_Flav1", "dxyErr");
    PlotHist(LILG1_dxyErr,      sample, plot_dir, "dxyErr_IronLong_Flav1", "dxyErr");
    PlotHist(LIFG1_dxyErr,      sample, plot_dir, "dxyErr_IronFake_Flav1", "dxyErr");
    
    PlotHist(LIG5_dxyErr,      sample, plot_dir, "dxyErr_Iron_Flav5", "dxyErr");
    PlotHist(LILG5_dxyErr,      sample, plot_dir, "dxyErr_IronLong_Flav5", "dxyErr");
    PlotHist(LIFG5_dxyErr,      sample, plot_dir, "dxyErr_IronFake_Flav5", "dxyErr");
    
    PlotHist(LIG0_dz,      sample, plot_dir, "dz_Iron_Flav0", "dz");
    PlotHist(LILG0_dz,      sample, plot_dir, "dz_IronLong_Flav0","dz");
    PlotHist(LIFG0_dz,      sample, plot_dir, "dz_IronFake_Flav0", "dz");
    
    PlotHist(LIG1_dz,      sample, plot_dir, "dz_Iron_Flav1", "dz");
    PlotHist(LILG1_dz,      sample, plot_dir, "dz_IronLong_Flav1", "dz");
    PlotHist(LIFG1_dz,      sample, plot_dir, "dz_IronFake_Flav1", "dz");
    
    PlotHist(LIG5_dz,      sample, plot_dir, "dz_Iron_Flav5", "dz");
    PlotHist(LILG5_dz,      sample, plot_dir, "dz_IronLong_Flav5", "dz");
    PlotHist(LIFG5_dz,      sample, plot_dir, "dz_IronFake_Flav5", "dz");
    
    
    PlotHist(LIG0_dzErr,      sample, plot_dir, "dzErr_Iron_Flav0", "dzErr");
    PlotHist(LILG0_dzErr,      sample, plot_dir, "dzErr_IronLong_Flav0","dzErr");
    PlotHist(LIFG0_dzErr,      sample, plot_dir, "dzErr_IronFake_Flav0", "dzErr");
    
    PlotHist(LIG1_dzErr,      sample, plot_dir, "dzErr_Iron_Flav1", "dzErr");
    PlotHist(LILG1_dzErr,      sample, plot_dir, "dzErr_IronLong_Flav1", "dzErr");
    PlotHist(LIFG1_dzErr,      sample, plot_dir, "dzErr_IronFake_Flav1", "dzErr");
    
    PlotHist(LIG5_dzErr,      sample, plot_dir, "dzErr_Iron_Flav5", "dzErr");
    PlotHist(LILG5_dzErr,      sample, plot_dir, "dzErr_IronLong_Flav5", "dzErr");
    PlotHist(LIFG5_dzErr,      sample, plot_dir, "dzErr_IronFake_Flav5", "dzErr");
    
    PlotHist(LIG0_CONV,      sample, plot_dir, "CONV_Iron_Flav0", "CONV");
    PlotHist(LILG0_CONV,      sample, plot_dir, "CONV_IronLong_Flav0","CONV");
    PlotHist(LIFG0_CONV,      sample, plot_dir, "CONV_IronFake_Flav0", "CONV");
    
    PlotHist(LIG1_CONV,      sample, plot_dir, "CONV_Iron_Flav1", "CONV");
    PlotHist(LILG1_CONV,      sample, plot_dir, "CONV_IronLong_Flav1", "CONV");
    PlotHist(LIFG1_CONV,      sample, plot_dir, "CONV_IronFake_Flav1", "CONV");
    
    PlotHist(LIG5_CONV,      sample, plot_dir, "CONV_Iron_Flav5", "CONV");
    PlotHist(LILG5_CONV,      sample, plot_dir, "CONV_IronLong_Flav5", "CONV");
    PlotHist(LIFG5_CONV,      sample, plot_dir, "CONV_IronFake_Flav5", "CONV");
    
    PlotHist(LIG0_ISO,      sample, plot_dir, "ISO_Iron_Flav0", "ISO");
    PlotHist(LILG0_ISO,      sample, plot_dir, "ISO_IronLong_Flav0","ISO");
    PlotHist(LIFG0_ISO,      sample, plot_dir, "ISO_IronFake_Flav0", "ISO");
    
    PlotHist(LIG1_ISO,      sample, plot_dir, "ISO_Iron_Flav1", "ISO");
    PlotHist(LILG1_ISO,      sample, plot_dir, "ISO_IronLong_Flav1", "ISO");
    PlotHist(LIFG1_ISO,      sample, plot_dir, "ISO_IronFake_Flav1", "ISO");
    
    PlotHist(LIG5_ISO,      sample, plot_dir, "ISO_Iron_Flav5", "ISO");
    PlotHist(LILG5_ISO,      sample, plot_dir, "ISO_IronLong_Flav5", "ISO");
    PlotHist(LIFG5_ISO,      sample, plot_dir, "ISO_IronFake_Flav5", "ISO");
    
    PlotHist(LIG0_IpSig,      sample, plot_dir, "IpSig_Iron_Flav0", "IpSig");
    PlotHist(LILG0_IpSig,      sample, plot_dir, "IpSig_IronLong_Flav0","IpSig");
    PlotHist(LIFG0_IpSig,      sample, plot_dir, "IpSig_IronFake_Flav0", "IpSig");
    
    PlotHist(LIG1_IpSig,      sample, plot_dir, "IpSig_Iron_Flav1", "IpSig");
    PlotHist(LILG1_IpSig,      sample, plot_dir, "IpSig_IronLong_Flav1", "IpSig");
    PlotHist(LIFG1_IpSig,      sample, plot_dir, "IpSig_IronFake_Flav1", "IpSig");
    
    PlotHist(LIG5_IpSig,      sample, plot_dir, "IpSig_Iron_Flav5", "IpSig");
    PlotHist(LILG5_IpSig,      sample, plot_dir, "IpSig_IronLong_Flav5", "IpSig");
    PlotHist(LIFG5_IpSig,      sample, plot_dir, "IpSig_IronFake_Flav5", "IpSig");
    
    PlotHist(LIG0_dxySig,      sample, plot_dir, "dxySigIron_Flav0", "dxySig");
    PlotHist(LILG0_dxySig,      sample, plot_dir, "dxySigIronLong_Flav0","dxySig");
    PlotHist(LIFG0_dxySig,      sample, plot_dir, "dxySigIronFake_Flav0", "dxySig");
    
    PlotHist(LIG1_dxySig,      sample, plot_dir, "dxySigIron_Flav1", "dxySig");
    PlotHist(LILG1_dxySig,      sample, plot_dir, "dxySigIronLong_Flav1", "dxySig");
    PlotHist(LIFG1_dxySig,      sample, plot_dir, "dxySigIronFake_Flav1", "dxySig");
    
    PlotHist(LIG5_dxySig,      sample, plot_dir, "dxySigIron_Flav5", "dxySig");
    PlotHist(LILG5_dxySig,      sample, plot_dir, "dxySigIronLong_Flav5", "dxySig");
    PlotHist(LIFG5_dxySig,      sample, plot_dir, "dxySigIronFake_Flav5", "dxySig");
    
    PlotHist(LIG0_dzSig,      sample, plot_dir, "dzSig_Iron_Flav0", "dzSig");
    PlotHist(LILG0_dzSig,      sample, plot_dir, "dzSig_IronLong_Flav0","dzSig");
    PlotHist(LIFG0_dzSig,      sample, plot_dir, "dzSig_IronFake_Flav0", "dzSig");
    
    PlotHist(LIG1_dzSig,      sample, plot_dir, "dzSig_Iron_Flav1", "dzSig");
    PlotHist(LILG1_dzSig,      sample, plot_dir, "dzSig_IronLong_Flav1", "dzSig");
    PlotHist(LIFG1_dzSig,      sample, plot_dir, "dzSig_IronFake_Flav1", "dzSig");
    
    PlotHist(LIG5_dzSig,      sample, plot_dir, "dzSig_Iron_Flav5", "dzSig");
    PlotHist(LILG5_dzSig,      sample, plot_dir, "dzSig_IronLong_Flav5", "dzSig");
    PlotHist(LIFG5_dzSig,      sample, plot_dir, "dzSig_IronFake_Flav5", "dzSig");
}
