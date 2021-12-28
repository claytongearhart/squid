import json
import expTree
import numpy as np
from itertools import zip_longest
import binarytree

treeThing = []
index = 0
def btToArr(node, arr):
    if not node:
        return
    btToArr(node.left, arr)
    arr.append(node.data)
    btToArr(node.right, arr)
    return arr


tokenJsonFile = open("tokens.json")
JsonData = json.load(tokenJsonFile)
tokenJson = JsonData['tokens']
typenames = JsonData['typenames']

class token:
    def __init__(self, type, value):
        self.type = type
        self.value = value

class parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.symbols = {'uPre': ['--', '++', '~'], 'uPost': ['--', '++'], 'binary': ['+', '-', '*', '/', '%', '==', '!=', '&&', '||']}

    def findNextDelim(self, start):
        for i in range(start, len(self.tokens)):
            if (self.tokens[i]['type'] == "Delimiter Token"):
                return i
        raise IndexError("Missing Semicolon")
    
    def numExpression(self, start):
        if(False): # Test case
            return False
        return expTree.parseExpression(self.tokens[int(start): int(self.findNextDelim(start))])
            

    def varCreation(self, start):
        if ((self.tokens[start]['value'] not in typenames)):
            return False
        else:
            returnValue = {'type': self.tokens[start]['value'], 'name': self.tokens[start + 1]['value'], 'assignment': {}}

            return returnValue

    def FuncDef(self, start):
        if ((self.tokens[start]['value'] not in typenames) or (self.tokens[start + 2]['value'] != "(")):
            return False
        else:
            returnValue = {'returnType': self.tokens[start]['value'], 'name': self.tokens[start + 1]['value'], 'args': {}}
            pScope = 1
            i = start + 3
            while True:
                if (self.tokens[i]['value'] == ")"):
                    break
                else:
                    if (self.tokens[i]['type'] == "Type Token"):
                        returnValue['args'][self.tokens[i]['value']] = self.tokens[i + 1]['value']
                        i += 2

            return returnValue



tokenList = []

for entry in tokenJson:
    tokenList.append(token(entry['type'], entry['value']))

mainParser = parser(tokenJson)


tree = binarytree.build(btToArr(mainParser.numExpression(0).root, []))
graph = tree.graphviz()
graph.body
graph.render()

print()
print(mainParser.numExpression(0).root)