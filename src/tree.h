//stdout print format
#define PRN_INT_FRMT "%d (%d)\n" 
#define PRN_CHR_FRMT "%c (%d)\n"
#define PRN_STR_FRMT "%s (%d)\n"

//file write format
#define WRT_INT_FRMT "%d [label=\"%d\" color=\"%s\"]\n"
#define WRT_CHR_FRMT "%d [label=\"%c\" color=\"%s\"]\n"
#define WRT_STR_FRMT "%d [label=\"%s\" color=\"%s\"]\n"

typedef enum { INT, FLOAT, CHAR, FUNC, VAR } type;

//Tree nodes !
typedef struct leaf* tree;
typedef struct leaf { int val; char str[MAX_STR]; type typ; int reg; tree left; tree right; } leaf;

//int & char
leaf *new_num_leaf(int val, type typ, leaf *left, leaf *right, int reg)
{
  leaf *l = (leaf *)malloc(sizeof(leaf));
  
  l->reg = reg;
  l->typ = typ;
  l->val = val;
  l->left = left, l->right = right;

  return l;
}

//String only
leaf *new_str_leaf(char *val, type typ, leaf *left, leaf *right, int reg)
{
  leaf *l = (leaf *)malloc(sizeof(leaf));
  
  l->reg = reg;
  l->typ = typ;
  strcpy(l->str, val);
  l->left = left, l->right = right;

  return l;
}

//
void print_tree_(leaf *l, int d)
{
  if (l)
    {
      for (int i = 0; i < d; i++)
	printf("|");
      
      if (l->typ == FUNC || l->typ == VAR)
	printf(PRN_STR_FRMT, l->str, l->reg);
      else
	printf((l->typ == INT) ? PRN_INT_FRMT : PRN_CHR_FRMT, l->val, l->reg);
      
      print_tree_(l->left, d + 1);
      print_tree_(l->right, d + 1);
    }
}
  
//
void print_tree(leaf *l)
{ print_tree_(l, 0); }

//
void print_expr(leaf *l)
{
  if (l)
    {
      printf("(");
      print_expr(l->left);
      
      if (l->typ == FUNC || l->typ == VAR)
	printf("%s", l->str);
      else
	printf((l->typ == INT) ? "%d" : "%c", l->val);	

      print_expr(l->right);
      printf(")");
    }
}
