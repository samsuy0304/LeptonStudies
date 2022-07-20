#define NanoClass_cxx
#include "NanoClass.h"
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <string>
#include <cmath>
#include <map>


// get label for variable
std::string NanoClass::GetLabel(std::string variable)
{
    // labels for variables
    std::map<std::string, std::string> labels {
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
        {"dz",          "d_{z}"},
        {"dzErr",       "d_{z} err"},
        {"dzSig",       "d_{z} sig"},
        {"ID",          "ID"},
        {"embeddedID",  "embedded ID"},
    };
    std::string label = "";
    // check if variable exists in labels
    if (labels.find(variable) == labels.end())
    {
        // variable does not exist
        printf("WARNING: the variable '%s' does not exist in labels; using '%s' for label.\n", variable.c_str(), variable.c_str());
        label = variable;
    }
    else
    {
        // variable exists
        label = labels[variable];
    }
    return label;
}

void NanoClass::SetupHist(TH1F &hist, std::string title, std::string x_title, std::string y_title, int color, int line_width)
{
    //hist.SetStats(kFALSE);
    hist.SetStats(kTRUE);
    
    TAxis* x_axis = hist.GetXaxis();
    TAxis* y_axis = hist.GetYaxis();
    
    hist.SetTitle(title.c_str());
    x_axis->SetTitle(x_title.c_str());
    y_axis->SetTitle(y_title.c_str());
    hist.SetLineColor(color);
    hist.SetLineWidth(line_width);
}

void NanoClass::PlotHist(TH1F &hist, std::string sample_name, std::string plot_dir, std::string plot_name, std::string variable)
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
    gStyle->SetOptStat(111111);
    
    if (fChain == 0)
    {
        return;
    }

    std::string plot_dir = "macro_plots";
    
    // T2-4bd
    //std::string sample = "SMS-T2-4bd_genMET-80_mStop-500_mLSP-490";
    // TTJets_DiLept
    std::string sample = "TTJets_DiLept";
    
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
    TH1F h_LowPtElectron_genPartFlav = TH1F("h_LowPtElectron_genPartFlav",  "h_LowPtElectron_genPartFlav",  30,    0.0,  30.0);
    TH1F h_LowPtElectron_dxy         = TH1F("h_LowPtElectron_dxy",          "h_LowPtElectron_dxy",          50,  -0.02,  0.02);
    TH1F h_LowPtElectron_dxyErr      = TH1F("h_LowPtElectron_dxyErr",       "h_LowPtElectron_dxyErr",       50,    0.0,  0.1);
    TH1F h_LowPtElectron_dxySig      = TH1F("h_LowPtElectron_dxySig",       "h_LowPtElectron_dxySig",       50,    0.0,  5.0);
    TH1F h_LowPtElectron_dz          = TH1F("h_LowPtElectron_dz",           "h_LowPtElectron_dz",           50,  -0.02,  0.02);
    TH1F h_LowPtElectron_dzErr       = TH1F("h_LowPtElectron_dzErr",        "h_LowPtElectron_dzErr",        50,    0.0,  0.1);
    TH1F h_LowPtElectron_dzSig       = TH1F("h_LowPtElectron_dzSig",        "h_LowPtElectron_dzSig",        50,    0.0,  5.0);
    TH1F h_LowPtElectron_ID          = TH1F("h_LowPtElectron_ID",           "h_LowPtElectron_ID",           50,   -1.0,  15.0);
    TH1F h_LowPtElectron_embeddedID  = TH1F("h_LowPtElectron_embeddedID",   "h_LowPtElectron_embeddedID",   50,   -1.0,  15.0);
    // LowPtElectron_genPartFlav == 0
    TH1F h_LowPtElectron_dxy_genPartFlav0           = TH1F("h_LowPtElectron_dxy_genPartFlav0",          "h_LowPtElectron_dxy_genPartFlav0",         50,  -0.02,  0.02);
    TH1F h_LowPtElectron_dxyErr_genPartFlav0        = TH1F("h_LowPtElectron_dxyErr_genPartFlav0",       "h_LowPtElectron_dxyErr_genPartFlav0",      50,    0.0,  0.1);
    TH1F h_LowPtElectron_dxySig_genPartFlav0        = TH1F("h_LowPtElectron_dxySig_genPartFlav0",       "h_LowPtElectron_dxySig_genPartFlav0",      50,    0.0,  5.0);
    TH1F h_LowPtElectron_dz_genPartFlav0            = TH1F("h_LowPtElectron_dz_genPartFlav0",           "h_LowPtElectron_dz_genPartFlav0",          50,  -0.02,  0.02);
    TH1F h_LowPtElectron_dzErr_genPartFlav0         = TH1F("h_LowPtElectron_dzErr_genPartFlav0",        "h_LowPtElectron_dzErr_genPartFlav0",       50,    0.0,  0.1);
    TH1F h_LowPtElectron_dzSig_genPartFlav0         = TH1F("h_LowPtElectron_dzSig_genPartFlav0",        "h_LowPtElectron_dzSig_genPartFlav0",       50,    0.0,  5.0);
    TH1F h_LowPtElectron_ID_genPartFlav0            = TH1F("h_LowPtElectron_ID_genPartFlav0",           "h_LowPtElectron_ID_genPartFlav0",          50,   -1.0,  15.0);
    TH1F h_LowPtElectron_embeddedID_genPartFlav0    = TH1F("h_LowPtElectron_embeddedID_genPartFlav0",   "h_LowPtElectron_embeddedID_genPartFlav0",  50,   -1.0,  15.0);
    // LowPtElectron_genPartFlav == 1
    TH1F h_LowPtElectron_dxy_genPartFlav1           = TH1F("h_LowPtElectron_dxy_genPartFlav1",          "h_LowPtElectron_dxy_genPartFlav1",         50,  -0.02,  0.02);
    TH1F h_LowPtElectron_dxyErr_genPartFlav1        = TH1F("h_LowPtElectron_dxyErr_genPartFlav1",       "h_LowPtElectron_dxyErr_genPartFlav1",      50,    0.0,  0.1);
    TH1F h_LowPtElectron_dxySig_genPartFlav1        = TH1F("h_LowPtElectron_dxySig_genPartFlav1",       "h_LowPtElectron_dxySig_genPartFlav1",      50,    0.0,  5.0);
    TH1F h_LowPtElectron_dz_genPartFlav1            = TH1F("h_LowPtElectron_dz_genPartFlav1",           "h_LowPtElectron_dz_genPartFlav1",          50,  -0.02,  0.02);
    TH1F h_LowPtElectron_dzErr_genPartFlav1         = TH1F("h_LowPtElectron_dzErr_genPartFlav1",        "h_LowPtElectron_dzErr_genPartFlav1",       50,    0.0,  0.1);
    TH1F h_LowPtElectron_dzSig_genPartFlav1         = TH1F("h_LowPtElectron_dzSig_genPartFlav1",        "h_LowPtElectron_dzSig_genPartFlav1",       50,    0.0,  5.0);
    TH1F h_LowPtElectron_ID_genPartFlav1            = TH1F("h_LowPtElectron_ID_genPartFlav1",           "h_LowPtElectron_ID_genPartFlav1",          50,   -1.0,  15.0);
    TH1F h_LowPtElectron_embeddedID_genPartFlav1    = TH1F("h_LowPtElectron_embeddedID_genPartFlav1",   "h_LowPtElectron_embeddedID_genPartFlav1",  50,   -1.0,  15.0);
    // LowPtElectron_genPartFlav == 5
    TH1F h_LowPtElectron_dxy_genPartFlav5           = TH1F("h_LowPtElectron_dxy_genPartFlav5",          "h_LowPtElectron_dxy_genPartFlav5",         50,  -0.02,  0.02);
    TH1F h_LowPtElectron_dxyErr_genPartFlav5        = TH1F("h_LowPtElectron_dxyErr_genPartFlav5",       "h_LowPtElectron_dxyErr_genPartFlav5",      50,    0.0,  0.1);
    TH1F h_LowPtElectron_dxySig_genPartFlav5        = TH1F("h_LowPtElectron_dxySig_genPartFlav5",       "h_LowPtElectron_dxySig_genPartFlav5",      50,    0.0,  5.0);
    TH1F h_LowPtElectron_dz_genPartFlav5            = TH1F("h_LowPtElectron_dz_genPartFlav5",           "h_LowPtElectron_dz_genPartFlav5",          50,  -0.02,  0.02);
    TH1F h_LowPtElectron_dzErr_genPartFlav5         = TH1F("h_LowPtElectron_dzErr_genPartFlav5",        "h_LowPtElectron_dzErr_genPartFlav5",       50,    0.0,  0.1);
    TH1F h_LowPtElectron_dzSig_genPartFlav5         = TH1F("h_LowPtElectron_dzSig_genPartFlav5",        "h_LowPtElectron_dzSig_genPartFlav5",       50,    0.0,  5.0);
    TH1F h_LowPtElectron_ID_genPartFlav5            = TH1F("h_LowPtElectron_ID_genPartFlav5",           "h_LowPtElectron_ID_genPartFlav5",          50,   -1.0,  15.0);
    TH1F h_LowPtElectron_embeddedID_genPartFlav5    = TH1F("h_LowPtElectron_embeddedID_genPartFlav5",   "h_LowPtElectron_embeddedID_genPartFlav5",  50,   -1.0,  15.0);
    
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
            float dzSig  = -999;
            // avoid dividing by 0
            if (LowPtElectron_dxyErr[k] != 0)
            {
                dxySig = abs(LowPtElectron_dxy[k] / LowPtElectron_dxyErr[k]);
            }
            if (LowPtElectron_dzErr[k] != 0)
            {
                dzSig = abs(LowPtElectron_dz[k] / LowPtElectron_dzErr[k]);
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
            h_LowPtElectron_dz.Fill(LowPtElectron_dz[k]);
            h_LowPtElectron_dzErr.Fill(LowPtElectron_dzErr[k]);
            h_LowPtElectron_dzSig.Fill(dzSig);
            h_LowPtElectron_ID.Fill(LowPtElectron_ID[k]);
            h_LowPtElectron_embeddedID.Fill(LowPtElectron_embeddedID[k]);
            // LowPtElectron_genPartFlav == 0
            if (LowPtElectron_genPartFlav[k] == 0)
            {
                h_LowPtElectron_dxy_genPartFlav0.Fill(LowPtElectron_dxy[k]);
                h_LowPtElectron_dxyErr_genPartFlav0.Fill(LowPtElectron_dxyErr[k]);
                h_LowPtElectron_dxySig_genPartFlav0.Fill(dxySig);
                h_LowPtElectron_dz_genPartFlav0.Fill(LowPtElectron_dz[k]);
                h_LowPtElectron_dzErr_genPartFlav0.Fill(LowPtElectron_dzErr[k]);
                h_LowPtElectron_dzSig_genPartFlav0.Fill(dzSig);
                h_LowPtElectron_ID_genPartFlav0.Fill(LowPtElectron_ID[k]);
                h_LowPtElectron_embeddedID_genPartFlav0.Fill(LowPtElectron_embeddedID[k]);
            }
            // LowPtElectron_genPartFlav == 1
            if (LowPtElectron_genPartFlav[k] == 1)
            {
                h_LowPtElectron_dxy_genPartFlav1.Fill(LowPtElectron_dxy[k]);
                h_LowPtElectron_dxyErr_genPartFlav1.Fill(LowPtElectron_dxyErr[k]);
                h_LowPtElectron_dxySig_genPartFlav1.Fill(dxySig);
                h_LowPtElectron_dz_genPartFlav1.Fill(LowPtElectron_dz[k]);
                h_LowPtElectron_dzErr_genPartFlav1.Fill(LowPtElectron_dzErr[k]);
                h_LowPtElectron_dzSig_genPartFlav1.Fill(dzSig);
                h_LowPtElectron_ID_genPartFlav1.Fill(LowPtElectron_ID[k]);
                h_LowPtElectron_embeddedID_genPartFlav1.Fill(LowPtElectron_embeddedID[k]);
            }
            // LowPtElectron_genPartFlav == 5
            if (LowPtElectron_genPartFlav[k] == 5)
            {
                h_LowPtElectron_dxy_genPartFlav5.Fill(LowPtElectron_dxy[k]);
                h_LowPtElectron_dxyErr_genPartFlav5.Fill(LowPtElectron_dxyErr[k]);
                h_LowPtElectron_dxySig_genPartFlav5.Fill(dxySig);
                h_LowPtElectron_dz_genPartFlav5.Fill(LowPtElectron_dz[k]);
                h_LowPtElectron_dzErr_genPartFlav5.Fill(LowPtElectron_dzErr[k]);
                h_LowPtElectron_dzSig_genPartFlav5.Fill(dzSig);
                h_LowPtElectron_ID_genPartFlav5.Fill(LowPtElectron_ID[k]);
                h_LowPtElectron_embeddedID_genPartFlav5.Fill(LowPtElectron_embeddedID[k]);
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
    PlotHist(h_LowPtElectron_dz,            sample, plot_dir, "h_LowPtElectron_dz",             "dz");
    PlotHist(h_LowPtElectron_dzErr,         sample, plot_dir, "h_LowPtElectron_dzErr",          "dzErr");
    PlotHist(h_LowPtElectron_dzSig,         sample, plot_dir, "h_LowPtElectron_dzSig",          "dzSig");
    PlotHist(h_LowPtElectron_ID,            sample, plot_dir, "h_LowPtElectron_ID",             "ID");
    PlotHist(h_LowPtElectron_embeddedID,    sample, plot_dir, "h_LowPtElectron_embeddedID",     "embeddedID");
    // LowPtElectron_genPartFlav == 0
    PlotHist(h_LowPtElectron_dxy_genPartFlav0,          sample, plot_dir, "h_LowPtElectron_dxy_genPartFlav0",           "dxy");
    PlotHist(h_LowPtElectron_dxyErr_genPartFlav0,       sample, plot_dir, "h_LowPtElectron_dxyErr_genPartFlav0",        "dxyErr");
    PlotHist(h_LowPtElectron_dxySig_genPartFlav0,       sample, plot_dir, "h_LowPtElectron_dxySig_genPartFlav0",        "dxySig");
    PlotHist(h_LowPtElectron_dz_genPartFlav0,           sample, plot_dir, "h_LowPtElectron_dz_genPartFlav0",            "dz");
    PlotHist(h_LowPtElectron_dzErr_genPartFlav0,        sample, plot_dir, "h_LowPtElectron_dzErr_genPartFlav0",         "dzErr");
    PlotHist(h_LowPtElectron_dzSig_genPartFlav0,        sample, plot_dir, "h_LowPtElectron_dzSig_genPartFlav0",         "dzSig");
    PlotHist(h_LowPtElectron_ID_genPartFlav0,           sample, plot_dir, "h_LowPtElectron_ID_genPartFlav0",            "ID");
    PlotHist(h_LowPtElectron_embeddedID_genPartFlav0,   sample, plot_dir, "h_LowPtElectron_embeddedID_genPartFlav0",    "embeddedID");
    // LowPtElectron_genPartFlav == 1
    PlotHist(h_LowPtElectron_dxy_genPartFlav1,          sample, plot_dir, "h_LowPtElectron_dxy_genPartFlav1",           "dxy");
    PlotHist(h_LowPtElectron_dxyErr_genPartFlav1,       sample, plot_dir, "h_LowPtElectron_dxyErr_genPartFlav1",        "dxyErr");
    PlotHist(h_LowPtElectron_dxySig_genPartFlav1,       sample, plot_dir, "h_LowPtElectron_dxySig_genPartFlav1",        "dxySig");
    PlotHist(h_LowPtElectron_dz_genPartFlav1,           sample, plot_dir, "h_LowPtElectron_dz_genPartFlav1",            "dz");
    PlotHist(h_LowPtElectron_dzErr_genPartFlav1,        sample, plot_dir, "h_LowPtElectron_dzErr_genPartFlav1",         "dzErr");
    PlotHist(h_LowPtElectron_dzSig_genPartFlav1,        sample, plot_dir, "h_LowPtElectron_dzSig_genPartFlav1",         "dzSig");
    PlotHist(h_LowPtElectron_ID_genPartFlav1,           sample, plot_dir, "h_LowPtElectron_ID_genPartFlav1",            "ID");
    PlotHist(h_LowPtElectron_embeddedID_genPartFlav1,   sample, plot_dir, "h_LowPtElectron_embeddedID_genPartFlav1",    "embeddedID");
    // LowPtElectron_genPartFlav == 5
    PlotHist(h_LowPtElectron_dxy_genPartFlav5,          sample, plot_dir, "h_LowPtElectron_dxy_genPartFlav5",           "dxy");
    PlotHist(h_LowPtElectron_dxyErr_genPartFlav5,       sample, plot_dir, "h_LowPtElectron_dxyErr_genPartFlav5",        "dxyErr");
    PlotHist(h_LowPtElectron_dxySig_genPartFlav5,       sample, plot_dir, "h_LowPtElectron_dxySig_genPartFlav5",        "dxySig");
    PlotHist(h_LowPtElectron_dz_genPartFlav5,           sample, plot_dir, "h_LowPtElectron_dz_genPartFlav5",            "dz");
    PlotHist(h_LowPtElectron_dzErr_genPartFlav5,        sample, plot_dir, "h_LowPtElectron_dzErr_genPartFlav5",         "dzErr");
    PlotHist(h_LowPtElectron_dzSig_genPartFlav5,        sample, plot_dir, "h_LowPtElectron_dzSig_genPartFlav5",         "dzSig");
    PlotHist(h_LowPtElectron_ID_genPartFlav5,           sample, plot_dir, "h_LowPtElectron_ID_genPartFlav5",            "ID");
    PlotHist(h_LowPtElectron_embeddedID_genPartFlav5,   sample, plot_dir, "h_LowPtElectron_embeddedID_genPartFlav5",    "embeddedID");
}
