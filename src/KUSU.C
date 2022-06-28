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
    
    // histograms
    // no selection
    TH1F h_nLowPtElectron            = TH1F("h_nLowPtElectron",             "h_nLowPtElectron",              6,    0.0,  6.0);
    TH1F h_LowPtElectron_pt          = TH1F("h_LowPtElectron_pt",           "h_LowPtElectron_pt",           20,    0.0,  20.0);
    TH1F h_LowPtElectron_eta         = TH1F("h_LowPtElectron_eta",          "h_LowPtElectron_eta",          20,   -3.0,  3.0);
    TH1F h_LowPtElectron_phi         = TH1F("h_LowPtElectron_phi",          "h_LowPtElectron_phi",          20,  -M_PI,  M_PI);
    TH1F h_LowPtElectron_mass        = TH1F("h_LowPtElectron_mass",         "h_LowPtElectron_mass",         20,  -0.01,  0.01);
    TH1F h_LowPtElectron_genPartFlav = TH1F("h_LowPtElectron_genPartFlav",  "h_LowPtElectron_genPartFlav",  30,      0,  30);
    TH1F h_LowPtElectron_dxy         = TH1F("h_LowPtElectron_dxy",          "h_LowPtElectron_dxy",          50,  -0.02,  0.02);
    TH1F h_LowPtElectron_dxyErr      = TH1F("h_LowPtElectron_dxyErr",       "h_LowPtElectron_dxyErr",       50,      0,  0.1);
    TH1F h_LowPtElectron_dxySig      = TH1F("h_LowPtElectron_dxySig",       "h_LowPtElectron_dxySig",       50,      0,  5.0);
    // LowPtElectron_genPartFlav == 0
    TH1F h_LowPtElectron_dxy_genPartFlav0       = TH1F("h_LowPtElectron_dxy_genPartFlav0",      "h_LowPtElectron_dxy_genPartFlav0",     50,  -0.02,  0.02);
    TH1F h_LowPtElectron_dxyErr_genPartFlav0    = TH1F("h_LowPtElectron_dxyErr_genPartFlav0",   "h_LowPtElectron_dxyErr_genPartFlav0",  50,      0,  0.1);
    TH1F h_LowPtElectron_dxySig_genPartFlav0    = TH1F("h_LowPtElectron_dxySig_genPartFlav0",   "h_LowPtElectron_dxySig_genPartFlav0",  50,      0,  5.0);
    // LowPtElectron_genPartFlav == 1
    TH1F h_LowPtElectron_dxy_genPartFlav1       = TH1F("h_LowPtElectron_dxy_genPartFlav1",      "h_LowPtElectron_dxy_genPartFlav1",     50,  -0.02,  0.02);
    TH1F h_LowPtElectron_dxyErr_genPartFlav1    = TH1F("h_LowPtElectron_dxyErr_genPartFlav1",   "h_LowPtElectron_dxyErr_genPartFlav1",  50,      0,  0.1);
    TH1F h_LowPtElectron_dxySig_genPartFlav1    = TH1F("h_LowPtElectron_dxySig_genPartFlav1",   "h_LowPtElectron_dxySig_genPartFlav1",  50,      0,  5.0);
    // LowPtElectron_genPartFlav == 5
    TH1F h_LowPtElectron_dxy_genPartFlav5       = TH1F("h_LowPtElectron_dxy_genPartFlav5",      "h_LowPtElectron_dxy_genPartFlav5",     50,  -0.02,  0.02);
    TH1F h_LowPtElectron_dxyErr_genPartFlav5    = TH1F("h_LowPtElectron_dxyErr_genPartFlav5",   "h_LowPtElectron_dxyErr_genPartFlav5",  50,      0,  0.1);
    TH1F h_LowPtElectron_dxySig_genPartFlav5    = TH1F("h_LowPtElectron_dxySig_genPartFlav5",   "h_LowPtElectron_dxySig_genPartFlav5",  50,      0,  5.0);
    
    
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
            
            
            float dzSig = -999;
            
            if (LowPtElectron_dxyErr[k] != 0)
            {
                dzSig = abs(LowPtElectron_dz[k] / LowPtElectron_dzErr[k]);
            }
                
            
            float IPsig =-999;
            if (LowPtElectron_dxyErr[k] != 0)
            {
                IPsig = sqrt(dxySig*dxySig + dzsig*dzSig)
        
            }
    
               
            // fill histograms
            h_LowPtElectron_pt.Fill(LowPtElectron_pt[k]);
            h_LowPtElectron_eta.Fill(LowPtElectron_eta[k]);
            h_LowPtElectron_phi.Fill(LowPtElectron_phi[k]);
            h_LowPtElectron_mass.Fill(LowPtElectron_mass[k]);
            h_LowPtElectron_genPartFlav.Fill(LowPtElectron_genPartFlav[k]);
            h_LowPtElectron_dxy.Fill(LowPtElectron_dxy[k]);
            h_LowPtElectron_dxyErr.Fill(LowPtElectron_dxyErr[k]);
            h_LowPtElectron_dxySig.Fill(dxySig);
            // LowPtElectron_genPartFlav == 0
            if (LowPtElectron_genPartFlav[k] == 0)
            {
                h_LowPtElectron_dxy_genPartFlav0.Fill(LowPtElectron_dxy[k]);
                h_LowPtElectron_dxyErr_genPartFlav0.Fill(LowPtElectron_dxyErr[k]);
                h_LowPtElectron_dxySig_genPartFlav0.Fill(dxySig);
            }
            // LowPtElectron_genPartFlav == 1
            if (LowPtElectron_genPartFlav[k] == 1)
            {
                h_LowPtElectron_dxy_genPartFlav1.Fill(LowPtElectron_dxy[k]);
                h_LowPtElectron_dxyErr_genPartFlav1.Fill(LowPtElectron_dxyErr[k]);
                h_LowPtElectron_dxySig_genPartFlav1.Fill(dxySig);
            }
            // LowPtElectron_genPartFlav == 5
            if (LowPtElectron_genPartFlav[k] == 5)
            {
                h_LowPtElectron_dxy_genPartFlav5.Fill(LowPtElectron_dxy[k]);
                h_LowPtElectron_dxyErr_genPartFlav5.Fill(LowPtElectron_dxyErr[k]);
                h_LowPtElectron_dxySig_genPartFlav5.Fill(dxySig);
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
                    }
                    // LowPtElectron_genPartFlav == 1
                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        LILG0_EMID.Fill(LowPtElectron_embeddedID[k]);
                    }
                    // LowPtElectron_genPartFlav == 5
                    if (LowPtElectron_genPartFlav[k] == 5)
                    {
                        LIFG0_EMID.Fill(LowPtElectron_embeddedID[k]);
                    }

                }
                
                
                
                
                
                if (LowPtElectron_miniPFRelIso_all[k] <4 &&  IPsig>=2)
                {
                    //////////////////////////////////////////////////
                    //Irom Long
                    
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {
                        LIG1_EMID.Fill(LowPtElectron_embeddedID[k]);
                    }
                    // LowPtElectron_genPartFlav == 1
                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        LILG1_EMID.Fill(LowPtElectron_embeddedID[k]);
                    }
                    // LowPtElectron_genPartFlav == 5
                    if (LowPtElectron_genPartFlav[k] == 5)
                    {
                        LIFG1_EMID.Fill(LowPtElectron_embeddedID[k]);
                    }

                }
                
                
                
                
                
                if (LowPtElectron_miniPFRelIso_all[k] <4 && LowPtElectron_dxy[k]<0.05 && LowPtElectron_dz[k]<0.1 && IPsig<2)
                {
                    //////////////////////////////////////////////////
               
                    //Iron Fake
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {
                        LIG5_EMID.Fill(LowPtElectron_embeddedID[k]);
                    }
                    // LowPtElectron_genPartFlav == 1
                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        LILG5_EMID.Fill(LowPtElectron_embeddedID[k]);
                    }
                    // LowPtElectron_genPartFlav == 5
                    if (LowPtElectron_genPartFlav[k] == 5)
                    {
                        LIFG5_EMID.Fill(LowPtElectron_embeddedID[k]);
                    }

                }
            }   
        }
    }
    // plot histograms
    PlotHist(h_LowPtElectron_pt,            sample, plot_dir, "h_LowPtElectron_pt",             "pt");
    PlotHist(h_LowPtElectron_eta,           sample, plot_dir, "h_LowPtElectron_eta",            "eta");
    PlotHist(h_LowPtElectron_phi,           sample, plot_dir, "h_LowPtElectron_phi",            "phi");
    PlotHist(h_LowPtElectron_mass,          sample, plot_dir, "h_LowPtElectron_mass",           "mass");
    PlotHist(h_nLowPtElectron,              sample, plot_dir, "h_nLowPtElectron",               "nElectrons");
    PlotHist(h_LowPtElectron_genPartFlav,   sample, plot_dir, "h_LowPtElectron_genPartFlav",    "genPartFlav");
    PlotHist(h_LowPtElectron_dxy,           sample, plot_dir, "h_LowPtElectron_dxy",            "dxy");
    PlotHist(h_LowPtElectron_dxyErr,        sample, plot_dir, "h_LowPtElectron_dxyErr",         "dxyErr");
    PlotHist(h_LowPtElectron_dxySig,        sample, plot_dir, "h_LowPtElectron_dxySig",         "dxySig");
    // LowPtElectron_genPartFlav == 0
    PlotHist(h_LowPtElectron_dxy_genPartFlav0,      sample, plot_dir, "h_LowPtElectron_dxy_genPartFlav0",       "dxy");
    PlotHist(h_LowPtElectron_dxyErr_genPartFlav0,   sample, plot_dir, "h_LowPtElectron_dxyErr_genPartFlav0",    "dxyErr");
    PlotHist(h_LowPtElectron_dxySig_genPartFlav0,   sample, plot_dir, "h_LowPtElectron_dxySig_genPartFlav0",    "dxySig");
    // LowPtElectron_genPartFlav == 1
    PlotHist(h_LowPtElectron_dxy_genPartFlav1,      sample, plot_dir, "h_LowPtElectron_dxy_genPartFlav1",       "dxy");
    PlotHist(h_LowPtElectron_dxyErr_genPartFlav1,   sample, plot_dir, "h_LowPtElectron_dxyErr_genPartFlav1",    "dxyErr");
    PlotHist(h_LowPtElectron_dxySig_genPartFlav1,   sample, plot_dir, "h_LowPtElectron_dxySig_genPartFlav1",    "dxySig");
    // LowPtElectron_genPartFlav == 5
    PlotHist(h_LowPtElectron_dxy_genPartFlav5,      sample, plot_dir, "h_LowPtElectron_dxy_genPartFlav5",       "dxy");
    PlotHist(h_LowPtElectron_dxyErr_genPartFlav5,   sample, plot_dir, "h_LowPtElectron_dxyErr_genPartFlav5",    "dxyErr");
    PlotHist(h_LowPtElectron_dxySig_genPartFlav5,   sample, plot_dir, "h_LowPtElectron_dxySig_genPartFlav5",    "dxySig");
    
    
    PlotHist(LIG0_EMID,      sample, plot_dir, "Iron_Flav0", "EMID");
    PlotHist(LILG0_EMID,      sample, plot_dir, "IronLong_Flav0","EMID");
    PlotHist(LIFG0_EMID,      sample, plot_dir, "IronFake_Flav0", "EMID");
    
    PlotHist(LIG1_EMID,      sample, plot_dir, "Iron_Flav1", "EMID");
    PlotHist(LILG1_EMID,      sample, plot_dir, "IronLong_Flav1", "EMID");
    PlotHist(LIFG1_EMID,      sample, plot_dir, "IronFake_Flav1", "EMID");
    
    PlotHist(LIG5_EMID,      sample, plot_dir, "Iron_Flav5", "EMID");
    PlotHist(LILG5_EMID,      sample, plot_dir, "IronLong_Flav5", "EMID");
    PlotHist(LIFG5_EMID,      sample, plot_dir, "IronFake_Flav5", "EMID");
    
    
    
}
