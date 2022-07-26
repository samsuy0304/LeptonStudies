
#define TTJETS_cxx
#include "TTJETS.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <string>
#include <fstream>

std::string TTJETS::GetLabel(std::string variable)
{
    std::map < std::string, std::string> labels {
        // variables
        { "nElectrons",  "n_{e}"},
        { "pt",          "p_{T} [GeV]"},
        { "eta",         "#eta"},
        { "phi",         "#phi"},
        { "mass",        "m [GeV]"},
        { "genPartIdx",  "Gen Part Idx"},
        { "genPartFlav", "Gen Part Flav"},
        { "dxy",         "d_{xy}"},
        { "dxyErr",      "d_{xy} err"},
        { "dxySig",      "d_{xy} sig"},
        { "EMID",        "EmbeddedID"},
        { "dz",         "d_{z}"},
        { "dzErr",      "d_{z} err"},
        { "dzSig",      "d_{z} sig"},
        
        { "ISO",         "ISO"},
        { "CONV",      "Conveto"},
        { "IPSig1",      "d_{IP} sig1"},
        { "IPSig2",      "d_{IP} sig2"},
        { "IP",      "d_{IP}"},
        { "GenID ",      "GenID"},
        { "Flav", "Flav"}
    };
    std::string label = "MidCut";
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

void TTJETS::SetupHist(TH1F &hist, std::string title, std::string x_title, std::string y_title, int color, int line_width)
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

void TTJETS::PlotHist(TH1F &hist, std::string sample_name, std::string plot_dir, std::string pt_cut, std::string specific, std::string plot_name, std::string variable)
{
    printf("Plotting %s\n", plot_name.c_str());

    // canvas
    TCanvas c = TCanvas("c", "c", 800, 800);
    c.SetLeftMargin(0.15);

    // setup histogram
    std::string title = "TTbar_" + pt_cut + plot_name;
    std::string x_title = GetLabel(variable);
    std::string y_title = "Entries";
    int color = kBlack;
    int line_width = 1;
    SetupHist(hist, title, x_title, y_title, color, line_width);
    gStyle->SetOptStat(111111);
    // Set stat options
    gStyle->SetStatY(0.9);                
    // Set y-position (fraction of pad size)
    gStyle->SetStatX(0.9);
    // draw
    hist.Draw("hist error same");
    hist.SetStats(kTRUE);
    int lepnum = hist.GetEntries();
    std::string s = std::to_string(lepnum);
    std::string entry = pt_cut + "_" + plot_name + "," + s;
    std::string storefile = plot_dir + "Present.csv";//+ sample_name + "/" + pt_cut + "/" + specific+"/Store.csv";

    ofstream fw(storefile, std::ofstream::app);
    //check if file was successfully opened for writing
    if (fw.is_open())
    {
        //store array contents to text file

        fw << entry.c_str() << "\n";

        fw.close();
    }
    else cout << "Problem with opening file";


    // save plot
    std::string output_name = plot_dir + sample_name + "/" + pt_cut + "/" + specific + "/" + pt_cut + "_" + plot_name;
    std::string output_name_pdf = output_name + ".pdf";
    c.Update();
    c.SaveAs(output_name_pdf.c_str()); ;
}


void TTJETS::SetupHist2(TH2F &hist, std::string title, std::string x_title, std::string y_title, int color, int line_width)
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

void TTJETS::PlotHist2(TH2F &hist, std::string sample_name, std::string plot_dir, std::string plot_name, std::string variable, std::string variable2)
{
    printf("Plotting %s\n", plot_name.c_str());

    // canvas
    TCanvas c = TCanvas("c", "c", 800, 800);
    c.SetLeftMargin(0.15);

    // setup histogram
    std::string title = plot_name;
    std::string x_title = GetLabel(variable);
    std::string y_title = GetLabel(variable2);
    int color = kBlack;
    int line_width = 1;
    SetupHist2(hist, title, x_title, y_title, color, line_width);
    gStyle->SetOptStat(111111);
    // Set stat options
    gStyle->SetStatY(0.9);                
    // Set y-position (fraction of pad size)
    gStyle->SetStatX(0.1);
    // draw
    hist.Draw("colz");
    hist.SetStats(kTRUE);

    // save plot
    std::string output_name = plot_dir+"2DPlots/" + plot_name;
    std::string output_name_pdf = output_name + ".pdf";
    c.Update();
    c.SaveAs(output_name_pdf.c_str());
}

void TTJETS::ROC(TH1F &sigHist, TH1F &bkgHist, std::string plot_name)//, std::string sample_name, std::string plot_dir,"NoCut", std::string plot_name, std::string variable,std::string variable2)
{
    printf("Plotting ROC");
    // canvas
    TCanvas c = TCanvas("c", "c", 800, 800);
    c.SetLeftMargin(0.15);

    int nbins = sigHist.GetNbinsX(); // Finding out the number of bins

    // get the total integrals for each histogram
    float sig_integral = sigHist.Integral(1, nbins);
    float bkg_integral = bkgHist.Integral(1, nbins);

    // create containers sig = x points, bkg = y points
    std::vector<float> sigPoints(nbins);
    std::vector<float> bkgPoints(nbins);

    // in the loop, fill the containers with the x and y points
    // each x point is this loop's slice integral over total integral (signal)
    // each y point is this loop's slice integral over total integral (background)
    for (int i = 0; i < nbins; ++i)
    {
        // notice the slice integral is dependent on i!
        // on each iteration we take a larger and larger slice of the histograms
        // eventually the slice will be the total integral (from bin 1 to bin nbins)
        // that point is (1,1) on the ROC curve.
        float sig_slice_integral = sigHist.Integral(nbins - i, nbins);
        float bkg_slice_integral = bkgHist.Integral(nbins - i, nbins);
        int Fake = 3688;
        int Real = 589;
        std::cout << std::fixed << std::setprecision(2) << sig_slice_integral;
        std::cout << std::fixed << std::setprecision(2) << bkg_slice_integral;
        sigPoints.push_back(sig_slice_integral / sig_integral);
        bkgPoints.push_back(bkg_slice_integral / bkg_integral);
    }
    // create a TGraph from the containers
    // this graph will have N (=nbins) number of points forming the curve.
    TGraph* g = new TGraph(sigPoints.size(), &bkgPoints[0], &sigPoints[0]);
    g->SetTitle(plot_name.c_str());
    g->GetYaxis()->SetTitle("True Positive Rate");
    g->GetXaxis()->SetTitle("False Positive Rate");
    g->Draw();

    c.Update();
    std::string output_name = std::string("/eos/user/s/ssakhare/ROCPlots/") + std::string("TTJETS_MidCut_") + plot_name;
    std::string output_name_pdf = output_name + ".pdf";
    c.SaveAs(output_name_pdf.c_str());

}
void TTJETS::Graph(std::string sample_name, float Lower_pt, float Higher_pt, std::string nam)
{


    gROOT->SetBatch(kTRUE);

    if (fChain == 0)
    {
        return;
    }

    std::string plot_dir = "/eos/user/s/ssakhare/ttbar/";
    std::string sample = "TTbar_" + sample_name;
    printf("Running over %s\n", sample.c_str());

    Long64_t nentries = fChain->GetEntriesFast();
    Long64_t nbytes = 0, nb = 0;
    float EMID_cut = 4.0;
    Long64_t max_event = 150000;

    int chch = 1;

    // Int_t           LowPtElectron_genPartIdx[5];   //[nLowPtElectron]
    // UChar_t         LowPtElectron_genPartFlav[5];   //[nLowPtElectron]

    // Only Flavors
    // NO Flav

    TH1F EMID = TH1F("EMID", "EMID", 22, EMID_cut, 12.0);
    TH1F Pt = TH1F("pt", "pt", 80, Lower_pt, Higher_pt);
    TH1F Eta = TH1F("eta", "eta", 30, -2.4, 2.4);
    TH1F Dxy = TH1F("dxy", "dxy", 25, -0.05, 0.05);
    TH1F DxyErr = TH1F("dxyErr", "dxyErr", 50, 0, 0.02);
    TH1F Dz = TH1F("dz", "dz", 25, -0.05, 0.05);
    TH1F DzErr = TH1F("dzErr", "dzErr", 50, 0, 0.06);
    TH1F DxySig = TH1F("dxySig", "dxySig", 100, -3.0, 3.0);
    TH1F DzSig = TH1F("dzSig", "dzSig", 50, -3.0, 3.0);
    TH1F IP_graph = TH1F("IP", "IP", 100, 0.0, 0.01);
    TH1F IPErr_graph = TH1F("IPErr", "IPErr", 50, -5.0, 5.0);
    TH1F IPSig1_graph = TH1F("IPSig1", "IPSig1", 100, 0.0, 10.0);
    TH1F IPSig2_graph = TH1F("IPSig2", "IPSig2", 100, 0.0, 10.0);
    TH1F IPSigDiff = TH1F("IPSigDiff", "IPSigDiff", 50, -5.0, 5.0);
    TH1F CONV = TH1F("CONV", "CONV", 2, 0.0, 2.0);
    TH1F ISO = TH1F("ISO", "ISO", 50, 0.0, 20.0);
    // Flav 0                                               
    TH1F Flav0_EMID = TH1F("Flav0_EMID", "Flav0_EMID", 22, EMID_cut, 12.0);
    TH1F Flav0_pt = TH1F("Flav0_pt", "Flav0_pt", 80, Lower_pt, Higher_pt);
    TH1F Flav0_eta = TH1F("Flav0_eta", "Flav0_eta", 30, -2.4, 2.4);
    TH1F Flav0_dxy = TH1F("Flav0_dxy", "Flav0_dxy", 25, -0.05, 0.05);
    TH1F Flav0_dxyErr = TH1F("Flav0_dxyErr", "Flav0_dxyErr", 50, 0, 0.02);
    TH1F Flav0_dz = TH1F("Flav0_dz", "Flav0_dz", 25, -0.05, 0.05);
    TH1F Flav0_dzErr = TH1F("Flav0_dzErr", "Flav0_dzErr", 50, 0, 0.06);
    TH1F Flav0_dxySig = TH1F("Flav0_dxySig", "Flav0_dxySig", 100, -3.0, 3.0);
    TH1F Flav0_dzSig = TH1F("Flav0_dzSig", "Flav0_dzSig", 100, -3.0, 3.0);
    TH1F Flav0_IP = TH1F("Flav0_IP", "Flav0_IP", 100, 0.0, 0.01);
    TH1F Flav0_IPErr = TH1F("Flav0_IPErr", "Flav0_IPErr", 50, -5.0, 5.0);
    TH1F Flav0_IPSig1 = TH1F("Flav0_IPSig1", "Flav0_IPSig1", 100, 0.0, 10.0);
    TH1F Flav0_IPSig2 = TH1F("Flav0_IPSig2", "Flav0_IPSig2", 100, 0.0, 10.0);
    TH1F Flav0_IPSigdiff = TH1F("Flav0_IPSigDiff", "Flav0_IPSigDiff", 100, -1.0, 1.0);
    TH1F Flav0_CONV = TH1F("Flav0_CONV", "Flav0_CONV", 2, 0.0, 2.0);
    TH1F Flav0_ISO = TH1F("Flav0_ISO", "Flav0_ISO", 50, 0.0, 20.0);
    //Flav1
    TH1F Flav1_EMID = TH1F("Flav1_EMID", "Flav1_EMID", 22, EMID_cut, 12.0);
    TH1F Flav1_pt = TH1F("Flav1_pt", "Flav1_pt", 80, Lower_pt, Higher_pt);
    TH1F Flav1_eta = TH1F("Flav1_eta", "Flav1_eta", 30, -2.4, 2.4);
    TH1F Flav1_dxy = TH1F("Flav1_dxy", "Flav1_dxy", 25, -0.05, 0.05);
    TH1F Flav1_dxyErr = TH1F("Flav1_dxyErr", "Flav1_dxyErr", 50, 0, 0.02);
    TH1F Flav1_dz = TH1F("Flav1_dz", "Flav1_dz", 25, -0.05, 0.05);
    TH1F Flav1_dzErr = TH1F("Flav1_dzErr", "Flav1_dzErr", 50, 0.0, 0.06);
    TH1F Flav1_dxySig = TH1F("Flav1_dxySig", "Flav1_dxySig", 100, -3.0, 3.0);
    TH1F Flav1_dzSig = TH1F("Flav1_dzSig", "Flav1_dzSig", 50, 0.0, 20.0);
    TH1F Flav1_IP = TH1F("Flav1_IP", "Flav1_IP", 100, 0.0, 0.01);
    TH1F Flav1_IPErr = TH1F("Flav1_IPErr", "Flav1_IPErr", 50, -5.0, 5.0);
    TH1F Flav1_IPSig1 = TH1F("Flav1_IPSig1", "Flav1_IPSig1", 100, 0.0, 10.0);
    TH1F Flav1_IPSig2 = TH1F("Flav1_IPSig2", "Flav1_IPSig2", 100, 0.0, 10.0);
    TH1F Flav1_IPSigdiff = TH1F("Flav1_IPSigDiff", "Flav1_IPSigDiff", 100, -1.0, 1.0);
    TH1F Flav1_CONV = TH1F("Flav1_CONV", "Flav1_CONV", 2, 0.0, 2.0);
    TH1F Flav1_ISO = TH1F("Flav1_ISO", "Flav1_ISO", 50, 0.0, 20.0);
    //Flav5
    TH1F Flav5_EMID = TH1F("Flav5_EMID", "Flav5_EMID", 22, EMID_cut, 12.0);
    TH1F Flav5_pt = TH1F("Flav5_pt", "Flav5_pt", 80, Lower_pt, Higher_pt);
    TH1F Flav5_eta = TH1F("Flav5_eta", "Flav5_eta", 30, -2.4, 2.4);
    TH1F Flav5_dxy = TH1F("Flav5_dxy", "Flav5_dxy", 25, -0.05, 0.05);
    TH1F Flav5_dxyErr = TH1F("Flav5_dxyErr", "Flav5_dxyErr", 50, 0, 0.02);
    TH1F Flav5_dz = TH1F("Flav5_dz", "Flav5_dz", 25, -0.05, 0.05);
    TH1F Flav5_dzErr = TH1F("Flav5_dzErr", "Flav5_dzErr", 50, 0.0, 0.06);
    TH1F Flav5_dxySig = TH1F("Flav5_dxySig", "Flav5_dxySig", 100, -3.0, 3.0);
    TH1F Flav5_dzSig = TH1F("Flav5_dzSig", "Flav5_dzSig", 50, 0.0, 20.0);
    TH1F Flav5_IP = TH1F("Flav5_IP", "Flav5_IP", 100, 0.0, 0.01);
    TH1F Flav5_IPErr = TH1F("Flav5_IPErr", "Flav5_IPErr", 50, -5.0, 5.0);
    TH1F Flav5_IPSig1 = TH1F("Flav5_IPSig1", "Flav5_IPSig1", 100, 0.0, 10.0);
    TH1F Flav5_IPSig2 = TH1F("Flav5_IPSig2", "Flav5_IPSig2", 100, 0.0, 10.0);
    TH1F Flav5_IPSigdiff = TH1F("Flav5_IPSigDiff", "Flav5_IPSigDiff", 100, -1.0, 1.0);
    TH1F Flav5_CONV = TH1F("Flav5_CONV", "Flav5_CONV", 2, 0.0, 2.0);
    TH1F Flav5_ISO = TH1F("Flav5_ISO", "Flav5_ISO", 50, 0.0, 20.0);

    /// IRON1 No Flav
    TH1F IRON1_EMID = TH1F("IRON1_EMID", "IRON1_EMID", 22, EMID_cut, 12.0);
    TH1F IRON1_pt = TH1F("IRON1_pt", "IRON1_pt", 80, Lower_pt, Higher_pt);
    TH1F IRON1_eta = TH1F("IRON1_eta", "IRON1_eta", 30, -2.4, 2.4);
    TH1F IRON1_dxy = TH1F("IRON1_dxy", "IRON1_dxy", 25, -0.05, 0.05);
    TH1F IRON1_dxyErr = TH1F("IRON1_dxyErr", "IRON1_dxyErr", 50, 0, 0.02);
    TH1F IRON1_dz = TH1F("IRON1_dz", "IRON1_dz", 25, -0.05, 0.05);
    TH1F IRON1_dzErr = TH1F("IRON1_dzErr", "IRON1_dzErr", 50, 0.0, 0.06);
    TH1F IRON1_dxySig = TH1F("IRON1_dxySig", "IRON1_dxySig", 100, -3.0, 3.0);
    TH1F IRON1_dzSig = TH1F("IRON1_dzSig", "IRON1_dzSig", 50, 0.0, 20.0);
    TH1F IRON1_IP = TH1F("IRON1_IP", "IRON1_IP", 100, 0.0, 0.01);
    TH1F IRON1_IPErr = TH1F("IRON1_IPErr", "IRON1_IPErr", 50, -5.0, 5.0);
    TH1F IRON1_IPSig1 = TH1F("IRON1_IPSig1", "IRON1_IPSig1", 100, 0.0, 2.0);
    TH1F IRON1_IPSig2 = TH1F("IRON1_IPSig2", "IRON1_IPSig2", 100, 0.0, 2.0);
    TH1F IRON1_IPSigdiff = TH1F("IRON1_IPSigDiff", "IRON1_IPSigDiff", 100, -1.0, 1.0);
    TH1F IRON1_CONV = TH1F("IRON1_CONV", "IRON1_CONV", 2, 0.0, 2.0);
    TH1F IRON1_ISO = TH1F("IRON1_ISO", "IRON1_ISO", 50, 0.0, 20.0);
    TH1F IRON1_GenID_idx = TH1F("IRON1_GenID_idx", "IRON1_GenID_idx", 50, 0.0, 50.0);


    //IRON2 No Flav
    TH1F IRON2_EMID = TH1F("IRON2_EMID", "IRON2_EMID", 22, EMID_cut, 12.0);
    TH1F IRON2_pt = TH1F("IRON2_pt", "IRON2_pt", 80, Lower_pt, Higher_pt);
    TH1F IRON2_eta = TH1F("IRON2_eta", "IRON2_eta", 30, -2.4, 2.4);
    TH1F IRON2_dxy = TH1F("IRON2_dxy", "IRON2_dxy", 25, -0.05, 0.05);
    TH1F IRON2_dxyErr = TH1F("IRON2_dxyErr", "IRON2_dxyErr", 50, 0, 0.02);
    TH1F IRON2_dz = TH1F("IRON2_dz", "IRON2_dz", 25, -0.05, 0.05);
    TH1F IRON2_dzErr = TH1F("IRON2_dzErr", "IRON2_dzErr", 50, 0.0, 0.06);
    TH1F IRON2_dxySig = TH1F("IRON2_dxySig", "IRON2_dxySig", 100, -3.0, 3.0);
    TH1F IRON2_dzSig = TH1F("IRON2_dzSig", "IRON2_dzSig", 50, 0.0, 20.0);
    TH1F IRON2_IP = TH1F("IRON2_IP", "IRON2_IP", 100, 0.0, 0.01);
    TH1F IRON2_IPErr = TH1F("IRON2_IPErr", "IRON2_IPErr", 50, -5.0, 5.0);
    TH1F IRON2_IPSig1 = TH1F("IRON2_IPSig1", "IRON2_IPSig1", 100, 0.0, 10.0);
    TH1F IRON2_IPSig2 = TH1F("IRON2_IPSig2", "IRON2_IPSig2", 100, 0.0, 10.0);
    TH1F IRON2_IPSigdiff = TH1F("IRON2_IPSigDiff", "IRON2_IPSigDiff", 100, -1.0, 1.0);
    TH1F IRON2_CONV = TH1F("IRON2_CONV", "IRON2_CONV", 2, 0.0, 2.0);
    TH1F IRON2_ISO = TH1F("IRON2_ISO", "IRON2_ISO", 50, 0.0, 20.0);


    //Long 1 No Flav
    TH1F Long1_EMID = TH1F("Long1_EMID", "Long1_EMID", 22, EMID_cut, 12.0);
    TH1F Long1_pt = TH1F("Long1_pt", "Long1_pt", 80, Lower_pt, Higher_pt);
    TH1F Long1_eta = TH1F("Long1_eta", "Long1_eta", 30, -2.4, 2.4);
    TH1F Long1_dxy = TH1F("Long1_dxy", "Long1_dxy", 25, -0.05, 0.05);
    TH1F Long1_dxyErr = TH1F("Long1_dxyErr", "Long1_dxyErr", 50, 0, 0.02);
    TH1F Long1_dz = TH1F("Long1_dz", "Long1_dz", 25, -0.05, 0.05);
    TH1F Long1_dzErr = TH1F("Long1_dzErr", "Long1_dzErr", 50, 0.0, 0.06);
    TH1F Long1_dxySig = TH1F("Long1_dxySig", "Long1_dxySig", 100, -3.0, 3.0);
    TH1F Long1_dzSig = TH1F("Long1_dzSig", "Long1_dzSig", 50, 0.0, 20.0);
    TH1F Long1_IP = TH1F("Long1_IP", "Long1_IP", 100, 0.0, 0.01);
    TH1F Long1_IPErr = TH1F("Long1_IPErr", "Long1_IPErr", 50, -5.0, 5.0);
    TH1F Long1_IPSig1 = TH1F("Long1_IPSig1", "Long1_IPSig1", 100, 0.0, 10.0);
    TH1F Long1_IPSig2 = TH1F("Long1_IPSig2", "Long1_IPSig2", 100, 0.0, 10.0);
    TH1F Long1_IPSigdiff = TH1F("Long1_IPSigDiff", "Long1_IPSigDiff", 100, -1.0, 1.0);
    TH1F Long1_CONV = TH1F("Long1_CONLONG1", "Long1_CONV", 2, 0.0, 2.0);
    TH1F Long1_ISO = TH1F("Long1_ISO", "Long1_ISO", 50, 0.0, 20.0);

    //Long 2 No Flav
    TH1F Long2_EMID = TH1F("Long2_EMID", "Long2_EMID", 22, EMID_cut, 12.0);
    TH1F Long2_pt = TH1F("Long2_pt", "Long2_pt", 80, Lower_pt, Higher_pt);
    TH1F Long2_eta = TH1F("Long2_eta", "Long2_eta", 30, -2.4, 2.4);
    TH1F Long2_dxy = TH1F("Long2_dxy", "Long2_dxy", 25, -0.05, 0.05);
    TH1F Long2_dxyErr = TH1F("Long2_dxyErr", "Long2_dxyErr", 50, 0, 0.02);
    TH1F Long2_dz = TH1F("Long2_dz", "Long2_dz", 25, -0.05, 0.05);
    TH1F Long2_dzErr = TH1F("Long2_dzErr", "Long2_dzErr", 50, 0.0, 0.06);
    TH1F Long2_dxySig = TH1F("Long2_dxySig", "Long2_dxySig", 100, -3.0, 3.0);
    TH1F Long2_dzSig = TH1F("Long2_dzSig", "Long2_dzSig", 50, 0.0, 20.0);
    TH1F Long2_IP = TH1F("Long2_IP", "Long2_IP", 100, 0.0, 0.01);
    TH1F Long2_IPErr = TH1F("Long2_IPErr", "Long2_IPErr", 50, -5.0, 5.0);
    TH1F Long2_IPSig1 = TH1F("Long2_IPSig1", "Long2_IPSig1", 100, 0.0, 10.0);
    TH1F Long2_IPSig2 = TH1F("Long2_IPSig2", "Long2_IPSig2", 100, 0.0, 10.0);
    TH1F Long2_IPSigdiff = TH1F("Long2_IPSigDiff", "Long2_IPSigDiff", 100, -1.0, 1.0);
    TH1F Long2_CONV = TH1F("Long2_CONLONG1", "Long2_CONV", 2, 0.0, 2.0);
    TH1F Long2_ISO = TH1F("Long2_ISO", "Long2_ISO", 50, 0.0, 20.0);

    //IRON Fake no flav
    TH1F FAKE_EMID = TH1F("FAKE_EMID", "FAKE_EMID", 22, EMID_cut, 12.0);
    TH1F FAKE_pt = TH1F("FAKE_pt", "IRONFAKE_pt", 80, Lower_pt, Higher_pt);
    TH1F FAKE_eta = TH1F("IRONFAKE_eta", "IRONFAKE_eta", 30, -2.4, 2.4);
    TH1F FAKE_dxy = TH1F("IRONFAKE_dxy", "IRONFAKE_dxy", 25, -0.05, 0.05);
    TH1F FAKE_dxyErr = TH1F("IRONFAKE_dxyErr", "IRONFAKE_dxyErr", 50, 0, 0.02);
    TH1F FAKE_dz = TH1F("IRONFAKE_dz", "IRONFAKE_dz", 25, -0.05, 0.05);
    TH1F FAKE_dzErr = TH1F("IRONFAKE_dzErr", "IRONFAKE_dzErr", 50, 0.0, 0.06);
    TH1F FAKE_dxySig = TH1F("IRONFAKE_dxySig", "IRONFAKE_dxySig", 100, -3.0, 3.0);
    TH1F FAKE_dzSig = TH1F("IRONFAKE_dzSig", "IRONFAKE_dzSig", 50, 0.0, 20.0);
    TH1F FAKE_IP = TH1F("IRONFAKE_IP", "IRONFAKE_IP", 100, 0.0, 0.01);
    TH1F FAKE_IPErr = TH1F("IRONFAKE_IPErr", "IRONFAKE_IPErr", 50, -5.0, 5.0);
    TH1F FAKE_IPSig1 = TH1F("IRONFAKE_IPSig1", "IRONFAKE_IPSig1", 100, 0.0, 10.0);
    TH1F FAKE_IPSig2 = TH1F("IRONFAKE_IPSig2", "IRONFAKE_IPSig2", 100, 0.0, 10.0);
    TH1F FAKE_IPSigdiff = TH1F("IRONFAKE_IPSigDiff", "IRONFAKE_IPSigDiff", 100, -1.0, 1.0);
    TH1F FAKE_CONV = TH1F("IRONFAKE_CONLONG1", "IRONFAKE_CONV", 2, 0.0, 2.0);
    TH1F FAKE_ISO = TH1F("IRONFAKE_ISO", "IRONFAKE_ISO", 50, 0.0, 20.0);


    // LowPtElectron_genPartFlav == 0
    TH1F IRON2_FLAV0_EMID = TH1F("Iron2_Flav0_EMID", "Iron2_Flav0_EMID", 22, EMID_cut, 12.0);
    TH1F LONG2_FLAV0_EMID = TH1F("Long2_Flav0_EMID", "Long2_Flav0_EMID", 22, EMID_cut, 12.0);
    TH1F IRON1_FLAV0_EMID = TH1F("Iron1_Flav0_EMID", "Iron1_Flav0_EMID", 22, EMID_cut, 12.0);
    TH1F LONG1_FLAV0_EMID = TH1F("Long1_Flav0_EMID", "Long1_Flav0_EMID", 22, EMID_cut, 12.0);
    TH1F FAKE_FLAV0_EMID = TH1F("IronFake_Flav0_EMID", "IronFake_Flav0_EMID", 22, EMID_cut, 12.0);
    // LowPtElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_EMID = TH1F("Iron1_Flav1_EMID", "Iron1_Flav1_EMID", 22, EMID_cut, 12.0);
    TH1F LONG1_FLAV1_EMID = TH1F("Long1_Flav1_EMID", "Long1_Flav1_EMID", 22, EMID_cut, 12.0);
    TH1F IRON2_FLAV1_EMID = TH1F("Iron2_Flav1_EMID", "Iron2_Flav1_EMID", 22, EMID_cut, 12.0);
    TH1F LONG2_FLAV1_EMID = TH1F("Long2_Flav1_EMID", "Long2_Flav1_EMID", 22, EMID_cut, 12.0);
    TH1F FAKE_FLAV1_EMID = TH1F("IronFake_Flav1_EMID", "IronFake_Flav1_EMID", 22, EMID_cut, 12.0);
    // LowPtElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_EMID = TH1F("Iron1_Flav5_EMID", "Iron1_Flav5_EMID", 22, EMID_cut, 12.0);
    TH1F LONG1_FLAV5_EMID = TH1F("Long1_Flav5_EMID", "Long1_Flav5_EMID", 22, EMID_cut, 12.0);
    TH1F IRON2_FLAV5_EMID = TH1F("Iron2_Flav5_EMID", "Iron2_Flav5_EMID", 22, EMID_cut, 12.0);
    TH1F LONG2_FLAV5_EMID = TH1F("Long2_Flav5_EMID", "Long2_Flav5_EMID", 22, EMID_cut, 12.0);
    TH1F FAKE_FLAV5_EMID = TH1F("IronFake_Flav5_EMID", "IronFake_Flav5_EMID", 22, EMID_cut, 12.0);




    ///PT

    // LowPtElectron_genPartFlav == 0
    TH1F IRON1_FLAV0_pt = TH1F("Iron1_Flav0_pt", "Iron1_Flav0_pt", 80, Lower_pt, Higher_pt);
    TH1F LONG1_FLAV0_pt = TH1F("Long1_Flav0_pt", "Long1_Flav0_pt", 80, Lower_pt, Higher_pt);
    TH1F IRON2_FLAV0_pt = TH1F("Iron2_Flav0_pt", "Iron2_Flav0_pt", 80, Lower_pt, Higher_pt);
    TH1F LONG2_FLAV0_pt = TH1F("Long2_Flav0_pt", "Long2_Flav0_pt", 80, Lower_pt, Higher_pt);
    TH1F FAKE_FLAV0_pt = TH1F("IronFake_Flav0_pt", "IronFake_Flav0_pt", 80, Lower_pt, Higher_pt);
    // LowPtElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_pt = TH1F("Iron1_Flav1_pt", "Iron1_Flav1_pt", 80, Lower_pt, Higher_pt);
    TH1F LONG1_FLAV1_pt = TH1F("Long1_Flav1_pt", "Long1_Flav1_pt", 80, Lower_pt, Higher_pt);
    TH1F IRON2_FLAV1_pt = TH1F("Iron2_Flav1_pt", "Iron2_Flav1_pt", 80, Lower_pt, Higher_pt);
    TH1F LONG2_FLAV1_pt = TH1F("Long2_Flav1_pt", "Long2_Flav1_pt", 80, Lower_pt, Higher_pt);
    TH1F FAKE_FLAV1_pt = TH1F("IronFake_Flav1_pt", "IronFake_Flav1_pt", 80, Lower_pt, Higher_pt);
    // LowPtElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_pt = TH1F("Iron1_Flav5_pt", "Iron1_Flav5_pt", 80, Lower_pt, Higher_pt);
    TH1F LONG1_FLAV5_pt = TH1F("Long1_Flav5_pt", "Long1_Flav5_pt", 80, Lower_pt, Higher_pt);
    TH1F IRON2_FLAV5_pt = TH1F("Iron2_Flav5_pt", "Iron2_Flav5_pt", 80, Lower_pt, Higher_pt);
    TH1F LONG2_FLAV5_pt = TH1F("Long2_Flav5_pt", "Long2_Flav5_pt", 80, Lower_pt, Higher_pt);
    TH1F FAKE_FLAV5_pt = TH1F("IronFake_Flav5_pt", "IronFake_Flav5_pt", 80, Lower_pt, Higher_pt);




    //// Eta


    // LowetaElectron_genPartFlav == 0
    TH1F IRON1_FLAV0_eta = TH1F("Iron1_Flav0_eta", "Iron1_Flav0_eta", 30, -2.4, 2.4);
    TH1F LONG1_FLAV0_eta = TH1F("Long1_Flav0_eta", "Long1_Flav0_eta", 30, -2.4, 2.4);
    TH1F IRON2_FLAV0_eta = TH1F("Iron2_Flav0_eta", "Iron2_Flav0_eta", 30, -2.4, 2.4);
    TH1F LONG2_FLAV0_eta = TH1F("Long2_Flav0_eta", "Long2_Flav0_eta", 30, -2.4, 2.4);
    TH1F FAKE_FLAV0_eta = TH1F("IronFake_Flav0_eta", "IronFake_Flav0_eta", 30, -2.4, 2.4);
    // LowetaElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_eta = TH1F("Iron1_Flav1_eta", "Iron1_Flav1_eta", 30, -2.4, 2.4);
    TH1F LONG1_FLAV1_eta = TH1F("Long1_Flav1_eta", "Long1_Flav1_eta", 30, -2.4, 2.4);
    TH1F IRON2_FLAV1_eta = TH1F("Iron2_Flav1_eta", "Iron2_Flav1_eta", 30, -2.4, 2.4);
    TH1F LONG2_FLAV1_eta = TH1F("Long2_Flav1_eta", "Long2_Flav1_eta", 30, -2.4, 2.4);
    TH1F FAKE_FLAV1_eta = TH1F("IronFake_Flav1_eta", "IronFake_Flav1_eta", 30, -2.4, 2.4);
    // LowetaElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_eta = TH1F("Iron1_Flav5_eta", "Iron1_Flav5_eta", 30, -2.4, 2.4);
    TH1F LONG1_FLAV5_eta = TH1F("Long1_Flav5_eta", "Long1_Flav5_eta", 30, -2.4, 2.4);
    TH1F IRON2_FLAV5_eta = TH1F("Iron2_Flav5_eta", "Iron2_Flav5_eta", 30, -2.4, 2.4);
    TH1F LONG2_FLAV5_eta = TH1F("Long2_Flav5_eta", "Long2_Flav5_eta", 30, -2.4, 2.4);
    TH1F FAKE_FLAV5_eta = TH1F("IronFake_Flav5_eta", "IronFake_Flav5_eta", 30, -2.4, 2.4);

    ///DXY



    // LowdxyElectron_genPartFlav == 0
    TH1F IRON1_FLAV0_dxy = TH1F("Iron1_Flav0_dxy", "Iron1_Flav0_dxy", 25, -0.05, 0.05);
    TH1F LONG1_FLAV0_dxy = TH1F("Long1_Flav0_dxy", "Long1_Flav0_dxy", 25, -0.05, 0.05);
    TH1F IRON2_FLAV0_dxy = TH1F("Iron2_Flav0_dxy", "Iron2_Flav0_dxy", 25, -0.05, 0.05);
    TH1F LONG2_FLAV0_dxy = TH1F("Long2_Flav0_dxy", "Long2_Flav0_dxy", 25, -0.05, 0.05);
    TH1F FAKE_FLAV0_dxy = TH1F("IronFake_Flav0_dxy", "IronFake_Flav0_dxy", 25, -0.05, 0.05);
    // LowdxyElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_dxy = TH1F("Iron1_Flav1_dxy", "Iron1_Flav1_dxy", 25, -0.05, 0.05);
    TH1F LONG1_FLAV1_dxy = TH1F("Long1_Flav1_dxy", "Long1_Flav1_dxy", 25, -0.05, 0.05);
    TH1F IRON2_FLAV1_dxy = TH1F("Iron2_Flav1_dxy", "Iron2_Flav1_dxy", 25, -0.05, 0.05);
    TH1F LONG2_FLAV1_dxy = TH1F("Long2_Flav1_dxy", "Long2_Flav1_dxy", 25, -0.05, 0.05);
    TH1F FAKE_FLAV1_dxy = TH1F("IronFake_Flav1_dxy", "IronFake_Flav1_dxy", 25, -0.05, 0.05);
    // LowdxyElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_dxy = TH1F("Iron1_Flav5_dxy", "Iron1_Flav5_dxy", 25, -0.05, 0.05);
    TH1F LONG1_FLAV5_dxy = TH1F("Long1_Flav5_dxy", "Long1_Flav5_dxy", 25, -0.05, 0.05);
    TH1F IRON2_FLAV5_dxy = TH1F("Iron2_Flav5_dxy", "Iron2_Flav5_dxy", 25, -0.05, 0.05);
    TH1F LONG2_FLAV5_dxy = TH1F("Long2_Flav5_dxy", "Long2_Flav5_dxy", 25, -0.05, 0.05);
    TH1F FAKE_FLAV5_dxy = TH1F("IronFake_Flav5_dxy", "IronFake_Flav5_dxy", 25, -0.05, 0.05);


    // Dz


    // LowdzElectron_genPartFlav == 0
    TH1F IRON1_FLAV0_dz = TH1F("Iron1_Flav0_dz", "Iron1_Flav0_dz", 25, -0.05, 0.05);
    TH1F LONG1_FLAV0_dz = TH1F("Long1_Flav0_dz", "Long1_Flav0_dz", 25, -0.05, 0.05);
    TH1F IRON2_FLAV0_dz = TH1F("Iron2_Flav0_dz", "Iron2_Flav0_dz", 25, -0.05, 0.05);
    TH1F LONG2_FLAV0_dz = TH1F("Long2_Flav0_dz", "Long2_Flav0_dz", 25, -0.05, 0.05);
    TH1F FAKE_FLAV0_dz = TH1F("IronFake_Flav0_dz", "IronFake_Flav0_dz", 25, -0.05, 0.05);
    // LowdzElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_dz = TH1F("Iron1_Flav1_dz", "Iron1_Flav1_dz", 25, -0.05, 0.05);
    TH1F LONG1_FLAV1_dz = TH1F("Long1_Flav1_dz", "Long1_Flav1_dz", 25, -0.05, 0.05);
    TH1F IRON2_FLAV1_dz = TH1F("Iron2_Flav1_dz", "Iron2_Flav1_dz", 25, -0.05, 0.05);
    TH1F LONG2_FLAV1_dz = TH1F("Long2_Flav1_dz", "Long2_Flav1_dz", 25, -0.05, 0.05);
    TH1F FAKE_FLAV1_dz = TH1F("IronFake_Flav1_dz", "IronFake_Flav1_dz", 25, -0.05, 0.05);
    // LowdzElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_dz = TH1F("Iron1_Flav5_dz", "Iron1_Flav5_dz", 25, -0.05, 0.05);
    TH1F LONG1_FLAV5_dz = TH1F("Long1_Flav5_dz", "Long1_Flav5_dz", 25, -0.05, 0.05);
    TH1F IRON2_FLAV5_dz = TH1F("Iron2_Flav5_dz", "Iron2_Flav5_dz", 25, -0.05, 0.05);
    TH1F LONG2_FLAV5_dz = TH1F("Long2_Flav5_dz", "Long2_Flav5_dz", 25, -0.05, 0.05);
    TH1F FAKE_FLAV5_dz = TH1F("IronFake_Flav5_dz", "IronFake_Flav5_dz", 25, -0.05, 0.05);



    // dz Err

    // LowdzErrElectron_genPartFlav == 0
    TH1F IRON1_FLAV0_dzErr = TH1F("Iron1_Flav0_dzErr", "Iron1_Flav0_dzErr", 100, 0.0, 0.1);
    TH1F LONG1_FLAV0_dzErr = TH1F("Long1_Flav0_dzErr", "Long1_Flav0_dzErr", 100, 0.0, 0.1);
    TH1F IRON2_FLAV0_dzErr = TH1F("Iron2_Flav0_dzErr", "Iron2_Flav0_dzErr", 100, 0.0, 0.1);
    TH1F LONG2_FLAV0_dzErr = TH1F("Long2_Flav0_dzErr", "Long2_Flav0_dzErr", 100, 0.0, 0.1);
    TH1F FAKE_FLAV0_dzErr = TH1F("IronFake_Flav0_dzErr", "IronFake_Flav0_dzErr", 100, 0.0, 0.1);
    // LowdzErrElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_dzErr = TH1F("Iron1_Flav1_dzErr", "Iron1_Flav1_dzErr", 100, 0.0, 0.1);
    TH1F LONG1_FLAV1_dzErr = TH1F("Long1_Flav1_dzErr", "Long1_Flav1_dzErr", 100, 0.0, 0.1);
    TH1F IRON2_FLAV1_dzErr = TH1F("Iron2_Flav1_dzErr", "Iron2_Flav1_dzErr", 100, 0.0, 0.1);
    TH1F LONG2_FLAV1_dzErr = TH1F("Long2_Flav1_dzErr", "Long2_Flav1_dzErr", 100, 0.0, 0.1);
    TH1F FAKE_FLAV1_dzErr = TH1F("IronFake_Flav1_dzErr", "IronFake_Flav1_dzErr", 100, 0.0, 0.1);
    // LowdzErrElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_dzErr = TH1F("Iron1_Flav5_dzErr", "Iron1_Flav5_dzErr", 100, 0.0, 0.1);
    TH1F LONG1_FLAV5_dzErr = TH1F("Long1_Flav5_dzErr", "Long1_Flav5_dzErr", 100, 0.0, 0.1);
    TH1F IRON2_FLAV5_dzErr = TH1F("Iron2_Flav5_dzErr", "Iron2_Flav5_dzErr", 100, 0.0, 0.1);
    TH1F LONG2_FLAV5_dzErr = TH1F("Long2_Flav5_dzErr", "Long2_Flav5_dzErr", 100, 0.0, 0.1);
    TH1F FAKE_FLAV5_dzErr = TH1F("IronFake_Flav5_dzErr", "IronFake_Flav5_dzErr", 100, 0.0, 0.1);


    // dxy ERr


    // LowdxyErrElectron_genPartFlav == 0
    TH1F IRON1_FLAV0_dxyErr = TH1F("Iron1_Flav0_dxyErr", "Iron1_Flav0_dxyErr", 100, 0.0, 0.1);
    TH1F LONG1_FLAV0_dxyErr = TH1F("Long1_Flav0_dxyErr", "Long1_Flav0_dxyErr", 100, 0.0, 0.1);
    TH1F IRON2_FLAV0_dxyErr = TH1F("Iron2_Flav0_dxyErr", "Iron2_Flav0_dxyErr", 100, 0.0, 0.1);
    TH1F LONG2_FLAV0_dxyErr = TH1F("Long2_Flav0_dxyErr", "Long2_Flav0_dxyErr", 100, 0.0, 0.1);
    TH1F FAKE_FLAV0_dxyErr = TH1F("IronFake_Flav0_dxyErr", "IronFake_Flav0_dxyErr", 100, 0.0, 0.1);
    // LowdxyErrElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_dxyErr = TH1F("Iron1_Flav1_dxyErr", "Iron1_Flav1_dxyErr", 100, 0.0, 0.1);
    TH1F LONG1_FLAV1_dxyErr = TH1F("IronLong_Flav1_dxyErr", "IronLong_Flav1_dxyErr", 100, 0.0, 0.1);
    TH1F IRON2_FLAV1_dxyErr = TH1F("Iron1_Flav2_dxyErr", "Iron2_Flav1_dxyErr", 100, 0.0, 0.1);
    TH1F LONG2_FLAV1_dxyErr = TH1F("IronLong_Flav2_dxyErr", "Long2_Flav1_dxyErr", 100, 0.0, 0.1);
    TH1F FAKE_FLAV1_dxyErr = TH1F("IronFake_Flav1_dxyErr", "IronFake_Flav1_dxyErr", 100, 0.0, 0.1);
    // LowdxyErrElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_dxyErr = TH1F("Iron1_Flav5_dxyErr", "Iron1_Flav5_dxyErr", 100, 0.0, 0.1);
    TH1F LONG1_FLAV5_dxyErr = TH1F("IronLong_Flav5_dxyErr", "IronLong_Flav5_dxyErr", 100, 0.0, 0.1);
    TH1F IRON2_FLAV5_dxyErr = TH1F("Iron2_Flav5_dxyErr", "Iron2_Flav5_dxyErr", 100, 0.0, 0.1);
    TH1F LONG2_FLAV5_dxyErr = TH1F("Long2_Flav5_dxyErr", "Long2_Flav5_dxyErr", 100, 0.0, 0.1);
    TH1F FAKE_FLAV5_dxyErr = TH1F("IronFake_Flav5_dxyErr", "IronFake_Flav5_dxyErr", 100, 0.0, 0.1);



    // ISO

    // LowISOElectron_genPartFlav == 0
    TH1F IRON1_FLAV0_ISO = TH1F("Iron1_Flav0_ISO", "Iron1_Flav0_ISO", 50, 0.0, 4.0);
    TH1F LONG1_FLAV0_ISO = TH1F("IronLong_Flav0_ISO", "IronLong_Flav0_ISO", 50, 0.0, 4.0);
    TH1F IRON2_FLAV0_ISO = TH1F("Iron2_Flav0_ISO", "Iron2_Flav0_ISO", 50, 0.0, 4.0);
    TH1F LONG2_FLAV0_ISO = TH1F("Long2_Flav0_ISO", "Long2_Flav0_ISO", 50, 0.0, 4.0);
    TH1F FAKE_FLAV0_ISO = TH1F("IronFake_Flav0_ISO", "IronFake_Flav0_ISO", 50, 4.0, 20.0);
    // LowISOElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_ISO = TH1F("Iron1_Flav1_ISO", "Iron1_Flav1_ISO", 50, 0.0, 4.0);
    TH1F LONG1_FLAV1_ISO = TH1F("IronLong_Flav1_ISO", "IronLong_Flav1_ISO", 50, 0.0, 4.0);
    TH1F IRON2_FLAV1_ISO = TH1F("Iron2_Flav1_ISO", "Iron2_Flav1_ISO", 50, 0.0, 4.0);
    TH1F LONG2_FLAV1_ISO = TH1F("Long2_Flav1_ISO", "Long2_Flav1_ISO", 50, 0.0, 4.0);
    TH1F FAKE_FLAV1_ISO = TH1F("IronFake_Flav1_ISO", "IronFake_Flav1_ISO", 50, 4.0, 20.0);
    // LowISOElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_ISO = TH1F("Iron1_Flav5_ISO", "Iron1_Flav5_ISO", 50, 0.0, 4.0);
    TH1F LONG1_FLAV5_ISO = TH1F("IronLong_Flav5_ISO", "IronLong_Flav5_ISO", 50, 0.0, 4.0);
    TH1F IRON2_FLAV5_ISO = TH1F("Iron2_Flav5_ISO", "Iron2_Flav5_ISO", 50, 0.0, 4.0);
    TH1F LONG2_FLAV5_ISO = TH1F("Long2_Flav5_ISO", "Long21_Flav5_ISO", 50, 0.0, 4.0);
    TH1F FAKE_FLAV5_ISO = TH1F("IronFake_Flav5_ISO", "IronFake_Flav5_ISO", 50, 4.0, 20.0);



    // Conveto



    // LowCONVElectron_genPartFlav == 0
    TH1F IRON1_FLAV0_CONV = TH1F("Iron1_Flav0_CONV", "Iron1_Flav0_CONV", 2, 0.0, 2.0);
    TH1F LONG1_FLAV0_CONV = TH1F("Long1_Flav0_CONV", "Long1_Flav0_CONV", 2, 0.0, 2.0);
    TH1F IRON2_FLAV0_CONV = TH1F("Iron2_Flav0_CONV", "Iron2_Flav0_CONV", 2, 0.0, 2.0);
    TH1F LONG2_FLAV0_CONV = TH1F("Long2_Flav0_CONV", "Long2_Flav0_CONV", 2, 0.0, 2.0);
    TH1F FAKE_FLAV0_CONV = TH1F("IronFake_Flav0_CONV", "IronFake_Flav0_CONV", 2, 0.0, 2.0);
    // LowCONVElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_CONV = TH1F("Iron1_Flav1_CONV", "Iron1_Flav1_CONV", 2, 0.0, 2.0);
    TH1F LONG1_FLAV1_CONV = TH1F("Long1_Flav1_CONV", "Long1_Flav1_CONV", 2, 0.0, 2.0);
    TH1F IRON2_FLAV1_CONV = TH1F("Iron2_Flav1_CONV", "Iron2_Flav1_CONV", 2, 0.0, 2.0);
    TH1F LONG2_FLAV1_CONV = TH1F("Long2_Flav1_CONV", "Long2_Flav1_CONV", 2, 0.0, 2.0);
    TH1F FAKE_FLAV1_CONV = TH1F("IronFake_Flav1_CONV", "IronFake_Flav1_CONV", 2, 0.0, 2.0);
    // LowCONVElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_CONV = TH1F("Iron_Flav5_CONV", "Iron_Flav5_CONV", 2, 0.0, 2.0);
    TH1F LONG1_FLAV5_CONV = TH1F("IronLon1g_Flav5_CONV", "Long1_Flav5_CONV", 2, 0.0, 2.0);
    TH1F IRON2_FLAV5_CONV = TH1F("Iron2_Flav5_CONV", "Iron2_Flav5_CONV", 2, 0.0, 2.0);
    TH1F LONG2_FLAV5_CONV = TH1F("Long2_Flav5_CONV", "Long2_Flav5_CONV", 2, 0.0, 2.0);
    TH1F FAKE_FLAV5_CONV = TH1F("IronFake_Flav5_CONV", "IronFake_Flav5_CONV", 2, 0.0, 2.0);




    //dxy Sig



    // LowdxySigElectron_genPartFlav == 0
    TH1F IRON1_FLAV0_dxySig = TH1F("Iron1_Flav0_dxySig", "Iron1_Flav0_dxySig", 100, -3.0, 3.0);
    TH1F LONG1_FLAV0_dxySig = TH1F("Long1_Flav0_dxySig", "Long1_Flav0_dxySig", 100, -3.0, 3.0);
    TH1F IRON2_FLAV0_dxySig = TH1F("Iron2_Flav0_dxySig", "Iron2_Flav0_dxySig", 100, -3.0, 3.0);
    TH1F LONG2_FLAV0_dxySig = TH1F("Long2_Flav0_dxySig", "Long2_Flav0_dxySig", 100, -3.0, 3.0);
    TH1F FAKE_FLAV0_dxySig = TH1F("IronFake_Flav0_dxySig", "IronFake_Flav0_dxySig", 100, -3.0, 3.0);
    // LowdxySigElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_dxySig = TH1F("Iron1_Flav1_dxySig", "Iron1_Flav1_dxySig", 100, -3.0, 3.0);
    TH1F LONG1_FLAV1_dxySig = TH1F("Long1_Flav1_dxySig", "Long1_Flav1_dxySig", 100, -3.0, 3.0);
    TH1F IRON2_FLAV1_dxySig = TH1F("Iron2_Flav1_dxySig", "Iron2_Flav1_dxySig", 100, -3.0, 3.0);
    TH1F LONG2_FLAV1_dxySig = TH1F("Long2_Flav1_dxySig", "Long2_Flav1_dxySig", 100, -3.0, 3.0);
    TH1F FAKE_FLAV1_dxySig = TH1F("IronFake_Flav1_dxySig", "IronFake_Flav1_dxySig", 100, -3.0, 3.0);
    // LowdxySigElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_dxySig = TH1F("Iron1_Flav5_dxySig", "Iron1_Flav5_dxySig", 100, -3.0, 3.0);
    TH1F LONG1_FLAV5_dxySig = TH1F("Long1_Flav5_dxySig", "Long1_Flav5_dxySig", 100, -3.0, 3.0);
    TH1F IRON2_FLAV5_dxySig = TH1F("Iron2_Flav5_dxySig", "Iron2_Flav5_dxySig", 100, -3.0, 3.0);
    TH1F LONG2_FLAV5_dxySig = TH1F("Long2_Flav5_dxySig", "Long2_Flav5_dxySig", 100, -3.0, 3.0);
    TH1F FAKE_FLAV5_dxySig = TH1F("IronFake_Flav5_dxySig", "IronFake_Flav5_dxySig", 100, -3.0, 3.0);


    //dz Sig



    // LowdzSigElectron_genPartFlav == 0
    TH1F IRON1_FLAV0_dzSig = TH1F("Iron1_Flav0_dzSig", "Iron1_Flav0_dzSig", 50, -3.0, 3.0);
    TH1F LONG1_FLAV0_dzSig = TH1F("Long1_Flav0_dzSig", "Long1_Flav0_dzSig", 50, -3.0, 3.0);
    TH1F IRON2_FLAV0_dzSig = TH1F("Iron2_Flav0_dzSig", "Iron2_Flav0_dzSig", 50, -3.0, 3.0);
    TH1F LONG2_FLAV0_dzSig = TH1F("Long2_Flav0_dzSig", "Long2_Flav0_dzSig", 50, -3.0, 3.0);
    TH1F FAKE_FLAV0_dzSig = TH1F("IronFake_Flav0_dzSig", "IronFake_Flav0_dzSig", 50, -3.0, 3.0);
    // LowdzSigElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_dzSig = TH1F("Iron1_Flav1_dzSig", "Iron1_Flav1_dzSig", 50, -3.0, 3.0);
    TH1F LONG1_FLAV1_dzSig = TH1F("Long1_Flav1_dzSig", "Long1_Flav1_dzSig", 50, -3.0, 3.0);
    TH1F IRON2_FLAV1_dzSig = TH1F("Iron2_Flav1_dzSig", "Iron2_Flav1_dzSig", 50, -3.0, 3.0);
    TH1F LONG2_FLAV1_dzSig = TH1F("Long2_Flav1_dzSig", "Long2_Flav1_dzSig", 50, -3.0, 3.0);
    TH1F FAKE_FLAV1_dzSig = TH1F("IronFake_Flav1_dzSig", "IronFake_Flav1_dzSig", 50, -3.0, 3.0);
    // LowdzSigElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_dzSig = TH1F("Iron1_Flav5_dzSig", "Iron1_Flav5_dzSig", 50, -3.0, 3.0);
    TH1F LONG1_FLAV5_dzSig = TH1F("Long1_Flav5_dzSig", "Long1_Flav5_dzSig", 50, -3.0, 3.0);
    TH1F IRON2_FLAV5_dzSig = TH1F("Iron2_Flav5_dzSig", "Iron2_Flav5_dzSig", 50, -3.0, 3.0);
    TH1F LONG2_FLAV5_dzSig = TH1F("Long2_Flav5_dzSig", "Long2_Flav5_dzSig", 50, -3.0, 3.0);
    TH1F FAKE_FLAV5_dzSig = TH1F("IronFake_Flav5_dzSig", "IronFake_Flav5_dzSig", 50, -3.0, 3.0);




    //IP Sig1

    // LowIPSig1Electron_genPartFlav == 0
    TH1F IRON1_FLAV0_IPSig1 = TH1F("Iron1_Flav0_IPSig1", "Iron1_Flav0_IPSig1", 100, 0.0, 10.0);
    TH1F LONG1_FLAV0_IPSig1 = TH1F("Long1_Flav0_IPSig1", "Long1_Flav0_IPSig1", 100, 0.0, 10.0);
    TH1F IRON2_FLAV0_IPSig1 = TH1F("Iron2_Flav0_IPSig1", "Iron2_Flav0_IPSig1", 100, 0.0, 10.0);
    TH1F LONG2_FLAV0_IPSig1 = TH1F("Long2_Flav0_IPSig1", "Long2_Flav0_IPSig1", 100, 0.0, 10.0);
    TH1F FAKE_FLAV0_IPSig1 = TH1F("IronFake_Flav0_IPSig1", "IronFake_Flav0_IPSig1", 100, 0.0, 10.0);
    // LowIPSig1Electron_genPartFlav == 1
    TH1F IRON1_FLAV1_IPSig1 = TH1F("Iron1_Flav1_IPSig1", "Iron1_Flav1_IPSig1", 100, 0.0, 10.0);
    TH1F LONG1_FLAV1_IPSig1 = TH1F("Long1_Flav1_IPSig1", "Long1_Flav1_IPSig1", 100, 0.0, 10.0);
    TH1F IRON2_FLAV1_IPSig1 = TH1F("Iron2_Flav1_IPSig1", "Iron2_Flav1_IPSig1", 100, 0.0, 10.0);
    TH1F LONG2_FLAV1_IPSig1 = TH1F("Long2_Flav1_IPSig1", "Long2_Flav1_IPSig1", 100, 0.0, 10.0);
    TH1F FAKE_FLAV1_IPSig1 = TH1F("IronFake_Flav1_IPSig1", "IronFake_Flav1_IPSig1", 100, 0.0, 10.0);
    // LowIPSig1Electron_genPartFlav == 5
    TH1F IRON1_FLAV5_IPSig1 = TH1F("Iron1_Flav5_IPSig1", "Iron1_Flav5_IPSig1", 100, 0.0, 10.0);
    TH1F LONG1_FLAV5_IPSig1 = TH1F("Long1_Flav5_IPSig1", "Long1_Flav5_IPSig1", 100, 0.0, 10.0);
    TH1F IRON2_FLAV5_IPSig1 = TH1F("Iron2_Flav5_IPSig1", "Iron2_Flav5_IPSig1", 100, 0.0, 10.0);
    TH1F LONG2_FLAV5_IPSig1 = TH1F("Long2_Flav5_IPSig1", "Long2_Flav5_IPSig1", 100, 0.0, 10.0);
    TH1F FAKE_FLAV5_IPSig1 = TH1F("IronFake_Flav5_IPSig1", "IronFake_Flav5_IPSig1", 100, 0.0, 10.0);


    //IP Sig2

    // LowIPSig2Electron_genPartFlav == 0
    TH1F IRON1_FLAV0_IPSig2 = TH1F("Iron1_Flav0_IPSig2", "Iron1_Flav0_IPSig2", 100, 0.0, 10.0);
    TH1F LONG1_FLAV0_IPSig2 = TH1F("Long1_Flav0_IPSig2", "Long1_Flav0_IPSig2", 100, 0.0, 10.0);
    TH1F IRON2_FLAV0_IPSig2 = TH1F("Iron2_Flav0_IPSig2", "Iron2_Flav0_IPSig2", 100, 0.0, 10.0);
    TH1F LONG2_FLAV0_IPSig2 = TH1F("Long2_Flav0_IPSig2", "Long2_Flav0_IPSig2", 100, 0.0, 10.0);
    TH1F FAKE_FLAV0_IPSig2 = TH1F("IronFake_Flav0_IPSig2", "IronFake_Flav0_IPSig2", 100, 0.0, 10.0);
    // LowIPSig2Electron_genPartFlav == 1
    TH1F IRON1_FLAV1_IPSig2 = TH1F("Iron1_Flav1_IPSig2", "Iron1_Flav1_IPSig2", 100, 0.0, 10.0);
    TH1F LONG1_FLAV1_IPSig2 = TH1F("Long1_Flav1_IPSig2", "Long1_Flav1_IPSig2", 100, 0.0, 10.0);
    TH1F IRON2_FLAV1_IPSig2 = TH1F("Iron2_Flav1_IPSig2", "Iron2_Flav1_IPSig2", 100, 0.0, 10.0);
    TH1F LONG2_FLAV1_IPSig2 = TH1F("Long2_Flav1_IPSig2", "Long2_Flav1_IPSig2", 100, 0.0, 10.0);
    TH1F FAKE_FLAV1_IPSig2 = TH1F("IronFake_Flav1_IPSig2", "IronFake_Flav1_IPSig2", 100, 0.0, 10.0);
    // LowIPSig2Electron_genPartFlav == 5
    TH1F IRON1_FLAV5_IPSig2 = TH1F("Iron1_Flav5_IPSig2", "Iron1_Flav5_IPSig2", 100, 0.0, 10.0);
    TH1F LONG1_FLAV5_IPSig2 = TH1F("Long1_Flav5_IPSig2", "Long1_Flav5_IPSig2", 100, 0.0, 10.0);
    TH1F IRON2_FLAV5_IPSig2 = TH1F("Iron2_Flav5_IPSig2", "Iron2_Flav5_IPSig2", 100, 0.0, 10.0);
    TH1F LONG2_FLAV5_IPSig2 = TH1F("Long2_Flav5_IPSig2", "Long2_Flav5_IPSig2", 100, 0.0, 10.0);
    TH1F FAKE_FLAV5_IPSig2 = TH1F("IronFake_Flav5_IPSig2", "IronFake_Flav5_IPSig2", 100, 0.0, 10.0);



    //IP Err
    // LowIPErrElectron_genPartFlav == 0
    TH1F IRON1_FLAV0_IPErr = TH1F("Iron1_Flav0_IPErr", "Iron1_Flav0_IPErr", 50, -5.0, 5.0);
    TH1F LONG1_FLAV0_IPErr = TH1F("Long1_Flav0_IPErr", "Long1_Flav0_IPErr", 50, -5.0, 5.0);
    TH1F IRON2_FLAV0_IPErr = TH1F("Iron2_Flav0_IPErr", "Iron2_Flav0_IPErr", 50, -5.0, 5.0);
    TH1F LONG2_FLAV0_IPErr = TH1F("Long2_Flav0_IPErr", "Long2_Flav0_IPErr", 50, -5.0, 5.0);
    TH1F FAKE_FLAV0_IPErr = TH1F("IronFake_Flav0_IPErr", "IronFake_Flav0_IPErr", 50, -5.0, 5.0);
    // LowIPErrElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_IPErr = TH1F("Iron1_Flav1_IPErr", "Iron1_Flav1_IPErr", 50, -5.0, 5.0);
    TH1F LONG1_FLAV1_IPErr = TH1F("Long1_Flav1_IPErr", "Long1_Flav1_IPErr", 50, -5.0, 5.0);
    TH1F IRON2_FLAV1_IPErr = TH1F("Iron2_Flav1_IPErr", "Iron2_Flav1_IPErr", 50, -5.0, 5.0);
    TH1F LONG2_FLAV1_IPErr = TH1F("Long2_Flav1_IPErr", "Long2_Flav1_IPErr", 50, -5.0, 5.0);
    TH1F FAKE_FLAV1_IPErr = TH1F("IronFake_Flav1_IPErr", "IronFake_Flav1_IPErr", 50, -5.0, 5.0);
    // LowIPErrElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_IPErr = TH1F("Iron1_Flav5_IPErr", "Iron1_Flav5_IPErr", 50, -5.0, 5.0);
    TH1F LONG1_FLAV5_IPErr = TH1F("Long1_Flav5_IPErr", "Long1_Flav5_IPErr", 50, -5.0, 5.0);
    TH1F IRON2_FLAV5_IPErr = TH1F("Iron2_Flav5_IPErr", "Iron2_Flav5_IPErr", 50, -5.0, 5.0);
    TH1F LONG2_FLAV5_IPErr = TH1F("Long2_Flav5_IPErr", "Long2_Flav5_IPErr", 50, -5.0, 5.0);
    TH1F FAKE_FLAV5_IPErr = TH1F("IronFake_Flav5_IPErr", "IronFake_Flav5_IPErr", 50, -5.0, 5.0);


    //IP
    // LowIPElectron_genPartFlav == 0
    TH1F IRON1_FLAV0_IP = TH1F("Iron1_Flav0_IP", "Iron1_Flav0_IP", 100, 0.0, 0.01);
    TH1F LONG1_FLAV0_IP = TH1F("Long1_Flav0_IP", "Long1_Flav0_IP", 100, 0.0, 0.01);
    TH1F IRON2_FLAV0_IP = TH1F("Iron2_Flav0_IP", "Iron2_Flav0_IP", 100, 0.0, 0.01);
    TH1F LONG2_FLAV0_IP = TH1F("Long2_Flav0_IP", "Long2_Flav0_IP", 100, 0.0, 0.01);
    TH1F FAKE_FLAV0_IP = TH1F("IronFake_Flav0_IP", "IronFake_Flav0_IP", 100, 0.0, 0.01);
    // LowIPElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_IP = TH1F("Iron1_Flav1_IP", "Iron1_Flav1_IP", 100, 0.0, 0.01);
    TH1F LONG1_FLAV1_IP = TH1F("Long1_Flav1_IP", "Long1_Flav1_IP", 100, 0.0, 0.01);
    TH1F IRON2_FLAV1_IP = TH1F("Iron2_Flav1_IP", "Iron2_Flav1_IP", 100, 0.0, 0.01);
    TH1F LONG2_FLAV1_IP = TH1F("Long2_Flav1_IP", "Long2_Flav1_IP", 100, 0.0, 0.01);
    TH1F FAKE_FLAV1_IP = TH1F("IronFake_Flav1_IP", "IronFake_Flav1_IP", 100, 0.0, 0.01);
    // LowIPElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_IP = TH1F("Iron1_Flav5_IP", "Iron1_Flav5_IP", 100, 0.0, 0.01);
    TH1F LONG1_FLAV5_IP = TH1F("Long1_Flav5_IP", "Long1_Flav5_IP", 100, 0.0, 0.01);
    TH1F IRON2_FLAV5_IP = TH1F("Iron2_Flav5_IP", "Iron2_Flav5_IP", 100, 0.0, 0.01);
    TH1F LONG2_FLAV5_IP = TH1F("Long2_Flav5_IP", "Long2_Flav5_IP", 100, 0.0, 0.01);
    TH1F FAKE_FLAV5_IP = TH1F("IronFake_Flav5_IP", "IronFake_Flav5_IP", 100, 0.0, 0.01);



    //IPSigdiff
    // LowIPSigdiffElectron_genPartFlav == 0
    TH1F IRON1_FLAV0_IPSigdiff = TH1F("Iron1_Flav0_IPSigdiff", "Iron1_Flav0_IPSigdiff", 50, 0.0, 5.0);
    TH1F LONG1_FLAV0_IPSigdiff = TH1F("Long1_Flav0_IPSigdiff", "Long1_Flav0_IPSigdiff", 50, -8.0, 8.0);
    TH1F IRON2_FLAV0_IPSigdiff = TH1F("Iron2_Flav0_IPSigdiff", "Iron2_Flav0_IPSigdiff", 50, 0.0, 5.0);
    TH1F LONG2_FLAV0_IPSigdiff = TH1F("Long2_Flav0_IPSigdiff", "Long2_Flav0_IPSigdiff", 50, -8.0, 8.0);
    TH1F FAKE_FLAV0_IPSigdiff = TH1F("IronFake_Flav0_IPSigdiff", "IronFake_Flav0_IPSigdiff", 50, -5.0, 5.0);
    // LowIPSigdiffElectron_genPartFlav == 1
    TH1F IRON1_FLAV1_IPSigdiff = TH1F("Iron1_Flav1_IPSigdiff", "Iron1_Flav1_IPSigdiff", 50, 0.0, 5.0);
    TH1F LONG1_FLAV1_IPSigdiff = TH1F("Long1_Flav1_IPSigdiff", "Long1_Flav1_IPSigdiff", 50, -8.0, 8.0);
    TH1F IRON2_FLAV1_IPSigdiff = TH1F("Iron2_Flav1_IPSigdiff", "Iron2_Flav1_IPSigdiff", 50, 0.0, 5.0);
    TH1F LONG2_FLAV1_IPSigdiff = TH1F("Long2_Flav1_IPSigdiff", "Long2_Flav1_IPSigdiff", 50, -8.0, 8.0);
    TH1F FAKE_FLAV1_IPSigdiff = TH1F("IronFake_Flav1_IPSigdiff", "IronFake_Flav1_IPSigdiff", 50, -5.0, 5.0);
    // LowIPSigdiffElectron_genPartFlav == 5
    TH1F IRON1_FLAV5_IPSigdiff = TH1F("Iron1_Flav5_IPSigdiff", "Iron1_Flav5_IPSigdiff", 50, 0.0, 5.0);
    TH1F LONG1_FLAV5_IPSigdiff = TH1F("Long1_Flav5_IPSigdiff", "Long1_Flav5_IPSigdiff", 50, -8.0, 8.0);
    TH1F IRON2_FLAV5_IPSigdiff = TH1F("Iron2_Flav5_IPSigdiff", "Iron2_Flav5_IPSigdiff", 50, 0.0, 5.0);
    TH1F LONG2_FLAV5_IPSigdiff = TH1F("Long2_Flav5_IPSigdiff", "Long2_Flav5_IPSigdiff", 50, -8.0, 8.0);
    TH1F FAKE_FLAV5_IPSigdiff = TH1F("IronFake_Flav5_IPSigdiff", "IronFake_Flav5_IPSigdiff", 50, -5.0, 5.0);
    //Ratio Plots



    TH2F pt_vs_EMID = TH2F("pt_vs_EMID", "pt_vs_EMID", 20, Lower_pt, Higher_pt, 22, 4.0, 12.0);
    TH2F pt_vs_eta = TH2F("pt_vs_eta", "pt_vs_eta", 20, Lower_pt, Higher_pt, 30, -2.4, 2.4);
    TH2F pt_vs_dxy = TH2F("pt_vs_dxy", "pt_vs_dxy", 20, Lower_pt, Higher_pt, 50, -0.05, 0.05);
    TH2F pt_vs_dxyErr = TH2F("pt_vs_dxyErr", "pt_vs_dxyErr", 20, Lower_pt, Higher_pt, 50, 0.0, 0.02);
    TH2F pt_vs_dxySig = TH2F("pt_vs_dxySig", "pt_vs_dxySig", 20, Lower_pt, Higher_pt, 50, -3, 3);
    TH2F pt_vs_dz = TH2F("pt_vs_dz", "pt_vs_dz", 20, Lower_pt, Higher_pt, 100, -0.05, 0.05);
    TH2F pt_vs_dzErr = TH2F("pt_vs_dzErr", "pt_vs_dzErr", 20, Lower_pt, Higher_pt, 50, 0, 0.06);
    TH2F pt_vs_dzSig = TH2F("pt_vs_dzSig", "pt_vs_dzSig", 20, Lower_pt, Higher_pt, 50, -5.0, 5.0);
    TH2F pt_vs_Ip = TH2F("pt_vs_Ip", "pt_vs_IP", 20, Lower_pt, Higher_pt, 100, 0.0, 0.01);
    TH2F pt_vs_IpErr = TH2F("pt_vs_IpErr", "pt_vs_IpErr", 20, Lower_pt, Higher_pt, 50, -5.0, 5.0);
    TH2F pt_vs_IpSig1 = TH2F("pt_vs_IpSig1", "pt_vs_IPSig1", 20, Lower_pt, Higher_pt, 100, 0.0, 6.0);
    TH2F pt_vs_IpSig2 = TH2F("pt_vs_IpSig2", "pt_vs_IPSig2", 20, Lower_pt, Higher_pt, 100, 0.0, 6.0);
    TH2F pt_vs_ISO = TH2F("pt_vs_ISO", "pt_vs_ISO", 20, Lower_pt, Higher_pt, 50, 0.0, 8.0);

    TH2F pt_vs_Flav = TH2F("pt_vs_Flav", "pt_vs_Flav", 20, Lower_pt, Higher_pt, 50, 0.0, 8.0);
    TH2F Flav_vs_EMID = TH2F("Flav_vs_EMID", "Flav_vs_EMID", 6, 0, 6, 22, EMID_cut, 12.0);



    TH2F dxysig_vs_dzsig = TH2F("dxysig_vs_dzsig", "dxysig_vs_dzsig", 100, -3.0, 3.0, 100, -3.0, 3.0);
    TH2F dzsig_vs_IPsig1 = TH2F("dzsig_vs_IPsig1", "dzsig_vs_IPsig1", 10, -3.0, 3.0, 100, 0.0, 10.0);
    TH2F dxysig_vs_IPsig1 = TH2F("dxysig_vs_IPsig1", "dxysig_vs_IPsig1", 10, -3.0, 3.0, 100, 0.0, 10.0);
    TH2F dzsig_vs_IPsig2 = TH2F("dzsig_vs_IPsig2", "dzsig_vs_IPsig2", 10, -3.0, 3.0, 100, 0.0, 10.0);
    TH2F dxysig_vs_IPsig2 = TH2F("dxysig_vs_IPsig2", "dxysig_vs_IPsig2", 10, -3.0, 3.0, 100, 0.0, 10.0);
    




    for (Long64_t jentry = 0; jentry < nentries; jentry++)
    {
        if (max_event > 0 && jentry >= max_event)
        {
            break;
        }
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
                dxySig = LowPtElectron_dxy[k] / LowPtElectron_dxyErr[k];
            }


            float dzSig = -999;

            if (LowPtElectron_dzErr[k] != 0)
            {
                dzSig = LowPtElectron_dz[k] / LowPtElectron_dzErr[k];
            }


            float IPSig1 = -999; //
            if (LowPtElectron_dxyErr[k] != 0 && LowPtElectron_dzErr[k] != 0)
            {
                IPSig1 = sqrt(dxySig * dxySig + dzSig * dzSig);

            }

            float IP = -999;
            float IPErr = -999;

            if (LowPtElectron_dxyErr[k] != 0 && LowPtElectron_dzErr[k] != 0)
            {
                IP = sqrt(LowPtElectron_dxy[k] * LowPtElectron_dxy[k] + LowPtElectron_dz[k] * LowPtElectron_dz[k]);
                IPErr = sqrt(LowPtElectron_dxyErr[k] * LowPtElectron_dxyErr[k] + LowPtElectron_dzErr[k] * LowPtElectron_dzErr[k]);

            }




            float IPSig2 = -999; //
            if (LowPtElectron_dxyErr[k] != 0)
            {
                IPSig2 = abs(IP / IPErr);

            }

            float IPdiff = IPSig1 - IPSig2;



            // No Flav
            if (LowPtElectron_pt[k] >= Lower_pt && LowPtElectron_pt[k] < Higher_pt && LowPtElectron_embeddedID[k] >= 1.5 && abs(LowPtElectron_eta[k]) < 2.4)
            {
                EMID.Fill(LowPtElectron_embeddedID[k]);
                Eta.Fill(LowPtElectron_eta[k]);
                Pt.Fill(LowPtElectron_pt[k]);
                Dxy.Fill(LowPtElectron_dxy[k]);
                Dz.Fill(LowPtElectron_dz[k]);
                DzErr.Fill(LowPtElectron_dzErr[k]);
                DxyErr.Fill(LowPtElectron_dxyErr[k]);
                DzSig.Fill(dzSig);
                DxySig.Fill(dxySig);
                IPSig1_graph.Fill(IPSig1);
                CONV.Fill(LowPtElectron_convVeto[k]);
                ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                IPSigDiff.Fill(IPdiff);
                IPSig2_graph.Fill(IPSig2);
                IP_graph.Fill(IP);
                IPErr_graph.Fill(IPErr);



                //Flavor 0

                if (LowPtElectron_genPartFlav[k] == 0)
                {
                    Flav0_EMID.Fill(LowPtElectron_embeddedID[k]);
                    Flav0_eta.Fill(LowPtElectron_eta[k]);
                    Flav0_pt.Fill(LowPtElectron_pt[k]);
                    Flav0_dxy.Fill(LowPtElectron_dxy[k]);
                    Flav0_dz.Fill(LowPtElectron_dz[k]);
                    Flav0_dzErr.Fill(LowPtElectron_dzErr[k]);
                    Flav0_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                    Flav0_dzSig.Fill(dzSig);
                    Flav0_dxySig.Fill(dxySig);
                    Flav0_IPSig1.Fill(IPSig1);
                    Flav0_CONV.Fill(LowPtElectron_convVeto[k]);
                    Flav0_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                    Flav0_IPSigdiff.Fill(IPdiff);
                    Flav0_IPSig2.Fill(IPSig2);
                    Flav0_IP.Fill(IP);
                    Flav0_IPErr.Fill(IPErr);


                }

                if (LowPtElectron_genPartFlav[k] == 1)
                {
                    Flav1_EMID.Fill(LowPtElectron_embeddedID[k]);
                    Flav1_eta.Fill(LowPtElectron_eta[k]);
                    Flav1_pt.Fill(LowPtElectron_pt[k]);
                    Flav1_dxy.Fill(LowPtElectron_dxy[k]);
                    Flav1_dz.Fill(LowPtElectron_dz[k]);
                    Flav1_dzErr.Fill(LowPtElectron_dzErr[k]);
                    Flav1_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                    Flav1_dzSig.Fill(dzSig);
                    Flav1_dxySig.Fill(dxySig);
                    Flav1_IPSig1.Fill(IPSig1);
                    Flav1_CONV.Fill(LowPtElectron_convVeto[k]);
                    Flav1_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                    Flav1_IPSigdiff.Fill(IPdiff);
                    Flav1_IPSig2.Fill(IPSig2);
                    Flav1_IP.Fill(IP);
                    Flav1_IPErr.Fill(IPErr);
                }

                if (LowPtElectron_genPartFlav[k] == 5)
                {
                    Flav5_EMID.Fill(LowPtElectron_embeddedID[k]);
                    Flav5_eta.Fill(LowPtElectron_eta[k]);
                    Flav5_pt.Fill(LowPtElectron_pt[k]);
                    Flav5_dxy.Fill(LowPtElectron_dxy[k]);
                    Flav5_dz.Fill(LowPtElectron_dz[k]);
                    Flav5_dzErr.Fill(LowPtElectron_dzErr[k]);
                    Flav5_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                    Flav5_dzSig.Fill(dzSig);
                    Flav5_dxySig.Fill(dxySig);
                    Flav5_IPSig1.Fill(IPSig1);
                    Flav5_CONV.Fill(LowPtElectron_convVeto[k]);
                    Flav5_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                    Flav5_IPSigdiff.Fill(IPdiff);
                    Flav5_IPSig2.Fill(IPSig2);
                    Flav5_IP.Fill(IP);
                    Flav5_IPErr.Fill(IPErr);

                }

                    ////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////
                    //GENERAL_PARAMETERS
                if (abs(LowPtElectron_eta[k]) < 2.4 && LowPtElectron_embeddedID[k] >= EMID_cut && LowPtElectron_convVeto[k]==1)
                {

                    
                 

                    //////IRON1_PARAMETERS

                    if (LowPtElectron_miniPFRelIso_all[k] < 4 && abs(LowPtElectron_dxy[k]) < 0.05 && abs(LowPtElectron_dz[k]) < 0.1 && IPSig1 < 2)
                    {
                        //////////////////////////////////////////////////
                        //Iron
                        IRON1_EMID.Fill(LowPtElectron_embeddedID[k]);
                        IRON1_eta.Fill(LowPtElectron_eta[k]);
                        IRON1_pt.Fill(LowPtElectron_pt[k]);
                        IRON1_dxy.Fill(LowPtElectron_dxy[k]);
                        IRON1_dz.Fill(LowPtElectron_dz[k]);
                        IRON1_dzErr.Fill(LowPtElectron_dzErr[k]);
                        IRON1_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                        IRON1_dzSig.Fill(dzSig);
                        IRON1_dxySig.Fill(dxySig);
                        IRON1_IPSig1.Fill(IPSig1);
                        IRON1_CONV.Fill(LowPtElectron_convVeto[k]);
                        IRON1_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                        IRON1_IPSigdiff.Fill(IPdiff);
                        IRON1_IPSig2.Fill(IPSig2);
                        IRON1_IP.Fill(IP);
                        IRON1_IPErr.Fill(IPErr);


                        pt_vs_EMID.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                        pt_vs_eta.Fill(LowPtElectron_pt[k], LowPtElectron_eta[k]);
                        pt_vs_dxy.Fill(LowPtElectron_pt[k], LowPtElectron_dxy[k]);
                        pt_vs_dxyErr.Fill(LowPtElectron_pt[k], LowPtElectron_dxyErr[k]);
                        pt_vs_dxySig.Fill(LowPtElectron_pt[k], dxySig);
                        pt_vs_dz.Fill(LowPtElectron_pt[k], LowPtElectron_dz[k]);
                        pt_vs_dzErr.Fill(LowPtElectron_pt[k], LowPtElectron_dzErr[k]);
                        pt_vs_dzSig.Fill(LowPtElectron_pt[k], dzSig);
                        pt_vs_Ip.Fill(LowPtElectron_pt[k], IP);
                        pt_vs_IpErr.Fill(LowPtElectron_pt[k], IPErr);
                        pt_vs_IpSig1.Fill(LowPtElectron_pt[k], IPSig1);
                        pt_vs_IpSig2.Fill(LowPtElectron_pt[k], IPSig2);
                        pt_vs_ISO.Fill(LowPtElectron_pt[k], LowPtElectron_miniPFRelIso_all[k]);

                        pt_vs_Flav.Fill(LowPtElectron_pt[k], LowPtElectron_genPartFlav[k]);
                        Flav_vs_EMID.Fill(LowPtElectron_genPartFlav[k], LowPtElectron_embeddedID[k]);



                        IRON1_GenID_idx.Fill(LowPtElectron_genPartIdx[k]);



                        dxysig_vs_dzsig.Fill(dxySig, dzSig);
                        dzsig_vs_IPsig1.Fill(dzSig, IPSig1);
                        dxysig_vs_IPsig1.Fill(dxySig, IPSig1);
                        dzsig_vs_IPsig2.Fill(dzSig, IPSig2);
                        dxysig_vs_IPsig2.Fill(dxySig, IPSig2);


                        if (LowPtElectron_genPartFlav[k] == 5)
                        {
                            IRON1_FLAV5_EMID.Fill(LowPtElectron_embeddedID[k]);
                            IRON1_FLAV5_eta.Fill(LowPtElectron_eta[k]);
                            IRON1_FLAV5_pt.Fill(LowPtElectron_pt[k]);
                            IRON1_FLAV5_dxy.Fill(LowPtElectron_dxy[k]);
                            IRON1_FLAV5_dz.Fill(LowPtElectron_dz[k]);
                            IRON1_FLAV5_dzErr.Fill(LowPtElectron_dzErr[k]);
                            IRON1_FLAV5_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            IRON1_FLAV5_dzSig.Fill(dzSig);
                            IRON1_FLAV5_dxySig.Fill(dxySig);
                            IRON1_FLAV5_IPSig1.Fill(IPSig1);
                            IRON1_FLAV5_CONV.Fill(LowPtElectron_convVeto[k]);
                            IRON1_FLAV5_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            IRON1_FLAV5_IPSigdiff.Fill(IPdiff);
                            IRON1_FLAV5_IPSig2.Fill(IPSig2);
                            IRON1_FLAV5_IP.Fill(IP);
                            IRON1_FLAV5_IPErr.Fill(IPErr);
                        }
                        // LowPtElectron_genPartFlav == 1
                        if (LowPtElectron_genPartFlav[k] == 0)
                        {
                            IRON1_FLAV0_EMID.Fill(LowPtElectron_embeddedID[k]);
                            IRON1_FLAV0_eta.Fill(LowPtElectron_eta[k]);
                            IRON1_FLAV0_pt.Fill(LowPtElectron_pt[k]);
                            IRON1_FLAV0_dxy.Fill(LowPtElectron_dxy[k]);
                            IRON1_FLAV0_dz.Fill(LowPtElectron_dz[k]);
                            IRON1_FLAV0_dzErr.Fill(LowPtElectron_dzErr[k]);
                            IRON1_FLAV0_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            IRON1_FLAV0_dzSig.Fill(dzSig);
                            IRON1_FLAV0_dxySig.Fill(dxySig);
                            IRON1_FLAV0_IPSig1.Fill(IPSig1);
                            IRON1_FLAV0_CONV.Fill(LowPtElectron_convVeto[k]);
                            IRON1_FLAV0_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            IRON1_FLAV0_IPSigdiff.Fill(IPdiff);
                            IRON1_FLAV0_IPSig2.Fill(IPSig2);
                            IRON1_FLAV0_IP.Fill(IP);
                            IRON1_FLAV0_IPErr.Fill(IPErr);
                        }
                        // LowPtElectron_genPartFlav == 5
                        if (LowPtElectron_genPartFlav[k] == 1)
                        {
                            IRON1_FLAV1_EMID.Fill(LowPtElectron_embeddedID[k]);
                            IRON1_FLAV1_eta.Fill(LowPtElectron_eta[k]);
                            IRON1_FLAV1_pt.Fill(LowPtElectron_pt[k]);
                            IRON1_FLAV1_dxy.Fill(LowPtElectron_dxy[k]);
                            IRON1_FLAV1_dz.Fill(LowPtElectron_dz[k]);
                            IRON1_FLAV1_dzErr.Fill(LowPtElectron_dzErr[k]);
                            IRON1_FLAV1_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            IRON1_FLAV1_dzSig.Fill(dzSig);
                            IRON1_FLAV1_dxySig.Fill(dxySig);
                            IRON1_FLAV1_IPSig1.Fill(IPSig1);
                            IRON1_FLAV1_CONV.Fill(LowPtElectron_convVeto[k]);
                            IRON1_FLAV1_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            IRON1_FLAV1_IPSigdiff.Fill(IPdiff);
                            IRON1_FLAV1_IPSig2.Fill(IPSig2);
                            IRON1_FLAV1_IP.Fill(IP);
                            IRON1_FLAV1_IPErr.Fill(IPErr);
                        }

                    }


                    //IRON2_PARAMETERS 
                    if (LowPtElectron_miniPFRelIso_all[k] < 4 && abs(LowPtElectron_dxy[k]) < 0.05 && abs(LowPtElectron_dz[k]) < 0.1 && abs(dzSig) < 2)
                    {
                        //////////////////////////////////////////////////
                        //Iron
                        IRON2_EMID.Fill(LowPtElectron_embeddedID[k]);
                        IRON2_eta.Fill(LowPtElectron_eta[k]);
                        IRON2_pt.Fill(LowPtElectron_pt[k]);
                        IRON2_dxy.Fill(LowPtElectron_dxy[k]);
                        IRON2_dz.Fill(LowPtElectron_dz[k]);
                        IRON2_dzErr.Fill(LowPtElectron_dzErr[k]);
                        IRON2_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                        IRON2_dzSig.Fill(dzSig);
                        IRON2_dxySig.Fill(dxySig);
                        IRON2_IPSig1.Fill(IPSig1);
                        IRON2_CONV.Fill(LowPtElectron_convVeto[k]);
                        IRON2_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                        IRON2_IPSigdiff.Fill(IPdiff);
                        IRON2_IPSig2.Fill(IPSig2);
                        IRON2_IP.Fill(IP);
                        IRON2_IPErr.Fill(IPErr);


                        if (LowPtElectron_genPartFlav[k] == 5)
                        {
                            IRON2_FLAV5_EMID.Fill(LowPtElectron_embeddedID[k]);
                            IRON2_FLAV5_eta.Fill(LowPtElectron_eta[k]);
                            IRON2_FLAV5_pt.Fill(LowPtElectron_pt[k]);
                            IRON2_FLAV5_dxy.Fill(LowPtElectron_dxy[k]);
                            IRON2_FLAV5_dz.Fill(LowPtElectron_dz[k]);
                            IRON2_FLAV5_dzErr.Fill(LowPtElectron_dzErr[k]);
                            IRON2_FLAV5_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            IRON2_FLAV5_dzSig.Fill(dzSig);
                            IRON2_FLAV5_dxySig.Fill(dxySig);
                            IRON2_FLAV5_IPSig1.Fill(IPSig1);
                            IRON2_FLAV5_CONV.Fill(LowPtElectron_convVeto[k]);
                            IRON2_FLAV5_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            IRON2_FLAV5_IPSigdiff.Fill(IPdiff);
                            IRON2_FLAV5_IPSig2.Fill(IPSig2);
                            IRON2_FLAV5_IP.Fill(IP);
                            IRON2_FLAV5_IPErr.Fill(IPErr);
                        }
                        // LowPtElectron_genPartFlav == 1
                        if (LowPtElectron_genPartFlav[k] == 0)
                        {
                            IRON2_FLAV0_EMID.Fill(LowPtElectron_embeddedID[k]);
                            IRON2_FLAV0_eta.Fill(LowPtElectron_eta[k]);
                            IRON2_FLAV0_pt.Fill(LowPtElectron_pt[k]);
                            IRON2_FLAV0_dxy.Fill(LowPtElectron_dxy[k]);
                            IRON2_FLAV0_dz.Fill(LowPtElectron_dz[k]);
                            IRON2_FLAV0_dzErr.Fill(LowPtElectron_dzErr[k]);
                            IRON2_FLAV0_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            IRON2_FLAV0_dzSig.Fill(dzSig);
                            IRON2_FLAV0_dxySig.Fill(dxySig);
                            IRON2_FLAV0_IPSig1.Fill(IPSig1);
                            IRON2_FLAV0_CONV.Fill(LowPtElectron_convVeto[k]);
                            IRON2_FLAV0_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            IRON2_FLAV0_IPSigdiff.Fill(IPdiff);
                            IRON2_FLAV0_IPSig2.Fill(IPSig2);
                            IRON2_FLAV0_IP.Fill(IP);
                            IRON2_FLAV0_IPErr.Fill(IPErr);
                        }
                        // LowPtElectron_genPartFlav == 5
                        if (LowPtElectron_genPartFlav[k] == 1)
                        {
                            IRON2_FLAV1_EMID.Fill(LowPtElectron_embeddedID[k]);
                            IRON2_FLAV1_eta.Fill(LowPtElectron_eta[k]);
                            IRON2_FLAV1_pt.Fill(LowPtElectron_pt[k]);
                            IRON2_FLAV1_dxy.Fill(LowPtElectron_dxy[k]);
                            IRON2_FLAV1_dz.Fill(LowPtElectron_dz[k]);
                            IRON2_FLAV1_dzErr.Fill(LowPtElectron_dzErr[k]);
                            IRON2_FLAV1_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            IRON2_FLAV1_dzSig.Fill(dzSig);
                            IRON2_FLAV1_dxySig.Fill(dxySig);
                            IRON2_FLAV1_IPSig1.Fill(IPSig1);
                            IRON2_FLAV1_CONV.Fill(LowPtElectron_convVeto[k]);
                            IRON2_FLAV1_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            IRON2_FLAV1_IPSigdiff.Fill(IPdiff);
                            IRON2_FLAV1_IPSig2.Fill(IPSig2);
                            IRON2_FLAV1_IP.Fill(IP);
                            IRON2_FLAV1_IPErr.Fill(IPErr);
                        }

                    }





                    //////////////////////////////////////////////////
                    //Irom Long
                    if (LowPtElectron_miniPFRelIso_all[k] < 4 && IPSig1 >= 2)
                    {
                        Long1_EMID.Fill(LowPtElectron_embeddedID[k]);
                        Long1_eta.Fill(LowPtElectron_eta[k]);
                        Long1_pt.Fill(LowPtElectron_pt[k]);
                        Long1_dxy.Fill(LowPtElectron_dxy[k]);
                        Long1_dz.Fill(LowPtElectron_dz[k]);
                        Long1_dzErr.Fill(LowPtElectron_dzErr[k]);
                        Long1_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                        Long1_dzSig.Fill(dzSig);
                        Long1_dxySig.Fill(dxySig);
                        Long1_IPSig1.Fill(IPSig1);
                        Long1_CONV.Fill(LowPtElectron_convVeto[k]);
                        Long1_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                        Long1_IPSigdiff.Fill(IPdiff);
                        Long1_IPSig2.Fill(IPSig2);
                        Long1_IP.Fill(IP);
                        Long1_IPErr.Fill(IPErr);


                        if (LowPtElectron_genPartFlav[k] == 5)
                        {
                            LONG1_FLAV5_EMID.Fill(LowPtElectron_embeddedID[k]);
                            LONG1_FLAV5_eta.Fill(LowPtElectron_eta[k]);
                            LONG1_FLAV5_pt.Fill(LowPtElectron_pt[k]);
                            LONG1_FLAV5_dxy.Fill(LowPtElectron_dxy[k]);
                            LONG1_FLAV5_dz.Fill(LowPtElectron_dz[k]);
                            LONG1_FLAV5_dzErr.Fill(LowPtElectron_dzErr[k]);
                            LONG1_FLAV5_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            LONG1_FLAV5_dzSig.Fill(dzSig);
                            LONG1_FLAV5_dxySig.Fill(dxySig);
                            LONG1_FLAV5_IPSig1.Fill(IPSig1);
                            LONG1_FLAV5_CONV.Fill(LowPtElectron_convVeto[k]);
                            LONG1_FLAV5_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            LONG1_FLAV5_IPSigdiff.Fill(IPdiff);
                            LONG1_FLAV5_IPSig2.Fill(IPSig2);
                            LONG1_FLAV5_IP.Fill(IP);
                            LONG1_FLAV5_IPErr.Fill(IPErr);
                        }
                        // LowPtElectron_genPartFlav == 1
                        if (LowPtElectron_genPartFlav[k] == 0)
                        {
                            LONG1_FLAV0_EMID.Fill(LowPtElectron_embeddedID[k]);
                            LONG1_FLAV0_eta.Fill(LowPtElectron_eta[k]);
                            LONG1_FLAV0_pt.Fill(LowPtElectron_pt[k]);
                            LONG1_FLAV0_dxy.Fill(LowPtElectron_dxy[k]);
                            LONG1_FLAV0_dz.Fill(LowPtElectron_dz[k]);
                            LONG1_FLAV0_dzErr.Fill(LowPtElectron_dzErr[k]);
                            LONG1_FLAV0_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            LONG1_FLAV0_dzSig.Fill(dzSig);
                            LONG1_FLAV0_dxySig.Fill(dxySig);
                            LONG1_FLAV0_IPSig1.Fill(IPSig1);
                            LONG1_FLAV0_CONV.Fill(LowPtElectron_convVeto[k]);
                            LONG1_FLAV0_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            LONG1_FLAV0_IPSigdiff.Fill(IPdiff);
                            LONG1_FLAV0_IPSig2.Fill(IPSig2);
                            LONG1_FLAV0_IP.Fill(IP);
                            LONG1_FLAV0_IPErr.Fill(IPErr);
                        }
                        // LowPtElectron_genPartFlav == 5
                        if (LowPtElectron_genPartFlav[k] == 1)
                        {
                            LONG1_FLAV1_EMID.Fill(LowPtElectron_embeddedID[k]);
                            LONG1_FLAV1_eta.Fill(LowPtElectron_eta[k]);
                            LONG1_FLAV1_pt.Fill(LowPtElectron_pt[k]);
                            LONG1_FLAV1_dxy.Fill(LowPtElectron_dxy[k]);
                            LONG1_FLAV1_dz.Fill(LowPtElectron_dz[k]);
                            LONG1_FLAV1_dzErr.Fill(LowPtElectron_dzErr[k]);
                            LONG1_FLAV1_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            LONG1_FLAV1_dzSig.Fill(dzSig);
                            LONG1_FLAV1_dxySig.Fill(dxySig);
                            LONG1_FLAV1_IPSig1.Fill(IPSig1);
                            LONG1_FLAV1_CONV.Fill(LowPtElectron_convVeto[k]);
                            LONG1_FLAV1_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            LONG1_FLAV1_IPSigdiff.Fill(IPdiff);
                            LONG1_FLAV1_IPSig2.Fill(IPSig2);
                            LONG1_FLAV1_IP.Fill(IP);
                            LONG1_FLAV1_IPErr.Fill(IPErr);
                        }

                    }

                    if (LowPtElectron_miniPFRelIso_all[k] < 4 && abs(dzSig) >= 2)
                    {
                        //////////////////////////////////////////////////
                        //LONG 2
                        Long2_EMID.Fill(LowPtElectron_embeddedID[k]);
                        Long2_eta.Fill(LowPtElectron_eta[k]);
                        Long2_pt.Fill(LowPtElectron_pt[k]);
                        Long2_dxy.Fill(LowPtElectron_dxy[k]);
                        Long2_dz.Fill(LowPtElectron_dz[k]);
                        Long2_dzErr.Fill(LowPtElectron_dzErr[k]);
                        Long2_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                        Long2_dzSig.Fill(dzSig);
                        Long2_dxySig.Fill(dxySig);
                        Long2_IPSig1.Fill(IPSig1);
                        Long2_CONV.Fill(LowPtElectron_convVeto[k]);
                        Long2_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                        Long2_IPSigdiff.Fill(IPdiff);
                        Long2_IPSig2.Fill(IPSig2);
                        Long2_IP.Fill(IP);
                        Long2_IPErr.Fill(IPErr);


                        if (LowPtElectron_genPartFlav[k] == 5)
                        {
                            LONG2_FLAV5_EMID.Fill(LowPtElectron_embeddedID[k]);
                            LONG2_FLAV5_eta.Fill(LowPtElectron_eta[k]);
                            LONG2_FLAV5_pt.Fill(LowPtElectron_pt[k]);
                            LONG2_FLAV5_dxy.Fill(LowPtElectron_dxy[k]);
                            LONG2_FLAV5_dz.Fill(LowPtElectron_dz[k]);
                            LONG2_FLAV5_dzErr.Fill(LowPtElectron_dzErr[k]);
                            LONG2_FLAV5_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            LONG2_FLAV5_dzSig.Fill(dzSig);
                            LONG2_FLAV5_dxySig.Fill(dxySig);
                            LONG2_FLAV5_IPSig1.Fill(IPSig1);
                            LONG2_FLAV5_CONV.Fill(LowPtElectron_convVeto[k]);
                            LONG2_FLAV5_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            LONG2_FLAV5_IPSigdiff.Fill(IPdiff);
                            LONG2_FLAV5_IPSig2.Fill(IPSig2);
                            LONG2_FLAV5_IP.Fill(IP);
                            LONG2_FLAV5_IPErr.Fill(IPErr);
                        }
                        // LowPtElectron_genPartFlav == 1
                        if (LowPtElectron_genPartFlav[k] == 0)
                        {
                            LONG2_FLAV0_EMID.Fill(LowPtElectron_embeddedID[k]);
                            LONG2_FLAV0_eta.Fill(LowPtElectron_eta[k]);
                            LONG2_FLAV0_pt.Fill(LowPtElectron_pt[k]);
                            LONG2_FLAV0_dxy.Fill(LowPtElectron_dxy[k]);
                            LONG2_FLAV0_dz.Fill(LowPtElectron_dz[k]);
                            LONG2_FLAV0_dzErr.Fill(LowPtElectron_dzErr[k]);
                            LONG2_FLAV0_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            LONG2_FLAV0_dzSig.Fill(dzSig);
                            LONG2_FLAV0_dxySig.Fill(dxySig);
                            LONG2_FLAV0_IPSig1.Fill(IPSig1);
                            LONG2_FLAV0_CONV.Fill(LowPtElectron_convVeto[k]);
                            LONG2_FLAV0_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            LONG2_FLAV0_IPSigdiff.Fill(IPdiff);
                            LONG2_FLAV0_IPSig2.Fill(IPSig2);
                            LONG2_FLAV0_IP.Fill(IP);
                            LONG2_FLAV0_IPErr.Fill(IPErr);
                        }
                        // LowPtElectron_genPartFlav == 5
                        if (LowPtElectron_genPartFlav[k] == 1)
                        {
                            LONG2_FLAV1_EMID.Fill(LowPtElectron_embeddedID[k]);
                            LONG2_FLAV1_eta.Fill(LowPtElectron_eta[k]);
                            LONG2_FLAV1_pt.Fill(LowPtElectron_pt[k]);
                            LONG2_FLAV1_dxy.Fill(LowPtElectron_dxy[k]);
                            LONG2_FLAV1_dz.Fill(LowPtElectron_dz[k]);
                            LONG2_FLAV1_dzErr.Fill(LowPtElectron_dzErr[k]);
                            LONG2_FLAV1_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            LONG2_FLAV1_dzSig.Fill(dzSig);
                            LONG2_FLAV1_dxySig.Fill(dxySig);
                            LONG2_FLAV1_IPSig1.Fill(IPSig1);
                            LONG2_FLAV1_CONV.Fill(LowPtElectron_convVeto[k]);
                            LONG2_FLAV1_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            LONG2_FLAV1_IPSigdiff.Fill(IPdiff);
                            LONG2_FLAV1_IPSig2.Fill(IPSig2);
                            LONG2_FLAV1_IP.Fill(IP);
                            LONG2_FLAV1_IPErr.Fill(IPErr);
                        }

                    }





                    if (LowPtElectron_miniPFRelIso_all[k] >= 4 && abs(LowPtElectron_dxy[k]) < 0.05 && abs(LowPtElectron_dz[k]) < 0.1 && IPSig1 < 2)
                    {
                        //////////////////////////////////////////////////
                        //Iron
                        FAKE_EMID.Fill(LowPtElectron_embeddedID[k]);
                        FAKE_eta.Fill(LowPtElectron_eta[k]);
                        FAKE_pt.Fill(LowPtElectron_pt[k]);
                        FAKE_dxy.Fill(LowPtElectron_dxy[k]);
                        FAKE_dz.Fill(LowPtElectron_dz[k]);
                        FAKE_dzErr.Fill(LowPtElectron_dzErr[k]);
                        FAKE_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                        FAKE_dzSig.Fill(dzSig);
                        FAKE_dxySig.Fill(dxySig);
                        FAKE_IPSig1.Fill(IPSig1);
                        FAKE_CONV.Fill(LowPtElectron_convVeto[k]);
                        FAKE_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                        FAKE_IPSigdiff.Fill(IPdiff);
                        FAKE_IPSig2.Fill(IPSig2);
                        FAKE_IP.Fill(IP);
                        FAKE_IPErr.Fill(IPErr);
                        if (LowPtElectron_genPartFlav[k] == 5)
                        {
                            FAKE_FLAV5_EMID.Fill(LowPtElectron_embeddedID[k]);
                            FAKE_FLAV5_eta.Fill(LowPtElectron_eta[k]);
                            FAKE_FLAV5_pt.Fill(LowPtElectron_pt[k]);
                            FAKE_FLAV5_dxy.Fill(LowPtElectron_dxy[k]);
                            FAKE_FLAV5_dz.Fill(LowPtElectron_dz[k]);
                            FAKE_FLAV5_dzErr.Fill(LowPtElectron_dzErr[k]);
                            FAKE_FLAV5_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            FAKE_FLAV5_dzSig.Fill(dzSig);
                            FAKE_FLAV5_dxySig.Fill(dxySig);
                            FAKE_FLAV5_IPSig1.Fill(IPSig1);
                            FAKE_FLAV5_CONV.Fill(LowPtElectron_convVeto[k]);
                            FAKE_FLAV5_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            FAKE_FLAV5_IPSigdiff.Fill(IPdiff);
                            FAKE_FLAV5_IPSig2.Fill(IPSig2);
                            FAKE_FLAV5_IP.Fill(IP);
                            FAKE_FLAV5_IPErr.Fill(IPErr);
                        }
                        // LowPtElectron_genPartFlav == 1
                        if (LowPtElectron_genPartFlav[k] == 0)
                        {
                            FAKE_FLAV0_EMID.Fill(LowPtElectron_embeddedID[k]);
                            FAKE_FLAV0_eta.Fill(LowPtElectron_eta[k]);
                            FAKE_FLAV0_pt.Fill(LowPtElectron_pt[k]);
                            FAKE_FLAV0_dxy.Fill(LowPtElectron_dxy[k]);
                            FAKE_FLAV0_dz.Fill(LowPtElectron_dz[k]);
                            FAKE_FLAV0_dzErr.Fill(LowPtElectron_dzErr[k]);
                            FAKE_FLAV0_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            FAKE_FLAV0_dzSig.Fill(dzSig);
                            FAKE_FLAV0_dxySig.Fill(dxySig);
                            FAKE_FLAV0_IPSig1.Fill(IPSig1);
                            FAKE_FLAV0_CONV.Fill(LowPtElectron_convVeto[k]);
                            FAKE_FLAV0_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            FAKE_FLAV0_IPSigdiff.Fill(IPdiff);
                            FAKE_FLAV0_IPSig2.Fill(IPSig2);
                            FAKE_FLAV0_IP.Fill(IP);
                            FAKE_FLAV0_IPErr.Fill(IPErr);
                        }
                        // LowPtElectron_genPartFlav == 5
                        if (LowPtElectron_genPartFlav[k] == 1)
                        {
                            FAKE_FLAV1_EMID.Fill(LowPtElectron_embeddedID[k]);
                            FAKE_FLAV1_eta.Fill(LowPtElectron_eta[k]);
                            FAKE_FLAV1_pt.Fill(LowPtElectron_pt[k]);
                            FAKE_FLAV1_dxy.Fill(LowPtElectron_dxy[k]);
                            FAKE_FLAV1_dz.Fill(LowPtElectron_dz[k]);
                            FAKE_FLAV1_dzErr.Fill(LowPtElectron_dzErr[k]);
                            FAKE_FLAV1_dxyErr.Fill(LowPtElectron_dxyErr[k]);
                            FAKE_FLAV1_dzSig.Fill(dzSig);
                            FAKE_FLAV1_dxySig.Fill(dxySig);
                            FAKE_FLAV1_IPSig1.Fill(IPSig1);
                            FAKE_FLAV1_CONV.Fill(LowPtElectron_convVeto[k]);
                            FAKE_FLAV1_ISO.Fill(LowPtElectron_miniPFRelIso_all[k]);
                            FAKE_FLAV1_IPSigdiff.Fill(IPdiff);
                            FAKE_FLAV1_IPSig2.Fill(IPSig2);
                            FAKE_FLAV1_IP.Fill(IP);
                            FAKE_FLAV1_IPErr.Fill(IPErr);
                        }

                    }

                }/// MAIN IF STATEMENT   
            }//Pt Cut 
        }


    }




    //EMID

    //EMID

    
    PlotHist(IRON1_FLAV0_EMID, sample, plot_dir, nam, "EMID", "EMID_IRON1_Flav0", "EMID");
    PlotHist(IRON2_FLAV0_EMID, sample, plot_dir, nam, "EMID", "EMID_IRON2_Flav0", "EMID");
    PlotHist(LONG1_FLAV0_EMID, sample, plot_dir, nam, "EMID", "EMID_Long1_Flav0", "EMID");
    PlotHist(LONG2_FLAV0_EMID, sample, plot_dir, nam, "EMID", "EMID_Long2_Flav0", "EMID");
    PlotHist(FAKE_FLAV0_EMID, sample, plot_dir, nam, "EMID", "EMID_Fake_Flav0", "EMID");

    PlotHist(IRON1_FLAV1_EMID, sample, plot_dir, nam, "EMID", "EMID_IRON1_Flav1", "EMID");
    PlotHist(LONG1_FLAV1_EMID, sample, plot_dir, nam, "EMID", "EMID_Long1_Flav1", "EMID");
    PlotHist(IRON2_FLAV1_EMID, sample, plot_dir, nam, "EMID", "EMID_IRON2_Flav1", "EMID");
    PlotHist(LONG2_FLAV1_EMID, sample, plot_dir, nam, "EMID", "EMID_Long2_Flav1", "EMID");
    PlotHist(FAKE_FLAV1_EMID, sample, plot_dir, nam, "EMID", "EMID_Fake_Flav1", "EMID");

    PlotHist(IRON1_FLAV5_EMID, sample, plot_dir, nam, "EMID", "EMID_IRON1_Flav5", "EMID");
    PlotHist(LONG1_FLAV5_EMID, sample, plot_dir, nam, "EMID", "EMID_Long1_Flav5", "EMID");
    PlotHist(IRON2_FLAV5_EMID, sample, plot_dir, nam, "EMID", "EMID_IRON2_Flav5", "EMID");
    PlotHist(LONG2_FLAV5_EMID, sample, plot_dir, nam, "EMID", "EMID_Long2_Flav5", "EMID");
    PlotHist(FAKE_FLAV5_EMID, sample, plot_dir, nam, "EMID", "EMID_Fake_Flav5", "EMID");


    //pt
    PlotHist(IRON1_FLAV0_pt, sample, plot_dir, nam, "PT", "pt_IRON1_Flav0", "pt");
    PlotHist(IRON2_FLAV0_pt, sample, plot_dir, nam, "PT", "pt_IRON2_Flav0", "pt");
    PlotHist(LONG1_FLAV0_pt, sample, plot_dir, nam, "PT", "pt_Long1_Flav0", "pt");
    PlotHist(LONG2_FLAV0_pt, sample, plot_dir, nam, "PT", "pt_Long2_Flav0", "pt");
    PlotHist(FAKE_FLAV0_pt, sample, plot_dir, nam, "PT", "pt_Fake_Flav0", "pt");

    PlotHist(IRON1_FLAV1_pt, sample, plot_dir, nam, "PT", "pt_IRON1_Flav1", "pt");
    PlotHist(LONG1_FLAV1_pt, sample, plot_dir, nam, "PT", "pt_Long1_Flav1", "pt");
    PlotHist(IRON2_FLAV1_pt, sample, plot_dir, nam, "PT", "pt_IRON2_Flav1", "pt");
    PlotHist(LONG2_FLAV1_pt, sample, plot_dir, nam, "PT", "pt_Long2_Flav1", "pt");
    PlotHist(FAKE_FLAV1_pt, sample, plot_dir, nam, "PT", "pt_Fake_Flav1", "pt");

    PlotHist(IRON1_FLAV5_pt, sample, plot_dir, nam, "PT", "pt_IRON1_Flav5", "pt");
    PlotHist(LONG1_FLAV5_pt, sample, plot_dir, nam, "PT", "pt_Long1_Flav5", "pt");
    PlotHist(IRON2_FLAV5_pt, sample, plot_dir, nam, "PT", "pt_IRON2_Flav5", "pt");
    PlotHist(LONG2_FLAV5_pt, sample, plot_dir, nam, "PT", "pt_Long2_Flav5", "pt");
    PlotHist(FAKE_FLAV5_pt, sample, plot_dir, nam, "PT", "pt_Fake_Flav5", "pt");

    //eta
    PlotHist(IRON1_FLAV0_eta, sample, plot_dir, nam, "ETA", "eta_IRON1_Flav0", "eta");
    PlotHist(IRON2_FLAV0_eta, sample, plot_dir, nam, "ETA", "eta_IRON2_Flav0", "eta");
    PlotHist(LONG1_FLAV0_eta, sample, plot_dir, nam, "ETA", "eta_Long1_Flav0", "eta");
    PlotHist(LONG2_FLAV0_eta, sample, plot_dir, nam, "ETA", "eta_Long2_Flav0", "eta");
    PlotHist(FAKE_FLAV0_eta, sample, plot_dir, nam, "ETA", "eta_Fake_Flav0", "eta");

    PlotHist(IRON1_FLAV1_eta, sample, plot_dir, nam, "ETA", "eta_IRON1_Flav1", "eta");
    PlotHist(LONG1_FLAV1_eta, sample, plot_dir, nam, "ETA", "eta_Long1_Flav1", "eta");
    PlotHist(IRON2_FLAV1_eta, sample, plot_dir, nam, "ETA", "eta_IRON2_Flav1", "eta");
    PlotHist(LONG2_FLAV1_eta, sample, plot_dir, nam, "ETA", "eta_Long2_Flav1", "eta");
    PlotHist(FAKE_FLAV1_eta, sample, plot_dir, nam, "ETA", "eta_Fake_Flav1", "eta");

    PlotHist(IRON1_FLAV5_eta, sample, plot_dir, nam, "ETA", "eta_IRON1_Flav5", "eta");
    PlotHist(LONG1_FLAV5_eta, sample, plot_dir, nam, "ETA", "eta_Long1_Flav5", "eta");
    PlotHist(IRON2_FLAV5_eta, sample, plot_dir, nam, "ETA", "eta_IRON2_Flav5", "eta");
    PlotHist(LONG2_FLAV5_eta, sample, plot_dir, nam, "ETA", "eta_Long2_Flav5", "eta");
    PlotHist(FAKE_FLAV5_eta, sample, plot_dir, nam, "ETA", "eta_Fake_Flav5", "eta");


    //dxy
    PlotHist(IRON1_FLAV0_dxy, sample, plot_dir, nam, "DXY", "dxy_IRON1_Flav0", "dxy");
    PlotHist(IRON2_FLAV0_dxy, sample, plot_dir, nam, "DXY", "dxy_IRON2_Flav0", "dxy");
    PlotHist(LONG1_FLAV0_dxy, sample, plot_dir, nam, "DXY", "dxy_Long1_Flav0", "dxy");
    PlotHist(LONG2_FLAV0_dxy, sample, plot_dir, nam, "DXY", "dxy_Long2_Flav0", "dxy");
    PlotHist(FAKE_FLAV0_dxy, sample, plot_dir, nam, "DXY", "dxy_Fake_Flav0", "dxy");

    PlotHist(IRON1_FLAV1_dxy, sample, plot_dir, nam, "DXY", "dxy_IRON1_Flav1", "dxy");
    PlotHist(LONG1_FLAV1_dxy, sample, plot_dir, nam, "DXY", "dxy_Long1_Flav1", "dxy");
    PlotHist(IRON2_FLAV1_dxy, sample, plot_dir, nam, "DXY", "dxy_IRON2_Flav1", "dxy");
    PlotHist(LONG2_FLAV1_dxy, sample, plot_dir, nam, "DXY", "dxy_Long2_Flav1", "dxy");
    PlotHist(FAKE_FLAV1_dxy, sample, plot_dir, nam, "DXY", "dxy_Fake_Flav1", "dxy");

    PlotHist(IRON1_FLAV5_dxy, sample, plot_dir, nam, "DXY", "dxy_IRON1_Flav5", "dxy");
    PlotHist(LONG1_FLAV5_dxy, sample, plot_dir, nam, "DXY", "dxy_Long1_Flav5", "dxy");
    PlotHist(IRON2_FLAV5_dxy, sample, plot_dir, nam, "DXY", "dxy_IRON2_Flav5", "dxy");
    PlotHist(LONG2_FLAV5_dxy, sample, plot_dir, nam, "DXY", "dxy_Long2_Flav5", "dxy");
    PlotHist(FAKE_FLAV5_dxy, sample, plot_dir, nam, "DXY", "dxy_Fake_Flav5", "dxy");


    //dxyErr
    PlotHist(IRON1_FLAV0_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_IRON1_Flav0", "dxyErr");
    PlotHist(IRON2_FLAV0_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_IRON2_Flav0", "dxyErr");
    PlotHist(LONG1_FLAV0_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_Long1_Flav0", "dxyErr");
    PlotHist(LONG2_FLAV0_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_Long2_Flav0", "dxyErr");
    PlotHist(FAKE_FLAV0_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_Fake_Flav0", "dxyErr");

    PlotHist(IRON1_FLAV1_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_IRON1_Flav1", "dxyErr");
    PlotHist(LONG1_FLAV1_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_Long1_Flav1", "dxyErr");
    PlotHist(IRON2_FLAV1_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_IRON2_Flav1", "dxyErr");
    PlotHist(LONG2_FLAV1_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_Long2_Flav1", "dxyErr");
    PlotHist(FAKE_FLAV1_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_Fake_Flav1", "dxyErr");

    PlotHist(IRON1_FLAV5_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_IRON1_Flav5", "dxyErr");
    PlotHist(LONG1_FLAV5_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_Long1_Flav5", "dxyErr");
    PlotHist(IRON2_FLAV5_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_IRON2_Flav5", "dxyErr");
    PlotHist(LONG2_FLAV5_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_Long2_Flav5", "dxyErr");
    PlotHist(FAKE_FLAV5_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_Fake_Flav5", "dxyErr");


    //dz
    PlotHist(IRON1_FLAV0_dz, sample, plot_dir, nam, "DZ", "dz_IRON1_Flav0", "dz");
    PlotHist(IRON2_FLAV0_dz, sample, plot_dir, nam, "DZ", "dz_IRON2_Flav0", "dz");
    PlotHist(LONG1_FLAV0_dz, sample, plot_dir, nam, "DZ", "dz_Long1_Flav0", "dz");
    PlotHist(LONG2_FLAV0_dz, sample, plot_dir, nam, "DZ", "dz_Long2_Flav0", "dz");
    PlotHist(FAKE_FLAV0_dz, sample, plot_dir, nam, "DZ", "dz_Fake_Flav0", "dz");

    PlotHist(IRON1_FLAV1_dz, sample, plot_dir, nam, "DZ", "dz_IRON1_Flav1", "dz");
    PlotHist(LONG1_FLAV1_dz, sample, plot_dir, nam, "DZ", "dz_Long1_Flav1", "dz");
    PlotHist(IRON2_FLAV1_dz, sample, plot_dir, nam, "DZ", "dz_IRON2_Flav1", "dz");
    PlotHist(LONG2_FLAV1_dz, sample, plot_dir, nam, "DZ", "dz_Long2_Flav1", "dz");
    PlotHist(FAKE_FLAV1_dz, sample, plot_dir, nam, "DZ", "dz_Fake_Flav1", "dz");

    PlotHist(IRON1_FLAV5_dz, sample, plot_dir, nam, "DZ", "dz_IRON1_Flav5", "dz");
    PlotHist(LONG1_FLAV5_dz, sample, plot_dir, nam, "DZ", "dz_Long1_Flav5", "dz");
    PlotHist(IRON2_FLAV5_dz, sample, plot_dir, nam, "DZ", "dz_IRON2_Flav5", "dz");
    PlotHist(LONG2_FLAV5_dz, sample, plot_dir, nam, "DZ", "dz_Long2_Flav5", "dz");
    PlotHist(FAKE_FLAV5_dz, sample, plot_dir, nam, "DZ", "dz_Fake_Flav5", "dz");

    //dzERr
    PlotHist(IRON1_FLAV0_dzErr, sample, plot_dir, nam, "DZ", "dzErr_IRON1_Flav0", "dzErr");
    PlotHist(IRON2_FLAV0_dzErr, sample, plot_dir, nam, "DZ", "dzErr_IRON2_Flav0", "dzErr");
    PlotHist(LONG1_FLAV0_dzErr, sample, plot_dir, nam, "DZ", "dzErr_Long1_Flav0", "dzErr");
    PlotHist(LONG2_FLAV0_dzErr, sample, plot_dir, nam, "DZ", "dzErr_Long2_Flav0", "dzErr");
    PlotHist(FAKE_FLAV0_dzErr, sample, plot_dir, nam, "DZ", "dzErr_Fake_Flav0", "dzErr");

    PlotHist(IRON1_FLAV1_dzErr, sample, plot_dir, nam, "DZ", "dzErr_IRON1_Flav1", "dzErr");
    PlotHist(LONG1_FLAV1_dzErr, sample, plot_dir, nam, "DZ", "dzErr_Long1_Flav1", "dzErr");
    PlotHist(IRON2_FLAV1_dzErr, sample, plot_dir, nam, "DZ", "dzErr_IRON2_Flav1", "dzErr");
    PlotHist(LONG2_FLAV1_dzErr, sample, plot_dir, nam, "DZ", "dzErr_Long2_Flav1", "dzErr");
    PlotHist(FAKE_FLAV1_dzErr, sample, plot_dir, nam, "DZ", "dzErr_Fake_Flav1", "dzErr");

    PlotHist(IRON1_FLAV5_dzErr, sample, plot_dir, nam, "DZ", "dzErr_IRON1_Flav5", "dzErr");
    PlotHist(LONG1_FLAV5_dzErr, sample, plot_dir, nam, "DZ", "dzErr_Long1_Flav5", "dzErr");
    PlotHist(IRON2_FLAV5_dzErr, sample, plot_dir, nam, "DZ", "dzErr_IRON2_Flav5", "dzErr");
    PlotHist(LONG2_FLAV5_dzErr, sample, plot_dir, nam, "DZ", "dzErr_Long2_Flav5", "dzErr");
    PlotHist(FAKE_FLAV5_dzErr, sample, plot_dir, nam, "DZ", "dzErr_Fake_Flav5", "dzErr");

    //CONV
    PlotHist(IRON1_FLAV0_CONV, sample, plot_dir, nam, "CONV", "CONV_IRON1_Flav0", "CONV");
    PlotHist(IRON2_FLAV0_CONV, sample, plot_dir, nam, "CONV", "CONV_IRON2_Flav0", "CONV");
    PlotHist(LONG1_FLAV0_CONV, sample, plot_dir, nam, "CONV", "CONV_Long1_Flav0", "CONV");
    PlotHist(LONG2_FLAV0_CONV, sample, plot_dir, nam, "CONV", "CONV_Long2_Flav0", "CONV");
    PlotHist(FAKE_FLAV0_CONV, sample, plot_dir, nam, "CONV", "CONV_Fake_Flav0", "CONV");

    PlotHist(IRON1_FLAV1_CONV, sample, plot_dir, nam, "CONV", "CONV_IRON1_Flav1", "CONV");
    PlotHist(LONG1_FLAV1_CONV, sample, plot_dir, nam, "CONV", "CONV_Long1_Flav1", "CONV");
    PlotHist(IRON2_FLAV1_CONV, sample, plot_dir, nam, "CONV", "CONV_IRON2_Flav1", "CONV");
    PlotHist(LONG2_FLAV1_CONV, sample, plot_dir, nam, "CONV", "CONV_Long2_Flav1", "CONV");
    PlotHist(FAKE_FLAV1_CONV, sample, plot_dir, nam, "CONV", "CONV_Fake_Flav1", "CONV");

    PlotHist(IRON1_FLAV5_CONV, sample, plot_dir, nam, "CONV", "CONV_IRON1_Flav5", "CONV");
    PlotHist(LONG1_FLAV5_CONV, sample, plot_dir, nam, "CONV", "CONV_Long1_Flav5", "CONV");
    PlotHist(IRON2_FLAV5_CONV, sample, plot_dir, nam, "CONV", "CONV_IRON2_Flav5", "CONV");
    PlotHist(LONG2_FLAV5_CONV, sample, plot_dir, nam, "CONV", "CONV_Long2_Flav5", "CONV");
    PlotHist(FAKE_FLAV5_CONV, sample, plot_dir, nam, "CONV", "CONV_Fake_Flav5", "CONV");


    //ISO
    PlotHist(IRON1_FLAV0_ISO, sample, plot_dir, nam, "ISO", "ISO_IRON1_Flav0", "ISO");
    PlotHist(IRON2_FLAV0_ISO, sample, plot_dir, nam, "ISO", "ISO_IRON2_Flav0", "ISO");
    PlotHist(LONG1_FLAV0_ISO, sample, plot_dir, nam, "ISO", "ISO_Long1_Flav0", "ISO");
    PlotHist(LONG2_FLAV0_ISO, sample, plot_dir, nam, "ISO", "ISO_Long2_Flav0", "ISO");
    PlotHist(FAKE_FLAV0_ISO, sample, plot_dir, nam, "ISO", "ISO_Fake_Flav0", "ISO");

    PlotHist(IRON1_FLAV1_ISO, sample, plot_dir, nam, "ISO", "ISO_IRON1_Flav1", "ISO");
    PlotHist(LONG1_FLAV1_ISO, sample, plot_dir, nam, "ISO", "ISO_Long1_Flav1", "ISO");
    PlotHist(IRON2_FLAV1_ISO, sample, plot_dir, nam, "ISO", "ISO_IRON2_Flav1", "ISO");
    PlotHist(LONG2_FLAV1_ISO, sample, plot_dir, nam, "ISO", "ISO_Long2_Flav1", "ISO");
    PlotHist(FAKE_FLAV1_ISO, sample, plot_dir, nam, "ISO", "ISO_Fake_Flav1", "ISO");

    PlotHist(IRON1_FLAV5_ISO, sample, plot_dir, nam, "ISO", "ISO_IRON1_Flav5", "ISO");
    PlotHist(LONG1_FLAV5_ISO, sample, plot_dir, nam, "ISO", "ISO_Long1_Flav5", "ISO");
    PlotHist(IRON2_FLAV5_ISO, sample, plot_dir, nam, "ISO", "ISO_IRON2_Flav5", "ISO");
    PlotHist(LONG2_FLAV5_ISO, sample, plot_dir, nam, "ISO", "ISO_Long2_Flav5", "ISO");
    PlotHist(FAKE_FLAV5_ISO, sample, plot_dir, nam, "ISO", "ISO_Fake_Flav5", "ISO");

    //IP Sig 1
    PlotHist(IRON1_FLAV0_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_IRON1_Flav0", "IPSig1");
    PlotHist(IRON2_FLAV0_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_IRON2_Flav0", "IPSig1");
    PlotHist(LONG1_FLAV0_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_Long1_Flav0", "IPSig1");
    PlotHist(LONG2_FLAV0_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_Long2_Flav0", "IPSig1");
    PlotHist(FAKE_FLAV0_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_Fake_Flav0", "IPSig1");

    PlotHist(IRON1_FLAV1_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_IRON1_Flav1", "IPSig1");
    PlotHist(LONG1_FLAV1_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_Long1_Flav1", "IPSig1");
    PlotHist(IRON2_FLAV1_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_IRON2_Flav1", "IPSig1");
    PlotHist(LONG2_FLAV1_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_Long2_Flav1", "IPSig1");
    PlotHist(FAKE_FLAV1_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_Fake_Flav1", "IPSig1");

    PlotHist(IRON1_FLAV5_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_IRON1_Flav5", "IPSig1");
    PlotHist(LONG1_FLAV5_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_Long1_Flav5", "IPSig1");
    PlotHist(IRON2_FLAV5_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_IRON2_Flav5", "IPSig1");
    PlotHist(LONG2_FLAV5_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_Long2_Flav5", "IPSig1");
    PlotHist(FAKE_FLAV5_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_Fake_Flav5", "IPSig1");

    //IP Sig 2
    PlotHist(IRON1_FLAV0_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_IRON1_Flav0", "IPSig2");
    PlotHist(IRON2_FLAV0_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_IRON2_Flav0", "IPSig2");
    PlotHist(LONG1_FLAV0_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_Long1_Flav0", "IPSig2");
    PlotHist(LONG2_FLAV0_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_Long2_Flav0", "IPSig2");
    PlotHist(FAKE_FLAV0_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_Fake_Flav0", "IPSig2");

    PlotHist(IRON1_FLAV1_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_IRON1_Flav1", "IPSig2");
    PlotHist(LONG1_FLAV1_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_Long1_Flav1", "IPSig2");
    PlotHist(IRON2_FLAV1_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_IRON2_Flav1", "IPSig2");
    PlotHist(LONG2_FLAV1_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_Long2_Flav1", "IPSig2");
    PlotHist(FAKE_FLAV1_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_Fake_Flav1", "IPSig2");

    PlotHist(IRON1_FLAV5_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_IRON1_Flav5", "IPSig2");
    PlotHist(LONG1_FLAV5_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_Long1_Flav5", "IPSig2");
    PlotHist(IRON2_FLAV5_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_IRON2_Flav5", "IPSig2");
    PlotHist(LONG2_FLAV5_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_Long2_Flav5", "IPSig2");
    PlotHist(FAKE_FLAV5_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_Fake_Flav5", "IPSig2");


    //dxy Sig
    PlotHist(IRON1_FLAV0_dxySig, sample, plot_dir, nam, "DXY", "dxySig_IRON1_Flav0", "dxySig");
    PlotHist(IRON2_FLAV0_dxySig, sample, plot_dir, nam, "DXY", "dxySig_IRON2_Flav0", "dxySig");
    PlotHist(LONG1_FLAV0_dxySig, sample, plot_dir, nam, "DXY", "dxySig_Long1_Flav0", "dxySig");
    PlotHist(LONG2_FLAV0_dxySig, sample, plot_dir, nam, "DXY", "dxySig_Long2_Flav0", "dxySig");
    PlotHist(FAKE_FLAV0_dxySig, sample, plot_dir, nam, "DXY", "dxySig_Fake_Flav0", "dxySig");

    PlotHist(IRON1_FLAV1_dxySig, sample, plot_dir, nam, "DXY", "dxySig_IRON1_Flav1", "dxySig");
    PlotHist(LONG1_FLAV1_dxySig, sample, plot_dir, nam, "DXY", "dxySig_Long1_Flav1", "dxySig");
    PlotHist(IRON2_FLAV1_dxySig, sample, plot_dir, nam, "DXY", "dxySig_IRON2_Flav1", "dxySig");
    PlotHist(LONG2_FLAV1_dxySig, sample, plot_dir, nam, "DXY", "dxySig_Long2_Flav1", "dxySig");
    PlotHist(FAKE_FLAV1_dxySig, sample, plot_dir, nam, "DXY", "dxySig_Fake_Flav1", "dxySig");

    PlotHist(IRON1_FLAV5_dxySig, sample, plot_dir, nam, "DXY", "dxySig_IRON1_Flav5", "dxySig");
    PlotHist(LONG1_FLAV5_dxySig, sample, plot_dir, nam, "DXY", "dxySig_Long1_Flav5", "dxySig");
    PlotHist(IRON2_FLAV5_dxySig, sample, plot_dir, nam, "DXY", "dxySig_IRON2_Flav5", "dxySig");
    PlotHist(LONG2_FLAV5_dxySig, sample, plot_dir, nam, "DXY", "dxySig_Long2_Flav5", "dxySig");
    PlotHist(FAKE_FLAV5_dxySig, sample, plot_dir, nam, "DXY", "dxySig_Fake_Flav5", "dxySig");


    //dz Sig
    PlotHist(IRON1_FLAV0_dzSig, sample, plot_dir, nam, "DZ", "dzSig_IRON1_Flav0", "dzSig");
    PlotHist(IRON2_FLAV0_dzSig, sample, plot_dir, nam, "DZ", "dzSig_IRON2_Flav0", "dzSig");
    PlotHist(LONG1_FLAV0_dzSig, sample, plot_dir, nam, "DZ", "dzSig_Long1_Flav0", "dzSig");
    PlotHist(LONG2_FLAV0_dzSig, sample, plot_dir, nam, "DZ", "dzSig_Long2_Flav0", "dzSig");
    PlotHist(FAKE_FLAV0_dzSig, sample, plot_dir, nam, "DZ", "dzSig_Fake_Flav0", "dzSig");

    PlotHist(IRON1_FLAV1_dzSig, sample, plot_dir, nam, "DZ", "dzSig_IRON1_Flav1", "dzSig");
    PlotHist(LONG1_FLAV1_dzSig, sample, plot_dir, nam, "DZ", "dzSig_Long1_Flav1", "dzSig");
    PlotHist(IRON2_FLAV1_dzSig, sample, plot_dir, nam, "DZ", "dzSig_IRON2_Flav1", "dzSig");
    PlotHist(LONG2_FLAV1_dzSig, sample, plot_dir, nam, "DZ", "dzSig_Long2_Flav1", "dzSig");
    PlotHist(FAKE_FLAV1_dzSig, sample, plot_dir, nam, "DZ", "dzSig_Fake_Flav1", "dzSig");

    PlotHist(IRON1_FLAV5_dzSig, sample, plot_dir, nam, "DZ", "dzSig_IRON1_Flav5", "dzSig");
    PlotHist(LONG1_FLAV5_dzSig, sample, plot_dir, nam, "DZ", "dzSig_Long1_Flav5", "dzSig");
    PlotHist(IRON2_FLAV5_dzSig, sample, plot_dir, nam, "DZ", "dzSig_IRON2_Flav5", "dzSig");
    PlotHist(LONG2_FLAV5_dzSig, sample, plot_dir, nam, "DZ", "dzSig_Long2_Flav5", "dzSig");
    PlotHist(FAKE_FLAV5_dzSig, sample, plot_dir, nam, "DZ", "dzSig_Fake_Flav5", "dzSig");

    //IP

    PlotHist(IRON1_FLAV0_IP, sample, plot_dir, nam, "IP", "IP_IRON1_Flav0", "IP");
    PlotHist(IRON2_FLAV0_IP, sample, plot_dir, nam, "IP", "IP_IRON2_Flav0", "IP");
    PlotHist(LONG1_FLAV0_IP, sample, plot_dir, nam, "IP", "IP_Long1_Flav0", "IP");
    PlotHist(LONG2_FLAV0_IP, sample, plot_dir, nam, "IP", "IP_Long2_Flav0", "IP");
    PlotHist(FAKE_FLAV0_IP, sample, plot_dir, nam, "IP", "IP_Fake_Flav0", "IP");

    PlotHist(IRON1_FLAV1_IP, sample, plot_dir, nam, "IP", "IP_IRON1_Flav1", "IP");
    PlotHist(LONG1_FLAV1_IP, sample, plot_dir, nam, "IP", "IP_Long1_Flav1", "IP");
    PlotHist(IRON2_FLAV1_IP, sample, plot_dir, nam, "IP", "IP_IRON2_Flav1", "IP");
    PlotHist(LONG2_FLAV1_IP, sample, plot_dir, nam, "IP", "IP_Long2_Flav1", "IP");
    PlotHist(FAKE_FLAV1_IP, sample, plot_dir, nam, "IP", "IP_Fake_Flav1", "IP");

    PlotHist(IRON1_FLAV5_IP, sample, plot_dir, nam, "IP", "IP_IRON1_Flav5", "IP");
    PlotHist(LONG1_FLAV5_IP, sample, plot_dir, nam, "IP", "IP_Long1_Flav5", "IP");
    PlotHist(IRON2_FLAV5_IP, sample, plot_dir, nam, "IP", "IP_IRON2_Flav5", "IP");
    PlotHist(LONG2_FLAV5_IP, sample, plot_dir, nam, "IP", "IP_Long2_Flav5", "IP");
    PlotHist(FAKE_FLAV5_IP, sample, plot_dir, nam, "IP", "IP_Fake_Flav5", "IP");



    //NO FLAV IRONS///////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    //EMID
    //EMID
    PlotHist(IRON1_EMID, sample, plot_dir, nam, "EMID", "EMID_IRON1", "EMID");
    PlotHist(IRON2_EMID, sample, plot_dir, nam, "EMID", "EMID_IRON2", "EMID");
    PlotHist(Long1_EMID, sample, plot_dir, nam, "EMID", "EMID_Long1", "EMID");
    PlotHist(Long2_EMID, sample, plot_dir, nam, "EMID", "EMID_Long2", "EMID");
    PlotHist(FAKE_EMID, sample, plot_dir, nam, "EMID", "EMID_Fake", "EMID");



    //pt
    PlotHist(IRON1_pt, sample, plot_dir, nam, "PT", "pt_IRON1", "pt");
    PlotHist(IRON2_pt, sample, plot_dir, nam, "PT", "pt_IRON2", "pt");
    PlotHist(Long1_pt, sample, plot_dir, nam, "PT", "pt_Long1", "pt");
    PlotHist(Long2_pt, sample, plot_dir, nam, "PT", "pt_Long2", "pt");
    PlotHist(FAKE_pt, sample, plot_dir, nam, "PT", "pt_Fake", "pt");


    //eta
    PlotHist(IRON1_eta, sample, plot_dir, nam, "ETA", "eta_IRON1", "eta");
    PlotHist(IRON2_eta, sample, plot_dir, nam, "ETA", "eta_IRON2", "eta");
    PlotHist(Long1_eta, sample, plot_dir, nam, "ETA", "eta_Long1", "eta");
    PlotHist(Long2_eta, sample, plot_dir, nam, "ETA", "eta_Long2", "eta");
    PlotHist(FAKE_eta, sample, plot_dir, nam, "ETA", "eta_Fake", "eta");






    //dxy
    PlotHist(IRON1_dxy, sample, plot_dir, nam, "DXY", "dxy_IRON1", "dxy");
    PlotHist(IRON2_dxy, sample, plot_dir, nam, "DXY", "dxy_IRON2", "dxy");
    PlotHist(Long1_dxy, sample, plot_dir, nam, "DXY", "dxy_Long1", "dxy");
    PlotHist(Long2_dxy, sample, plot_dir, nam, "DXY", "dxy_Long2", "dxy");
    PlotHist(FAKE_dxy, sample, plot_dir, nam, "DXY", "dxy_Fake", "dxy");




    //dxyErr
    PlotHist(IRON1_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_IRON1", "dxyErr");
    PlotHist(IRON2_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_IRON2", "dxyErr");
    PlotHist(Long1_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_Long1", "dxyErr");
    PlotHist(Long2_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_Long2", "dxyErr");
    PlotHist(FAKE_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_Fake", "dxyErr");




    //dz
    PlotHist(IRON1_dz, sample, plot_dir, nam, "DZ", "dz_IRON1", "dz");
    PlotHist(IRON2_dz, sample, plot_dir, nam, "DZ", "dz_IRON2", "dz");
    PlotHist(Long1_dz, sample, plot_dir, nam, "DZ", "dz_Long1", "dz");
    PlotHist(Long2_dz, sample, plot_dir, nam, "DZ", "dz_Long2", "dz");
    PlotHist(FAKE_dz, sample, plot_dir, nam, "DZ", "dz_Fake", "dz");



    //dzERr
    PlotHist(IRON1_dzErr, sample, plot_dir, nam, "DZ", "dzErr_IRON1", "dzErr");
    PlotHist(IRON2_dzErr, sample, plot_dir, nam, "DZ", "dzErr_IRON2", "dzErr");
    PlotHist(Long1_dzErr, sample, plot_dir, nam, "DZ", "dzErr_Long1", "dzErr");
    PlotHist(Long2_dzErr, sample, plot_dir, nam, "DZ", "dzErr_Long2", "dzErr");
    PlotHist(FAKE_dzErr, sample, plot_dir, nam, "DZ", "dzErr_Fake", "dzErr");



    //CONV
    PlotHist(IRON1_CONV, sample, plot_dir, nam, "CONV", "CONV_IRON1", "CONV");
    PlotHist(IRON2_CONV, sample, plot_dir, nam, "CONV", "CONV_IRON2", "CONV");
    PlotHist(Long1_CONV, sample, plot_dir, nam, "CONV", "CONV_Long1", "CONV");
    PlotHist(Long2_CONV, sample, plot_dir, nam, "CONV", "CONV_Long2", "CONV");
    PlotHist(FAKE_CONV, sample, plot_dir, nam, "CONV", "CONV_Fake", "CONV");



    //ISO
    PlotHist(IRON1_ISO, sample, plot_dir, nam, "ISO", "ISO_IRON1", "ISO");
    PlotHist(IRON2_ISO, sample, plot_dir, nam, "ISO", "ISO_IRON2", "ISO");
    PlotHist(Long1_ISO, sample, plot_dir, nam, "ISO", "ISO_Long1", "ISO");
    PlotHist(Long2_ISO, sample, plot_dir, nam, "ISO", "ISO_Long2", "ISO");
    PlotHist(FAKE_ISO, sample, plot_dir, nam, "ISO", "ISO_Fake", "ISO");



    //IP Sig 1
    PlotHist(IRON1_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_IRON1", "IPSig1");
    PlotHist(IRON2_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_IRON2", "IPSig1");
    PlotHist(Long1_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_Long1", "IPSig1");
    PlotHist(Long2_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_Long2", "IPSig1");
    PlotHist(FAKE_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_Fake", "IPSig1");



    //IP Sig 2
    PlotHist(IRON1_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_IRON1", "IPSig2");
    PlotHist(IRON2_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_IRON2", "IPSig2");
    PlotHist(Long1_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_Long1", "IPSig2");
    PlotHist(Long2_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_Long2", "IPSig2");
    PlotHist(FAKE_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_Fake", "IPSig2");



    //dxy Sig
    PlotHist(IRON1_dxySig, sample, plot_dir, nam, "DXY", "dxySig_IRON1", "dxySig");
    PlotHist(IRON2_dxySig, sample, plot_dir, nam, "DXY", "dxySig_IRON2", "dxySig");
    PlotHist(Long1_dxySig, sample, plot_dir, nam, "DXY", "dxySig_Long1", "dxySig");
    PlotHist(Long2_dxySig, sample, plot_dir, nam, "DXY", "dxySig_Long2", "dxySig");
    PlotHist(FAKE_dxySig, sample, plot_dir, nam, "DXY", "dxySig_Fake", "dxySig");




    //dz Sig
    PlotHist(IRON1_dzSig, sample, plot_dir, nam, "DZ", "dzSig_IRON1", "dzSig");
    PlotHist(IRON2_dzSig, sample, plot_dir, nam, "DZ", "dzSig_IRON2", "dzSig");
    PlotHist(Long1_dzSig, sample, plot_dir, nam, "DZ", "dzSig_Long1", "dzSig");
    PlotHist(Long2_dzSig, sample, plot_dir, nam, "DZ", "dzSig_Long2", "dzSig");
    PlotHist(FAKE_dzSig, sample, plot_dir, nam, "DZ", "dzSig_Fake", "dzSig");


    //IP

    PlotHist(IRON1_IP, sample, plot_dir, nam, "IP", "IP_IRON1", "IP");
    PlotHist(IRON2_IP, sample, plot_dir, nam, "IP", "IP_IRON2", "IP");
    PlotHist(Long1_IP, sample, plot_dir, nam, "IP", "IP_Long1", "IP");
    PlotHist(Long2_IP, sample, plot_dir, nam, "IP", "IP_Long2", "IP");
    PlotHist(FAKE_IP, sample, plot_dir, nam, "IP", "IP_Fake", "IP");



    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////
    //EMID
    PlotHist(Flav0_EMID, sample, plot_dir, nam, "EMID", "EMID_FLAV0", "EMID");
    PlotHist(Flav1_EMID, sample, plot_dir, nam, "EMID", "EMID_FLAV1", "EMID");
    PlotHist(Flav5_EMID, sample, plot_dir, nam, "EMID", "EMID_FLAV5", "EMID");
    //PlotHist(Long2_EMID,      sample, plot_dir,"NoCut","EMID", "EMID_Long2","EMID");
    PlotHist(EMID, sample, plot_dir, nam, "EMID", "EMID", "EMID");



    //pt
    PlotHist(Flav0_pt, sample, plot_dir, nam, "PT", "pt_FLAV0", "pt");
    PlotHist(Flav1_pt, sample, plot_dir, nam, "PT", "pt_FLAV1", "pt");
    PlotHist(Flav5_pt, sample, plot_dir, nam, "PT", "pt_FLAV5", "pt");
    //PlotHist(Long2_pt,      sample, plot_dir,"NoCut","PT", "pt_Long2","pt");
    PlotHist(Pt, sample, plot_dir, nam, "PT", "pt", "pt");


    //eta
    PlotHist(Flav0_eta, sample, plot_dir, nam, "ETA", "eta_FLAV0", "eta");
    PlotHist(Flav1_eta, sample, plot_dir, nam, "ETA", "eta_FLAV1", "eta");
    PlotHist(Flav5_eta, sample, plot_dir, nam, "ETA", "eta_FLAV5", "eta");
    //PlotHist(Long2_eta,      sample, plot_dir,"NoCut","ETA", "eta_Long2","eta");
    PlotHist(Eta, sample, plot_dir, nam, "ETA", "eta", "eta");






    //dxy
    PlotHist(Flav0_dxy, sample, plot_dir, nam, "DXY", "dxy_FLAV0", "dxy");
    PlotHist(Flav1_dxy, sample, plot_dir, nam, "DXY", "dxy_FLAV1", "dxy");
    PlotHist(Flav5_dxy, sample, plot_dir, nam, "DXY", "dxy_FLAV5", "dxy");
    //PlotHist(Long2_dxy,      sample, plot_dir,"NoCut","DXY", "dxy_Long2","dxy");
    PlotHist(Dxy, sample, plot_dir, nam, "DXY", "dxy", "dxy");




    //dxyErr
    PlotHist(Flav0_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_FLAV0", "dxyErr");
    PlotHist(Flav1_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_FLAV1", "dxyErr");
    PlotHist(Flav5_dxyErr, sample, plot_dir, nam, "DXY", "dxyErr_FLAV5", "dxyErr");
    //PlotHist(Long2_dxyErr,      sample, plot_dir,"NoCut","DXY", "dxyErr_Long2","dxyErr");
    PlotHist(DxyErr, sample, plot_dir, nam, "DXY", "dxyErr", "dxyErr");




    //dz
    PlotHist(Flav0_dz, sample, plot_dir, nam, "DZ", "dz_FLAV0", "dz");
    PlotHist(Flav1_dz, sample, plot_dir, nam, "DZ", "dz_FLAV1", "dz");
    PlotHist(Flav5_dz, sample, plot_dir, nam, "DZ", "dz_FLAV5", "dz");
    //PlotHist(Long2_dz,      sample, plot_dir,"NoCut","DZ", "dz_Long2","dz");
    PlotHist(Dz, sample, plot_dir, nam, "DZ", "dz", "dz");



    //dzERr
    PlotHist(Flav0_dzErr, sample, plot_dir, nam, "DZ", "dzErr_FLAV0", "dzErr");
    PlotHist(Flav1_dzErr, sample, plot_dir, nam, "DZ", "dzErr_FLAV1", "dzErr");
    PlotHist(Flav5_dzErr, sample, plot_dir, nam, "DZ", "dzErr_FLAV5", "dzErr");
    //PlotHist(Long2_dzErr,      sample, plot_dir,"NoCut","DZ", "dzErr_Long2","dzErr");
    PlotHist(DzErr, sample, plot_dir, nam, "DZ", "dzErr", "dzErr");



    //CONV
    PlotHist(Flav0_CONV, sample, plot_dir, nam, "CONV", "CONV_FLAV0", "CONV");
    PlotHist(Flav1_CONV, sample, plot_dir, nam, "CONV", "CONV_FLAV1", "CONV");
    PlotHist(Flav5_CONV, sample, plot_dir, nam, "CONV", "CONV_FLAV5", "CONV");
    //PlotHist(Long2_CONV,      sample, plot_dir,"NoCut","CONV", "CONV_Long2","CONV");
    PlotHist(CONV, sample, plot_dir, nam, "CONV", "CONV", "CONV");



    //ISO
    PlotHist(Flav0_ISO, sample, plot_dir, nam, "ISO", "ISO_FLAV0", "ISO");
    PlotHist(Flav1_ISO, sample, plot_dir, nam, "ISO", "ISO_FLAV1", "ISO");
    PlotHist(Flav5_ISO, sample, plot_dir, nam, "ISO", "ISO_FLAV5", "ISO");
    //PlotHist(Long2_ISO,      sample, plot_dir,"NoCut","ISO", "ISO_Long2","ISO");
    PlotHist(ISO, sample, plot_dir, nam, "ISO", "ISO", "ISO");



    //IP Sig 1
    PlotHist(Flav0_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_FLAV0", "IPSig1");
    PlotHist(Flav1_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_FLAV1", "IPSig1");
    PlotHist(Flav5_IPSig1, sample, plot_dir, nam, "IP", "IPSig1_FLAV5", "IPSig1");
    //PlotHist(Long2_IPSig1,      sample, plot_dir,"NoCut","IP", "IPSig1_Long2","IPSig1");
    PlotHist(IPSig1_graph, sample, plot_dir, nam, "IP", "IPSig1", "IPSig1");



    //IP Sig 2
    PlotHist(Flav0_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_FLAV0", "IPSig2");
    PlotHist(Flav1_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_FLAV1", "IPSig2");
    PlotHist(Flav5_IPSig2, sample, plot_dir, nam, "IP", "IPSig2_FLAV5", "IPSig2");
    //PlotHist(Long2_IPSig2,      sample, plot_dir,"NoCut","IP", "IPSig2_Long2","IPSig2");
    PlotHist(IPSig2_graph, sample, plot_dir, nam, "IP", "IPSig2", "IPSig2");



    //dxy Sig
    PlotHist(Flav0_dxySig, sample, plot_dir, nam, "DXY", "dxySig_FLAV0", "dxySig");
    PlotHist(Flav1_dxySig, sample, plot_dir, nam, "DXY", "dxySig_FLAV1", "dxySig");
    PlotHist(Flav5_dxySig, sample, plot_dir, nam, "DXY", "dxySig_FLAV5", "dxySig");
    //PlotHist(Long2_dxySig,      sample, plot_dir,"NoCut","DXY", "dxySig_Long2","dxySig");
    PlotHist(DxySig, sample, plot_dir, nam, "DXY", "dxySig", "dxySig");




    //dz Sig
    PlotHist(Flav0_dzSig, sample, plot_dir, nam, "DZ", "dzSig_FLAV0", "dzSig");
    PlotHist(Flav1_dzSig, sample, plot_dir, nam, "DZ", "dzSig_FLAV1", "dzSig");
    PlotHist(Flav5_dzSig, sample, plot_dir, nam, "DZ", "dzSig_FLAV5", "dzSig");
    //PlotHist(Long2_dzSig,      sample, plot_dir,"NoCut","DZ", "dzSig_Long2","dzSig");
    PlotHist(DzSig, sample, plot_dir, nam, "DZ", "dzSig", "dzSig");


    //IP

    PlotHist(Flav0_IP, sample, plot_dir, nam, "IP", "IP_FLAV0", "IP");
    PlotHist(Flav1_IP, sample, plot_dir, nam, "IP", "IP_FLAV1", "IP");
    PlotHist(Flav5_IP, sample, plot_dir, nam, "IP", "IP_FLAV5", "IP");
    //PlotHist(Long2_IP,      sample, plot_dir,"NoCut","IP", "IP_Long2","IP");
    PlotHist(IP_graph, sample, plot_dir, nam, "IP", "IP_", "IP");
    
    



}





void TTJETS::RunAll()
{
    Graph("Final", 1.0,2.0, "1_2");
    Graph("Final", 2.0, 3.0, "2_3");
    Graph("Final", 3.0, 4.0, "3_4");
    Graph("Final", 4.0, 5.0, "4_5");
    Graph("Final", 5.0, 6.0, "5_6");
    Graph("Final", 6.0, 7.0, "6_7");
    Graph("Final", 7.0, 8.0, "7_8");
    Graph("Final", 8.0, 9.0, "8_9");
    Graph("Final", 9.0, 10.0, "9_10");
    Graph("Final", 10.0, 11.0, "10_11");
    Graph("Final", 11.0, 12.0, "11_12");
    Graph("Final", 12.0, 13.0, "12_3");
    Graph("Final", 13.0, 14.0, "13_14");
    Graph("Final", 14.0, 15.0, "14_15");
    Graph("Final", 15.0, 16.0, "15_16");
    Graph("Final", 16.0, 17.0, "16_17");
    Graph("Final", 17.0, 18.0, "17_18");
    Graph("Final", 18.0, 19.0, "18_19");
    Graph("Final", 19.0, 20.0, "19_20");
    Graph("Final", 1.0, 5.0, "LowCut");
    Graph("Final", 5.0, 10.0, "MidCut");
    Graph("Final", 10.0, 20.0, "HighCut");
    Graph("Final", 1.0, 20.0, "General");
    


}



void TTJETS::Loop2()
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

    std::string plot_dir = "/eos/user/s/ssakhare/ROCPlots";
    std::string sample = "TTJETS";
    printf("Running over %s\n", sample.c_str());

    Long64_t nentries = fChain->GetEntriesFast();
    Long64_t nbytes = 0, nb = 0;
    float Lower_pt = 10.0;
    float Higher_pt = 20.0;
    std::string nam ="BetterCHangeit";

    TH1F Flav0_EMID_R = TH1F("Flav0_EMID", "Flav0_EMID", 22, 4.0, 12.0);
    TH1F Flav1_EMID_R = TH1F("Flav1_EMID", "Flav1_EMID", 22, 4.0, 12.0);

    TH1F Iron1_Flav0_EMID_R = TH1F("Iron1_Flav0_EMID", "Iron1_Flav0_EMID", 22, 4.0, 12.0);
    TH1F Iron1_Flav1_EMID_R = TH1F("Iron1_Flav1_EMID", "Iron1_Flav1_EMID", 22, 4.0, 12.0);

    TH1F Iron2_Flav0_EMID_R = TH1F("Iron2_Flav0_EMID", "Iron2_Flav0_EMID", 22, 4.0, 12.0);
    TH1F Iron2_Flav1_EMID_R = TH1F("Iron2_Flav1_EMID", "Iron2_Flav1_EMID", 22, 4.0, 12.0);

    TH1F Long1_Flav0_EMID_R = TH1F("Long1_Flav0_EMID", "Long1_Flav0_EMID", 22, 4.0, 12.0);
    TH1F Long1_Flav1_EMID_R = TH1F("Long1_Flav1_EMID", "Long1_Flav1_EMID", 22, 4.0, 12.0);
    TH1F Long2_Flav0_EMID_R = TH1F("Long2_Flav0_EMID", "Long2_Flav0_EMID", 22, 4.0, 12.0);
    TH1F Long2_Flav1_EMID_R = TH1F("Long2_Flav1_EMID", "Long2_Flav1_EMID", 22, 4.0, 12.0);

    TH1F IronFake_Flav0_EMID_R = TH1F("IronFake_Flav0_EMID", "IronFake_Flav0_EMID", 22, 4.0, 12.0);
    TH1F IronFake_Flav1_EMID_R = TH1F("IronFake_Flav1_EMID", "IronFake_Flav1_EMID", 22, 4.0, 12.0);

    for (Long64_t jentry = 0; jentry < nentries; jentry++)
    {
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
                dxySig = LowPtElectron_dxy[k] / LowPtElectron_dxyErr[k];
            }


            float dzSig = -999;

            if (LowPtElectron_dzErr[k] != 0)
            {
                dzSig = LowPtElectron_dz[k] / LowPtElectron_dzErr[k];
            }


            float IPSig1 = -999; //
            if (LowPtElectron_dxyErr[k] != 0 && LowPtElectron_dzErr[k] != 0)
            {
                IPSig1 = sqrt(dxySig * dxySig + dzSig * dzSig);

            }

            float IP = -999;
            float IPErr = -999;

            if (LowPtElectron_dxyErr[k] != 0 && LowPtElectron_dzErr[k] != 0)
            {
                IP = sqrt(LowPtElectron_dxy[k] * LowPtElectron_dxy[k] + LowPtElectron_dz[k] * LowPtElectron_dz[k]);
                IPErr = sqrt(LowPtElectron_dxyErr[k] * LowPtElectron_dxyErr[k] + LowPtElectron_dzErr[k] * LowPtElectron_dzErr[k]);

            }




            float IPSig2 = -999; //
            if (LowPtElectron_dxyErr[k] != 0)
            {
                IPSig2 = abs(IP / IPErr);

            }

            float IPdiff = IPSig1 - IPSig2;





            //Starting Partameters 
            if (LowPtElectron_convVeto[k] == 1 && LowPtElectron_pt[k] >= Lower_pt && LowPtElectron_pt[k] < Higher_pt && abs(LowPtElectron_eta[k]) < 2.4 && LowPtElectron_embeddedID[k] >= 4.0)
            {

                if (LowPtElectron_genPartFlav[k] == 0)
                {
                    Flav0_EMID_R.Fill(LowPtElectron_embeddedID[k]);
                }

                if (LowPtElectron_genPartFlav[k] == 1)
                {
                    Flav1_EMID_R.Fill(LowPtElectron_embeddedID[k]);
                }


                //IRON1
                if (LowPtElectron_miniPFRelIso_all[k] < 4 && abs(LowPtElectron_dxy[k]) < 0.05 && abs(LowPtElectron_dz[k]) < 0.1 && IPSig1 < 2)
                {
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {
                        Iron1_Flav0_EMID_R.Fill(LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        Iron1_Flav1_EMID_R.Fill(LowPtElectron_embeddedID[k]);
                    }
                }

                //IRON2_PARAMETERS 
                if (LowPtElectron_miniPFRelIso_all[k] < 4 && abs(LowPtElectron_dxy[k]) < 0.05 && abs(LowPtElectron_dz[k]) < 0.1 && abs(dzSig) < 2)
                {
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {
                        Iron2_Flav0_EMID_R.Fill(LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        Iron2_Flav1_EMID_R.Fill(LowPtElectron_embeddedID[k]);
                    }
                }

                //////////////////////////////////////////////////
                //Irom Long
                if (LowPtElectron_miniPFRelIso_all[k] < 4 && IPSig1 >= 2)
                {
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {
                        Long1_Flav0_EMID_R.Fill(LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        Long1_Flav1_EMID_R.Fill(LowPtElectron_embeddedID[k]);
                    }
                }


                //Long2 para

                if (LowPtElectron_miniPFRelIso_all[k] < 4 && abs(dzSig) >= 2)
                {
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {
                        Long2_Flav0_EMID_R.Fill(LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        Long2_Flav1_EMID_R.Fill(LowPtElectron_embeddedID[k]);
                    }
                }



                if (LowPtElectron_miniPFRelIso_all[k] >= 4 && abs(LowPtElectron_dxy[k]) < 0.05 && abs(LowPtElectron_dz[k]) < 0.1 && IPSig1 < 2)
                {
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {
                        IronFake_Flav0_EMID_R.Fill(LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        IronFake_Flav1_EMID_R.Fill(LowPtElectron_embeddedID[k]);
                    }
                }


            }

        }//End of loop
    }
    PlotHist(Long2_Flav0_EMID_R, sample, plot_dir, nam, "MidCut", "Long2_Flav0_EMID", "EMID");
    PlotHist(Long2_Flav1_EMID_R, sample, plot_dir, nam, "MidCut", "Long2_Flav1_EMID", "EMID");
    ROC(Long2_Flav1_EMID_R, Long2_Flav0_EMID_R, "Long2_Flav(SignalFLav1)");

    PlotHist(Flav0_EMID_R, sample, plot_dir, nam, "MidCut", "Flav0_EMID", "EMID");
    PlotHist(Flav1_EMID_R, sample, plot_dir, nam, "MidCut", "Flav1_EMID", "EMID");
    ROC(Flav1_EMID_R, Flav0_EMID_R, "Flav(SignalFLav1)");


    PlotHist(IronFake_Flav0_EMID_R, sample, plot_dir, nam, "MidCut", "Fake_Flav0_EMID", "EMID");
    PlotHist(IronFake_Flav1_EMID_R, sample, plot_dir, nam, "MidCut", "Fake_Flav1_EMID", "EMID");
    ROC(IronFake_Flav1_EMID_R, IronFake_Flav0_EMID_R, "Fake_Flav(SignalFLav1)");

    PlotHist(Long1_Flav0_EMID_R, sample, plot_dir, nam, "MidCut", "Long1_Flav0_EMID", "EMID");
    PlotHist(Long1_Flav1_EMID_R, sample, plot_dir, nam, "MidCut", "Long1_Flav1_EMID", "EMID");
    ROC(Long1_Flav1_EMID_R, Long1_Flav0_EMID_R, "Long1_Flav(SignalFLav1)");

    PlotHist(Iron1_Flav0_EMID_R, sample, plot_dir, nam, "MidCut", "Iron1_Flav0_EMID", "EMID");
    PlotHist(Iron1_Flav1_EMID_R, sample, plot_dir, nam, "MidCut", "Iron1_Flav1_EMID", "EMID");
    ROC(Iron1_Flav1_EMID_R, Iron1_Flav0_EMID_R, "Iron1_Flav(SignalFLav1)");

    PlotHist(Iron2_Flav0_EMID_R, sample, plot_dir, nam, "MidCut", "Iron2_Iron2_Flav0_EMID", "EMID");
    PlotHist(Iron2_Flav1_EMID_R, sample, plot_dir, nam, "MidCut", "Iron2_Flav1_EMID", "EMID");
    ROC(Iron2_Flav1_EMID_R, Iron2_Flav0_EMID_R, "Iron2_Flav(SignalFLav1)");



}



void TTJETS::Random()
{

    TH2F pt_vs_FLAV1_Low = TH2F("pt_vs_FLAV1_Low", "pt_vs_FLAV1_Low", 20, 0.0, 5.0, 1, 1.0, 2.0);
    TH2F pt_vs_FLAV1_Mid = TH2F("pt_vs_FLAV1_Mid", "pt_vs_FLAV1_Mid", 20, 5.0, 10.0, 1, 1.0, 2.0);
    TH2F pt_vs_FLAV1_High = TH2F("pt_vs_FLAV1_High", "pt_vs_FLAV1_High", 40, 10.0, 20.0, 1, 1.0, 2.0);
    TH2F pt_vs_FLAV1_NoPt = TH2F("pt_vs_FLAV1_NoPt", "pt_vs_FLAV1_NoPt", 80, 0.0, 20.0, 1, 1.0, 2.0);

    TH2F pt_vs_FLAV0_Low = TH2F("pt_vs_FLAV0_Low", "pt_vs_FLAV0_Low", 20, 0.0, 5.0, 1, 0.0, 1.0);
    TH2F pt_vs_FLAV0_Mid = TH2F("pt_vs_FLAV0_Mid", "pt_vs_FLAV0_Mid", 20, 5.0, 10.0, 1, 0.0, 1.0);
    TH2F pt_vs_FLAV0_High = TH2F("pt_vs_FLAV0_High", "pt_vs_FLAV0_High", 40, 10.0, 20.0, 1, 0.0, 1.0);
    TH2F pt_vs_FLAV0_NoPt = TH2F("pt_vs_FLAV0_NoPt", "pt_vs_FLAV0_NoPt", 80, 0.0, 20.0, 1, 0.0, 1.0);

    TH2F pt_vs_EMID_FLAV0_Low = TH2F("pt_vs_EMID_FLAV0_Low", "pt_vs_EMID_FLAV0_Low", 20, 0.0, 5.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FLAV1_Low = TH2F("pt_vs_EMID_FLAV1_Low", "pt_vs_EMID_FLAV1_Low", 20, 0.0, 5.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FLAV5_Low = TH2F("pt_vs_EMID_FLAV5_Low", "pt_vs_EMID_FLAV5_Low", 20, 0.0, 5.0, 56, 4, 12.0);

    TH2F pt_vs_EMID_FLAV0_Mid = TH2F("pt_vs_EMID_FLAV0_Mid", "pt_vs_EMID_FLAV0_Mid", 20, 5.0, 10.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FLAV1_Mid = TH2F("pt_vs_EMID_FLAV1_Mid", "pt_vs_EMID_FLAV1_Mid", 20, 5.0, 10.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FLAV5_Mid = TH2F("pt_vs_EMID_FLAV5_Mid", "pt_vs_EMID_FLAV5_Mid", 20, 5.0, 10.0, 56, 4, 12.0);

    TH2F pt_vs_EMID_FLAV0_High = TH2F("pt_vs_EMID_FLAV0_High", "pt_vs_EMID_FLAV0_High", 40, 10.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FLAV1_High = TH2F("pt_vs_EMID_FLAV1_High", "pt_vs_EMID_FLAV1_High", 40, 10.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FLAV5_High = TH2F("pt_vs_EMID_FLAV5_High", "pt_vs_EMID_FLAV5_High", 40, 10.0, 20.0, 56, 4, 12.0);

    TH2F pt_vs_EMID_FLAV0_NoPt = TH2F("pt_vs_EMID_FLAV0_NoPt", "pt_vs_EMID_FLAV0_NoPt", 80, 0.0, 20.0, 88, 1.5, 12.0);
    TH2F pt_vs_EMID_FLAV1_NoPt = TH2F("pt_vs_EMID_FLAV1_NoPt", "pt_vs_EMID_FLAV1_NoPt", 80, 0.0, 20.0, 88, 1.5, 12.0);
    TH2F pt_vs_EMID_FLAV5_NoPt = TH2F("pt_vs_EMID_FLAV5_NoPt", "pt_vs_EMID_FLAV5_NoPt", 80, 0.0, 20.0, 88, 1.5, 12.0);



    ///Iron 1

    TH2F pt_vs_EMID_IRON1_FLAV0_Low = TH2F("pt_vs_EMID_IRON1_FLAV0_Low", "pt_vs_EMID_IRON1_FLAV0_Low", 20, 0.0, 5.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_IRON1_FLAV1_Low = TH2F("pt_vs_EMID_IRON1_FLAV1_Low", "pt_vs_EMID_IRON1_FLAV1_Low", 20, 0.0, 5.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_IRON1_FLAV5_Low = TH2F("pt_vs_EMID_IRON1_FLAV5_Low", "pt_vs_EMID_IRON1_FLAV5_Low", 20, 0.0, 5.0, 56, 4, 12.0);
            
    TH2F pt_vs_EMID_IRON1_FLAV0_Mid = TH2F("pt_vs_EMID_IRON1_FLAV0_Mid", "pt_vs_EMID_IRON1_FLAV0_Mid", 20, 5.0, 10.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_IRON1_FLAV1_Mid = TH2F("pt_vs_EMID_IRON1_FLAV1_Mid", "pt_vs_EMID_IRON1_FLAV1_Mid", 20, 5.0, 10.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_IRON1_FLAV5_Mid = TH2F("pt_vs_EMID_IRON1_FLAV5_Mid", "pt_vs_EMID_IRON1_FLAV5_Mid", 20, 5.0, 10.0, 56, 4, 12.0);

    TH2F pt_vs_EMID_IRON1_FLAV0_High = TH2F("pt_vs_EMID_IRON1_FLAV0_High", "pt_vs_EMID_IRON1_FLAV0_High", 40, 10.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_IRON1_FLAV1_High = TH2F("pt_vs_EMID_IRON1_FLAV1_High", "pt_vs_EMID_IRON1_FLAV1_High", 40, 10.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_IRON1_FLAV5_High = TH2F("pt_vs_EMID_IRON1_FLAV5_High", "pt_vs_EMID_IRON1_FLAV5_High", 40, 10.0, 20.0, 56, 4, 12.0);

    TH2F pt_vs_EMID_IRON1_FLAV0_NoPt = TH2F("pt_vs_EMID_IRON1_FLAV0_NoPt", "pt_vs_EMID_IRON1_FLAV0_NoPt", 80, 0.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_IRON1_FLAV1_NoPt = TH2F("pt_vs_EMID_IRON1_FLAV1_NoPt", "pt_vs_EMID_IRON1_FLAV1_NoPt", 80, 0.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_IRON1_FLAV5_NoPt = TH2F("pt_vs_EMID_IRON1_FLAV5_NoPt", "pt_vs_EMID_IRON1_FLAV5_NoPt", 80, 0.0, 20.0, 56, 4, 12.0);

    //Long1

    TH2F pt_vs_EMID_LONG1_FLAV0_Low = TH2F("pt_vs_EMID_LONG1_FLAV0_Low", "pt_vs_EMID_LONG1_FLAV0_Low", 20, 0.0, 5.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_LONG1_FLAV1_Low = TH2F("pt_vs_EMID_LONG1_FLAV1_Low", "pt_vs_EMID_LONG1_FLAV1_Low", 20, 0.0, 5.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_LONG1_FLAV5_Low = TH2F("pt_vs_EMID_LONG1_FLAV5_Low", "pt_vs_EMID_LONG1_FLAV5_Low", 20, 0.0, 5.0, 56, 4, 12.0);

    TH2F pt_vs_EMID_LONG1_FLAV0_Mid = TH2F("pt_vs_EMID_LONG1_FLAV0_Mid", "pt_vs_EMID_LONG1_FLAV0_Mid", 20, 5.0, 10.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_LONG1_FLAV1_Mid = TH2F("pt_vs_EMID_LONG1_FLAV1_Mid", "pt_vs_EMID_LONG1_FLAV1_Mid", 20, 5.0, 10.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_LONG1_FLAV5_Mid = TH2F("pt_vs_EMID_LONG1_FLAV5_Mid", "pt_vs_EMID_LONG1_FLAV5_Mid", 20, 5.0, 10.0, 56, 4, 12.0);

    TH2F pt_vs_EMID_LONG1_FLAV0_High = TH2F("pt_vs_EMID_LONG1_FLAV0_High", "pt_vs_EMID_LONG1_FLAV0_High", 40, 10.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_LONG1_FLAV1_High = TH2F("pt_vs_EMID_LONG1_FLAV1_High", "pt_vs_EMID_LONG1_FLAV1_High", 40, 10.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_LONG1_FLAV5_High = TH2F("pt_vs_EMID_LONG1_FLAV5_High", "pt_vs_EMID_LONG1_FLAV5_High", 40, 10.0, 20.0, 56, 4, 12.0);

    TH2F pt_vs_EMID_LONG1_FLAV0_NoPt = TH2F("pt_vs_EMID_LONG1_FLAV0_NoPt", "pt_vs_EMID_LONG1_FLAV0_NoPt", 80, 0.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_LONG1_FLAV1_NoPt = TH2F("pt_vs_EMID_LONG1_FLAV1_NoPt", "pt_vs_EMID_LONG1_FLAV1_NoPt", 80, 0.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_LONG1_FLAV5_NoPt = TH2F("pt_vs_EMID_LONG1_FLAV5_NoPt", "pt_vs_EMID_LONG1_FLAV5_NoPt", 80, 0.0, 20.0, 56, 4, 12.0);


    //FAKE

    TH2F pt_vs_EMID_FAKE_FLAV0_Low = TH2F("pt_vs_EMID_FAKE_FLAV0_Low", "pt_vs_EMID_FAKE_FLAV0_Low", 20, 0.0, 5.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FAKE_FLAV1_Low = TH2F("pt_vs_EMID_FAKE_FLAV1_Low", "pt_vs_EMID_FAKE_FLAV1_Low", 20, 0.0, 5.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FAKE_FLAV5_Low = TH2F("pt_vs_EMID_FAKE_FLAV5_Low", "pt_vs_EMID_FAKE_FLAV5_Low", 20, 0.0, 5.0, 56, 4, 12.0);

    TH2F pt_vs_EMID_FAKE_FLAV0_Mid = TH2F("pt_vs_EMID_FAKE_FLAV0_Mid", "pt_vs_EMID_FAKE_FLAV0_Mid", 20, 5.0, 10.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FAKE_FLAV1_Mid = TH2F("pt_vs_EMID_FAKE_FLAV1_Mid", "pt_vs_EMID_FAKE_FLAV1_Mid", 20, 5.0, 10.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FAKE_FLAV5_Mid = TH2F("pt_vs_EMID_FAKE_FLAV5_Mid", "pt_vs_EMID_FAKE_FLAV5_Mid", 20, 5.0, 10.0, 56, 4, 12.0);

    TH2F pt_vs_EMID_FAKE_FLAV0_High = TH2F("pt_vs_EMID_FAKE_FLAV0_High", "pt_vs_EMID_FAKE_FLAV0_High", 40, 10.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FAKE_FLAV1_High = TH2F("pt_vs_EMID_FAKE_FLAV1_High", "pt_vs_EMID_FAKE_FLAV1_High", 40, 10.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FAKE_FLAV5_High = TH2F("pt_vs_EMID_FAKE_FLAV5_High", "pt_vs_EMID_FAKE_FLAV5_High", 40, 10.0, 20.0, 56, 4, 12.0);

    TH2F pt_vs_EMID_FAKE_FLAV0_NoPt = TH2F("pt_vs_EMID_FAKE_FLAV0_NoPt", "pt_vs_EMID_FAKE_FLAV0_NoPt", 80, 0.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FAKE_FLAV1_NoPt = TH2F("pt_vs_EMID_FAKE_FLAV1_NoPt", "pt_vs_EMID_FAKE_FLAV1_NoPt", 80, 0.0, 20.0, 56, 4, 12.0);
    TH2F pt_vs_EMID_FAKE_FLAV5_NoPt = TH2F("pt_vs_EMID_FAKE_FLAV5_NoPt", "pt_vs_EMID_FAKE_FLAV5_NoPt", 80, 0.0, 20.0, 56, 4, 12.0);


    //Iron Without the EMID Cut

    TH2F Pt_vs_EMID_IRON1_FLAV0_Low = TH2F("Pt_vs_EMID_IRON_FLAV0_Low", "Pt_vs_EMID_IRON_FLAV0_Low", 20, 0.0, 5.0, 88, 1.5, 12.0);
    TH2F Pt_vs_EMID_IRON1_FLAV1_Low = TH2F("Pt_vs_EMID_IRON_FLAV1_Low", "Pt_vs_EMID_IRON_FLAV1_Low", 20, 0.0, 5.0, 88, 1.5, 12.0);
    TH2F Pt_vs_EMID_IRON1_FLAV5_Low = TH2F("Pt_vs_EMID_IRON_FLAV5_Low", "Pt_vs_EMID_IRON_FLAV5_Low", 20, 0.0, 5.0, 88, 1.5, 12.0);
         
    TH2F Pt_vs_EMID_IRON1_FLAV0_Mid = TH2F("Pt_vs_EMID_IRON_FLAV0_Mid", "Pt_vs_EMID_IRON_FLAV0_Mid", 20, 5.0, 10.0, 88, 1.5, 12.0);
    TH2F Pt_vs_EMID_IRON1_FLAV1_Mid = TH2F("Pt_vs_EMID_IRON_FLAV1_Mid", "Pt_vs_EMID_IRON_FLAV1_Mid", 20, 5.0, 10.0, 88, 1.5, 12.0);
    TH2F Pt_vs_EMID_IRON1_FLAV5_Mid = TH2F("Pt_vs_EMID_IRON_FLAV5_Mid", "Pt_vs_EMID_IRON_FLAV5_Mid", 20, 5.0, 10.0, 88, 1.5, 12.0);

    TH2F Pt_vs_EMID_IRON1_FLAV0_High = TH2F("Pt_vs_EMID_IRON_FLAV0_High", "Pt_vs_EMID_IRON_FLAV0_High", 40, 10.0, 20.0, 88, 1.5, 12.0);
    TH2F Pt_vs_EMID_IRON1_FLAV1_High = TH2F("Pt_vs_EMID_IRON_FLAV1_High", "Pt_vs_EMID_IRON_FLAV1_High", 40, 10.0, 20.0, 88, 1.5, 12.0);
    TH2F Pt_vs_EMID_IRON1_FLAV5_High = TH2F("Pt_vs_EMID_IRON_FLAV5_High", "Pt_vs_EMID_IRON_FLAV5_High", 40, 10.0, 20.0, 88, 1.5, 12.0);

    TH2F Pt_vs_EMID_IRON1_FLAV0_NoPt = TH2F("Pt_vs_EMID_IRON_FLAV0_NoPt", "Pt_vs_EMID_IRON_FLAV0_NoPt", 80, 0.0, 20.0, 88, 1.5, 12.0);
    TH2F Pt_vs_EMID_IRON1_FLAV1_NoPt = TH2F("Pt_vs_EMID_IRON_FLAV1_NoPt", "Pt_vs_EMID_IRON_FLAV1_NoPt", 80, 0.0, 20.0, 88, 1.5, 12.0);
    TH2F Pt_vs_EMID_IRON1_FLAV5_NoPt = TH2F("Pt_vs_EMID_IRON_FLAV5_NoPt", "Pt_vs_EMID_IRON_FLAV5_NoPt", 80, 0.0, 20.0, 88, 1.5, 12.0);
         
    gROOT->SetBatch(kTRUE);

    if (fChain == 0)
    {
        return;
    }

    std::string plot_dir = "/eos/user/s/ssakhare/";
    std::string sample = "TTJETS";
    printf("Running over %s\n", sample.c_str());
    std::string nam = "BetterCHangeit";
    Long64_t nentries = fChain->GetEntriesFast();
    Long64_t nbytes = 0, nb = 0;




    for (Long64_t jentry = 0; jentry < nentries; jentry++)
    {
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
                dxySig = LowPtElectron_dxy[k] / LowPtElectron_dxyErr[k];
            }


            float dzSig = -999;

            if (LowPtElectron_dzErr[k] != 0)
            {
                dzSig = LowPtElectron_dz[k] / LowPtElectron_dzErr[k];
            }


            float IPSig1 = -999; //
            if (LowPtElectron_dxyErr[k] != 0 && LowPtElectron_dzErr[k] != 0)
            {
                IPSig1 = sqrt(dxySig * dxySig + dzSig * dzSig);

            }

            float IP = -999;
            float IPErr = -999;

            if (LowPtElectron_dxyErr[k] != 0 && LowPtElectron_dzErr[k] != 0)
            {
                IP = sqrt(LowPtElectron_dxy[k] * LowPtElectron_dxy[k] + LowPtElectron_dz[k] * LowPtElectron_dz[k]);
                IPErr = sqrt(LowPtElectron_dxyErr[k] * LowPtElectron_dxyErr[k] + LowPtElectron_dzErr[k] * LowPtElectron_dzErr[k]);

            }




            float IPSig2 = -999; //
            if (LowPtElectron_dxyErr[k] != 0)
            {
                IPSig2 = abs(IP / IPErr);

            }

            float IPdiff = IPSig1 - IPSig2;

            //Starting Partameters 
            if (abs(LowPtElectron_eta[k]) < 2.4)
            {
                if (LowPtElectron_genPartFlav[k] == 0)
                {
                    
                    pt_vs_EMID_FLAV0_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }

                if (LowPtElectron_genPartFlav[k] == 1)
                {
                    
                    pt_vs_EMID_FLAV1_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }


                if (LowPtElectron_genPartFlav[k] == 5)
                {

                    pt_vs_EMID_FLAV5_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }
            }//IRON1
            if (LowPtElectron_convVeto[k] == 1 && abs(LowPtElectron_eta[k]) < 2.4 && LowPtElectron_embeddedID[k] >= 4 && LowPtElectron_miniPFRelIso_all[k] < 4 && abs(LowPtElectron_dxy[k]) < 0.05 && abs(LowPtElectron_dz[k]) < 0.1 && IPSig1 < 2)
            {

                //FlavCut
                if (LowPtElectron_genPartFlav[k] == 0)
                {
                    
                    pt_vs_EMID_IRON1_FLAV0_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }

                if (LowPtElectron_genPartFlav[k] == 1)
                {
                    
                    pt_vs_EMID_IRON1_FLAV1_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }


                if (LowPtElectron_genPartFlav[k] == 5)
                {

                    pt_vs_EMID_IRON1_FLAV5_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }



                if (LowPtElectron_pt[k] >= 1.0 && LowPtElectron_pt[k] < 5.0)
                {// PT CUts
                 //FlavCut
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {
                        
                        pt_vs_EMID_IRON1_FLAV0_Low.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        
                        pt_vs_EMID_IRON1_FLAV1_Low.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 5)
                    {

                        pt_vs_EMID_IRON1_FLAV5_Low.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                }

                if (LowPtElectron_pt[k] >= 5.0 && LowPtElectron_pt[k] < 10.0)
                {
                    //FlavCut
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {
                        
                        pt_vs_EMID_IRON1_FLAV0_Mid.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        
                        pt_vs_EMID_IRON1_FLAV1_Mid.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 5)
                    {

                        pt_vs_EMID_IRON1_FLAV5_Mid.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }





                }

                if (LowPtElectron_pt[k] >= 10.0 && LowPtElectron_pt[k] < 20.0)
                {
                    //FlavCut
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {
                        
                        pt_vs_EMID_IRON1_FLAV0_High.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {
                        
                        pt_vs_EMID_IRON1_FLAV1_High.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 5)
                    {

                        pt_vs_EMID_IRON1_FLAV5_High.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                }


            }



            //LONG1
            if (LowPtElectron_convVeto[k] == 1 && abs(LowPtElectron_eta[k]) < 2.4 && LowPtElectron_embeddedID[k] >= 4 && LowPtElectron_miniPFRelIso_all[k] < 4 && IPSig1 >= 2)
            {

                //FlavCut
                if (LowPtElectron_genPartFlav[k] == 0)
                {

                    pt_vs_EMID_LONG1_FLAV0_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }

                if (LowPtElectron_genPartFlav[k] == 1)
                {

                    pt_vs_EMID_LONG1_FLAV1_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }


                if (LowPtElectron_genPartFlav[k] == 5)
                {

                    pt_vs_EMID_LONG1_FLAV5_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }



                if (LowPtElectron_pt[k] >= 1.0 && LowPtElectron_pt[k] < 5.0)
                {// PT CUts
                 //FlavCut
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {

                        pt_vs_EMID_LONG1_FLAV0_Low.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {

                        pt_vs_EMID_LONG1_FLAV1_Low.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 5)
                    {

                        pt_vs_EMID_LONG1_FLAV5_Low.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                }

                if (LowPtElectron_pt[k] >= 5.0 && LowPtElectron_pt[k] < 10.0)
                {
                    //FlavCut
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {

                        pt_vs_EMID_LONG1_FLAV0_Mid.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {

                        pt_vs_EMID_LONG1_FLAV1_Mid.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 5)
                    {

                        pt_vs_EMID_LONG1_FLAV5_Mid.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }





                }

                if (LowPtElectron_pt[k] >= 10.0 && LowPtElectron_pt[k] < 20.0)
                {
                    //FlavCut
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {

                        pt_vs_EMID_LONG1_FLAV0_High.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {

                        pt_vs_EMID_LONG1_FLAV1_High.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 5)
                    {

                        pt_vs_EMID_LONG1_FLAV5_High.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                }


            }


            //FAKE
            if (LowPtElectron_convVeto[k] == 1 && abs(LowPtElectron_eta[k]) < 2.4 && LowPtElectron_embeddedID[k] >= 4 && LowPtElectron_miniPFRelIso_all[k] >= 4 && abs(LowPtElectron_dxy[k]) < 0.05 && abs(LowPtElectron_dz[k]) < 0.1 && IPSig1 < 2)
            {

                //FlavCut
                if (LowPtElectron_genPartFlav[k] == 0)
                {

                    pt_vs_EMID_FAKE_FLAV0_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }

                if (LowPtElectron_genPartFlav[k] == 1)
                {

                    pt_vs_EMID_FAKE_FLAV1_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }


                if (LowPtElectron_genPartFlav[k] == 5)
                {

                    pt_vs_EMID_FAKE_FLAV5_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }



                if (LowPtElectron_pt[k] >= 1.0 && LowPtElectron_pt[k] < 5.0)
                {// PT CUts
                 //FlavCut
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {

                        pt_vs_EMID_FAKE_FLAV0_Low.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {

                        pt_vs_EMID_FAKE_FLAV1_Low.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 5)
                    {

                        pt_vs_EMID_FAKE_FLAV5_Low.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                }

                if (LowPtElectron_pt[k] >= 5.0 && LowPtElectron_pt[k] < 10.0)
                {
                    //FlavCut
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {

                        pt_vs_EMID_FAKE_FLAV0_Mid.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {

                        pt_vs_EMID_FAKE_FLAV1_Mid.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 5)
                    {

                        pt_vs_EMID_FAKE_FLAV5_Mid.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }





                }

                if (LowPtElectron_pt[k] >= 10.0 && LowPtElectron_pt[k] < 20.0)
                {
                    //FlavCut
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {

                        pt_vs_EMID_FAKE_FLAV0_High.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {

                        pt_vs_EMID_FAKE_FLAV1_High.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 5)
                    {

                        pt_vs_EMID_FAKE_FLAV5_High.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                }


            }

            if (LowPtElectron_convVeto[k] == 1 && abs(LowPtElectron_eta[k]) < 2.4 && LowPtElectron_miniPFRelIso_all[k] < 4 && abs(LowPtElectron_dxy[k]) < 0.05 && abs(LowPtElectron_dz[k]) < 0.1 && IPSig1 < 2)
            {

                //FlavCut
                if (LowPtElectron_genPartFlav[k] == 0)
                {

                    Pt_vs_EMID_IRON1_FLAV0_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }

                if (LowPtElectron_genPartFlav[k] == 1)
                {

                    Pt_vs_EMID_IRON1_FLAV1_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }


                if (LowPtElectron_genPartFlav[k] == 5)
                {

                    Pt_vs_EMID_IRON1_FLAV5_NoPt.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                }



                if (LowPtElectron_pt[k] >= 1.0 && LowPtElectron_pt[k] < 5.0)
                {// PT CUts
                 //FlavCut
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {

                        Pt_vs_EMID_IRON1_FLAV0_Low.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {

                        Pt_vs_EMID_IRON1_FLAV1_Low.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 5)
                    {

                        Pt_vs_EMID_IRON1_FLAV5_Low.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                }

                if (LowPtElectron_pt[k] >= 5.0 && LowPtElectron_pt[k] < 10.0)
                {
                    //FlavCut
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {

                        Pt_vs_EMID_IRON1_FLAV0_Mid.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {

                        Pt_vs_EMID_IRON1_FLAV1_Mid.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 5)
                    {

                        Pt_vs_EMID_IRON1_FLAV5_Mid.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }





                }

                if (LowPtElectron_pt[k] >= 10.0 && LowPtElectron_pt[k] < 20.0)
                {
                    //FlavCut
                    if (LowPtElectron_genPartFlav[k] == 0)
                    {

                        Pt_vs_EMID_IRON1_FLAV0_High.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 1)
                    {

                        Pt_vs_EMID_IRON1_FLAV1_High.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                    if (LowPtElectron_genPartFlav[k] == 5)
                    {

                        Pt_vs_EMID_IRON1_FLAV5_High.Fill(LowPtElectron_pt[k], LowPtElectron_embeddedID[k]);
                    }

                }


            }


        }//End of loop
    }
    PlotHist2(pt_vs_FLAV0_Low, sample, plot_dir, "pt_vs_FLAV0_Iron1Low", "pt", "FLAV0");
    PlotHist2(pt_vs_FLAV0_Mid, sample, plot_dir, "pt_vs_FLAV0_Iron1Mid", "pt", "FLAV0");
    PlotHist2(pt_vs_FLAV0_High, sample, plot_dir, "pt_vs_FLAV0_Iron1High", "pt", "FLAV0");
    PlotHist2(pt_vs_FLAV0_NoPt, sample, plot_dir, "pt_vs_FLAV0_Iron1NoPt", "pt", "FLAV0");

    PlotHist2(pt_vs_FLAV1_Low, sample, plot_dir, "pt_vs_FLAV1_Iron1Low", "pt", "FLAV1");
    PlotHist2(pt_vs_FLAV1_Mid, sample, plot_dir, "pt_vs_FLAV1_Iron1Mid", "pt", "FLAV1");
    PlotHist2(pt_vs_FLAV1_High, sample, plot_dir, "pt_vs_FLAV1_Iron1High", "pt", "FLAV1");
    PlotHist2(pt_vs_FLAV1_NoPt, sample, plot_dir, "pt_vs_FLAV1_Iron1NoPt", "pt", "FLAV1");

    PlotHist2(pt_vs_EMID_FLAV0_Low, sample, plot_dir, " pt_vs_EMID_FLAV0_Low", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FLAV1_Low, sample, plot_dir, " pt_vs_EMID_FLAV1_Low", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FLAV5_Low, sample, plot_dir, " pt_vs_EMID_FLAV5_Low", "pt", "EMID");

    PlotHist2(pt_vs_EMID_FLAV0_Mid, sample, plot_dir, " pt_vs_EMID_FLAV0_Mid", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FLAV1_Mid, sample, plot_dir, " pt_vs_EMID_FLAV1_Mid", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FLAV5_Mid, sample, plot_dir, " pt_vs_EMID_FLAV5_Mid", "pt", "EMID");

    PlotHist2(pt_vs_EMID_FLAV0_High, sample, plot_dir, "pt_vs_EMID_FLAV0_High", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FLAV1_High, sample, plot_dir, " pt_vs_EMID_FLAV1_High", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FLAV5_High, sample, plot_dir, " pt_vs_EMID_FLAV5_High", "pt", "EMID");

    PlotHist2(pt_vs_EMID_FLAV0_NoPt, sample, plot_dir, " pt_vs_EMID_FLAV0_NoPt", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FLAV1_NoPt, sample, plot_dir, " pt_vs_EMID_FLAV1_NoPt", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FLAV5_NoPt, sample, plot_dir, " pt_vs_EMID_FLAV5_NoPt", "pt", "EMID");


    PlotHist2(pt_vs_EMID_IRON1_FLAV0_Low, sample, plot_dir, " pt_vs_EMID_IRON1_FLAV0_Low", "pt", "EMID");
    PlotHist2(pt_vs_EMID_IRON1_FLAV1_Low, sample, plot_dir, " pt_vs_EMID_IRON1_FLAV1_Low", "pt", "EMID");
    PlotHist2(pt_vs_EMID_IRON1_FLAV5_Low, sample, plot_dir, " pt_vs_EMID_IRON1_FLAV5_Low", "pt", "EMID");

    PlotHist2(pt_vs_EMID_IRON1_FLAV0_Mid, sample, plot_dir, " pt_vs_EMID_IRON1_FLAV0_Mid", "pt", "EMID");
    PlotHist2(pt_vs_EMID_IRON1_FLAV1_Mid, sample, plot_dir, " pt_vs_EMID_IRON1_FLAV1_Mid", "pt", "EMID");
    PlotHist2(pt_vs_EMID_IRON1_FLAV5_Mid, sample, plot_dir, " pt_vs_EMID_IRON1_FLAV5_Mid", "pt", "EMID");

    PlotHist2(pt_vs_EMID_IRON1_FLAV0_High, sample, plot_dir, "pt_vs_EMID_IRON1_FLAV0_High", "pt", "EMID");
    PlotHist2(pt_vs_EMID_IRON1_FLAV1_High, sample, plot_dir, " pt_vs_EMID_IRON1_FLAV1_High", "pt", "EMID");
    PlotHist2(pt_vs_EMID_IRON1_FLAV5_High, sample, plot_dir, " pt_vs_EMID_IRON1_FLAV5_High", "pt", "EMID");

    PlotHist2(pt_vs_EMID_IRON1_FLAV0_NoPt, sample, plot_dir, " pt_vs_EMID_IRON1_FLAV0_NoPt", "pt", "EMID");
    PlotHist2(pt_vs_EMID_IRON1_FLAV1_NoPt, sample, plot_dir, " pt_vs_EMID_IRON1_FLAV1_NoPt", "pt", "EMID");
    PlotHist2(pt_vs_EMID_IRON1_FLAV5_NoPt, sample, plot_dir, " pt_vs_EMID_IRON1_FLAV5_NoPt", "pt", "EMID");

    PlotHist2(pt_vs_EMID_LONG1_FLAV0_Low, sample, plot_dir, " pt_vs_EMID_LONG1_FLAV0_Low", "pt", "EMID");
    PlotHist2(pt_vs_EMID_LONG1_FLAV1_Low, sample, plot_dir, " pt_vs_EMID_LONG1_FLAV1_Low", "pt", "EMID");
    PlotHist2(pt_vs_EMID_LONG1_FLAV5_Low, sample, plot_dir, " pt_vs_EMID_LONG1_FLAV5_Low", "pt", "EMID");

    PlotHist2(pt_vs_EMID_LONG1_FLAV0_Mid, sample, plot_dir, " pt_vs_EMID_LONG1_FLAV0_Mid", "pt", "EMID");
    PlotHist2(pt_vs_EMID_LONG1_FLAV1_Mid, sample, plot_dir, " pt_vs_EMID_LONG1_FLAV1_Mid", "pt", "EMID");
    PlotHist2(pt_vs_EMID_LONG1_FLAV5_Mid, sample, plot_dir, " pt_vs_EMID_LONG1_FLAV5_Mid", "pt", "EMID");

    PlotHist2(pt_vs_EMID_LONG1_FLAV0_High, sample, plot_dir, "pt_vs_EMID_LONG1_FLAV0_High", "pt", "EMID");
    PlotHist2(pt_vs_EMID_LONG1_FLAV1_High, sample, plot_dir, " pt_vs_EMID_LONG1_FLAV1_High", "pt", "EMID");
    PlotHist2(pt_vs_EMID_LONG1_FLAV5_High, sample, plot_dir, " pt_vs_EMID_LONG1_FLAV5_High", "pt", "EMID");

    PlotHist2(pt_vs_EMID_LONG1_FLAV0_NoPt, sample, plot_dir, " pt_vs_EMID_LONG1_FLAV0_NoPt", "pt", "EMID");
    PlotHist2(pt_vs_EMID_LONG1_FLAV1_NoPt, sample, plot_dir, " pt_vs_EMID_LONG1_FLAV1_NoPt", "pt", "EMID");
    PlotHist2(pt_vs_EMID_LONG1_FLAV5_NoPt, sample, plot_dir, " pt_vs_EMID_LONG1_FLAV5_NoPt", "pt", "EMID");

    PlotHist2(pt_vs_EMID_FAKE_FLAV0_Low, sample, plot_dir, " pt_vs_EMID_FAKE_FLAV0_Low", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FAKE_FLAV1_Low, sample, plot_dir, " pt_vs_EMID_FAKE_FLAV1_Low", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FAKE_FLAV5_Low, sample, plot_dir, " pt_vs_EMID_FAKE_FLAV5_Low", "pt", "EMID");

    PlotHist2(pt_vs_EMID_FAKE_FLAV0_Mid, sample, plot_dir, " pt_vs_EMID_FAKE_FLAV0_Mid", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FAKE_FLAV1_Mid, sample, plot_dir, " pt_vs_EMID_FAKE_FLAV1_Mid", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FAKE_FLAV5_Mid, sample, plot_dir, " pt_vs_EMID_FAKE_FLAV5_Mid", "pt", "EMID");

    PlotHist2(pt_vs_EMID_FAKE_FLAV0_High, sample, plot_dir, "pt_vs_EMID_FAKE_FLAV0_High", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FAKE_FLAV1_High, sample, plot_dir, " pt_vs_EMID_FAKE_FLAV1_High", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FAKE_FLAV5_High, sample, plot_dir, " pt_vs_EMID_FAKE_FLAV5_High", "pt", "EMID");

    PlotHist2(pt_vs_EMID_FAKE_FLAV0_NoPt, sample, plot_dir, " pt_vs_EMID_FAKE_FLAV0_NoPt", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FAKE_FLAV1_NoPt, sample, plot_dir, " pt_vs_EMID_FAKE_FLAV1_NoPt", "pt", "EMID");
    PlotHist2(pt_vs_EMID_FAKE_FLAV5_NoPt, sample, plot_dir, " pt_vs_EMID_FAKE_FLAV5_NoPt", "pt", "EMID");

    PlotHist2(Pt_vs_EMID_IRON1_FLAV0_Low, sample, plot_dir, " Pt_vs_EMID_IRON1_FLAV0_Low", "pt", "EMID");
    PlotHist2(Pt_vs_EMID_IRON1_FLAV1_Low, sample, plot_dir, " Pt_vs_EMID_IRON1_FLAV1_Low", "pt", "EMID");
    PlotHist2(Pt_vs_EMID_IRON1_FLAV5_Low, sample, plot_dir, " Pt_vs_EMID_IRON1_FLAV5_Low", "pt", "EMID");

    PlotHist2(Pt_vs_EMID_IRON1_FLAV0_Mid, sample, plot_dir, " Pt_vs_EMID_IRON1_FLAV0_Mid", "pt", "EMID");
    PlotHist2(Pt_vs_EMID_IRON1_FLAV1_Mid, sample, plot_dir, " Pt_vs_EMID_IRON1_FLAV1_Mid", "pt", "EMID");
    PlotHist2(Pt_vs_EMID_IRON1_FLAV5_Mid, sample, plot_dir, " Pt_vs_EMID_IRON1_FLAV5_Mid", "pt", "EMID");

    PlotHist2(Pt_vs_EMID_IRON1_FLAV0_High, sample, plot_dir, "Pt_vs_EMID_IRON1_FLAV0_High", "pt", "EMID");
    PlotHist2(Pt_vs_EMID_IRON1_FLAV1_High, sample, plot_dir, " Pt_vs_EMID_IRON1_FLAV1_High", "pt", "EMID");
    PlotHist2(Pt_vs_EMID_IRON1_FLAV5_High, sample, plot_dir, " Pt_vs_EMID_IRON1_FLAV5_High", "pt", "EMID");

    PlotHist2(Pt_vs_EMID_IRON1_FLAV0_NoPt, sample, plot_dir, " Pt_vs_EMID_IRON1_FLAV0_NoPt", "pt", "EMID");
    PlotHist2(Pt_vs_EMID_IRON1_FLAV1_NoPt, sample, plot_dir, " Pt_vs_EMID_IRON1_FLAV1_NoPt", "pt", "EMID");
    PlotHist2(Pt_vs_EMID_IRON1_FLAV5_NoPt, sample, plot_dir, " Pt_vs_EMID_IRON1_FLAV5_NoPt", "pt", "EMID");


}





