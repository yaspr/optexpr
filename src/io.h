//
void write_tree_dot_(FILE *fd, leaf *l)
{
  if (l && l->right)
    { 
      if (l->left) //For unary operators (functions)
	{
	  fprintf(fd, "%d -- ", (int)l);
	  //Go right !
	  write_tree_dot_(fd, l->left);
	}
      
      fprintf(fd, "%d -- ", (int)l);
      //Go left !
      write_tree_dot_(fd, l->right);
    }
  else
    fprintf(fd, "%d;\n", (int)l);
}

//
void write_leaf_lbl_(FILE *fd, leaf *l, lstack *s)
{
  if (l)
    {
      write_leaf_lbl_(fd, l->left, s);

      if (l->typ == FUNC || l->typ == VAR)
	fprintf(fd, WRT_STR_FRMT, (int)l, l->str, (llookup(s, l) < 0) ? "black" : "blue");
      else
	fprintf(fd, (l->typ == INT) ? WRT_INT_FRMT : WRT_CHR_FRMT, (int)l, l->val,
		(llookup(s, l) < 0) ? "black" : "blue");	

      write_leaf_lbl_(fd, l->right, s);
    }
}

//
void write_tree_dot(char *fname, char *gname, leaf *l, lstack *s)
{
  FILE *fd = fopen(fname, "wb");
  
  fprintf(fd, "graph \"%s\" {\n", gname);
  write_leaf_lbl_(fd, l, s);
  write_tree_dot_(fd, l);
  fprintf(fd, "}\n");

  fclose(fd);
}
