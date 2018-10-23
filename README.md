# optexpr
Arithmetic expressions optimizer  

-- Check out test.sh in the test directory for info on how to call the program.

This program takes a postfixed arithmetic expression and generates its AST and some assembly codes (simplified x86, 
stack machine, register machine). It also demonstrates some register saving techniques along with parallel sub-expressions
detection.

Suppose the following expression: x * 2 - ln(y) + (3 * 5)
The input expression of the program can be the following: x, 2, *, y, ln, -, 3, 4, *, + 
The program will build a dot format tree similar to this one:

                       +  
                   /       \
                  -         *
                /   \      /  \      
               *     ln   3    4
             /   \     \
            x     2     y

From the IR of the tree, the program will detect operations that can be performed in parallel, 
for example: (x * 2), ln(y), and (3 * 4) will be represented in a different color inside the dot graph to express
this parallelism. 

It would be nice to add constant propagation and immediate operation simplification. For example, replacing 3 * 4 by 12 
resulting in the following tree:

                       +  
                   /       \
                  -        12
                /   \           
               *     ln   
             /   \     \
            x     2     y

-- Compilation & running:

   #cd src
   #make
   #cd ..
   #cd test
   #./tests.sh -- make sure dot is installed if you want graphical images
   