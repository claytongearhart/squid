import json
import queue
from collections import deque


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
