from tabulate import tabulate
import json
from collections import deque
import sys


def findNextDelim(tokens, start):
    for i in range(start, len(tokens)):
        if (tokens[i]['value'] == ";"):
            return i
    return False


rMap = []
nnReg = -1
usedFunc = [""]


def getReg(varN):
    for ev in rMap:
        if varN in ev:
            return ev[1]


def requestReg(varN=None):
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


def getOpIns(operator):
    match operator:
        case "+":
            return "+"
        case "-":
            return "-"
        case "*":
            return "*"
        case "/":
            return "/"


def isLeaf(expr):
    if(expr.left or expr.right):
        return False
    else:
        return True


def expToIR_R(expr, size, cIR=""):
    if isLeaf(expr.left):
        arg1 = expr.left.data['value']
        if not arg1.isnumeric():
            arg1 = 'v' + str(requestReg(arg1))
    else:
        result = expToIR_R(expr.left, size, "")
        cIR = cIR.join(result['IR'])
        arg1 = result['reg']
    if isLeaf(expr.right):
        arg2 = expr.right.data['value']
        if not arg2.isnumeric():
            arg2 = 'v' + str(requestReg(arg2))
    else:
        result = expToIR_R(expr.right, size, "")
        cIR = cIR.join(result['IR'])
        arg2 = result['reg']
    regv = requestReg()
    cIR += '{s} v{reg} = {a1} {bOP} {a2};\n'.format(
        reg=regv, bOP=getOpIns(expr.data['value']), s=size, a1=arg1, a2=arg2)
    return {'IR': cIR, 'reg': f'v{regv}'}


def expToIR(expr, size):
    if (expr.data['value'] == '='):
        vName = expr.left.data['value']
        nex = expr.right
        r = expToIR_R(nex, size, "")
        for e in rMap:
            if e[1] == int(r['reg'][1:]):
                rMap.remove([e[0], e[1]])
                rMap.append([vName, int(r['reg'][1:])])
    else:
        r = expToIR_R(expr, size, "")
    return r['IR']


def typeSize(t):
    match t:
        case "int":
            return "int"
        case "char":
            return "char"
    return 32


def funcToIR(funcEle):
    args = ""
    for e in funcEle.p.args:
        args += "{size} v{name}, ".format(size=typeSize(
            e[0]['value']), name=requestReg(e[1]['value']))
    if (len(funcEle.p.args) > 0):
        args = args[:-2]

    size = typeSize(funcEle.p.type)
    name = funcEle.p.name
    body: str = funcEle.b.sabina()

    IRString = f"{size} {name}({args}){{\n" + body + "}"
    return IRString


types = []


class functionProto:

    def __init__(self, returnType, name, args):
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
            rString += expToIR(exp.root, 'int')
        return rString


class function:
    def __init__(self, prototype, body):
        self.p = prototype
        self.b = body

    def generateIR(self):
        return funcToIR(self)

    def __repr__(self):
        return str(self.p) + "\n" + str(self.b.sabina())


def parseFunction(input):
    # Validate Function
    if (input[0]['type'] != 'Type Token' or input[2]['value'] != "("):
        return False

    # Parse Prototype
    returnType = input[0]['value']
    funcName = input[1]['value']
    args = []
    pScope = 1
    i = 3
    while pScope != 0:
        if (input[i]['value'] == ")"):
            pScope = pScope - 1
        elif (input[i]['value'] == "("):
            pScope = pScope + 1
        elif (input[i]['type'] == "Type Token"):
            args.append([input[i], input[i+1]])
            i += 1
        i += 1

    proto = functionProto(returnType, funcName, args)

    # Parse Content
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

        if(findNextDelim(input, i)):
            nsmc = findNextDelim(input, i)
            expressions.append(parseExpression(input[i:nsmc]))
            i = nsmc + 1

        else:
            break
    funcBody = functionBody(expressions)
    ffunc = function(proto, funcBody)

    return ffunc


def parseExpression(tokens):
    tree_obj = BinaryExpressionTree(tokens)

    return tree_obj


opinfo = {"operators": ["+", "-", "*", "/", "%", "++", "--", "==", "!=", "<", ">", "<=", ">=", "!", "&&", "!!", "&", "|", "^", "~", "<<", ">>", "="],
          "precedence": {"=": 0, "||": 1, "&&": 2, "|": 3, "^": 4, "&": 5, "==": 6, "!=": 6, "<": 7, ">": 7, "<=": 7, ">=": 7, "<<": 8, ">>": 8, "+": 9, "-": 9, "*": 10, "/": 10, "%": 10, "!": 11, "~": 11, "++": 12, "--": 12, "::": 13},
          "associativity": {"++": "LR", "--": "LR", "*": "LR", "/": "LR", "%": "LR", "+": "LR", "-": "LR", "&&": "LR", "==": "LR", "=": "RL"}}


class binaryTreeNode(object):
    def __init__(self, data: dict):
        self.data = data
        self.right = None
        self.left = None
        self.operator = False

    def is_leaf(self):
        return self.left == None & self.right == None

    def __repr__(self):
        return 'ParseTreeNode({!r})'.format(self.data)


class BinaryExpressionTree(object):
    def __init__(self, expression):
        self.root = None
        self.size = 0

        if expression is not None:
            self.insert(expression)

    def __repr__(self):
        return 'BinarySearchTree({} nodes)'.format(self.size)

    def isEmpty(self):
        return self.root == None

    def insert(self, expression):
        postfixExp = self.toPostfix(expression)
        stack = deque()
        char: dict = postfixExp[0]
        node = binaryTreeNode(char)
        stack.appendleft(node)

        i = 1
        while len(stack) != 0:
            char = postfixExp[i]

            if char['type'] == "Operator Token":
                operatorNode = binaryTreeNode(char)
                operatorNode.operator = True

                rightChild = stack.popleft()  # Change here
                leftChild = stack.popleft()

                operatorNode.right = rightChild
                operatorNode.left = leftChild

                stack.appendleft(operatorNode)

                if len(stack) == 1 and i == len(postfixExp) - 1:
                    self.root = stack.popleft()
            else:

                node = binaryTreeNode(char)
                stack.appendleft(node)

            i += 1
            self.size += 1

    def itemsInOrder(self):
        items = []
        if not self.isEmpty():
            self._traverseInOrderRecursive(self.root, items.append)
        return items

    def _traverseInOrderRecursive(self, node, visit):
        if(node):
            self._traverseInOrderRecursive(node.left, visit)
            visit(node.data)
            self._traverseInOrderRecursive(node.right, visit)

    def toPostfix(self, infix: list[dict]):
        stack = []
        postfix = []

        for token in infix:
            if opinfo["operators"].count(token.get('value')) > 0:
                postfix.insert(0, token)
            else:
                if token['value'] == "(":
                    stack.insert(0, token)
                elif token['value'] == ')':
                    operator = stack.pop()
                    while token['value'] != '(':
                        postfix += operator
                        operator = stack.pop()
                else:
                    while (len(stack) > 0) and stack[0]['value'] in opinfo['precedence'] and (opinfo['precedence'][stack[0]['value']] >= opinfo['precedence'][token['value']]):
                        postfix.insert(stack.pop())
                    stack.append(token)

        while (len(stack) > 0):
            postfix.insert(0, stack.pop())
        return postfix

    def _cleanInput(self, infixExp):
        cleanExp = "".join(infixExp.split())
        cleanFormat = []

        i = 0
        while i < len(cleanExp):
            isOp = False
            char = ""
            if cleanExp[i] in opinfo['operators']:
                isOp = True
                char = cleanExp[i]
            elif cleanExp[i:i+1] in opinfo['operators']:
                isOp = True
                char = cleanExp[i:i+1]
            else:
                char = cleanExp[i]

            if isOp:
                cleanFormat.append(char)
                i += 1
            else:
                num = ""

                while char not in opinfo['operators']:
                    isOp = False
                    char = ""

                    if cleanExp[i] in opinfo['operators']:
                        isOp = True
                        char = cleanExp[i]
                    elif cleanExp[i:i+1] in opinfo['operators']:
                        isOp = True
                        char = cleanExp[i:i+1]
                    else:
                        char = cleanExp[i]

                    num += char
                    i += 1

                cleanFormat.append(num[:-1])
                cleanFormat.append(num[-1])
        return cleanFormat


treeThing = []
index = 0


def btToArr(node, arr):
    if not node:
        return
    btToArr(node.left, arr)
    arr.append(node.data)
    btToArr(node.right, arr)
    return arr


tokenJsonFile = open(sys.argv[1])
JsonData = json.load(tokenJsonFile)
tokenJson = JsonData['tokens']
typenames = JsonData['typenames']

types = typenames


class token:
    def __init__(self, type, value):
        self.type = type
        self.value = value


class parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.symbols = {'uPre': ['--', '++', '~'], 'uPost': ['--', '++'],
                        'binary': ['+', '-', '*', '/', '%', '==', '!=', '&&', '||']}

    def numExpression(self, start):
        if(False):  # Test case
            return False
        return parseExpression(self.tokens[int(start): int(findNextDelim(self.tokens, start))])

    def varCreation(self, start):
        if ((self.tokens[start]['value'] not in typenames)):
            return False
        else:
            returnValue = {'type': self.tokens[start]['value'],
                           'name': self.tokens[start + 1]['value'], 'assignment': {}}

            return returnValue

    def FuncDef(self, start):
        if ((self.tokens[start]['value'] not in typenames) or (self.tokens[start + 2]['value'] != "(")):
            return False
        else:
            returnValue = {'returnType': self.tokens[start]['value'],
                           'name': self.tokens[start + 1]['value'], 'args': {}}
            pScope = 1
            i = start + 3
            while True:
                if (self.tokens[i]['value'] == ")"):
                    break
                else:
                    if (self.tokens[i]['type'] == "Type Token"):
                        returnValue['args'][self.tokens[i]['value']
                                            ] = self.tokens[i + 1]['value']
                        i += 2

            return returnValue


tokenList = []

for entry in tokenJson:
    tokenList.append(token(entry['type'], entry['value']))

mainParser = parser(tokenJson)


print(parseFunction(tokenJson).generateIR())
