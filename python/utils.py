def findNextDelim(tokens, start):
    for i in range(start, len(tokens)):
        if (tokens[i]['value'] == ";"):
            return i
    return False