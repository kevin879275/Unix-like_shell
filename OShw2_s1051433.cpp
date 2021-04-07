#include<iostream>
using namespace std;
#include<string.h>
#include<vector>
#include<string>
#include<sstream>
#include <unistd.h>
#include<errno.h>
#include<dirent.h>
#include <pwd.h>
#include <sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<fcntl.h>
bool checkredirect(vector<string>arr,int size)
{
	if(size==0)
		return false;
	if(size>=1)
	{
		for(int i=0;i<size;i++)
			for(int j=0;j<arr[i].length();j++)
		if(arr[i][j]=='<'||arr[i][j]=='>')
			return true;
		return false;
	}
	return false;
}
int checkpipe(vector<string>arr,int size)
{
	int flag=0;
	if(size==0)
		return 0;
	if(size>=1)
	{
		for(int i=0;i<size;i++)
			for(int j=0;j<arr[i].length();j++)
		if(arr[i][j]=='|')
			flag++;	
	}
	return flag;
}
void pipe(char* input, int pip)
{
	if(pip==1)
	{
		char *tmp[100];
                char *argv[100];
                int fd[2];
                int count=1;
		int status;
                char *argv1[100];
                tmp[0] = strtok(input, "|");
               
                for(int i=1;tmp[i-1]!=NULL;i++)
                    tmp[i]=strtok(NULL,"|");
                   
                argv[0] = strtok(tmp[0], " ");
                for (int i = 1; argv[i-1] != NULL; i++)
                    argv[i] = strtok(NULL, " ");
                argv1[0] = strtok(tmp[1], " ");
                for (int i = 1; argv1[i - 1] != NULL; i++)               
                   		argv1[i] = strtok(NULL, " ");	
		if(pipe(fd)==-1)
		{
                   	cout<<"pipe wrong!"<<endl;
                    	exit(1);
		}
                pid_t pid = 0;
                pid = fork();
	
                if (pid == 0)
		{
                    	dup2(fd[1],STDOUT_FILENO);
                    	close(fd[0]);
                    	execvp(argv[0], argv);
                    	cout << "error" << endl;
                    	exit(1); 
		}       
                pid=fork();
                if (pid == 0)
		{
                   
                	dup2(fd[0],STDIN_FILENO);
                    	close(fd[1]);
               	        execvp(argv1[0], argv1);
                        cout << "error" << endl;
                        exit(1);
                }
                	close(fd[0]);
                	close(fd[1]);
                	wait(&status);
                	wait(&status);
	}
	else
	{	
		int status;
		char *argv_tmp[100];
		char *convert_argv[100];
		int fd[4];
		int count=1;
		char *convert_argv1[100];
		char *convert_argv2[100];
		argv_tmp[0] = strtok(input, "|");				
		for(int i=1;argv_tmp[i-1]!=NULL;i++)
			argv_tmp[i]=strtok(NULL,"|");
							
		convert_argv[0] = strtok(argv_tmp[0], " ");
			for (int i = 1; convert_argv[i-1] != NULL; i++)
				convert_argv[i] = strtok(NULL, " ");
							
			convert_argv1[0] = strtok(argv_tmp[1], " ");
			for (int i = 1; convert_argv1[i - 1] != NULL; i++)				
				convert_argv1[i] = strtok(NULL, " ");
							
			convert_argv2[0] = strtok(argv_tmp[2], " ");
			for (int i = 1; convert_argv2[i - 1] != NULL; i++)				
				convert_argv2[i] = strtok(NULL, " ");
							
			if(pipe(fd)==-1)
			{
				cout<<"pipe wrong!"<<endl;
				exit(1);
			}
			if(pipe(fd+2)==-1)
			{
				cout<<"pipe wrong!"<<endl;
				exit(1);
			}
			pid_t child = 0;
			child = fork();
			
			if (child == 0)
			{
				dup2(fd[1],STDOUT_FILENO);
				close(fd[0]);
				close(fd[1]);
				close(fd[2]);
				close(fd[3]);
				execvp(convert_argv[0], convert_argv);
				cout << "error" << endl;
				exit(1);
			}
			child=fork();
			if (child == 0)
			{
							
				dup2(fd[0],STDIN_FILENO);
				dup2(fd[3],STDOUT_FILENO);
				close(fd[0]);
				close(fd[1]);
				close(fd[2]);
				close(fd[3]);
				execvp(convert_argv1[0], convert_argv1);
				cout << "error" << endl;
				exit(1);
			}
			child=fork();
			if (child == 0)
			{
						
				dup2(fd[2],STDIN_FILENO);
				close(fd[0]);
				close(fd[1]);
				close(fd[2]);
				close(fd[3]);
				execvp(convert_argv2[0], convert_argv2);
				cout << "error" << endl;
				exit(1);
			}
			close(fd[0]);
			close(fd[1]);
			close(fd[2]);
			close(fd[3]);
			wait(&status);
			wait(&status);
			wait(&status);							
	}
}
void spawn(vector<string> arr,int c)
{	
	char ** arg_list = new char*[c+1];
	for(int i=0;i<c;i++)
	{
		arg_list[i]=new char[arr[i].length()+1];
		strcpy(arg_list[i],arr[i].c_str());
	}
	arg_list[c]=NULL;
		
	pid_t pid,pid2;
	int status;
	pid = fork();	
	if(pid<0)
		cout<<"fork error"<<endl;
	if(pid==0)
	{
		execvp(arg_list[0],arg_list);
		//sleep(3);
		exit(-3);
	}
	else
	{
		pid2=wait(&status);
	}
}
string getworkingpath()
{
	int maxpathlen=255;
	char temp[maxpathlen];
	return(getcwd(temp,maxpathlen)?string(temp):string(""));
}
string getusername()
{
	struct passwd *p;
	uid_t uid;
	p=getpwuid(uid=geteuid());
	return p->pw_name;
}
int main()
{	
	string path;
	int pip;
	while (1)
	{
		
		path = getworkingpath();
		cout<<getusername()<<"@"<<path<<">";
		vector<string>arr;                             
		string input;                                      
		string token;
		char *cmd;
		cmd=readline("");
		input.assign(cmd);
		add_history(cmd);
		int c = 0;                                             
		//getline(cin, input);                
		istringstream delim(input);               
		while (getline(delim, token, ' '))           
		{
			if(token!="")
			{
				arr.push_back(token);                 
				c++;
			}                                            
		}
		pip=checkpipe(arr,c);	
		bool redirec = checkredirect(arr,c);
		char * put = new char[input.length()+1];
		strcpy(put,input.c_str());
		if (arr[0] == "exit")
		{
			cout << "ByeBye!"<<endl;
			break;
		}	
		else if(arr[0]=="cd")
		{
			char *exchange = new char[arr[1].length()+1];
			strcpy(exchange,arr[1].c_str());
			chdir(exchange);
			delete[]exchange;
		}
		else
		{
			if(pip == 0&&redirec==false)
				spawn(arr,c);
			else if(pip>0&&redirec==false)
				pipe(put,pip);
		}
	}
	return 0;
}
