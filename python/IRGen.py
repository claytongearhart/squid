import numpy as np
from itertools import zip_longest

varMap = {}




def flatten(nested_list):
    return zip(*_flattengen(nested_list)) # in py3, wrap this in list()

def _flattengen(iterable):
    for element in zip_longest(*iterable, fillvalue=""):
        if isinstance(element[0], list):
            yield from _flattengen(element)
        else:
            yield element


def getLeavesAtDepth_R(node, depth):
    leaves = []

    if depth > 0:
        if node.left:
            leaves.append(getLeavesAtDepth_R(node.left ,depth - 1))
        if node.right:
            leaves.append(getLeavesAtDepth_R(node.right, depth - 1))
    else:
        leaves.append(node.data)
    return leaves


def getLeavesAtDepth(node, depth):
    cleanLeaves = []
    noisyLeaves = getLeavesAtDepth_R(node, depth)


    cleanLeaves = np.array(list(flatten(noisyLeaves)), dtype=object)
    cleanLeaves = cleanLeaves.flatten()

    cl2 = np.array([])

    for e in cleanLeaves:
        if e:
            cl2 = np.append(cl2, e)
        else:
            continue 

    return cl2

def treeDepth(node):
    if not node:
        return -1

    else:
        lDepth = treeDepth(node.left)
        rDepth = treeDepth(node.right)

        if (lDepth > rDepth):
            return lDepth + 1
        else:
            return rDepth + 1

def exprToIR(expr):
    i = treeDepth(expr)
    expA = [[]]
    while i > 0:
        expA.append(getLeavesAtDepth(i))
        i -= 1