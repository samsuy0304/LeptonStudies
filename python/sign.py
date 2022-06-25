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
    ID_Lw =ROOT.TH1F("IDEMB", "IDEMB",45,-1,8)#, 20, 0.0, 20.0)
    pt_Lw =ROOT.TH1F("pt","pt",20,0.0,20.0)#, 20, 0.0, 20.0)
    eta_Lw =ROOT.TH1F("eta","eta", 30,-3,3)#, 20, 0.0, 20.0)
    dxy_Lw =ROOT.TH1F("dxy","dxy", 50,-0.2,0.2)#, 50, -0.2, 0.2)
    dxyErr_Lw =ROOT.TH1F("dxyErr", "dxyErr",50,0,0.2)#, 20, 0.0, 0.1)
    dz_Lw =ROOT.TH1F("dz","dz",50,-0.2,0.2)#, 50,-0.2, 0.2)
    dzErr_Lw =ROOT.TH1F("dzErr","dzErr", 20,0,0.2)#, 20, 0.0, 0.1)
    miniPFRelIso_all_Lw =ROOT.TH1F("PF","Pf",50,0.0,20.0)#, 50, 0.0, 20.0)
    convVeto_Lw =ROOT.TH1F("Veto","Veto",2,0.0,2.0)#, 2, 0.0, 2)
    dxy_dxyErr_Lw =ROOT.TH1F("dxsig","dxysig",50,-5.0,5.0)#, 2, 0.0, 2)
    dz_dzErr_Lw =ROOT.TH1F("dzsig","dzsig",50,-5.0,5.0)#, 2, 0.0, 2)
    IP3d_Lw =ROOT.TH1F("Convlusion","Convlusion",50,0.0,10.0)#, 2, 0.0, 2)
    for i in range(n_events):
        if i % 1000 == 0:
            print("Event {0}".format(i))
        tree.GetEntry(i)
        nLowPtElectron      = tree.nLowPtElectron
        LowPtElectron_ID    = tree.LowPtElectron_embeddedID
        LowPtElectron_pt    = tree.LowPtElectron_pt
        LowPtElectron_eta   = tree.LowPtElectron_eta
        LowPtElectron_dxy   = tree.LowPtElectron_dxy
        LowPtElectron_dxyErr  = tree.LowPtElectron_dxyErr
        LowPtElectron_dz   = tree.LowPtElectron_dz
        LowPtElectron_dzErr  = tree.LowPtElectron_dzErr
        LowPtElectron_miniPFRelIso_all = tree.LowPtElectron_miniPFRelIso_all
        LowPtElectron_convVeto   = tree.LowPtElectron_convVeto
        LowPtElectron_phi   = tree.LowPtElectron_phi
        LowPtElectron_mass  = tree.LowPtElectron_mass
        LowPtElectron_genpartflav = tree.LowPtElectron_genPartFlav
        
        # loop over LowPtElectron
        for j in range(nLowPtElectron):
            if verbose:
                print("LowPtElectron {0}: pt = {1:.3f}, eta = {2:.3f}, phi = {3:.3f}, mass = {4:.3f}".format(j, LowPtElectron_pt[j], LowPtElectron_eta[j], LowPtElectron_phi[j], LowPtElectron_mass[j]))
            h_LowPtElectron_pt.Fill(LowPtElectron_pt[j])
            ID_Lw.Fill(LowPtElectron_ID[j])
            pt_Lw.Fill(LowPtElectron_pt[j])
            eta_Lw.Fill(LowPtElectron_eta[j])
            dxy_Lw.Fill(LowPtElectron_dxy[j])
            dxyErr_Lw.Fill(LowPtElectron_dxyErr[j])
            dz_Lw.Fill(LowPtElectron_dz[j])
            dzErr_Lw.Fill(LowPtElectron_dzErr[j])
            miniPFRelIso_all_Lw.Fill(LowPtElectron_miniPFRelIso_all[j])
            convVeto_Lw.Fill(LowPtElectron_convVeto[j])
            
            dxysig = -999
            if LowPtElectron_dxyErr[j]!=0:
                dxysig = LowPtElectron_dxy[j]/LowPtElectron_dxyErr[j]
            dzsig = -999
            if LowPtElectron_dzErr[j]!=0:
                dzsig = LowPtElectron_dz[j]/LowPtElectron_dzErr[j]
            Ipsig = np.sqrt(dxysig*dxysig + dzsig*dzsig)
            
            dxy_dxyErr_Lw.Fill(dxysig)
            dz_dzErr_Lw.Fill(dzsig)
            IP3d_Lw.Fill(Ipsig)

    plot(h_LowPtElectron_pt, sample_name, plot_dir, "h_LowPtElectron_pt")
    plot(ID_Lw, sample_name, plot_dir, "ID")
    plot(pt_Lw, sample_name, plot_dir, "ptt")
    plot(eta_Lw, sample_name, plot_dir, "eta")
    plot(dxy_Lw, sample_name, plot_dir, "dxy")
    plot(dxyErr_Lw, sample_name, plot_dir, "dxyErr")
    plot(dz_Lw, sample_name, plot_dir, "dz")
    plot(dzErr_Lw, sample_name, plot_dir, "dzErr")
    plot(miniPFRelIso_all_Lw, sample_name, plot_dir, "miniPFRelIso")
    plot(convVeto_Lw, sample_name, plot_dir, "convVeto")
    plot(dxy_dxyErr_Lw, sample_name, plot_dir, "dxsig")
    plot(dz_dzErr_Lw, sample_name, plot_dir, "dzsig")
    plot(IP3d_Lw, sample_name, plot_dir, "Ipsig")

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

