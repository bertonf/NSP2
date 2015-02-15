#ifndef SVRFILE_H
#define SVRFILE_H
/*
[USRINFO]
NAME=TOTO
IP=10.10.10.10
PORT=25000
*/

#define SECTIONSERV "[SERVINFO]"

typedef struct s_svrData
{
    char* name;
    char* ip;
    char* port;
}svrData;

svrData* newSvrData();
svrData* GetSvrData(char *svrName);
int SaveSvrData(svrData *sData);
void DebugTraceSvrData(svrData* svr);
void FreeSvrData(svrData* svr);

#endif // SVRFILE_H
