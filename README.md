# LeptonStudies

In order to search for beyond standard model compressed supersymmetric decay channels, low
 momentumobjects must be identifiable in the CMS detector. We have adapted electron identification
 criteria to observe low momentum (pT <5 GeV) prompt electrons. We have studied the electron
 identification and misidentification efficiencies using Monte Carlo simulations, where the generator
 information is kept. We find an identification efficiency for electrons with 1 GeV≤ pT <5 GeV to be
 in the range from 20-70%, and the misidentification efficiency to be less than 8%. A framework was
 developed to measure the signal from a specific compressed super symmetric stop sample as well as
 the background from a t¯ tdilepton sample. The signal and background from the KUCMS recursive
 jigsaw reconstruction analysis were used to study the sensitivity of the analysis to low momentum
 (pT <5) leptons. Currently, the analysis only includes muons with momentum from 3-5 GeV. The
 sensitivity in this range was dominated by these low momentum muons. Thus, this study predicts
 that the addition of low momentum electrons to future searches would be efficient and beneficial.

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

