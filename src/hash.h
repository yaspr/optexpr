//
typedef struct node { char stat; char str[MAX_STR]; struct node *next; } node;

//
typedef struct hashtab { node *tab; int (*hf)(char *, int); int MAX_TAB; } hashtab; 

//
int hashtab_init(hashtab *h, int (*f)(char *, int), int size)
{
  h->hf = f;
  h->MAX_TAB = size;
  h->tab = (node *)malloc(sizeof(node) * size);
  
  for (int i = 0; i < size; i++)
    h->tab[i].stat = 1, h->tab[i].next = NULL;
  
  return 0;
}

//
int hashtab_free(hashtab *h)
{
  h->hf = NULL;
  h->MAX_TAB = 0;
  
  free(h->tab);
}

//
node *search(hashtab *h, char *str)
{
  int index = h->hf(str, h->MAX_TAB);
  node *l = &h->tab[index];

  while (l && strcmp(l->str, str))
    l = l->next;
  
  return l; //l == NULL means not found !
}

//
int delete(hashtab *h, char *str)
{
  int index = h->hf(str, h->MAX_TAB);
  node *l = &h->tab[index], *t = NULL;
  
  return 0;
}

//
int insert(hashtab *h, char *str)
{
  int index = h->hf(str, h->MAX_TAB);
  node *l = &h->tab[index], *t = NULL;
 
  //First node free means no LL yet !!
  if (l->stat)
    {
      l->stat = 0;
      strcpy(l->str, str);      
    }
  else
    {
      while (l && l->next)
	l = l->next;
      
      t = (node *)malloc(sizeof(node));
      
      t->next = NULL;
      strcpy(t->str, str);
      
      l->next = t;
    }
  
  return 0;
}

//
void print_hashtab(hashtab *h)
{
  node *l = NULL;
  
  for (int i = 0; i < h->MAX_TAB; i++)
    {
      l = &h->tab[i];
      
      while (l && !l->stat)
	printf("%s%s", l->str, (l->next) ? " --> " : "\n"), l = l->next;
    }
}

//
int hash_0(char *str, int s)
{
  int i = 0, h = str[0];
  
  while (str[++i])
    h ^= str[i] | 0x7F;

  return h % s;
}

//
int hash_1(char *str, int s)
{
  int i = 0, h = str[0];

  while (str[++i])
    h &= (str[i] >> i) | 0x81CE;
  
  return h % s;
}

int hash_2(char *str, int s)
{
  int i = 0, h = str[0];
  
  while (str[++i])
    h ^= str[i] & 0xFFFF;

  return h % s;
}

