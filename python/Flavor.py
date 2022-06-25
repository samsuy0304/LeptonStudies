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

ROOT.gStyle.SetOptStat(111111)

def plot(hist, sample_name, plot_dir, plot_name):
    c = ROOT.TCanvas("c", "c", 800, 800)
    
    # draw
    hist.Draw()

    # save plot
    output_name = "{0}/{1}".format(plot_dir, plot_name)
    c.Update()
    c.SaveAs(output_name + ".pdf")

def run(plot_dir, sample_name, tree):
    verbose     = False
    n_events    = tree.GetEntries()
    # histograms
    h_LowPtElectron_pt = ROOT.TH1F("h_LowPtElectron_pt", "h_LowPtElectron_pt", 20, 0.0, 20.0)
   
    for i in range(n_events):
        if i % 1000 == 0:
            print("Event {0}".format(i))
        tree.GetEntry(i)
        nLowPtElectron      = tree.nLowPtElectron
        LowPtElectron_genpartflav = tree.LowPtElectron_genPartFlav
        
        
        print(LowPtElectron_genpartflav)
        # loop over LowPtElectron
        for j in range(nLowPtElectron):
            h_LowPtElectron_pt.Fill(nLowPtElectron[j])


    plot(h_LowPtElectron_pt, sample_name, plot_dir, "h_LowPtElectron_pt")
   

def makePlots():
    input_file  = "4153AE9C-1215-A847-8E0A-DEBE98140664.root"
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

