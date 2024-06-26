# Mysteries of Dark Matter: Search for Low Momentum Electrons

## Overview
This project focuses on the search for compressed supersymmetric decay channels using data from the CMS detector at CERN. The primary objective is to build algorithm to identify electrons with momentum < 5 pT. Low momentum electrons are produced as a result of interaction between two top squarks, a process that also produces the dark matter candidate - neutralino.


## Methodology
- Electron and muon identification efficiencies studied using Monte Carlo simulations with preserved generator information.
- Framework developed for signal and background analysis using specific CMS data samples.
- Sensitivity analysis conducted focusing on muons with 3-5 GeV momentum range.
- Prediction made regarding the efficiency and benefits of including low momentum electrons in future searches.


## Setup

Setup commands to run at cmslpc or lxplus.

Checkout CMSSW_10_6_5 in your desired working area.
```
cmsrel CMSSW_10_6_5
cd CMSSW_10_6_5/src
cmsenv
```

Download this repository in the CMSSW_10_6_5/src directory.
```
git clone https://github.com/ku-cms/LeptonStudies.git
cd LeptonStudies
```

Create plots with python plotting script.
```
python python/makePlots.py
```

Create plots with the ROOT macro.
```
cd src
mkdir -p macro_plots
# start a ROOT shell:
root -l
# enter the following commands in the ROOT shell:
.L NanoClass.C
NanoClass n;
n.Loop();
```

