import pyparsing as pp
from tabulate import tabulate
import utils as util
import expTree as et
import IRGen as IR
import varMap as rm

types = []


class functionProto:

    def __init__ (self, returnType, name, args ):
        self.type = returnType
        self.name = name
        self.args = args

    def __repr__(self):
        return f"Return Type: {self.type}\nFunction Name: {self.name}\nArgs:\n" + tabulate(self.args, headers=['Type', 'Name'])

class functionBody:
    def __init__(self, exps):
        self.exps = exps
    def sabina(self):
        rString = ""
        for exp in self.exps:  
            rString += IR.expToIR(exp.root, 'int')
        return rString

class function:
    def __init__(self, prototype, body):
        self.p = prototype
        self.b = body
    def generateIR(self):
        return IR.funcToIR(self)
    def __repr__(self):
        return str(self.p) + "\n" + str(self.b.sabina())

def parseFunction(input):
    #Validate Function
    if (input[0]['type'] != 'Type Token' or input[2]['value'] != "("):
        return False

    #Parse Prototype
    returnType = input[0]['value']
    funcName = input[1]['value']
    args = []
    pScope = 1
    i = 3
    while pScope != 0:
        print(input[i])
        if (input[i]['value'] == ")"):
            pScope = pScope - 1
        elif (input[i]['value'] == "("):
            pScope = pScope + 1
        elif (input[i]['type'] == "Type Token"):
            args.append([input[i], input[i+1]])
            i += 1
        i += 1
    
    proto = functionProto(returnType, funcName, args)

    #Parse Content
    expressions = []
    bScope = 1
    i += 1
    j = i
    while bScope != 0:
        if (input[j]['value'] == '{'):
            bScope += 1
        elif (input[j]['value'] == '}'):
            bScope -= 1
        j += 1
    j += 1
    while i < j:
        print(f"{i}, {j}")
        print(util.findNextDelim(input, i))
        if(util.findNextDelim(input, i)):
            nsmc = util.findNextDelim(input, i)
            expressions.append(et.parseExpression(input[i:nsmc]))
            i = nsmc + 1
            print(expressions)
            print(i)
        else:
            break
    funcBody = functionBody(expressions)
    ffunc = function(proto, funcBody)
    print(rm.rMap)

    return ffunc
