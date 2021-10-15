//
// Assembler program to print "Hello World!"
// to stdout.
//
// X0-X2 - parameters to linux function services
// X16 - linux function number
//
.global _start             // Provide program starting address to linker
.align 2

// Setup the parameters to print hello world
// and then call Linux to do it.

_start: mov X0, #1     // 1 = StdOut
        adr X1, mikeHawk // string to print
        mov X2, #3   // length of our string
        mov X16, #4     // MacOS write system call
        svc 0     // Call linux to output the string

        mov X0, #1
        adr X1, cock
        mov X2, #4
        mov X16, #4
        svc 0

// Setup the parameters to exit the program
// and then call Linux to do it.

        mov     X0, #0      // Use 0 return code
        mov     X16, #1     // Service command code 1 terminates this program
        svc     0           // Call MacOS to terminate the program

mikeHawk:      .ascii "PP\n" 
.align 2
cock:          .ascii "PPP\n" 