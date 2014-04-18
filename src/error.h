int err(int code)
{
  switch (code)
    {
    case 0 :
      printf("Unknown given function\n"); break;

    default :
      printf("Unknown error code !\n");
    }
}
