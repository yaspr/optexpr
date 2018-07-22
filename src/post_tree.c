#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 1024

#include "hash.h"
#include "tree.h"
#include "lstack.h"
#include "rstack.h"
#include "asm.h"
#include "error.h"
#include "io.h"

//
int is_digit(char c)
{ return (c >= '0' && c <= '9'); }

//
int is_alpha(char c)
{ return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')); }

//
int to_digit(char c)
{ return c - '0'; }

//
int is_opr(char c)
{
  switch (c)
    {
    case '+' :
    case '-' :
    case '*' :
    case '/' :
    case '%' :
    case '^' :
    case '=' :
      return 1;
    
    default :
      return 0;
    }
}

//
int get_number(char *expr, int *pos)
{
  int val = 0;
  
  while (is_digit(expr[(*pos)]))
    {
      val *= 10;
      val += to_digit(expr[(*pos)++]);
    }

  return val;
}

//
int get_string(char *expr, char *str, int *pos)
{
  int i = 0;

  while (is_alpha(expr[(*pos)]))
    str[i++] = expr[(*pos)++];
    
  str[i] = '\0';

  return i; //Return string's length (could be useful) !
}

/*

  Handles expression optimization for better register allocation.
  Register allocation is done using Strahler numbers.
  Left leaves need only 1 register when the right ones need none.
  The parent nodes get the maximum value plus one if the children need 
  similar number of registers. Of course, opcodes have a limited number
  of registers as parameters OPC_PRM (defined to 2).

  Example : x * (y + 1) --> x,y,1,+,*

                                     * (2)
                                   /   \
                              (1) x     + (1)
                                       / \
                                  (1) y   1 (0)

   Certain optimizations can be done to minimize the number of registers needed 
   by swaping children for certain associative operations, mainly add & mul.
   The swapping is invalid for foating point operations.
 */

leaf *build(char *expr, hashtab *h)
{
  lstack s;
  char opr;
  int i = -1, num;
  char str[MAX_STR];
  leaf *l = NULL, *r = NULL, *tmp = NULL;
  
  lstack_init(&s, 1024);
  
  while (expr[++i])
    {
      if (is_digit(expr[i]))
	{
	  num = get_number(expr, &i);
	  l = new_num_leaf(num, INT, NULL, NULL, -1);
	  lpush(&s, l);
	}
      else
	if (is_opr(expr[i]))
	  {
	    lpop(&s, &r);
	    lpop(&s, &l);
	    
	    //Optimization : reducing the amount of registers !!
	    if ((expr[i] == '+' || expr[i] == '*') && l->typ == INT && r->typ != INT) 
	      tmp = l, l = r, r = tmp;

	    l = new_num_leaf(expr[i], CHAR, l, r, get_reg_num(l, r));
	    lpush(&s, l);
	  }
	else
	  if (is_alpha(expr[i]))
	    {
	      get_string(expr, str, &i);
	      
	      if (search(h, str))
		{
		  lpop(&s, &r);	  
		  r->reg = 1; //Premice to register allocation !!!
		  l = new_str_leaf(str, FUNC, NULL, r, 1);
		  lpush(&s, l);
		}
	      else //If not a function then a variable identifier ! [extend the grammar]
		{
		  l = new_str_leaf(str, VAR, NULL, NULL, -1);
		  lpush(&s, l);
		}
	    }
    }
   
  lstack_free(&s);

  return l;
}

//
int main(int argc, char **argv)
{  
  //No checks, make sure the parameters are OK, otherwise SEGFAULT!
  if (argc < 6)
    return printf("%s [outfile.dot] [graph name] [reg file] [x86 file] [postfix expression]\n", argv[0]), 0;
  
  hashtab h;
  
  hashtab_init(&h, hash_2, 20);
  
  insert(&h, "ln");
  insert(&h, "log");
  insert(&h, "exp");
  insert(&h, "cos");
  insert(&h, "sin");
  insert(&h, "tan");
  insert(&h, "cosh");
  insert(&h, "sinh");
  insert(&h, "acos");
  insert(&h, "asin");
  insert(&h, "cotan");

  tree t = build(argv[6], &h);
  
  printf("Expr : ");
  print_expr(t);
  printf("\n\n");

  printf("Tree :\n");
  print_tree(t);

  //Fictional register machine code generation
  gen_reg_asm_code(argv[3], t);
  
  //x86 machine code generation
  gen_x86_asm_code(argv[4], t);

  //Stack machine code generation
  gen_stack_code(argv[5], t);

  printf("\nParallel sub expressions : \n");
  leaf *l;
  lstack *s = get_par_subtrees(t);

  //Write the AST with some nice colors
  write_tree_dot(argv[1], argv[2], t, s); 
  
  while (lpop(s, &l))
    print_tree(l), printf("\n");

  lstack_free(s);
  hashtab_free(&h);

  return 0;
}

