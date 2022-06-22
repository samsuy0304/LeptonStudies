# makePlots.py 

import os
import ROOT
import tools
import numpy as np

# Make sure ROOT.TFile.Open(fileURL) does not seg fault when $ is in sys.argv (e.g. $ passed in as argument)
ROOT.PyConfig.IgnoreCommandLineOptions = True
# Make plots faster without displaying them
ROOT.gROOT.SetBatch(ROOT.kTRUE)
# Tell ROOT not to be in charge of memory, fix issue of histograms being deleted when ROOT file is closed:
ROOT.TH1.AddDirectory(False)

# get label based on a key
def getLabel(key):
    labels = {
        "nElectrons"    : "n_{e}",
        "pt"            : "p_{T} [GeV]",
        "eta"           : "#eta",
        "phi"           : "#phi",
        "mass"          : "m [GeV]",
        "genPartIdx"    : "Gen Part Idx",
        "genPartFlav"   : "Gen Part Flav",
        "dxy"           : "d_{xy}",
        "dxyErr"        : "d_{xy} err",
        "dxySig"        : "d_{xy} sig",
    }
    return labels[key]

# plot a histogram
def plotHist(hist, sample_name, plot_dir, plot_name, variable):
    # get y limits
    y_min = 0.0
    y_max = 1.5 * hist.GetMaximum()
    #print("{0}: GetMaximum = {1}, y_max = {2}".format(variable, hist.GetMaximum(), y_max))
    
    # canvas
    c = ROOT.TCanvas("c", "c", 800, 800)
    c.SetLeftMargin(0.15)

    # setup histogram
    title = plot_name
    x_title = getLabel(variable) 
    y_title = "Entries"
    color = "black"
    lineWidth = 1
    tools.setupHist(hist, title, x_title, y_title, y_min, y_max, color, lineWidth)
    
    # draw
    hist.Draw("hist error same")

    # save plot
    output_name = "{0}/{1}_{2}".format(plot_dir, sample_name, plot_name)
    c.Update()
    c.SaveAs(output_name + ".pdf")

# loop over tree, fill and plot histograms
def run(plot_dir, sample_name, tree):
    verbose     = False
    n_events    = tree.GetEntries()
    
    # histograms
    h_nLowPtElectron            = ROOT.TH1F("h_nLowPtElectron",             "h_nLowPtElectron",              6,    0.0,  6.0)
    h_LowPtElectron_pt          = ROOT.TH1F("h_LowPtElectron_pt",           "h_LowPtElectron_pt",           20,    0.0,  20.0)
    h_LowPtElectron_eta         = ROOT.TH1F("h_LowPtElectron_eta",          "h_LowPtElectron_eta",          20,   -3.0,  3.0)
    h_LowPtElectron_phi         = ROOT.TH1F("h_LowPtElectron_phi",          "h_LowPtElectron_phi",          20, -np.pi,  np.pi)
    h_LowPtElectron_mass        = ROOT.TH1F("h_LowPtElectron_mass",         "h_LowPtElectron_mass",         20,  -0.01,  0.01)
    h_LowPtElectron_genPartIdx  = ROOT.TH1F("h_LowPtElectron_genPartIdx",   "h_LowPtElectron_genPartIdx",   20,      0,  100)
    h_LowPtElectron_genPartFlav = ROOT.TH1F("h_LowPtElectron_genPartFlav",  "h_LowPtElectron_genPartFlav",  30,      0,  30)
    h_LowPtElectron_dxy         = ROOT.TH1F("h_LowPtElectron_dxy",          "h_LowPtElectron_dxy",          40,  -0.02,  0.02)
    h_LowPtElectron_dxyErr      = ROOT.TH1F("h_LowPtElectron_dxyErr",       "h_LowPtElectron_dxyErr",       40,      0,  0.1)
    h_LowPtElectron_dxySig      = ROOT.TH1F("h_LowPtElectron_dxySig",       "h_LowPtElectron_dxySig",       50,      0,  5.0)
    h_LowPtElectron_dxySig0     = ROOT.TH1F("h_LowPtElectron_dxySig0",      "h_LowPtElectron_dxySig0",      50,      0,  5.0)
    h_LowPtElectron_dxySig5     = ROOT.TH1F("h_LowPtElectron_dxySig5",      "h_LowPtElectron_dxySig5",      50,      0,  5.0)
    
    # loop over events
    for i in range(n_events):
        # print event number
        if i % 1000 == 0:
            print("Event {0}".format(i))
        
        # select event
        tree.GetEntry(i)
        
        # get branches
        nLowPtElectron              = tree.nLowPtElectron
        LowPtElectron_pt            = tree.LowPtElectron_pt
        LowPtElectron_eta           = tree.LowPtElectron_eta
        LowPtElectron_phi           = tree.LowPtElectron_phi
        LowPtElectron_mass          = tree.LowPtElectron_mass
        LowPtElectron_genPartIdx    = tree.LowPtElectron_genPartIdx
        LowPtElectron_genPartFlav   = tree.LowPtElectron_genPartFlav
        LowPtElectron_dxy           = tree.LowPtElectron_dxy
        LowPtElectron_dxyErr        = tree.LowPtElectron_dxyErr
            
        # fill histograms (per event)
        h_nLowPtElectron.Fill(nLowPtElectron)

        #LowPtElectron_genPartIdx[5];   //[nLowPtElectron]
        #LowPtElectron_genPartFlav[5];   //[nLowPtElectron]

        #print("LowPtElectron_genPartFlav object:")
        #print(LowPtElectron_genPartFlav)
        
        # loop over LowPtElectron
        for j in range(nLowPtElectron):
            dxySig = abs(LowPtElectron_dxy[j] / LowPtElectron_dxyErr[j])
            if verbose:
                print("LowPtElectron {0}: pt = {1:.3f}, eta = {2:.3f}, phi = {3:.3f}, mass = {4:.3f}".format(j, LowPtElectron_pt[j], LowPtElectron_eta[j], LowPtElectron_phi[j], LowPtElectron_mass[j]))
            
            #if LowPtElectron_genPartFlav[j] == "":
            #    print("BLANK STRING")
            #else:
            #    print("NOT BLANK STRING")
            #
            #if LowPtElectron_genPartFlav[j] == " ":
            #    print("SPACE STRING")
            #else:
            #    print("NOT SPACE STRING")
            #
            #if LowPtElectron_genPartFlav[j] == "0":
            #    print("ZERO STRING")
            #else:
            #    print("NOT ZERO STRING")
            
            #if LowPtElectron_genPartFlav[j] == "\0":
            #    print("NULL STRING")
            #else:
            #    print("NOT NULL STRING")
            
            #print("LowPtElectron_genPartFlav Type: {0}".format(type(LowPtElectron_genPartFlav[j])))
            #if LowPtElectron_genPartFlav[j]: 
            #    print("LowPtElectron {0}: LowPtElectron_genPartFlav = {1}".format(j, LowPtElectron_genPartFlav[j]))
            #    #print("LowPtElectron {0}: LowPtElectron_genPartFlav = {1}".format(j, int(LowPtElectron_genPartFlav[j])))
            # fill histograms (per LowPtElectron)
            h_LowPtElectron_pt.Fill(LowPtElectron_pt[j])
            h_LowPtElectron_eta.Fill(LowPtElectron_eta[j])
            h_LowPtElectron_phi.Fill(LowPtElectron_phi[j])
            h_LowPtElectron_mass.Fill(LowPtElectron_mass[j])
            h_LowPtElectron_genPartIdx.Fill(LowPtElectron_genPartIdx[j])
            #if LowPtElectron_genPartFlav[j]: 
            #    h_LowPtElectron_genPartFlav.Fill(int(LowPtElectron_genPartFlav[j]))
            h_LowPtElectron_dxy.Fill(LowPtElectron_dxy[j])
            h_LowPtElectron_dxyErr.Fill(LowPtElectron_dxyErr[j])
            h_LowPtElectron_dxySig.Fill(dxySig)
            if LowPtElectron_genPartFlav[j]: 
                if LowPtElectron_genPartFlav[j] == '0':
                    h_LowPtElectron_dxySig0.Fill(dxySig)
                if LowPtElectron_genPartFlav[j] == '5':
                    h_LowPtElectron_dxySig5.Fill(dxySig)
    
    # plot histograms
    plotHist(h_nLowPtElectron,              sample_name, plot_dir, "nLowPtElectron",            "nElectrons")
    plotHist(h_LowPtElectron_pt,            sample_name, plot_dir, "LowPtElectron_pt",          "pt")
    plotHist(h_LowPtElectron_eta,           sample_name, plot_dir, "LowPtElectron_eta",         "eta")
    plotHist(h_LowPtElectron_phi,           sample_name, plot_dir, "LowPtElectron_phi",         "phi")
    plotHist(h_LowPtElectron_mass,          sample_name, plot_dir, "LowPtElectron_mass",        "mass")
    plotHist(h_LowPtElectron_genPartIdx,    sample_name, plot_dir, "LowPtElectron_genPartIdx",  "genPartIdx")
    plotHist(h_LowPtElectron_genPartFlav,   sample_name, plot_dir, "LowPtElectron_genPartFlav", "genPartFlav")
    plotHist(h_LowPtElectron_dxy,           sample_name, plot_dir, "LowPtElectron_dxy",         "dxy")
    plotHist(h_LowPtElectron_dxyErr,        sample_name, plot_dir, "LowPtElectron_dxyErr",      "dxyErr")
    plotHist(h_LowPtElectron_dxySig,        sample_name, plot_dir, "LowPtElectron_dxySig",      "dxySig")
    plotHist(h_LowPtElectron_dxySig0,       sample_name, plot_dir, "LowPtElectron_dxySig0",     "dxySig")
    plotHist(h_LowPtElectron_dxySig5,       sample_name, plot_dir, "LowPtElectron_dxySig5",     "dxySig")

# run over input file
def makePlots():
    
    plot_dir    = "plots"
    
    # map sample names to input files
    samples = {}
    samples["SMS-T2-4bd_genMET-80_mStop-500_mLSP-490"]  = "/uscms/home/caleb/nobackup/KU_Compressed_SUSY/samples/SMS-T2-4bd_genMET-80_mStop-500_mLSP-490_TuneCP5_13TeV-madgraphMLM-pythia8_NanoAODv9/4153AE9C-1215-A847-8E0A-DEBE98140664.root"
    #samples["TTJets_DiLept"]                            = "/uscms/home/caleb/nobackup/KU_Compressed_SUSY/samples/TTJets_DiLept_TuneCP5_13TeV-madgraphMLM-pythia8_NanoAODv9/5457F199-A129-2A40-8127-733D51A9A3E6.root"

    for sample in samples:
        print("Running over {0}".format(sample))
        
        input_file = samples[sample]
    
        # WARNING: Make sure to open file here, not within getTree() so that TFile stays open. 
        #          If TFile closes, then TTree object is destroyed.
        tree_name   = "Events"
        open_file   = ROOT.TFile.Open(input_file)
        tree        = tools.getTree(open_file, tree_name)

        tools.makeDir(plot_dir)
        run(plot_dir, sample, tree)

def main():
    makePlots()

if __name__ == "__main__":
    main()

