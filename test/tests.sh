rm *.dot *.reg *.x86 *.png

post_tree expr1.dot expr1 expr1.reg expr1.x86 expr1.stk y,1,-,3,6,*,/,y,1,+,2,y,3,-,x,6,+,cos,*,x,ln,/,*,+,+,x,x,1,-,ln,sin,-,*,21,33,y,1,+,y,3,-,x,/,2,*,+,cos,ln,-,*,+

post_tree expr2.dot expr2 expr2.reg expr2.x86 expr2.stk y,1,-,3,6,*,/,y,1,+,2,y,3,-,x,6,+,cos,*,x,ln,/,*,+,+,x,x,1,-,ln,sin,-,*

dot -Tpng expr1.dot -o expr1.png
dot -Tpng expr2.dot -o expr2.png
