# dalg
 This programming language is based on LLVM's Kaleidoscope example. It supports only the "double" data type. The syntax will be simplified over time, but for now, it follows a C-style syntax. Currently, the following features are supported:
   + Functions
   + Function Call
   + Variable Assign
   + If-else Expression
   + Print Support ( linked to printf)
   + Strings

### Syntax:
     
 ```
 fn s() {
    4 * 2 + 5 - 3
}

fn v(a, b, c) {
    a * b / c + 4
}

fn f(c) {
    a = 3;
    b = 4;
    a + b * 2 + c
}

fn pr() {
    a = f(4) / 2
}

fn cmp() {
    a = 5;
    b = 3;
    if (a < b) {
        print(a * 4)
    } else {
        print(b - 6)
    }
}

fn str() {
    cmp()
    print("blah blah blah")
}
```

## Usage 
````
  Generate LLVM IR Code-> dalg.exe input.dalg output.ll
````
````
  Executable file -> clang.exe output.ll -o output.exe
 ````  

### To-Do
  + For loop expression
  + Operator precedence
  + Global Variable
  + Better Error Messages
  + Automatic parallelization
  + More libc linking

    
