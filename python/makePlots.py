# makePlots.py 

import os
import ROOT
import tools

# Make sure ROOT.TFile.Open(fileURL) does not seg fault when $ is in sys.argv (e.g. $ passed in as argument)
ROOT.PyConfig.IgnoreCommandLineOptions = True
# Make plots faster without displaying them
ROOT.gROOT.SetBatch(ROOT.kTRUE)
# Tell ROOT not to be in charge of memory, fix issue of histograms being deleted when ROOT file is closed:
ROOT.TH1.AddDirectory(False)

def plot(plot_dir, output_name, tree):
    print("Plotting")
    n_events = tree.GetEntries()
    print("n_events = {0}".format(n_events))
    for i in range(n_events):
        if i % 1000 == 0:
            print("Event {0}".format(i))
        tree.GetEntry(i)
        nLowPtElectron      = tree.nLowPtElectron
        LowPtElectron_pt    = tree.LowPtElectron_pt
        LowPtElectron_eta   = tree.LowPtElectron_eta
        LowPtElectron_phi   = tree.LowPtElectron_phi
        LowPtElectron_mass  = tree.LowPtElectron_mass
        for j in range(nLowPtElectron):
            #print("LowPtElectron_pt[{0}]: {1:.3f}".format(j, LowPtElectron_pt[j]))
            print("LowPtElectron {0}: pt = {1:.3f}, eta = {2:.3f}, phi = {3:.3f}, mass = {4:.3f}".format(j, LowPtElectron_pt[j], LowPtElectron_eta[j], LowPtElectron_phi[j], LowPtElectron_mass[j]))

def makePlots():
    input_file  = "/uscms/home/caleb/nobackup/KU_Compressed_SUSY/samples/SMS-T2-4bd_genMET-80_mStop-500_mLSP-490_TuneCP5_13TeV-madgraphMLM-pythia8_NanoAODv9/4153AE9C-1215-A847-8E0A-DEBE98140664.root"
    plot_dir    = "plots"
    output_name = "SMS-T2-4bd_genMET-80_mStop-500_mLSP-490"
    
    # WARNING: Make sure to open file here, not within getTree() so that TFile stays open. 
    #          If TFile closes, then TTree object is destroyed.
    tree_name   = "Events"
    open_file   = ROOT.TFile.Open(input_file)
    tree        = tools.getTree(open_file, tree_name)

    tools.makeDir(plot_dir)
    plot(plot_dir, output_name, tree)

def main():
    makePlots()

if __name__ == "__main__":
    main()

