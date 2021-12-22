import sys
import json

tokenJsonString = open('tokens.json', "r")
tokens = json.loads(tokenJsonString.read())

#print(json.dumps(tokens, indent=4))

# def functionDec(input: json):
#     if (input[0]['type'] == "Keyword Token" & input[2]['value'] == "("):
#         s


def varAssignment(input: json):
    if (input[0]['type'] == "Type Token" and input[2]['value'] == "="):
        statement = {'type': 'Object Assignment',
                     'Object Type': input[0]['type'], 'Object Value': input[3]['value']}

