//Leaf Stack
typedef struct rstack { int *val; int sp; int MAX_STACK; } rstack;

//
int rstack_init(rstack *s, int size)
{
  s->sp  = -1;
  s->MAX_STACK = size;  
  s->val = (int *)malloc(sizeof(int) * size);
  
  return (s->val != NULL);
}

//
int rstack_free(rstack *s)
{ s->sp = -1, free(s->val); }

//
int is_rstack_full(rstack *s)
{ return (s->sp == s->MAX_STACK); }

//
int is_rstack_empty(rstack *s)
{ return (s->sp < 0); }

//
int rpush(rstack *s, int *r)
{
  if (is_rstack_full(s))
    return 0;
  else
    {
      s->val[++s->sp] = *r;
      return 1;
    }
}

//
int rpop(rstack *s, int *r)
{
  if (is_rstack_empty(s))
    return 0;
  else
    {
      *r = s->val[s->sp--];
      return 1;
    }
}

//
int rtop(rstack *s, int *r)
{
  if (is_rstack_empty(s))
    return 0;
  else
    {
      *r = s->val[s->sp];
      return 1;
    }
}
