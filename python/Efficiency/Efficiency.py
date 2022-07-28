#!/usr/bin/python3
import pandas as pd
import numpy as np

file = "Lepton_Numbers.csv"
Df = pd.read_csv(file)






def Flav_pres(a):
	if "flav0" in a.lower():
		return 2
	elif "flav1" in a.lower():
		return 1
	elif "flav5" in a.lower():
		return 0
	else:
		return 3
def Var_pres(a):
	if "emid" in a.lower():
		return 6

	elif "pt" in a.lower():
		return 11
	elif "dxy" in a.lower():
		if "dxyerr" in a.lower():
			return 56
	
		elif "dxysig" in a.lower():
			return 66
		else:
			return 16

	elif "dz" in a.lower():
		
		if "dzsig" in a.lower():
			return 61
		elif "dzerr" in a.lower():
			return 51
		else:
			return 21

	elif "iso" in a.lower():
		return 26

	elif "conv" in a.lower():
		return 31

	elif "ip" in a.lower():

		if "ipsig1" in a.lower():
			return 41
		elif "ipsig2" in a.lower():
			return 46
		else:
			return 36
	elif "eta" in a.lower():
		return 71
	
	else:
		return 0

def RowDet(a):

	return Var_pres(a)-Flav_pres(a)


def Ironman(a):
	if "fake" in a.lower():
		return 0
	elif "long2" in a.lower():
		return 1
	elif "long1" in a.lower():
		return 2

	elif "iron2" in a.lower():
		return 3
	elif "iron1" in a.lower():
		return 4

	else:
		return 5 

def Cutter(a):
	if "lowcut" in a.lower():
		return 8
	elif "highcut" in a.lower():
		return 14
	elif "midcut" in a.lower():
		return 20

	elif "general" in a.lower():
		return 26
	
	elif "1_2" in a.lower():
		return 32
	elif "2_3" in a.lower():
		return 38

	elif "3_4" in a.lower():
		return 44
	elif "4_5" in a.lower():
		return 50
	elif "5_6" in a.lower():
		return 56

	elif "6_7" in a.lower():
		return 62
	elif "7_8" in a.lower():
		return 68
	elif "8_9" in a.lower():
		return 74

	elif "9_10" in a.lower():
		return 80
	elif "10_11" in a.lower():
		return 86
	elif "11_12" in a.lower():
		return 92
	elif "12_13" in a.lower():
		return 98

	elif "13_14" in a.lower():
		return 104
	elif "14_15" in a.lower():
		return 110
	elif "15_16" in a.lower():
		return 116

	elif "16_17" in a.lower():
		return 122
	elif "17_18" in a.lower():
		return 128
	elif "18_19" in a.lower():
		return 134

	elif "19_20" in a.lower():
		return 140
	

def ColDet(a):
	return Cutter(a)-Ironman(a)


def ExtracNumber(a):
	list = a.split(",")
	return list[1]

def Put(a,b):
	r = int(RowDet(a))-2
	c = int(ColDet(a))-1
	n = int(ExtracNumber(a))

	b.iat[r,c] = n
	return b
	
	
input = "Final.txt"

f = open(input, "r")
for x in f:
  Put(x,Df)

Df.to_csv("FI.csv",  encoding='utf-8')


rf = Df.iloc[:5]



Title = rf.iloc[0, :].values.flatten().tolist()
rf = rf.drop(rf.index[0])
Signal_n = rf.iloc[2, :].values.flatten().tolist()
Bkg_n = rf.iloc[1, :].values.flatten().tolist()
del Signal_n[0:26]
del Bkg_n[0:26]





for i in range(2,135,6):
	
	rf.iloc[:,i+1]=rf.iloc[:,i+1]/rf.iloc[:,i]
	rf.iloc[:,i+2]=rf.iloc[:,i+2]/rf.iloc[:,i]
	rf.iloc[:,i+3]=rf.iloc[:,i+3]/rf.iloc[:,i]
	rf.iloc[:,i+4]=rf.iloc[:,i+4]/rf.iloc[:,i]
	rf.iloc[:,i+5]=rf.iloc[:,i+5]/rf.iloc[:,i]
	rf.iloc[:,i]=rf.iloc[:,i]/rf.iloc[:,i]


Signal = rf.iloc[2, :].values.flatten().tolist()
Bkg = rf.iloc[1, :].values.flatten().tolist()


del Signal[0:26]
del Bkg[0:26]

print(Signal[1:114:6])
print(Bkg[1:114:6])

x=[1.5	,
2.5	,
3.5	,
4.5	,
5.5	,
6.5	,
7.5	,
8.5	,
9.5	,
10.5	,
11.5	,
12.5	,
13.5	,
14.5	,
15.5	,
16.5	,
17.5	,
18.5	,
19.5	
]

def Error(a,b):
	err =[]
	for i in range(0,109,6):
		err.append(np.sqrt((a[i]*(1-a[i]))/b[i]))
		err.append(np.sqrt((a[i+1]*(1-a[i+1]))/b[i]))
		err.append(np.sqrt((a[i+2]*(1-a[i+2]))/b[i]))
		err.append(np.sqrt((a[i+3]*(1-a[i+3]))/b[i]))
		err.append(np.sqrt((a[i+4]*(1-a[i+4]))/b[i]))
		err.append(np.sqrt((a[i+5]*(1-a[i+5]))/b[i]))
	return err

Err_Sig=Error(Signal,Signal_n)
Err_Bkg=Error(Bkg,Bkg_n)
import matplotlib.pyplot as plt
from matplotlib import style


plt.errorbar(x,Signal[1:114:6],yerr=Err_Sig[1:114:6], fmt="r^", label="Iron1_Flav1")
plt.errorbar(x,Bkg[1:114:6],yerr=Err_Bkg[1:114:6], fmt="r^", label = "Iron1_Flav0",markerfacecolor='none')
plt.errorbar(x,Signal[3:116:6],yerr=Err_Sig[3:116:6], fmt="bo", label="Long1_Flav1")
plt.errorbar(x,Bkg[3:116:6],yerr=Err_Bkg[3:116:6], fmt="bo", label = "Long1_Flav0",markerfacecolor='none')
plt.errorbar(x,Signal[5:118:6],yerr=Err_Sig[5:118:6], fmt="ms", label="IronFake1_Flav1")
plt.errorbar(x,Bkg[5:118:6],yerr=Err_Bkg[5:118:6], fmt="ms", label = "IronFake1_Flav0",markerfacecolor='none')
plt.ylabel('Efficiency',fontsize=18)
plt.rc('ytick',labelsize=18)
plt.rc('xtick',labelsize=18)
plt.xlabel('Electron Pt [GeV]',fontsize=18)
plt.title("TTJETS Fall 17",fontsize=18)
plt.legend(bbox_to_anchor=(0.99, 0.8))


plt.show()


