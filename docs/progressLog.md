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
Got get child by index vector working. My idea of using a for loop and reference didn't work. Instead I had to use some new c++20 features like `std::span`. I thought that migrating from XML to JSON would be cleaner so I started work on that.

### 2021-10-28 2 Hours
Worked on implementing AST parsers, struglled with segmentation faults / illegal memory access. I tried to find the problem using a debugger but that didn't work. I observed that it was less likley to segfault on a cold run. Also, sometimes instead of throwing a segfault it would throw a c++ vector access runtime error. Another error that got thrown, albiet less frequently, was `type: vector std::length_error`

### 2021-10-29 4 Hours
Got segfaults fixed(mostly), will still cause if input file is above a certian length which is pretty curious. Then worked on adding more information such as token type. I ran into an error here with explicit conversion which led me to having my object class accepting types other than `std::string`. This lead me down a rabbit-hole of exploring different constructors types until I finally found the conversion constructor I need for the `node` class.

### 2021-10-30 - 2 Hours
I spent a lot of time trying to get child class to work, ended giving up on it. However I decided to solve my problem by converting enums to string when they first get inserted and had the idea of overloading the constructor on the `node` class instead of using std::variants which have been the cause of so many problems. 

### 2021-11-2 - 1 Hour
I had the idea to give up deserializing raw scope depth object and instead only serialize more complex object with word types and an more context of the content. I then wrote the necessary code for it

### 2021-11-4 - NaN
<<<<<<< HEAD
I brainstormed ideas for entire word namespace. I want to utilize inheritance and polymorphism because I've never used them before. I'm planning on having base classes of `word` and `objectCall` and then have derived objects for every `word` such as `variable` and `function` derived from `word` and have objects such as `variableAccess` and `function` will be derived from `objectCall`.

## 2021-11-27 - 3 Hours
While I was trying to figure out my AST, I noticed that most of the publications I read did everything quite a bit differently that I had so I decided to overhaul everything. I consolidated my scanner and preprocessor into one source file that compiles to one executable and I plan on writing my AST in python because of there are more resources also I think the implicit and dynamic nature of python will help.
=======
I brainstormed ideas for a `word` namespace. I want to utilize inheritance and polymorphism so I'm planning on having base classes of `word` and `objectCall` and then have derived objects for every `word` such as `variable` and `function` derived from `word`. I would have objects such as `variableAccess` and `function` will be derived from `objectCall`. 

## 2021-11-28 - NaN
Over the past few weeks I've had lots of problems with implementing my AST. First I thought to completed rewrite my AST by just having structs with pointers for sub structs to make a binary tree because that's how I solved the problem for my lexer. Then I looked for other ways and stumbled across a way to do it in python, which looked intruging because python is a high level lanuage so it would be easier to implement things like class objects representing words because I can dynamically allocated and define them. To get started on this I first thought to write the IPC layer, which would be started by the master script at runtime and called via a shell pipe to read a file written by the main proccess and would then process and output a processed json or xml file. Once I actually started work I found I couldn't use a shell pipe like I orginally wanted, so I compromised by only invoking the python script once the file had been outputed by the main process. Once I had finished this is only got more difficult. I couldn't even begin to make the actual AST. After sevral hours of googling I still couldn't figure out how to use both binary and unary operators in the same block.

## 2021-12-02
I still can't figure out how to make the AST, I've been thinking about switching my project to something still realted to computer science but not this low level, like a search engine.
>>>>>>> c4967248bdd5ecdc1b3b97d1b851988ac0b9ae16
