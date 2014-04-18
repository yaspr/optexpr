//Leaf Stack
typedef struct lstack { leaf **val; int sp; int MAX_STACK; } lstack;

//
int lstack_init(lstack *s, int size)
{
  s->sp  = -1;
  s->MAX_STACK = size;
  s->val = (leaf **)malloc(sizeof(leaf *) * size);
  
  return (s->val != NULL);
}

//
int lstack_free(lstack *s)
{ s->sp = -1, free(s->val); }

//
int is_lstack_full(lstack *s)
{ return (s->sp == s->MAX_STACK); }

//
int is_lstack_empty(lstack *s)
{ return (s->sp < 0); }

//
int lpush(lstack *s, leaf *l)
{
  if (is_lstack_full(s))
    return 0;
  else
    {
      s->val[++s->sp] = l;
      return 1;
    }
}

//
int lpop(lstack *s, leaf **l)
{
  if (is_lstack_empty(s))
    return 0;
  else
    {
      *l = s->val[s->sp--];
      return 1;
    }
}

//
int ltop(lstack *s, leaf **l)
{
  if (is_lstack_empty(s))
    return 0;
  else
    {
      *l = s->val[s->sp];
      return 1;
    }
}

//
int llookup(lstack *s, leaf *l)
{
  for (int i = 0; i <= s->sp; i++)
    if (s->val[i] == l)
      return i;
  
  return -1;
}
