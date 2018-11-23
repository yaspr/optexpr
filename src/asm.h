//
#define max(a, b) (a > b) ? (a) : (b) //
#define min(a, b) (a < b) ? (a) : (b) //

#define OPC_PRM 2 //

//
typedef struct reg { char stat; char str[MAX_STR]; } reg;
 
//
char *opcodes[MAX_STR] = { "add", "sub", "mul", "div", "mod", "mov" };

//
int get_opr_code(char c)
{
  switch (c)
    {
    case '+' :
      return 0;

    case '-' :
      return 1;

    case '*' :
      return 2;

    case '/' :
      return 3;

    case '%' :
      return 4;

    case '=' :
      return 5;
    }
}

//
int get_free_reg(reg *regs, int s)
{  
  for (int i = 0; i < s; i++)
    if (regs[i].stat) 
      return i;
  
  return -1;
}

//
static inline int free_reg(reg *regs, int i)
{ regs[i].stat = 1; }

//
static inline int lock_reg(reg *regs, int i)
{ regs[i].stat = 0; }

/*
  
  Assembly code generation with register allocation.
  This function scares the F out of me.

  rstack is an integer stack for registers, it stores the registers'
  positions in regs !

  For x86 ssembly code change regs to hold : eax, ebx, ecx, edx, ...

  TODO:
        For x86 assign ecx to inc & dec x + 1, x++, x - 1, x--, ...
	Support for x86 SIMD (SSE - xmm (128b), AVX - ymm(256), KNC - zmm(512))

*/
//Instead of printf print to file !
void gen_reg_asm_code_(FILE *fd, leaf *l, reg *regs, int regs_s, rstack *s)
{
  static int tmp1, tmp2;
  
  if (l && l->right)
    {
      gen_reg_asm_code_(fd, l->left, regs, regs_s, s);
      gen_reg_asm_code_(fd, l->right, regs, regs_s, s);

      //Register allocation is HELL !
      if (l->typ == CHAR)
	{
	  //OPCODE REG REG
	  if (l->reg == OPC_PRM)
	    {
	      rpop(s, &tmp1), rpop(s, &tmp2); 
	      fprintf(fd, "%s %s, %s\n", opcodes[get_opr_code(l->val)], regs[tmp2].str, regs[tmp1].str);
	      //Release right register tmp1
	      free_reg(regs, tmp1);
	    }
	  else //OPCODE REG VAL or OPCODE REG MEM
	    {
	      rpop(s, &tmp2);
	      fprintf(fd, "%s %s, ", opcodes[get_opr_code(l->val)], regs[tmp2].str);
	      fprintf(fd, (l->right->typ == VAR) ? "$(%s)\n" : "%d\n", 
		      (l->right->typ == VAR) ? l->right->str : l->right->val);
	    }
	
	  rpush(s, &tmp2);
	}
      else
	if (l->typ == FUNC)
	  {
	    rpop(s, &tmp1);
	    fprintf(fd, "push %s\ncall %s\n", regs[tmp1].str, l->str);
	    rpush(s, &tmp1);
	  }      
    }
  else
    if (l) //If leaf
      if (l->reg) //Left leaf has always reg to one !
	{
	  tmp1 = get_free_reg(regs, regs_s);
	  
	  if (tmp1 >= 0)
	    {
	      fprintf(fd, "mov %s, ", regs[tmp1].str);
	      lock_reg(regs, tmp1); //Set register as dirty (or busy) !
	      rpush(s, &tmp1);
	      fprintf(fd, (l->typ == VAR) ? "$(%s)\n" : "%d\n", (l->typ == VAR) ? l->str : l->val);
	    }
	  else
	    fprintf(fd, "No more registers available !!!\n");
	}
}

//8 register fictional machine (0 to 15)
void gen_reg_asm_code(char *fname, leaf *l)
{
  rstack s;
  FILE *fd = fopen(fname, "wb");
  reg regs[8] = { {1, "r1"}, {1, "r2"}, {1, "r3"}, {1, "r4"}, 
		  {1, "r5"}, {1, "r6"}, {1, "r7"}, {1, "r8"} };
  
  rstack_init(&s, 1024);
  gen_reg_asm_code_(fd, l, regs, 8, &s);
  rstack_free(&s);
  
  fclose(fd);
}

//Specific function for x86 inspired assembly
void gen_x86_asm_code(char *fname, leaf *l)
{
  rstack s;
  FILE *fd = fopen(fname, "wb");
  reg regs[9] = { {1, "eax"}, {1, "ebx"}, {1, "ecx"}, {1, "edx"}, 
		  {1, "esi"}, {1, "edi"}, {1, "ebp"}, {1, "eip"}, 
		  {1, "esp"} };

  rstack_init(&s, 1024);
  gen_reg_asm_code_(fd, l, regs, 9, &s);
  rstack_free(&s);

  fclose(fd);
}

//
void gen_stack_code_(FILE *fd, leaf *l)
{
  if (l && l->right)
    {
      gen_stack_code_(fd, l->left);

      leaf *lft = l->left, *rgt = l->right;
      
      if (lft && lft->typ == INT)
	fprintf(fd, "push %d\n", lft->val);
      else
	if (lft && lft->typ == VAR)
	  fprintf(fd, "push $(%s)\n", lft->str);
      
      if (rgt && rgt->typ == INT)
	fprintf(fd, "push %d\n", rgt->val);
      else
	if (rgt && rgt->typ == VAR)
	  fprintf(fd, "push $(%s)\n", rgt->str);

      gen_stack_code_(fd, l->right);

      fprintf(fd, "%s\n", (l->typ == CHAR) ? opcodes[get_opr_code(l->val)] : (l->typ == FUNC) ? l->str : "");
    }
}

//
void gen_stack_code(char *fname, leaf *l)
{
  FILE *fd = fopen(fname, "wb");
  
  gen_stack_code_(fd, l);

  fclose(fd);
}

//
int get_reg_num(leaf *l, leaf *r)
{
  int reg = -1;

  if (!l->left && !l->right)
    l->reg = 1;

  if (!r->left && !r->right)
    r->reg = 0;

  reg = max(l->reg, r->reg) + ((l->reg == r->reg) && (l->reg < OPC_PRM));
  
  /*
    Slight correction for the optimization.
    
    An expression such as : y,1,+,2,y,3,-,z,ln,/,*,+ will be optimized for 
    register use minimization by permutating operands of associative operators.
    The AST will be representing the equivalent expression : y,1,+,y,3,-,z,ln,/,2,*,+.

    The expression y,1,+,2,y,3,-,z,ln,/,*,+ has the AST shown below :

             +(2)
        /         \
   (1)+            *(2)
     / \          / \ 
 (1)y   1     (1)2  '/'(2)  
                    / \ 
                (1)-   ln(1)
                  / \   \
              (1)y   3   x(1)
     
     You can notice that the amount of registers used for the multiplication 
     is 2 where only 1 register is needed. The AST below shows how permutating 
     the operands will reduce the amount of registers needed from 1 to 0 for the operand 2
     and with a sligh correction, reduce the numbers of registers needed by the multiplication 
     to one. This is important when only few registers are available. 
     
     y,1,+,y,3,-,z,ln,/,2,*,+

             +(2)
        /         \
   (1)+            *(1)
     / \          / \ 
 (1)y   1    (2)'/'  2  
                / \ 
            (1)-   ln(1)
              / \   \
          (1)y   3   x(1)
     
     This function calculates the amount of registers needed
   */    
  
  reg -= ((l->reg == 2) && (r->reg == 0));

  //
  return reg;
}

//Parallel expressions ! [Parallelism will induce an extensive use of registers]
void get_par_subtrees_(leaf *l, lstack *s)
{
  if (l && l->right)
    {
      get_par_subtrees_(l->left, s);
      get_par_subtrees_(l->right, s);
      
      leaf *lft = l->left, *rgt = l->right;
      
      //This is a hideous test !!
      //Push the @ in a stack or a list instead of printing & return the stack or the list
      if ((lft || l->typ == FUNC) && rgt)
	if (((lft && !lft->left && !lft->right) || l->typ == FUNC) && !rgt->left && !rgt->right)
	  lpush(s, l); //Push the root in a stack !
    }
}

//
lstack *get_par_subtrees(leaf *l)
{
  lstack *s = (lstack *)malloc(sizeof(lstack));
  
  lstack_init(s, 1024);
  get_par_subtrees_(l, s);
  
  return s;
}
