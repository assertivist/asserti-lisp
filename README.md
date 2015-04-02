# asserti-lisp
My Toy Lisp - www.buildyourownlisp.com

# Building
This is a really simple, small project. There is an included makefile. You need a C 
compiler and `libreadline`/`libeditline`. Just typing `make` will get you a binary 
called `al`, running this will give you a REPL.

    user@compy:~/code/asserti-lisp$ ./al
    assertilisp version 0.01
    ^C to quit

    al> (+ 3 4)
    7
    al> (\ {x y} {+ x y}) 10 20
    30
    al>

# Wandows
You'll need to remove `-ledit` from the compilation arguments on Windows (no readline 
there) but it will still build (tested with a recent version of MingW only).
