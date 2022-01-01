import regex as re

values = ["pp", "sabina"]

regex = re.compile(r"return \L<values>", values=values)
teststr = "ret pp"

print(regex.search(teststr))