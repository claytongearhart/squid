import numpy as np

rMap = []
nnReg = -1
usedFunc = [""]

def getReg(varN):
    for ev in rMap:
        if varN in ev:
            return ev[1]

def requestReg(varN = None):
    global nnReg
    if any(varN in e for e in rMap):
        return getReg(varN)
    else:
        nnReg += 1
        if (not varN):
            rMap.append(['BINA', nnReg])
        else:
            rMap.append([varN, nnReg])

        return nnReg
