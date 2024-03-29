#include "femtoshell.h"
/* static functions Declarations */
static char **Parser (unsigned long *);
static int echo (int argc, char **argv);
static int pwd (void);
/*****************************************/
ReturnStatus
GetShellMessage (void)
{
  char *env_user = NULL, **tokens = NULL;
  unsigned long argc;
  int err;
  env_user = getlogin ();
  ReturnStatus status = STATUS_TRUE;
  printf ("%s@stm-linux:$ ", env_user);
  tokens = Parser (&argc);
  if (0 == strcmp (tokens[0], "exit"))
    {
      status = STATUS_FALSE;
    }
  else if (0 == strcmp (tokens[0], "echo"))
    {
      err = echo (argc, tokens);
      if (err != 0)
	{
	  printf ("errno = %d :Error occurred with echo", errno);
	}
    }
  else if (0 == strcmp (tokens[0], "pwd"))
    {
      err = pwd ();
      if (err != 0)
	{
	  printf ("errno = %d :Error occurred with pwd", errno);
	}


    }
  else if (0 == strcmp (tokens[0], "cd"))
    {
      if (argc != 2)
	{
	  printf ("Usage of cd is: cd path\n");
	}
      else
	{
	 if ( chdir(tokens[1]) == 0 )
	 {
		// done 
	 }
	 else
	 {
		printf("errno = %d:Error occurred with cd\n",errno) ; 
	 }
	}

    }
  else
    {
      printf ("%s: command not found\n", tokens[0]);
    }
  for (unsigned long i = 0; i < argc; i++)
    free (tokens[i]);
  free (tokens);
  return status;
}

char **
Parser (unsigned long *argc)
{                      
  size_t index = 0;
  char ch;
  char **argv = NULL;
  unsigned char start = 1; // start ==  1 when we will start new token
  unsigned long l_argc = 0;  // number of tokens
  fflush (stdin); // fflush stdin to avoid buffer undefined behaviour 
  while ((ch = getchar ()) != '\n') // loop on characters entered in stdin
    {

      if ((ch != ' ')) // if char is not space 
	{
	  if (start == 1) // if it's about to start new token
	    {
	      l_argc++; // increase number of tokens by one
	      argv = (char **) realloc (argv, sizeof (char *) * l_argc); // allocate space for the new pointer to the token 
	      if (argv == NULL) // if it can't allocate print and break
		{
		 // printf ("Can't Allocate = 1");
		  break;
		}
	      argv[l_argc - 1] = NULL; // avoid dangling pointer :D
	      start = 0;	
	    }
	  argv[l_argc - 1] =
	    (char *) realloc (argv[l_argc - 1], sizeof (char) * (index + 1)); //alocate memory for the character .
	  if (argv[l_argc - 1] == NULL) // if it can't allocate print and break
	    {
	     // printf ("Can't Allocate = 2"); 
	      break;
	    }
	  argv[l_argc - 1][index] = ch; // store the char
	  index++;
	}
      else if ((start == 0)) // if start == 0 and ch == space , allocate space for ch and store '\0'
	{
	  start = 1;
	  argv[l_argc - 1] =
	    (char *) realloc (argv[l_argc - 1], sizeof (char) * (index + 1));
	  if (argv[l_argc - 1] == NULL) // same as above xD 
	    {
	    //  printf ("Can't Allocate = 3");
	      break;
	    }
	  argv[l_argc - 1][index] = '\0';
	  index = 0; // reset index 
	}
      else // else  space and start == 1 
	{
		// do nothing 
		// don't be like this else :D
	}
    }
  if (start == 0) // after looping if start == 0 put '\0' 
    {
      argv[l_argc - 1] =
	(char *) realloc (argv[l_argc - 1], sizeof (char) * (index + 1));
      if (argv[l_argc - 1] == NULL)
	{
	  printf ("Can't Allocate = 4");
	}
      argv[l_argc - 1][index] = '\0';

    }

  *argc = l_argc;
  return argv;
}

static int
echo (int argc, char **argv) // echo implementation 
{
  unsigned char count = 1;
  int num_write = 0;
  while (count != argc)
    {
      num_write = write (1, argv[count], strlen (argv[count]));	// echo the arg

      num_write = write (1, " ", 2);	// echo space between each arg 
      if (num_write == -1)	// check if there is an error happened
	{
	  return -1;
	}
      count++;
    }

  num_write = write (1, "\n", 2);	// echo space between each argument
  return 0;
}


static int
pwd (void) // built in pwd 
{
  size_t buf_size = INITIAL_BUF_SIZE;
  char *buf = NULL;
  if (buf == (char *) NULL)
    {
      errno = ENOMEM;
    }
  buf = getcwd (buf, buf_size);
  while (((char *) NULL == buf) && (errno == ERANGE))	// if there is a range error happened and  NULL returned
    {
      free (buf);
      buf_size += BUF_SIZE_INC;
      buf = getcwd (buf, buf_size);

    }
  switch (errno)
    {
    case EACCES:
      printf ("errno = %d : Permision Denied\n", errno);
      break;
    case EFAULT:
      printf ("errno = %d : Bad Address \n", errno);
      break;
    case EINVAL:
      printf ("errno = %d : Size argument is zero and buf is not null\n",
	      errno);
      break;
    case ENOENT:
      printf ("errno = %d : Unlinked Dir \n", errno);
      break;
    case ENOMEM:
      printf ("errno = %d : Out of Memory \n", errno);
      break;
    default:
      printf ("%s\n", buf);
    }
  free (buf);
  return 0;
}
