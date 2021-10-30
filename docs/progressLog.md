# PP Progress Log

### 2021-9-03 2 Hours
Worked on scanner. Learned about array manipulation(I had always used vectors before) and got to use templates. Started work on checking token type via dynamic programming. Implemented sanitization of raw input.

### 2021-9-07 1.5 Hours
Created main function for entry. Continued work on token type checker. Worked on organization and readablity. Wrapped scanner functions in class.

### 2021-9-08 3 Hours
Researched programming lanuage theory, type theory, and semantics. Cleaned up `scanner.hpp` Did lots of debugging, fixed the segfaults. Started work on cleaning up the if else mess of the seperator.

### 2021-9-09 2 Hours
Started implementing stage 1 abstract syntax tree. Worked on `tree` class, was a debugging nightmare of pointers. Implmented http://github.com/kpeeters/tree.hh/ instead of custom class.

### 2021-9-10 3 Hours
Cleaned up if else chain in the token check system. Started fixing token detector, better but isn't perfect. Worked on the `isInArray()` function, which seems to be the cause of the forementioned problem.

### 2021-9-11 3 Hours
Fixed `isInArray` function, is now working as expected, however switched to vectors instead of arrays do to pointer mess. Organized repo, moved docs from Notion to markdown hosted in this repo. Switched `push_back` to `emplace_back` where useful, greatly improved peformance.

### 2021-9-13 1.5 Hours
Fixed `sanatizeTokens` function so it no longer removes spaces inside string, created `stringLocations` and `isInString` functions to aid the `sanatizeTokens` function and others, may use to make an more optimized token generator.

### 20212-9-20 - 2 Hours
Worked on ways to fix strings, did not succed. Although did learn the ins and outs of a lot of std memebers

### 2021-9-21 - 1 Hour
Contiuned to work on ways to fix strings, did not succed but came of with idea to work on, when I have time.

### 2021-9-24 - 3 Hours
Found out problem with ways to fix string, went to the source and making broken strings and fixing them later make good strings at start. Found it `isInString()` function had some errors and created a wrapper around `std::string::find_first_of()` and evreything works now.

### 2021-10-14 - 5 Hours
Thought of ways to implement basic tree structure. At first I thought of using JSON to implement but using third party json libaries was a pain, I ran into issues with accessing child nodes recursivly. I then tried to use third party xml libaries, and I ran into the same problem. I ended up making my own xml like class. At first I was thinking of storing types in a vector with some sort of location table but I ended up making a `node` class that contains a vector of `std::variant`s of strings or nodes. I then overloaded the index operator. 

### 2021-10-21 - 2.5 Hours
Got get child by index vector working. My idea of using a for loop and reference didn't work. Instead I had to use some new c++20 features like std::span. I thought that migrating from XML to JSON would be cleaner so I started work on that.

### 2021-10-28 2 Hours
Worked on implementing AST parsers, struglled with seg faults. I tried to find the problem using a debugger but that didn't work. Sometimes if it hadn't been run in a while it wouldn't segfault. Also, sometimes instead of throwing a segfault it would throw a c++ vector access runtime error. Another error that got thrown, albiet less frequently, was type: vector std::length_error

### 2021-10-29 4 Hours
Got segfaults fixed(mostly), will still cause if input file is above a certian length which is pretty curious. Then worked on adding more information such as token type. I ran into an error here with explicit conversion which led me to having my object class accepting more types than just strings. This lead to down a rabbit-hole of exploring different types of constructors until I finally found the conversion constructor I need.

### 2021-10-30 
Spent a lot of time trying to get child class to work, ended giving up on it. However I decided to solve my problem by converting enums to string when they first get inserted and had the idea of overloading the constructor on the `node` class instead of using std::variants which have been the cause of so many problems. 