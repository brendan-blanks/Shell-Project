
/* ==================================================================== */
/* PROGRAM */

/*
    AUTHOR: Brendan Blanks
    FSU MAIL NAME: bfb09c@my.fsu.edu
    COP 4610 - FALL 2012
    PROJECT NUMBER: 1
    DUE DATE: Thursday 10/09/2012
*/

/* PROGRAM */
/* ==================================================================== */
/* HEADER FILES */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<time.h>

/* HEADER FILES */
/* ==================================================================== */
/* TYPE DEFINITIONS */



/* TYPE DEFINITIONS */
/* ==================================================================== */
/* CONSTANTS */

const int MAX_INPUT_LENGTH=80;
const int FALSE=0;
const int TRUE=1;
const int REVERSETRUE=2;
const int MAXPIPES = 2;
const char NULLCHAR='\0';

/* CONSTANTS */
/* ==================================================================== */
/* GLOBALS */


/* GLOBALS */
/* ==================================================================== */
/* FUNCTION PROTOTYPES */

void PrintPrompt();
void GetInput(char*);
int  ExitOrNo(char*);
void TimedCall(char*, char*);
char* GetCommand(char*);
void MainExecutionModule(char*, char*);
void GetArguments(char*, char**);
void Echo(char**);
void ChangeDir(char**);
void ViewProccess(char**);
void Execute(char*, char**);

/* FUNCTION PROTOTYPES */
/* ==================================================================== */
/* MAIN FUNCTION */

int main()
{
        char theInput[MAX_INPUT_LENGTH];
        char* theCommand = "";
        int keepGoing = TRUE;

	theInput[0] = NULLCHAR;
        theInput[MAX_INPUT_LENGTH -1] = NULLCHAR;
        while(keepGoing)
        {
                PrintPrompt();
                GetInput(theInput);
//		printf("Affirming input: %s\n", theInput);
		keepGoing = ExitOrNo(theInput);
		if(keepGoing)
                {
			theCommand = GetCommand(theInput);
			if(strcmp(theCommand, "time") == 0)
				TimedCall(theInput, theCommand);
			else
				MainExecutionModule(theInput, theCommand);
                }
//		printf("clearing input\n");
		theInput[0] = NULLCHAR;
//		printf("clearing command\n");
		theCommand = NULL;
        }
	return 0;
}


/* MAIN FUNCTION */
/* ==================================================================== */
/* MODULES */

void PrintPrompt()
{
        char hostname[128];

        hostname[127] = NULLCHAR;
        gethostname(hostname, 128);

        printf("%s@", getenv("USER"));

        printf("%s:", hostname);
        printf("%s-| ", getenv("PWD"));

        return;
}

void GetInput(char* input)
{

	fgets(input, MAX_INPUT_LENGTH, stdin);
        if(input[MAX_INPUT_LENGTH - 1] != NULLCHAR)
        {
                printf("Input longer than %d characters.\n", MAX_INPUT_LENGTH);
                printf("Input truncated to %d characters.\n", MAX_INPUT_LENGTH);
		printf("Do NOT input more than %d characters!\n", MAX_INPUT_LENGTH);
                input[MAX_INPUT_LENGTH] = NULLCHAR;
        }
	if(input[strlen(input)-1] =='\n')
		input[strlen(input)-1] = NULLCHAR;

	return;
}

int  ExitOrNo(char* input)
{
	if(strcmp(input, "exit") == 0 || strcmp(input, "x") == 0)
        {
                printf("exit\n");
		return(FALSE);
	}
	else
	{
		return(TRUE);
	}
}

void TimedCall(char* theInput, char* theCommand)
{
	double timePassed;
	time_t begin, end;
	int i;
	char temp[MAX_INPUT_LENGTH - 5];
	for(i = 0; i < MAX_INPUT_LENGTH - 5; i++)
	{
		temp[i] = theInput[i+5];
	}
	strcpy(theInput, temp);
	time(&begin);
	theCommand = GetCommand(theInput);
	MainExecutionModule(theInput, theCommand);
	time(&end);
	timePassed = difftime(end, begin);
	printf("The command took %.3lf seconds to complete.\n", timePassed);
	return;
}


char* GetCommand(char* input)
{
	char* command = (char *) malloc(strlen(input));
	sscanf(input, "%s", command);
        return(command);
}

void MainExecutionModule(char* theInput, char* theCommand)
{
	char* theArguments[40];
	GetArguments(theInput, theArguments);
//	printf("Affirming command recieved: %s\n", theCommand);
//	printf("Affirming arguments recieved:\n");
	if(strcmp(theCommand, "cd") == 0)
	{
		ChangeDir(theArguments);
	}
	else if(strcmp(theCommand, "echo") == 0)
	{
		Echo(theArguments);
	}
	else if(strcmp(theCommand, "viewproc") == 0)
	{
		ViewProccess(theArguments);
	}
	else
	{
		Execute(theCommand, theArguments);
	}
	return;
}

void GetArguments(char* input, char* arguments[])
{
	int i, k;
	char inTemput[MAX_INPUT_LENGTH];
	strcpy(inTemput, input);
	for(i = 0; i < 40; i++)
	{
		arguments[i] = (char *) malloc (MAX_INPUT_LENGTH*sizeof(char));
		for(k = 0; k < MAX_INPUT_LENGTH; k++)
			arguments[i][k] = '\0';
	}
	char* temp;
	temp = strtok(inTemput, " \0");
	for(i = 0; temp != NULL; i++)
	{
		temp = strtok(NULL, " \0");
		if(temp != NULL)
			arguments[i] = strdup(temp);
	}
//	printf("Input was %s\n", input);
/*	for(i = 0; i < 30 && arguments[i] != NULL; i++)
		printf("The arguments were %s\n", arguments[i]); */
	for(i = 0; i < 30 && arguments[i] != NULL; i++)
		if(arguments[i][strlen(arguments[i])-1] == '\n')
			arguments[i][strlen(arguments[i])-1] = NULLCHAR;
	return;
}

void Echo(char** argument)
{
	uint i;
	char* first = (char *) malloc (1);
	sscanf(argument[0], "%c", first);
	char theFirst = first[0];
	if(theFirst == '$')
	{
		char* theEV = (char *) malloc (strlen(argument[0])-1);
		for (i = 0; i < (strlen(argument[0])-1); i++)
		{
			theEV[i] = argument[0][i+1];
		}
		if(getenv(theEV))
		printf("%s\n", getenv(theEV));
	}
	else
	{
		printf("%s\n", argument[0]);
	}

	return;
}

void ChangeDir(char** argument)
{
	char* token;
//	printf("Changing to %s\n", argument[0]);
	int success = chdir(argument[0]);
	if(success != 0)
	{
		printf("cd: %s: No such file or directory\n", argument[0]);
		return;
	}
	char* first = (char *) malloc (1);
	sscanf(argument[0], "%c", first);
	char theFirst = first[0];
	if(theFirst == '/')
	{
		setenv("PWD", argument[0], 1);
	}
	else if(theFirst == '.')
	{
		char* tempArg = (char *) malloc (strlen(argument[0]));
		char* remainingArg = (char *) malloc (strlen(argument[0]));
		char* newPWD;
		strcpy(tempArg, argument[0]);
		for(token = strtok_r(tempArg, "/\0", &remainingArg);
			token != NULL;
			token = strtok_r(NULL, "/\0", &remainingArg))
		{
			if(strcmp(token, "..") == 0)
			{
				newPWD = getenv("PWD");
				while(newPWD[strlen(newPWD)-2] != '/')
				{
					newPWD[strlen(newPWD)-1] = NULLCHAR;
				} 
				newPWD[strlen(newPWD)-1] = NULLCHAR;
				newPWD[strlen(newPWD)-1] = NULLCHAR;
				setenv("PWD", newPWD, 1);
			}
			else if(strcmp(token, ".") == 0)
			{
				
			}
			else
			{
				newPWD = getenv("PWD");
				strcat(newPWD, "/");
				strcat(newPWD, token);
				setenv("PWD", newPWD, 1);
			}
		}
	}
	else if(theFirst == '~')
	{
		
	}
	else
	{
		char* newPWD;
		newPWD = getenv("PWD");
		strcat(newPWD, "/");
		strcat(newPWD, argument[0]);
		setenv("PWD", newPWD, 1);
	}
	// checks for and removes erroneous / on the end of PWD, allowing 
	// for users to use cd with either convention ( cd dir1/dir2 or 
	// cd dir1/dir2/) without breaking the shell

	char* justChecking = getenv("PWD");
	if(justChecking[strlen(justChecking)-1] == '/' && strlen(justChecking) > 1)
	{
		justChecking[strlen(justChecking)-1] = NULLCHAR;
		setenv("PWD", justChecking, 1);
	}
	return;
}

void ViewProccess(char** argument)
{
	FILE *file;
	char name[] = "/proc/";
	strcat(name, argument[0]);
	file = fopen(name, "r");
	if(!file){
		printf("error: %s is not in /proc\n", name);
		return;
	}
	char fileChar;
	fileChar = fgetc(file);
	while(fileChar != EOF)
	{
		printf("%c", fileChar);
		fileChar = fgetc(file);
	}
	return;
}

void Execute(char* command, char** arguments)
{
	int k;
	char* path = strdup(getenv("PATH"));
	const char *const delim = ":";
	char* tempPath;

	FILE* file;
	int found = FALSE;

	char* token = strtok(path, delim);
	while(token != NULL && !found)
	{
		if(command[0] != '/')
		{
			tempPath = strdup(token);
			strcat(tempPath, "/");
			strcat(tempPath, command);
			file = fopen(tempPath, "r");
		}
		else
		{
			file = fopen(command, "r");
			tempPath = strdup(command);
		}
		if(file)
		{
			found = TRUE;
		}
		token = strtok(NULL, delim);
	}
	if(!found)
	{
		printf("%s: no %s in (%s)\n", command, command, getenv("PATH"));
	}
	else
	{
		pid_t pid;
		if(strcmp(arguments[0], command) != 0)
		{ 
			char* theArguments[42];
			theArguments[0] = strdup(tempPath);
			int i;
			int thePipe[2],
			    thePipe2[2];
			int backgroundMe = FALSE;
			int redirectMe = FALSE;
			int numPipes = 0;
			int wheresPipe[2];
			wheresPipe[0] = -1;
			wheresPipe[1] = -1;
			for(i = 0; i < 40 && strcmp(arguments[i], "") != 0; i++)
			{
//				printf("Adding %s to arguments[%d] passed to %s.\n", arguments[i], i, tempPath);
				theArguments[i+1]=arguments[i];
			}
			theArguments[i+1]= NULL;
			int status;
			char* fileName;
//			printf("Path: %s\nArguments:\n", tempPath);
/*			for(k = 0; k < 42 && theArguments[k] != NULL; k++)
				printf("%d: %s\n", k, theArguments[k]); */
			for(k = 0; k < 42 && theArguments[k] != NULL && !backgroundMe && !redirectMe; k++)
			{
				if(strcmp(theArguments[k], "&") == 0 && theArguments[k+1] == NULL)
					backgroundMe = TRUE;
				else if(strcmp(theArguments[k], "&") == 0 && theArguments[k+1] != NULL)
				{
					printf("Badly placed background signal. & must be placed at END of entry.\n");
					return;
				}
				else if(strcmp(theArguments[k], "<") == 0 && theArguments[k+2] == NULL && theArguments[k+1] != NULL)
				{
					redirectMe = TRUE;
					fileName = strdup(theArguments[k+1]);
					theArguments[k] = NULL;
					theArguments[k+1] = NULL;
				}
				else if(strcmp(theArguments[k], "<") == 0 && theArguments[k+2] != NULL && theArguments[k+1] != NULL)
				{
					printf("Too many arguments. You may only redirect with one file name in each command.\n");
					return;
				}
				else if(strcmp(theArguments[k], "<") == 0 && theArguments[k+2] == NULL && theArguments[k+1] == NULL)
				{
					printf("Nothing after redirect signal. Need a filename to redirect into command.\n");
					return;
				}
				else if(strcmp(theArguments[k], ">") == 0 && theArguments[k+2] == NULL && theArguments[k+1] != NULL)
				{
					redirectMe = REVERSETRUE;
					fileName = strdup(theArguments[k+1]);
					theArguments[k] = NULL;
					theArguments[k+1] = NULL;
				}
				else if(strcmp(theArguments[k], ">") == 0 && theArguments[k+2] != NULL && theArguments[k+1] != NULL)
				{
					printf("Too many arguments. You may only redirect with one file name in each command.\n");
					return;
				}
				else if(strcmp(theArguments[k], ">") == 0 && theArguments[k+2] == NULL && theArguments[k+1] == NULL)
				{
					printf("Nothing after redirect signal. Need a filename to redirect into command.\n");
					return;
				}
				else if(strcmp(theArguments[k], "|") == 0 && theArguments[k+1] != NULL && strcmp(theArguments[k+1], ">") && strcmp(theArguments[k+1], "<") && strcmp(theArguments[k+1], "|") )
				{
					numPipes = numPipes + 1;
					if(wheresPipe[0] == -1)
						wheresPipe[0] = k;
					else if(wheresPipe[1] == -1)
						wheresPipe[1] = k;
				}
			}
			if(backgroundMe)
			{
				theArguments[i] = NULL;
			}
			int newFileDes;
			if(numPipes == 0)
			{
				if((pid = fork()) == 0)
				{
					if(redirectMe == TRUE)
					{
						newFileDes = open(fileName, O_RDONLY);
						close(0);
						dup(newFileDes);
						close(newFileDes);
					}
					else if(redirectMe == REVERSETRUE)
					{
						newFileDes = open(fileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
						close(1);
						dup(newFileDes);
						close(newFileDes);
					}
					execv(tempPath, theArguments);
				}
				else
				{				
					if(backgroundMe)
						waitpid(-1, &status, WNOHANG);
					else
						waitpid(-1, &status, 0);
				}
			}
			else if(numPipes == 1)
			{
				if(pipe(thePipe))
				{
					fprintf(stderr, "PIPE FAILED.\n");
					return;
				}
				if((pid = fork()) == 0)
				{
					close(1);
					dup2(thePipe[1],1);
					close(thePipe[0]);
					close(thePipe[1]);

					theArguments[wheresPipe[0]] = NULL;
					execv(tempPath, theArguments);
				}
				else
				{ 
					waitpid(-1, &status, 0);
					if((pid = fork()) == 0)
					{
						char* tempArgs[42 - wheresPipe[0]];
						for(i = 0; i < 42 - wheresPipe[0]; i++);
							tempArgs[i] = theArguments[i + wheresPipe[0]];
						char* commandTwo = strdup(tempArgs[0]);
						close(0);
						dup2(thePipe[0],0);
						close(thePipe[0]);
						close(thePipe[1]);	

						token = strtok(path, delim);
						while(token != NULL && !found)
						{
							if(commandTwo[0] != '/')
							{
								tempPath = strdup(token);
								strcat(tempPath, "/");
								strcat(tempPath, commandTwo);
								file = fopen(tempPath, "r");
							}
							else
							{
								file = fopen(commandTwo, "r");
								tempPath = strdup(commandTwo);
							}
							if(file)
							{
								found = TRUE;
							}
							token = strtok(NULL, delim);
						}
						if(!found)
						{
							printf("%s: no %s in (%s)\n", commandTwo, commandTwo, getenv("PATH"));
						}
						else
						{
							execv(tempPath, tempArgs);
						}
					}
					else
					{
						close(thePipe[0]);
						close(thePipe[1]);
						waitpid(-1, &status, 0);
//						waitpid(-1, &status, 0);
					}
				}
			}
			else if(numPipes == 2)
			{
				if(pipe(thePipe))
				{
					fprintf(stderr, "PIPE FAILURE.\n");
					return;
				}
				if((pid = fork()) == 0)
				{
					close(1);
					dup2(thePipe[1],1);
					close(thePipe[0]);
					close(thePipe[1]);

					theArguments[wheresPipe[0]] = NULL;
					execv(tempPath, theArguments);
				}
				else
				{ 
					if(pipe(thePipe2))
					{
						fprintf(stderr, "PIPE2 FAILURE.\n");
						return;
					}
					waitpid(-1, &status, 0);
					if((pid = fork()) == 0)
					{
						char* tempArgs[42 - wheresPipe[0]];
						for(i = 0; i < 42 - wheresPipe[0]; i++);
							tempArgs[i] = theArguments[i + wheresPipe[0]];
						char* commandTwo = strdup(tempArgs[0]);
						close(0);
						dup2(thePipe[0],0);
						close(thePipe[0]);
						close(thePipe[1]);
						close(1);
						dup2(thePipe2[1],1);
						close(thePipe2[0]);
						close(thePipe2[1]);

						token = strtok(path, delim);
						while(token != NULL && !found)
						{
							if(commandTwo[0] != '/')
							{
								tempPath = strdup(token);
								strcat(tempPath, "/");
								strcat(tempPath, commandTwo);
								file = fopen(tempPath, "r");
							}
							else
							{
								file = fopen(commandTwo, "r");
								tempPath = strdup(commandTwo);
							}
							if(file)
							{
								found = TRUE;
							}
							token = strtok(NULL, delim);
						}
						if(!found)
						{
							printf("%s: no %s in (%s)\n", commandTwo, commandTwo, getenv("PATH"));
						}
						else
						{
							execv(tempPath, tempArgs);
						}
					}
					else
					{
						waitpid(-1, &status, 0);
						if((pid = fork()) == 0)
						{
							char* tempArgs[42 - wheresPipe[1]];
							for(i = 0; i < 42 - wheresPipe[0]; i++);						
								tempArgs[i] = theArguments[i + wheresPipe[1]];
							char* commandThree = strdup(tempArgs[0]);
							close(thePipe[0]);
							close(thePipe[1]);
							close(0);
							dup2(thePipe2[0],0);
							close(thePipe2[0]);
							close(thePipe2[1]);
							token = strtok(path, delim);
							while(token != NULL && !found)
							{
								if(commandThree[0] != '/')
								{
									tempPath = strdup(token);
									strcat(tempPath, "/");
									strcat(tempPath, commandThree);
									file = fopen(tempPath, "r");
								}
								else
								{
									file = fopen(commandThree, "r");
									tempPath = strdup(commandThree);
								}
								if(file)
								{
									found = TRUE;
								}
								token = strtok(NULL, delim);
							}
							if(!found)
							{
								printf("%s: no %s in (%s)\n", commandThree, commandThree, getenv("PATH"));
							}
							else
							{
								execv(tempPath, tempArgs);
							}
						}
						else
						{
							close(thePipe[0]);
							close(thePipe[1]);
							close(thePipe2[0]);
							close(thePipe2[1]);
							waitpid(-1, &status, 0);
						}
					}
				}
			}
			else
			{
				printf("Too many pipes! Only implemented for 2 pipes in one command!\n");
			}
		}
		else
		{
			char* theArguments[] = {tempPath, NULL};
			int status;
//			printf("Path: %s\nArguments: %s, %s\n", tempPath, theArguments[0], theArguments[1]);		
	
			if((pid = fork()) == 0)
				execv(tempPath, theArguments);
			else
				waitpid(-1, &status, 0);
			
		}
	}
	return;
}
/* MODULES */ 
/* ==================================================================== */
/* END OF PROGRAM */


