import numpy as np
from itertools import zip_longest
import varMap as rm
import math


# def flatten(nested_list):
#     return zip(*_flattengen(nested_list)) # in py3, wrap this in list()

# def _flattengen(iterable):
#     for element in zip_longest(*iterable, fillvalue=""):
#         if isinstance(element[0], list):
#             yield from _flattengen(element)
#         else:
#             yield element


# def getLeavesAtDepth_R(node, depth):
#     leaves = []

#     if depth > 0:
#         if node.left:
#             leaves.append(getLeavesAtDepth_R(node.left ,depth - 1))
#         if node.right:
#             leaves.append(getLeavesAtDepth_R(node.right, depth - 1))
#     else:
#         leaves.append(node.data)
#     return leaves


# def getLeavesAtDepth(node, depth):
#     cleanLeaves = []
#     noisyLeaves = getLeavesAtDepth_R(node, depth)


#     cleanLeaves = np.array(list(flatten(noisyLeaves)), dtype=object)
#     cleanLeaves = cleanLeaves.flatten()

#     cl2 = np.array([])

#     for e in cleanLeaves:
#         if e:
#             cl2 = np.append(cl2, e)
#         else:
#             continue

#     return cl2

# def treeDepth(node):
#     if not node:
#         return -1
#     else:
#         lDepth = treeDepth(node.left)
#         rDepth = treeDepth(node.right)

#         if (lDepth > rDepth):
#             return lDepth + 1
#         else:
#             return rDepth + 1


def getOpIns(operator):
    match operator:
        case "+":
            return "add"
        case "-":
            return "sub"
        case "*":
            return "mul"
        case "/":
            return "sdiv"


def isLeaf(expr):
    if(expr.left or expr.right):
        return False
    else:
        return True


def expToIR_R(expr, size, cIR=""):
    if isLeaf(expr.left):
        arg1 = expr.left.data['value']
        if not arg1.isnumeric():
            arg1 = '%' + str(rm.requestReg(arg1))
    else:
        result = expToIR_R(expr.left, size, "")
        cIR = cIR.join(result['IR'])
        arg1 = result['reg']
    if isLeaf(expr.right):
        arg2 = expr.right.data['value']
        if not arg2.isnumeric():
            arg2 = '%' + str(rm.requestReg(arg2))
    else:
        result = expToIR_R(expr.right, size, "")
        cIR = cIR.join(result['IR'])
        arg2 = result['reg']
    regv = rm.requestReg()
    stackSize = size / 8
    cIR = f'%{regv} = alloca i{size}, align {stackSize}\n'
    cIR += '%{reg} = {bOP} i{s} {a1}, {a2}\n'.format(
        reg=regv, bOP=getOpIns(expr.data['value']), s=size, a1=arg1, a2=arg2)
    return {'IR': cIR, 'reg': f'%{regv}'}


def expToIR(expr, size):
    if (expr.data['value'] == '='):
        vName = expr.left.data['value']
        nex = expr.right
        r = expToIR_R(nex, size, "")
        for e in rm.rMap:
            print(e)
            print(r['reg'][1:])
            if e[1] == int(r['reg'][1:]):
                rm.rMap.remove([e[0], e[1]])
                rm.rMap.append([vName, int(r['reg'][1:])])
    else:
        r = expToIR_R(expr, size, "")
    print(rm.rMap)
    return r['IR']

def typeSize(t):
    match t:
        case "int":
            return 32
        case "char":
            return 8
    return 32

def funcToIR(funcEle):
    body = ""
    #Args gen
    args = ""
    for e in funcEle.p.args:
        s = typeSize(e[0]['value'])
        args += "i{size} %{name}, ".format(size=s, name=rm.requestReg(e[1]['value']))
        body += '%{reg} = alloca i{size}, align {st}'.format( reg=rm.requestReg(), size=s, st = s/4)
    if (len(funcEle.p.args) > 0):
        args = args[:-2]
    
    size = typeSize(funcEle.p.type)
    name = funcEle.p.name
    body  += funcEle.b.sabina()

    for j in 

    IRString = f"define i{size} @{name}({args}){{\n" + body + "}"
    return IRString