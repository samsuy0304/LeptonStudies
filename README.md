# LeptonStudies

Study low transverse momenta leptons for CMS compressed SUSY searches.

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

