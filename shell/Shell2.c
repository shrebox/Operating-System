
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
char** history;
//int hist_index = 0;


#define MAX_CMD_LEN  128
#define HISTORY_COUNT 20
#define DELIM " \n"
#define PIPEDELIM "|\n"
#define OUTPUTDELIM ">\n"
#define INPUTDELIM "<\n"
#define BOTHDELIM "<>\n"
const char s[2] = " ";
char *token;
//*line;
char **args;
int bsize,pos,hpos,current;
char** history;
int hist_index = 0;
void sigintHandler(int sig_num);
void addhistory(char* line);
char* trim(char *s) ;
char** splitstring( char* s, char* delim );
void pipehandler( char* s, int num_cmds );
void redirecthandler( char* s, int redirection );
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
//int lsh_history(char *hist[]);

char *builtin_str[] = 
{
	"cd",
  	"help",
  	"exit"
  	//"history"
};

int (*builtin_func[]) (char **) = 
{
	&lsh_cd,
  	&lsh_help,
  	&lsh_exit
  	//&lsh_history
};

// int lsh_num_builtins() {
//   return sizeof(builtin_str) / sizeof(char *);
// }

int lsh_cd(char **args)
{
	if(args[1]!=NULL)
	{
		if (chdir(args[1]) != 0) 
	    {
	      perror("lsh");
	    }
	}

	else
	{
		fprintf(stderr, "lsh: expected argument to \"cd\"\n");
	}
  
  return 1;
}

int lsh_help(char **args)
{
	int i;
	printf("The following are the built-in commands:\n");

	int loop = sizeof(builtin_str) / sizeof(char *);
  	
  	for (i = 0; i < loop; ++i)
  	{
  		 printf("  %s\n", builtin_str[i]);
  	}
  
 	printf("  history\n");
  	printf("Use the man command for information on other programs.\n");
  
 	return 1;
}

int lsh_exit(char **args)
{
	return 0;
}

void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    printf("\n Cannot be terminated using Ctrl+C \n");
     //return;
    fflush(stdout);
}

void allocerror()
{
	
	fprintf(stderr, "lsh: allocation error\n");
	exit(EXIT_FAILURE);

}

void addtohistory( char* line )
{
	history[hist_index] = (char*)malloc( 50 * sizeof(char));
	printf("Hlllllll\n");
	strcpy( history[hist_index ] , line );
	hist_index++; 
}

char* trim(char *s) {
  while (isspace((unsigned char) *s)) s++;
  if (*s) {
    char *p = s;
    while (*p) p++;
    while (isspace((unsigned char) *(--p)));
    p[1] = '\0';
  }
  return s;
}

char** splitstring( char* s, char* delim ){
	char **s_parsed = ( char**)malloc( sizeof( char* ) * 50 );
	char *temp;
	temp = strtok( s, delim ); 
	int i = 0;
	while( temp!= NULL)
	{
		s_parsed[i] = temp;
		i++;
		//printf( "%s\n", s_parsed[i]);
		temp = strtok( NULL, delim );
	}
	s_parsed[i] = NULL;
	return s_parsed;
}

void pipehandler( char* s, int num_cmds ){
	int fd[100][2],cid1,length,status;
	char* command[100];
	int pipe_count = num_cmds - 1;
	int i, j;

	/*
	fd[][2] is array of file descriptors. fd[i][0] is input 
	of i+1 command and fd[i][1] is output of ith command. 
	So output of i goes to input of i+1 using fd[i] pipe.
	*/

	char** cmd = splitstring(s, PIPEDELIM);  //split the strings by "\n" and "|""
	if(pipe_count)
	    {
	        for(i = 0;i < pipe_count;i++){
	            pipe(fd[i]);  // Creates a pipe with file descriptors for each array element
	        }

	        for(i = 0;i <= pipe_count;i++){
	            cid1 = fork();  //fork a new process
	            if(!cid1){
	            	if(i!=0){  //parent process
	                    dup2(fd[i-1][0],0);
	                }
	                //child process
	                if(i!=pipe_count){  //if this is not the last process in the pipe
	                    dup2(fd[i][1],1);
	                }

	                for(j = 0;j < pipe_count;j++){   
	                        close(fd[j][0]);
	                        close(fd[j][1]);
	                }
	                char** present_cmd = splitstring( cmd[i], DELIM );  //if the present cmd is also composite, split it 
	                execvp( present_cmd[0], present_cmd );
	                exit(0);
	            }


	        }
	        for(i = 0;i < pipe_count;i++)
	        {
	            close(fd[i][0]);
	            close(fd[i][1]);
	        }
	        waitpid(cid1,&status,0);
	    }

}

void redirecthandler( char* s, int redirection ){
	//
	int fd;
	pid_t pid;
	if( redirection == 1 ){
		char **s_parsed = splitstring( s, OUTPUTDELIM );
		//printf( "Input cmd : %s Output file : %s\n", s_parsed[0], s_parsed[1]);
		
		if( s_parsed[1] == NULL ) {
			printf("Not enough output arguments!\n");
			return;
		}
		char* outputfile = trim(s_parsed[1]);
		//printf("%s\n", outputfile );
		//printf("HELEOEEOELEOELOL\n");
		char** actualcmd = splitstring( s_parsed[0], DELIM );
		//printf( "%s %s\n", actualcmd[0], actualcmd[1]);


		pid = fork();
		if( pid == 0 ) //child
		{
			fd = open( outputfile, O_CREAT | O_TRUNC | O_WRONLY , 0777 );
			// S_IRUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP| S_IROTH | S_IWOTH| S_IXOTH ); 
			dup2( fd, STDOUT_FILENO );
			//printf("yaha aa raha hai?\n");
			close( fd );
			
			execvp( actualcmd[0], actualcmd );
		}
		else{
			waitpid(pid,NULL,0);
			//close( fd );
		}
	}
	else if( redirection == 2 ){
		char **s_parsed = splitstring( s, INPUTDELIM );
		//printf( "Input cmd : %s Output file : %s\n", s_parsed[0], s_parsed[1]);
		if( s_parsed[1] == NULL ) {
			printf("Not enough input arguments!\n");
			return;
		}
		char* inputfile = trim(s_parsed[1]);
		//printf("%s\n", inputfile );
		//printf("HELEOEEOELEOELOL\n");
		char** actualcmd = splitstring( s_parsed[0], DELIM );
		//printf( "%s %s\n", actualcmd[0], actualcmd[1]);
		pid = fork();
		if( pid == 0 ) //child
		{
			fd = open( inputfile, O_RDONLY , 0600 );
			// S_IRUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP| S_IROTH | S_IWOTH| S_IXOTH ); 
			dup2( fd, STDIN_FILENO );
			close( fd );
			execvp( actualcmd[0], actualcmd );
		}
		else{
			waitpid(pid,NULL,0);
			//close( fd );
		}
	}
	else{  // both input and output redirection
		int fd1, fd2;
		char **s_parsed = splitstring( s, BOTHDELIM );
		if( s_parsed[1] == NULL ) {
			printf("Not enough output arguments!\n");
			return;
		}
		char* inputfile = trim(s_parsed[1]);
		char* outputfile = trim(s_parsed[2]);
		//printf( "CMD: %s Input cmd : %s Output file : %s\n", s_parsed[0], inputfile, outputfile );
		//printf("HELEOEEOELEOELOL\n");
		char** actualcmd = splitstring( s_parsed[0], DELIM );
		//printf( "%s %s\n", actualcmd[0], actualcmd[1]);
		pid = fork();
		if( pid == 0 ) //child
		{
			fd1 = open( inputfile, O_RDONLY , 0600 );
			dup2( fd1, STDIN_FILENO );
			close( fd1 );

			fd2 = open( outputfile, O_CREAT | O_TRUNC | O_WRONLY , 0777 );
			// S_IRUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP| S_IROTH | S_IWOTH| S_IXOTH ); 
			dup2( fd2, STDOUT_FILENO );
			close( fd2 );
			execvp( actualcmd[0], actualcmd );
		}
		else{
			waitpid(pid,NULL,0);
			//close( fd );
		}
	}
}

int main()
{
	signal(SIGINT, sigintHandler);
	int status = 1;
	char *hist[HISTORY_COUNT];
	int j;
	current = 0;

	for(j=0;j<HISTORY_COUNT;j++)
		hist[j] = NULL;

	while(status)
	{
		printf("> ");

		int c;
		pos = 0;
		bsize = 1024;
		
		// line = malloc(sizeof(char)*1024);
		// if(!line)
		// 	allocerror();

		char *line = NULL;
		ssize_t bufsize = 0;
		getline(&line,&bufsize,stdin);

		printf("%c\n",line[0] );
		
		// while(1)
		// {
		// 	//printf("here 1\n");

		// 	c = getchar();

		// 	if(c==EOF || c=='\n')
		// 	{
		// 		line[pos] = '\0';
		// 		break;
		// 	}

		// 	else
		// 	{
		// 		line[pos] = c;
		// 	}

		// 	pos++;

		// 	if(pos>=bsize)
		// 	{
		// 		bsize += 1024;
		// 		line = realloc(line,bsize);
		// 		//history = realloc(history,bsize);
		// 		if(!line)
		// 			allocerror();
		// 	}

		// }

		if(line[0] != '\n')
		{
			printf("here1\n");
			//addtohistory(line);
			int num_cmds = 0, l = 0;
			int redirection = 0;

			while ( line[l] != '\n')
			{
				printf("here2\n");
				if ( line[l] == '|' )
				{
					num_cmds++;
				}
				
				if( line[l] == '>' && redirection == 0){

				 // only output redirection
					printf("here3a\n");
					redirection = 1;
				}
				else if( line[l] == '>' && redirection == 2 ){
				//both
					printf("here3\n");
					redirection = 3;
				}
				if( line[l] == '<' ) //only input redirection
					redirection = 2;
				
				l++;
			}
			num_cmds++; 

			if( num_cmds == 1 && redirection == 0 )
			{

				printf("here5\n");	
			}

			else
			{
				if( redirection == 0 )
					pipehandler( line, num_cmds );
				else{

					printf("Hiiiiiiiiii\n");
					printf("%d\n",redirection );
					redirecthandler( line, redirection );
				}
			}


		}

		// if(strcmp(line,"")!=0)
		// {
		// 	free(hist[current]);
		// 	hist[current] = strdup(line);
		// 	current = (current + 1) % HISTORY_COUNT;
		// }

		
		if(strcmp(line,"history")==0)
		{
			for(j=0;j<current;j++)
			{
				printf("%4d  %s\n", j+1, hist[j]);
			}
		}

		else
		{
			hpos = pos;
			bsize = 64, pos =0;
			
			args = malloc(64*sizeof(char*));
			if(!args)
				allocerror();
		   
			token = strtok(line," \t\r\n\a");

		    while( token != NULL ) 
		    {
		        //if(i==0) {strcpy(arg0,token);}
		         
		        args[pos]=token;      
		        pos++;

		        if(pos>=bsize)
				{
					bsize += 64;
					args = realloc(args,bsize*sizeof(char*));
					if(!args)
						allocerror();
				}

				token = strtok(NULL," \t\r\n\a");

		    }

		    args[pos]=NULL;

		    int flag =0;
		    if(args[0]!=NULL)
		    {

		    	int loop = sizeof(builtin_str) / sizeof(char *);

		    	int i;
		    	for(i=0;i<loop;i++)
		    	{
		    		if (strcmp(args[0], builtin_str[i]) == 0) {
	     				status =  (*builtin_func[i])(args);
	    				flag=1;
	    			}
		    	}

		    	if(flag==0)
		    	{
		    		pid_t pid;
					int stat;

					pid = fork();
				  	if(pid==0)
				  	{
				  		if (execvp(args[0], args) == -1) 
				  		{
	     	 				perror("lsh");
					    }

					    exit(EXIT_FAILURE);	
				  	}
					    
					else if (pid < 0) 
					{
					    perror("lsh");
					} 

					 else 
					{
					    do 
					    {
					      waitpid(pid, &status, WUNTRACED);
					    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
					 }

					 status = 1;
		    	}
		    }

		    free(line);
		    free(args);
		} 

	}
	    
	return EXIT_SUCCESS;


}