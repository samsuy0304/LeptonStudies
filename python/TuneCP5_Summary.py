#!/usr/bin/python3
import pandas as pd
import numpy as np
import pprint

import os


def Decode(input,b):
    bar =[]
    CP5=[]
    Info =[]
    f = open(b+"/"+input, "r")
    for x in f:
        if "ttbar" in x:
            bar.append(x.replace("\n",''))
        elif "CP5" in x:
            CP5.append(x.replace("\n",''))
    Info.append(input)
    Info.append(bar)
    Info.append(CP5)
    
    return Info


def Sum(input,b):
    a = Decode(input,b)
    
    name = a[0]
    print(name)
    tt = a[1]
    CP5 = a[2]
    l =[]

    ts = []
    te = []
    cps = []
    cpe =[]

    for i in tt:
        d = i.split(" ")
        ts.append(float(d[5]))
        te.append(float(d[6]))

    for j in CP5:
        k = j.split(" ")
        cps.append(float(k[5]))
        cpe.append(float(k[6]))
    
    e = np.array(ts)
    r = np.array(te)
    y =np.array(cps)
    u = np.array(cpe)
    
    l.append(str(name))
    if len(e)>0:
        l.append(str(np.sum(e)))
        l.append(str(np.sqrt(np.sum(np.square(r)))))
    elif len(e)==0:
        l.append("0")
        l.append("0")
    if len(y)>0:
        l.append(str(np.sum(y)))
        l.append(str(np.sqrt(np.sum(np.square(u)))))
    elif len(y)==0:
        l.append("0")
        l.append("0")
    h = ",".join(l)


    
    

    if "gold" in h.lower():
        file1 = open(b+"_gold.csv", "a")  # append mode
        file1.write(h+"\n")
        file1.close()
    elif "slvr" in h.lower():
        file2 = open(b+"_silver.csv", "a")  # append mode
        file2.write(h+"\n")
        file2.close()

    elif "bron" in h.lower():
        file3 = open(b+"_bronze.csv", "a")  # append mode
        file3.write(h+"\n")
        file3.close()



    return h


def Finder(b):
    file1 = open(b+"_gold.csv", "w")  # append mode
    file1.write("Name,TTbar,TTbar_Err,CP5,CP5_Err\n")
    file1.flush()

    file2 = open(b+"_silver.csv", "w")  # append mode
    file2.write("Name,TTbar,TTbar_Err,CP5,CP5_Err\n")
    file2.flush()

    file3 = open(b+"_bronze.csv", "w")  # append mode
    file3.write("Name,TTbar,TTbar_Err,CP5,CP5_Err\n")
    file3.flush()


    gold =[]
    silver = []
    bronze = []

    for root, dirs, files in os.walk("./"+b, topdown=False):
        
        for name in files:
            if "gold" in os.path.join(name).lower():
                gold.append(os.path.join(name))
            elif "slvr" in os.path.join(name).lower():
                silver.append(os.path.join(name))
            elif  "bron" in os.path.join(name).lower():
                bronze.append(os.path.join(name))
    print(bronze)   
    for x in gold:
        Sum(x,b)

        print("g")
    for x in silver:
        Sum(x,b)
        print("s")
    for x in bronze:
        Sum(x,b)
        print("b")

    
def Summary(i):
    
    df = pd.read_csv(i)
    TTbar = df["TTbar"].to_numpy()
    TTbar_Err = df["TTbar_Err"].to_numpy()
    CP5 = df["CP5"].to_numpy()
    CP5_Err = df["CP5_Err"].to_numpy()

    tt = str(np.sum(TTbar))
    cp = str(np.sum(CP5))
    terr = str(np.sqrt(np.sum(np.square(TTbar_Err))))
    cperr= str(np.sqrt(np.sum(np.square(CP5_Err))))
    u = [tt,terr,cp,cperr]
    return u





    
    
def SumFind(input):
    

    Finder(input+"_0L")
    Finder(input+"_1L")
    Finder(input+"_2L")
    Finder(input+"_3L")
    
    l=[]
    for root, dirs, files in os.walk("./", topdown=False):
        
        for name in files:
            if input in name:
                l.append(name)
                l.append(",".join(Summary(name)))
                l.append("\n")
    
    print(",".join(l))

    f = open("Summary.csv","w")
    f.write(",".join(l))
    f.close()

ii= input("Sample name?")

SumFind(ii)
