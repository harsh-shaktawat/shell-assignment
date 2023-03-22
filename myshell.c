/********************************************************************************************

This is a template for assignment on writing a custom Shell. 
Students may change the return types and arguments of the functions given in this template,
but do not change the names of these functions.
Though use of any extra functions is not recommended, students may use new functions if they need to, 
but that should not make code unnecessorily complex to read.
Students should keep names of declared variable (and any new functions) self explanatory,
and add proper comments for every logical step.
Students need to be careful while forking a new process (no unnecessory process creations) 
or while inserting the single handler code (should be added at the correct places).
Finally, keep your filename as myshell.c, do not change this name (not even myshell.cpp, 
as you not need to use any features for this assignment that are supported by C++ but not by C).

*********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>			// exit()
#include <unistd.h>			// fork(), getpid(), exec(),getcwd()
#include <sys/wait.h>		// wait()
#include <signal.h>			// signal()
#include <fcntl.h>			// close(), open()
#define CMD_LEN 1024
#define ARGS_LEN 10
#define CMD 10
int parseInput(char *str,char *Commands[])
{
	// This function will parse the input string into multiple commands or a single command with arguments depending on the delimiter (&&, ##, >, or spaces).
    char* temp=str;
    int i;
    //parsing &&
    for (i = 0; i < CMD; i++) 
    {
        Commands[i] = strsep(&str, "&&");
        if (Commands[i] == NULL)
        {
            break;
        }
        if(strlen(Commands[i])==0)
        {
            i--;
        }
    }
    if(Commands[1]!=NULL)
    {
        return 1;
    }
    str=temp;
    //parsing ##
    for (i = 0; i < CMD; i++)
    {
        Commands[i] = strsep(&str, "##");
        if (Commands[i] == NULL)
        {
           break;
        }
        if(strlen(Commands[i])==0)
        {
            i--;
        }
    }
    if(Commands[1]!=NULL)
    {
        return 2;
    }
    str=temp;
    //parsing >
    for (i = 0; i < CMD; i++)
    {
        Commands[i] = strsep(&str, ">");
        if (Commands[i] == NULL)
        {
           break;
        }
        if(strlen(Commands[i])==0)
        {
            i--;
        }
    }
    if(Commands[1]!=NULL)
    {
        return 3;
    }
    return 0;
}
void executeCommand(char *cmd)
{
	// This function will fork a new process to execute a command
    int i;
    char *args[ARGS_LEN];
    for (i = 0; i < ARGS_LEN; i++) {
        args[i] = strsep(&cmd, " ");
        if (args[i] == NULL)
        {
            break;
        }
    }
    if(strcmp(args[0],"cd")==0)
    {
        if(chdir(args[1])<0)
        {
            printf("Shell: Incorrect command\n");
        }
    }
    else
    {
        int pid=fork();
        if(pid==0)
        {
          signal(SIGINT, SIG_DFL);
           signal(SIGTSTP,SIG_DFL);
           if(execvp(args[0],args)<0)
           {
             printf("Shell: Incorrect command\n");
             exit(0);
           }
        }
        else if(pid<0)
        {
           
		   printf("Forking failed\n");
        }
        else
       {
          int rc_wait = waitpid(pid, NULL, WUNTRACED);
       }
    }
}
void executeParallelCommands(char *Commands[])
{
	// This function will run multiple commands in parallel
    int i=0;
    int j;
    char *args[ARGS_LEN];
    while(Commands[i]!=NULL)
    {
        for (j= 0; j < ARGS_LEN; j++) 
        {
           args[j] = strsep(&Commands[i], " ");
           if (args[j] == NULL)
           {
              break;
           }
           if(strlen(args[j]) == 0)
           {
               j--;
           }
        }
        if(strcmp(args[0],"cd")==0)
        {
            if(chdir(args[1])<0)
            {
                printf("Shell: Incorrect command\n");
            }
        }
        else
        {
            int pid=fork();
            if(pid==0)
            {
               signal(SIGINT, SIG_DFL);
               signal(SIGTSTP,SIG_DFL);
               if(execvp(args[0],args)<0)
               {
                  printf("Shell: Incorrect command\n");
                  exit(0);
               }
            }
            else if(pid<0)
            {
               
			   printf("forking failed");
            }
        }
        i++;
    }
    while(wait(NULL)>0);
}

void executeSequentialCommands(char *Commands[])
{	
	// This function will run multiple commands in parallel
    int i=0;
    int j;
    char *args[ARGS_LEN];
    while(Commands[i]!=NULL)
    {
        for (j= 0; j < ARGS_LEN; j++) 
        {
           args[j] = strsep(&Commands[i], " ");
           if (args[j] == NULL)
           {
              break;
           }
           if(strlen(args[j]) == 0)
           {
               j--;
           }
        }
        if(strcmp(args[0],"cd")==0)
        {
            if(chdir(args[1])<0)
            {
                printf("Shell: Incorrect command\n");
            }
        }
        else
        {
            int pid=fork();
            if(pid==0)
            {
               signal(SIGINT, SIG_DFL);
               signal(SIGTSTP,SIG_DFL);
               if(execvp(args[0],args)<0)
               {
                  printf("Shell: Incorrect command\n");
                  exit(0);
               }
            }
            else if(pid<0)
            {
               
			   printf("forking failed\n");
            }
            else
            {
               wait(NULL);
            }
        }
        i++;
    }
}

void executeCommandRedirection(char *Commands[])
{
	// This function will run a single command with output redirected to an output file specificed by user
    char *args[ARGS_LEN];
    if(Commands[1][0]==' ')
    {
        Commands[1]=Commands[1]+1;
    }
    int j;
    for (j= 0; j < ARGS_LEN; j++) 
    {
        args[j] = strsep(&Commands[0], " ");
        if (args[j] == NULL)
        {
            break;
        }
        if(strlen(args[j]) == 0)
        {
            j--;
        }
    }
    int pid=fork();
    if(pid==0)
    {
       	close(STDOUT_FILENO);
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP,SIG_DFL);
		open(Commands[1], O_CREAT | O_WRONLY);
		if(execvp(args[0], args)<0)
        {
            printf("Shell: Incorrect command\n");
            exit(0);
        }
    }
    else if(pid<0)
    {
        printf("forking failed\n");
    }
    else
    {
        int rc_wait = waitpid(pid, NULL, WUNTRACED);
    }
}
int main()
{
	// Initial declarations
    char Input_String[CMD_LEN];
    char **ptr=&Input_String;
    size_t size=CMD_LEN;
    char *Commands[CMD];
    char dir[1024];
    signal(SIGINT, SIG_IGN); 
    signal(SIGTSTP,SIG_IGN);
    // This loop will keep your shell running until user exits.
	while(1)
    {
		// Print the prompt in format - currentWorkingDirectory$
        getcwd(dir,sizeof(dir));
        printf("%s$",dir);
        // accept input with 'getline()'
        getline(&ptr,&size,stdin);
        int len=strlen(Input_String);
        Input_String[len-1]='\0'; //removing \n which is read from the command prompt
        if(strcmp(Input_String,"exit")==0)  // When user uses exit command.
        {
            printf("Exiting shell...\n");
            exit(0);
        }
		// Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.
		int res=parseInput(Input_String,Commands); 	
		if(res==1)
			executeParallelCommands(Commands);		// This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
		else if(res==2)
			executeSequentialCommands(Commands);	// This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
		else if(res==3)
			executeCommandRedirection(Commands);	// This function is invoked when user wants redirect output of a single command to and output file specificed by user
		else
			executeCommand(Commands[0]);		    // This function is invoked when user wants to run a single commands
	}
	return 0;
}