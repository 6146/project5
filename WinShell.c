#define _WIN32_WINNT 0x0501         /*Windows API�汾*/
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winbase.h>
#include <Wincon.h>
#include <tlhelp32.h>
#include <malloc.h>
#include <string.h>
#include <direct.h>
#include "WinShell.h"
void cd_cmd(char *route);
void dir_cmd(char *route , int way);
void ftime(FILETIME filetime);
void GetProcessList();
void add_history(char *inputcmd);
void history_cmd();
HANDLE process(int bg, char appName[]);
BOOL killProcess(char *pid);
BOOL WINAPI ConsoleHandler(DWORD CEvent);
void help();
void fgProcsee(char *pid);

char path[BUFSIZE];
HANDLE win;
int pid_for_fg;            //��̨���� pid
int fg_flag;               //��̨�ܵ������־
int bg_flag;               //ǰ̨���б�־
int pid_for_bg;            //ǰ̨���̺�

HANDLE wait[2];            //���ڽ���ͬ��

int main()
{
	/******************************�����������õ��ĺ���**************************************/

	void cd_cmd(char *dir);                     /*��ʾcd����*/
	void dir_cmd(char *dir  , int way);         /*��ʾdir����*/
	void ftime(FILETIME filetime);              /*��ʾ�ļ�����ʱ��*/
	void GetProcessList();                      /*���ϵͳ��ǰ�����б�*/
	void history_cmd();                         /*���������������*/
	void add_history(char *inputcmd);           /*�������������ӵ�������ʷ��*/
	HANDLE process(int bg, char appName[]);                     /*��������*/
    BOOL killProcess(char *pid);                /*kill����*/
    BOOL WINAPI ConsoleHandler(DWORD CEvent);   /*�ص�����*/
	void help();                                /*��ʾ������Ϣ*/

	char c, *input, *arg[2];
	int input_len = 0, is_bg = 0, i, j, k;
	HANDLE hprocess;              /*����ִ�н��������ؽ��̾��*/
	DWORD dwRet;
    wait[1] = CreateEvent(NULL,TRUE,FALSE,NULL);
	while(1)
	{
		/*��ָ�����������ָ�������ʼ��*/
		for(i= 0; i < 2; i++)
			arg[i] = NULL;
		/*��õ�ǰĿ¼�����صĵ�ַ���롰path���У�BUFSIZE������ܹ�����ĵ�ַ����*/
		dwRet = GetCurrentDirectory(BUFSIZE, path);

		if( dwRet == 0 )
		{
			/*���ص�ǰĿ¼ʧ�ܣ����������Ϣ*/
			printf("GetCurrentDirectory failed (%d)\n", GetLastError());

		}
		else if(dwRet > BUFSIZE)
		{
			/*BUFSIZE����С�ڷ��ص�ַ���ȣ����������ٳ���*/
			printf("GetCurrentDirectory failed (buffer too small; need %d chars)\n", dwRet);

		}
		else
			/*�����ǰĿ¼*/
			printf("%s>", path);


		/************************************����***********************************************/

		input_len = 0;
		/*�������ַ����˵�*/
		while((c = getchar()) ==  ' ' || c == '\t' || c == EOF)
			;
		if(c == '\n')                   /*����Ϊ��ʱ��������ѭ����ӡ��ʾ��*/
			continue;
		while(c != '\n')
		{
			buf[input_len++] = c;
			c = getchar();
		}
		buf[input_len++] =  '\0';       /*���ϴ�������*/

		/*���䶯̬�洢�ռ䣬������ӻ��渴�Ƶ�input��*/
		input = (char*) malloc(sizeof(char)*(input_len));
		strcpy(input, buf);


		/************************************����ָ��********************************************/

		// TODO: ������������н��������洢���ַ���������
		for(i=0,j=0,k=0;i<input_len;i++){
		    if(input[i] ==' '|| input[i] =='\0'){
                if(j==0){
                    continue;
                }
                else{
                    buf[j++]='\0';
                    arg[k]=(char*)malloc(sizeof(char*)*j);
                    strcpy(arg[k++],buf);
                    j=0;
                }
		    }
		    else{
                buf[j++]=input[i];
		    }
		}
		/*********************************�ڲ������******************************************/

        //cd ����
        if(strcmp(arg[0] , "cd")==0){
            add_history(input);
            for(i=3 , j=0;i<=input_len;i++){
                buf[j++]=input[i];
            }
            buf[j]='\0';
            arg[1]=(char*)malloc(sizeof(char)*j);
            strcpy(arg[1],buf);
            cd_cmd(arg[1]);
            free(input);
            continue;
        }
        //dir
        if(strcmp(arg[0],"dir")==0||strcmp(arg[0],"dir/A")==0||strcmp(arg[0],"dir/B")==0||strcmp(arg[0],"dir/C")==0){
            char *route;
            add_history(input);
            if(strcmp(arg[0],"dir")==0){
                if(arg[1]==NULL){
                    route = path;
                    dir_cmd(route , 0);
                }
                else{
                    dir_cmd(arg[1] , 0);
                }
                free(input);
                continue;
            }
            else{
                if(arg[0][4]=='B'){
                    if(arg[1]==NULL){
                        route = path;
                        dir_cmd(route , 1);
                    }
                    else{
                        dir_cmd(arg[1] , 1);
                    }
                    free(input);
                    continue;
                }
                else if(arg[0][4]=='C'){
                    if(arg[1]==NULL){
                        route = path;
                        dir_cmd(route , 2);
                    }
                    else{
                        dir_cmd(arg[1] , 2);
                    }
                    free(input);
                    continue;
                }
                else{
                    if(arg[1]==NULL){
                        route = path;
                        dir_cmd(route , 0);
                    }
                    else{
                        dir_cmd(arg[1] , 0);
                    }
                    free(input);
                    continue;
                }
            }
        }

        //tasklist

        if(strcmp(arg[0],"tasklist")==0){
            add_history(input);
            GetProcessList();
            free(input);
            continue;
        }

        //ǰ̨����

        if(strcmp(arg[0],"fp")==0){
            add_history(input);
            if(arg[1]==NULL){
                printf("û��ָ����ִ���ļ�\n");
                free(input);
                continue;
            }
            is_bg = 0;
            hprocess=process(is_bg,arg[1]);

            wait[0]=hprocess;

            //if(WaitForSingleObject(hprocess,INFINITE)==WAIT_OBJECT_0)
            WaitForMultipleObjects(2,wait,FALSE,-1);
            printf("ǰ̨�������н�����ת����̨��\n");
            free(input);
            // �ͷŹ���
            SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,FALSE);
            continue;
        }

        //��̨����

        if(strcmp(arg[0],"bg&")==0){
            add_history(input);
            if(arg[1]==NULL){
                printf("û��ָ����ִ���ļ�\n");
                free(input);
                continue;
            }
            is_bg=1;
            process(is_bg,arg[1]);
            free(input);
            continue;
        }

        //kill
        if(strcmp(arg[0],"taskkill")==0){
            BOOL success;
            add_history(input);
            success = killProcess(arg[1]);
            if(!success){
                printf("kill process failed!\n");
            }
            free(input);
            continue;
        }

        //��ʾ��ʷָ��
        if(strcmp(arg[0],"history")==0){
            add_history(input);
            history_cmd();
            free(input);
            continue;
        }

        //exit
        if(strcmp(arg[0],"exit")==0){
            add_history(input);
            printf("bye bye!\n");
            free(input);
            break;
        }

        if(strcmp(arg[0],"fg&")==0){
            add_history(input);
            if(arg[1]==NULL){
                printf("û��ָ�����̺�\n");
                free(input);
                continue;
            }
            fgProcsee(arg[1]);
            free(input);
            continue;
        }
        if(strcmp(arg[0],"help")==0)
        {
            add_history(input);
            help();
        }
        /*pipe*/
		if(arg[1]!=NULL&&strcmp(arg[1],"|")==0)
		{
			add_history(input);
			if(arg[2]!=NULL)
			pipe(arg[0],arg[2]);
			free(input);
			continue;
		}

        else{
            printf("please type in correct command\n");
            continue;
        }
		// TODO: ���ݽ��������������Ӧ���������
	}
}

/**********************************��������*******************************************/


/*********************************����������*****************************************/


/**********************************cd����**********************************************/

void cd_cmd(char *route)
{

	if( !SetCurrentDirectory(route))                /*���õ�ǰĿ¼��ʧ�ܷ��س�����Ϣ*/
	{
		printf(TEXT("SetCurrentDirectory failed (%d)\n"), GetLastError());
	}
}


/************************************dir����*********************************************/

void dir_cmd(char *route , int way)
{

	WIN32_FIND_DATA FindFileData;                /*�����ҵ����ļ���Ŀ¼��WIN32_FIND_DATA�ṹ����*/
	files_Content head, *p, *q;	                 /*����ָ���ļ��ṹ���ָ��*/
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError;
	char typeBuf[36];
	char volume_name[256];
	int file = 0, dir = 0;                             /*�ļ���Ŀ��Ŀ¼��Ŀ��ʼֵΪ0*/
	__int64 sum_file = 0;                            /*���ļ���С��Bytes��ʾֵ�ϴ󣬱���Ϊ64λ����*/
	__int64 l_user, l_sum, l_idle;                 /*���ÿռ䣬�����������ÿռ�*/
	unsigned long volume_number;
	char *DirSpec[3];

	DirSpec[0] = (char*)malloc(sizeof(char) * 2);
	strncpy(DirSpec[0], route, 1);
	*(DirSpec[0] + 1) = '\0';                      /*DirSpec[0]Ϊ��������*/
	DirSpec[1] = (char*)malloc(sizeof(char) * 4);
    strcpy(DirSpec[1], DirSpec[0]);
	strncat(DirSpec[1], ":\\", 3);                  /*DirSpec[1]���ڻ����������Ϣ*/
    DirSpec[2] = (char*)malloc(sizeof(char) * (strlen(route) + 2));
    DirSpec[3] = (char*)malloc(sizeof(char) * (strlen(route) + 5));
	strcpy(DirSpec[2], route);
	strcpy(DirSpec[3], route);
	strncat(DirSpec[2], "\\", 2);	              /*DirSpec[2]Ϊdir�����Ŀ¼��*/
	strncat (DirSpec[3], "\\*.*", 5);             /*DirSpec[3]���ڲ���Ŀ¼�е��ļ�*/

	hFind = FindFirstFile(DirSpec[3], &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)            /*���Ҿ������Ϊ��Чֵ������ʧ��*/
	{
		printf ("Invalid file handle. Error is %u\n", GetLastError());

	}
	else
	{
		/*��ȡ��������Ϣ*/
		GetVolumeInformation(DirSpec[1], volume_name, 50, &volume_number, NULL, NULL, NULL, 10);
        if(way==0){
            if(strlen(volume_name) == 0)
                printf(" ������ %s �еľ�û�б�ǩ��\n", DirSpec[0]);
            else
                printf(" ������ %s �еľ��� %s \n", DirSpec[0], volume_name);
            printf(" ������к��� %X \n\n", volume_number);
            printf("  %s ��Ŀ¼ \n\n", DirSpec[2]);
        }
            head.time = FindFileData.ftCreationTime;              /*����ļ�����ʱ�䣬�����ļ��ṹ��*/
            strcpy(head.name, FindFileData.cFileName);             /*����ļ����������ļ��ṹ��*/
		/*�������������Ŀ¼��typeλΪ0*/
		if( FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			head.type = 0;
			dir++;
		}
		else
		{
			/*��������������ļ���typeλΪ1*/
			head.type = 1;
			head.size = FindFileData.nFileSizeLow;           /*���ļ���С����ṹ����*/
			file++;
			sum_file += FindFileData.nFileSizeLow;             /*���ļ���С�ۼ�*/
		}
        p = &head;

		/*���������һ�����ݣ���������*/
		while (FindNextFile(hFind, &FindFileData) != 0)
		{

			q = (files_Content*)malloc(sizeof(files_Content));
			q->time = FindFileData.ftCreationTime;
			strcpy(q->name, FindFileData.cFileName);
			if( FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				q->type = 0;
				dir++;
			}
			else
			{
				q->type = 1;
				q->size = FindFileData.nFileSizeLow;
				file++;
				sum_file += FindFileData.nFileSizeLow;
			}
			p->next = q;
			p = q;

		}
		p->next = NULL;
		p = &head;
		/*���ṹ�������ݵĴ���ʱ�䡢���͡���С�����Ƶ���Ϣ�������*/
		if(way==0){
            while(p != NULL)
            {
                ftime(p->time);
                if(p->type == 0)
                    printf("\t<DIR>\t\t");
                else
                    printf("\t\t%9lu", p->size);
                printf ("\t%s\n", p->name);
                p = p->next;
            }
        }
        else if(way ==1){
            while(p!=NULL)
            {
                printf("%s\n",p->name);
                p = p->next;
            }
        }
        else{
            while(p!=NULL)
            {
                ftime(p->time);
                if(p->type == 0)
                    printf("\t<DIR>\t\t");
                else{
                    toStringSize(p->size,typeBuf);
                    printf("\t\t%9s", typeBuf);
                }
                printf ("\t%s\n", p->name);
                p = p->next;
            }
        }
		free(p);
		/*��ʾ�ļ���Ŀ¼���������̿ռ������Ϣ*/
		printf("%15d ���ļ�\t\t\t%I64d �ֽ� \n", file, sum_file);
		GetDiskFreeSpaceEx(DirSpec[1], (PULARGE_INTEGER)&l_user, (PULARGE_INTEGER)&l_sum, (PULARGE_INTEGER)&l_idle);
		printf("%15d ��Ŀ¼\t\t\t%I64d �����ֽ� \n", dir, l_idle);

		dwError = GetLastError();
		FindClose(hFind);
		/*������������쳣��������������Ϣ*/
		if (dwError != ERROR_NO_MORE_FILES)
		{
			printf ("FindNextFile error. Error is %u\n", dwError);
		}
	}
}


/************************************ʱ�䴦����********************************************/

void ftime(FILETIME filetime)
{
	SYSTEMTIME systemtime;
	/*Win32ʱ��ĵ�32λ*/
	if (filetime.dwLowDateTime == -1)
	{
		wprintf(L"Never Expires ");
	}
	else
	{
		/*��UTC(Universal Time Coordinated)�ļ�ʱ��ת���ɱ����ļ�ʱ��*/
		if (FileTimeToLocalFileTime(&filetime, &filetime) != 0)
		{
            /*��64λʱ��ת��Ϊϵͳʱ��*/
			if (FileTimeToSystemTime(&filetime, &systemtime) != 0)
			{
				char str[50];
				/*��һ����ʽ���ʱ��*/
				wsprintf(str, "%d-%02d-%02d  %02d:%02d",
					systemtime.wYear, systemtime.wMonth, systemtime.wDay,
					systemtime.wHour, systemtime.wMinute);
				printf("%s", str);

			}
			else
			{   /*wprintf���UNICODE�ַ�*/
				wprintf(L"FileTimeToSystemTime failed ");
			}

		}
		else
		{
			wprintf(L"FileTimeToLocalFileTime failed ");
		}
	}
}


/**************************************��ȡϵͳ��������********************************************/

void GetProcessList()
{
	HANDLE  hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {0};
	/*��ϵͳ�н��̽�������*/
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if( hProcessSnap == INVALID_HANDLE_VALUE)
		printf("\nCreateToolhelp32Snapshot() failed:%d", GetLastError ());

	/*ʹ��ǰҪ���ṹ��С*/
	pe32.dwSize = sizeof(PROCESSENTRY32);
	/*�г�����*/
	if( Process32First (hProcessSnap, &pe32) )
	{
		DWORD dwPriorityClass;
		printf("\n���ȼ�\t\t����ID\t\t�߳�\t\t������\n");
		do{
			HANDLE hProcess;
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			dwPriorityClass = GetPriorityClass (hProcess);
			CloseHandle(hProcess);
			/*������*/
			printf("%d\t", pe32.pcPriClassBase);
			printf("\t%d\t", pe32.th32ProcessID);
			printf("\t%d\t", pe32.cntThreads);
            printf("\t%s\n", pe32.szExeFile);
		}
		while(Process32Next (hProcessSnap, &pe32));
	}
	else
		printf("\nProcess32finst() failed:%d", GetLastError ());
	CloseHandle (hProcessSnap);
}


/************************************history����*******************************************/

void add_history(char *inputcmd)
{
	/*endǰ��һλ*/
	envhis.end = (envhis.end + 1) % HISNUM;
	/*end��startָ��ͬһ����*/
	if(envhis.end == envhis.start)
	{
		/*startǰ��һλ*/
		envhis.start = (envhis.start+1) % HISNUM;
	}
	/*���������endָ���������*/
	strcpy(envhis.his_cmd[envhis.end], inputcmd);
}


/************************************��ʾhistory����********************************************/

void history_cmd()
{
	// TODO: ע�⣬������history���������⣬����ע���޸ġ�

	int i, j = 1;
	/*ѭ������Ϊ�գ�ʲôҲ����*/
	if(envhis.start == envhis.end)
		;
	else if(envhis.start < envhis.end){
		/*��ʾhistory����������start+1��end������*/
		/*for(i = envhis.start + 1;i <= envhis.end; i++)
		{
			printf("%d\t%s\n", j, envhis.his_cmd[i]);
			j++;
		}*/
		for(i=envhis.end;i>envhis.start;i--){
            printf("%d\t%s\n", j, envhis.his_cmd[i]);
            j++;
		}
	}else {
		/*��ʾhistory����������start+1��HISNUM-1������*/
		for(i = envhis.start + 1;i < HISNUM; i++)
		{
			printf("%d\t%s\n", j, envhis.his_cmd[i]);
			j++;
		}
		/*��ʾhistory����������0��end+1������*/
		for (i = 0; i <= envhis.end+1; i++)
		{
			printf("%d\t%s\n", j, envhis.his_cmd[i]);
			j++;
		}
	}
}


/***********************************������������*******************************************/

HANDLE process(int bg, char appName[])
{
    BOOL tmp;
	/*��ʼ�����������Ϣ*/
	STARTUPINFO si;
	PROCESS_INFORMATION   pi;
	/*���ڰ汾����*/
	si.cb = sizeof(si);
	GetStartupInfo(&si);
	/*��ȥpi������*/
	ZeroMemory(&pi, sizeof(pi));
	/*ǰ̨����*/
	if(bg == 0)
	{
	    si.dwFlags = STARTF_USESHOWWINDOW;
		/*���ش���*/
		si.wShowWindow = SW_SHOW;//SW_HIDE;
		/*���ù��ӣ���׽ctrl+c����յ�����������*/
		if(SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE) == FALSE)
		{
			printf("Unable to install handler!\n");
				return NULL;
		}
		/*���ý�����س��򣬴˴�����һ���Լ���д�ĳ���,����п���̨�����ע��·����ȷ*/
		bg_flag=1;
		if(CreateProcess(NULL, appName, NULL, NULL, FALSE, CREATE_NEW_CONSOLE , NULL, NULL, &si, &pi)){
        /*struct ProcessWindow procwin;
        procwin.dwProcessId = pi.dwProcessId;
        procwin.hwndWindow = NULL;

        // �ȴ��½��̳�ʼ�����
        WaitForInputIdle(pi.hProcess, 5000);
        // ����������
        EnumWindows(EnumWindowCallBack, (LPARAM)&procwin);
        win = procwin.hwndWindow;*/
        pid_for_bg=pi.dwProcessId;
        printf(" �½��̵Ľ���ID�ţ�%d \n", pi.dwProcessId);
        printf(" �½��̵����߳�ID�ţ�%d \n", pi.dwThreadId);
		}
		return pi.hProcess;

	}
	/*��̨����*/
	else
	{
		/*���ý��̴���ѡ��*/
		si.dwFlags = STARTF_USESHOWWINDOW;
		/*���ش���*/
		si.wShowWindow = SW_SHOW;//SW_HIDE;
		tmp=CreateProcess(NULL, appName, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
		if(tmp){
        printf(" �½��̵Ľ���ID�ţ�%d \n", pi.dwProcessId);
        printf(" �½��̵����߳�ID�ţ�%d \n", pi.dwThreadId);
		}
		return NULL;
	}
}


/***********************************kill��������*******************************************/

BOOL killProcess(char *pid)
{
	int id, i;
	DWORD   dwExitStatus;
	HANDLE hprocess;
	id = atoi(pid);
	hprocess = OpenProcess(PROCESS_TERMINATE, FALSE, id);
	GetExitCodeProcess(hprocess, &dwExitStatus);
	if(i = TerminateProcess(hprocess, dwExitStatus))
		return TRUE;
	else
		return FALSE;
}


/***********************************��̨��ǰ̨*****************************************/
void fgProcsee(char *pid)
{
    int id, i ;

	HANDLE hprocess;         //Ŀ�����
	id = atoi(pid);
	hprocess = OpenProcess(PROCESS_VM_OPERATION|SYNCHRONIZE, FALSE, id);
	if(SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE) == FALSE)
		{
			printf("Unable to install handler!\n");
				return NULL;
		}
	if(hprocess==NULL){
        printf("������ָ������\n");
        return;
	}
    fg_flag=1;
    pid_for_fg=id;
    bg_flag=1;
    pid_for_bg=id;



    wait[0]=hprocess;
    WaitForMultipleObjects(2,wait,FALSE,-1);
    printf("ǰ̨�������н�����ת����̨��\n");
	fg_flag=0;
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,FALSE);
	return;
}

/***********************************�ص�����*******************************************/

BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
    char buf[16];
	DWORD   dwExitStatus;
	HANDLE hprocess , win;
	switch(CEvent)
	{
	case CTRL_C_EVENT: {
        printf("ctrl-c\n");                /*��ϵͳ�����¼�������ctrl+c��*/
        if(fg_flag==1){
            hprocess = OpenProcess(PROCESS_TERMINATE, FALSE, pid_for_fg);
            GetExitCodeProcess(hprocess, &dwExitStatus);
            TerminateProcess(hprocess, dwExitStatus);
        }

        if(bg_flag==1){
            hprocess = OpenProcess(PROCESS_TERMINATE, FALSE, pid_for_bg);
            GetExitCodeProcess(hprocess, &dwExitStatus);
            TerminateProcess(hprocess, dwExitStatus);
        }
		break;
	}
	case CTRL_BREAK_EVENT:{
	    printf("ctrl-break\n");
        if(bg_flag==1){
            susp(pid_for_bg);
            hprocess = OpenProcess(PROCESS_TERMINATE, FALSE, pid_for_bg);
            while(1){
                printf("%s>",path);
                scanf("%s",buf);
                if(strcmp(buf,"bg")==0){
                    add_history(buf);
                    resp(pid_for_bg);
                    PulseEvent(wait[1]);      //����һ��һ���ź�
                    //������ ������������



                    break;
                }

                if(strcmp(buf,"fg")==0){
                    add_history(buf);
                    resp(pid_for_bg);
                    break;
                }
            }
        }
	}
	case CTRL_CLOSE_EVENT:
		break;
	case CTRL_LOGOFF_EVENT:
		break;
	case CTRL_SHUTDOWN_EVENT:
		break;
	}
	return TRUE;
}

/*********************************תΪǧλ���ָ���*************************/
void toStringSize(int value , char *in )
{
    int i=0,j=0,n=value,k;
    int flag=0;
    char tmp[32];
    while(n){
        k=n%10;
        n=n/10;
        tmp[i++]='0'+k;
        if(flag++/2==1&&n!=0){
            flag=0;
            tmp[i++]=',';
        }
    }
    in[i--]='\0';
    for(;i>=0;i--){
        in[j++]=tmp[i];
    }
}
/***********************************��ʾ����*******************************************/


void help()
{
// TODO: ��ӱ�Ҫ��ע�������Ϣ
    printf("cd:�л���ǰĿ¼��\n������ʽ��cd [drive:][path](cd C:\\temp)  \nע��cd�����Կո�Ϊ�ָ�������������Ͳ�����\n\n");
    printf("dir:��ʾĿ¼�е��ļ������ļ��б�\n������ʽ��dir [drive:][path](dir C:\\temp) \nע��dir�����Կո�Ϊ�ָ�������������Ͳ�����\n\n");
	printf("tasklist:��ʾϵͳ�е�ǰ�Ľ�����Ϣ��\n������ʽ��tasklist\n\n");
	printf("fp:�������̲���ǰִ̨�С�\n������ʽ��fp\n\n");
	printf("bg:�������̲��ں�ִ̨�С�\n������ʽ��bg\n\n");
	printf("taskkill:��ֹ���̡�\n������ʽ��taskkill [pid]\nע��taskkill�����Կո�Ϊ�ָ�����pidΪ����id��\n\n");
	printf("history:��ʾ��ʷ���\n������ʽ��history\n\n");
	printf("exit:�˳�\n������ʽ��exit\n\n");
	printf("����ctrl-break ����ǰ̨���̣�������ָ����ʽ��bg :��̨���� ��fg ��ǰ̨����\n");

}

//***********************************��ͣ/�ָ�����*************************************/
void susp(int id)
{
    HANDLE hProcess;
    HANDLE hSnapshoHandle;
    HANDLE hCurrentThread;
    BOOL bIsFound;
    THREADENTRY32 CThreadEny;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);

    hSnapshoHandle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, id);
    CThreadEny.dwSize=sizeof(THREADENTRY32);
    bIsFound = Thread32First(hSnapshoHandle, &CThreadEny);
    printf("%d\n",id);
    while(bIsFound != FALSE){

        bIsFound = Thread32Next(hSnapshoHandle, &CThreadEny);
        hCurrentThread = OpenThread(THREAD_ALL_ACCESS, FALSE, CThreadEny.th32ThreadID);

        if(hCurrentThread != NULL){
            if(CThreadEny.th32OwnerProcessID == id){
                SuspendThread(hCurrentThread);
            }
        }
    }

    CloseHandle(hSnapshoHandle);
}
void resp(int id)
{
    HANDLE hProcess;
    HANDLE hSnapshoHandle;
    HANDLE hCurrentThread;
    BOOL bIsFound;
    THREADENTRY32 CThreadEny;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);

    hSnapshoHandle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, id);
    CThreadEny.dwSize=sizeof(THREADENTRY32);
    bIsFound = Thread32First(hSnapshoHandle, &CThreadEny);

    while(bIsFound != FALSE){

        bIsFound = Thread32Next(hSnapshoHandle, &CThreadEny);
        hCurrentThread = OpenThread(THREAD_ALL_ACCESS, FALSE, CThreadEny.th32ThreadID);

        if(hCurrentThread != NULL){
            if(CThreadEny.th32OwnerProcessID == id){
                ResumeThread(hCurrentThread);
            }
        }
    }

    CloseHandle(hSnapshoHandle);
}

/***********************************�ܵ�**************************************/
void pipe(char appName1[],char appName2[])
{
    HANDLE hRead1,hRead2,hWrite1,hWrite2;
	STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    char szReadBuf[100];
    DWORD nReadNum;
    BOOL bRet;
	HANDLE hTemp=GetStdHandle(STD_OUTPUT_HANDLE);

	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle=TRUE;    //?????TRUE??????????��????????????????
    sa.lpSecurityDescriptor=NULL;
    sa.nLength=sizeof(SECURITY_ATTRIBUTES);
	bRet = CreatePipe(&hRead1,&hWrite1,&sa,0);
	bRet = CreatePipe(&hRead2,&hWrite2,&sa,0);
    GetStartupInfo( &si );
    si.dwFlags=STARTF_USESTDHANDLES;
    si.hStdOutput=hWrite1;
    si.hStdError=hWrite1;

	//?????????
    bRet = CreateProcess( NULL, appName1, NULL, NULL, TRUE,0, NULL, NULL, &si, &pi);

	CloseHandle( hWrite1 );

	GetStartupInfo( &si );
	si.dwFlags=STARTF_USESTDHANDLES;
    si.hStdInput=hRead1;
	si.hStdOutput=hWrite2;
    si.hStdError=hWrite2;

	bRet = CreateProcess( NULL, appName2, NULL, NULL, TRUE,0, NULL, NULL, &si, &pi);

	CloseHandle( hWrite2 );

	//???????��?????

    while ( ReadFile( hRead2, szReadBuf, 100, &nReadNum, NULL) )
    {
        szReadBuf[nReadNum] = '\0';
        printf( "%s\n", szReadBuf);
    }
	return;
}

