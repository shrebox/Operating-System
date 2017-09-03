#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  
    int pid = fork();
    if(pid==0)
    {  
        char** newterm = (char *[]){"gnome-terminal", "-e", "./Shell2", NULL };
       	execvp( "gnome-terminal", newterm );            
        exit(1);
    }

    else {wait(NULL);}

  
  return 0;

}

