# IB Project Progress Log

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
Fixed `isInArray` function, is now working as expected, however switched to vectors instead of arrays do to pointer mess. Organized repo, moved docs from Notion to markdown hosted in this repo.