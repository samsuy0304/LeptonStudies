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
    }
    return labels[key]

# plot a histogram
def plotHist(hist, sample_name, plot_dir, plot_name, variable, y_min, y_max):
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
    output_name = "{0}/{1}".format(plot_dir, plot_name)
    c.Update()
    c.SaveAs(output_name + ".pdf")

# loop over tree, fill and plot histograms
def run(plot_dir, sample_name, tree):
    verbose     = False
    n_events    = tree.GetEntries()
    
    # histograms
    h_nLowPtElectron        = ROOT.TH1F("h_nLowPtElectron",     "h_nLowPtElectron",      6,    0.0,   6.0)
    h_LowPtElectron_pt      = ROOT.TH1F("h_LowPtElectron_pt",   "h_LowPtElectron_pt",   20,    0.0,  20.0)
    h_LowPtElectron_eta     = ROOT.TH1F("h_LowPtElectron_eta",  "h_LowPtElectron_eta",  20,   -3.0,   3.0)
    h_LowPtElectron_phi     = ROOT.TH1F("h_LowPtElectron_phi",  "h_LowPtElectron_phi",  20, -np.pi, np.pi)
    h_LowPtElectron_mass    = ROOT.TH1F("h_LowPtElectron_mass", "h_LowPtElectron_mass", 20,  -0.01,  0.01)
    
    # loop over events
    for i in range(n_events):
        # print event number
        if i % 1000 == 0:
            print("Event {0}".format(i))
        
        # select event
        tree.GetEntry(i)
        
        # get branches
        nLowPtElectron      = tree.nLowPtElectron
        LowPtElectron_pt    = tree.LowPtElectron_pt
        LowPtElectron_eta   = tree.LowPtElectron_eta
        LowPtElectron_phi   = tree.LowPtElectron_phi
        LowPtElectron_mass  = tree.LowPtElectron_mass
            
        # fill histograms (per event)
        h_nLowPtElectron.Fill(nLowPtElectron)
        
        # loop over LowPtElectron
        for j in range(nLowPtElectron):
            if verbose:
                print("LowPtElectron {0}: pt = {1:.3f}, eta = {2:.3f}, phi = {3:.3f}, mass = {4:.3f}".format(j, LowPtElectron_pt[j], LowPtElectron_eta[j], LowPtElectron_phi[j], LowPtElectron_mass[j]))
            # fill histograms (per LowPtElectron)
            h_LowPtElectron_pt.Fill(LowPtElectron_pt[j])
            h_LowPtElectron_eta.Fill(LowPtElectron_eta[j])
            h_LowPtElectron_phi.Fill(LowPtElectron_phi[j])
            h_LowPtElectron_mass.Fill(LowPtElectron_mass[j])
    
    # plot histograms
    plotHist(h_nLowPtElectron,      sample_name, plot_dir, "nLowPtElectron",        "nElectrons",   0.0,  10000.0)
    plotHist(h_LowPtElectron_pt,    sample_name, plot_dir, "LowPtElectron_pt",      "pt",           0.0,   3000.0)
    plotHist(h_LowPtElectron_eta,   sample_name, plot_dir, "LowPtElectron_eta",     "eta",          0.0,   1000.0)
    plotHist(h_LowPtElectron_phi,   sample_name, plot_dir, "LowPtElectron_phi",     "phi",          0.0,   1000.0)
    plotHist(h_LowPtElectron_mass,  sample_name, plot_dir, "LowPtElectron_mass",    "mass",         0.0,   3000.0)

# run over input file
def makePlots():
    input_file  = "/uscms/home/caleb/nobackup/KU_Compressed_SUSY/samples/SMS-T2-4bd_genMET-80_mStop-500_mLSP-490_TuneCP5_13TeV-madgraphMLM-pythia8_NanoAODv9/4153AE9C-1215-A847-8E0A-DEBE98140664.root"
    plot_dir    = "plots"
    sample_name = "SMS-T2-4bd_genMET-80_mStop-500_mLSP-490"
    
    # WARNING: Make sure to open file here, not within getTree() so that TFile stays open. 
    #          If TFile closes, then TTree object is destroyed.
    tree_name   = "Events"
    open_file   = ROOT.TFile.Open(input_file)
    tree        = tools.getTree(open_file, tree_name)

    tools.makeDir(plot_dir)
    run(plot_dir, sample_name, tree)

def main():
    makePlots()

if __name__ == "__main__":
    main()

