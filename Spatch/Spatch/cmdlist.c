#include <libssh/libssh.h>
#include <stdlib.h>
#include <string.h>
#include "Spatch.h"
#include "usrFile.h"
#include "cmdlist.h"
#include "svrFile.h"

void cmd_createuser(char **cmd, sessionData* sesData)
{
    if (cmd[0] != NULL && cmd[1] != NULL && cmd[2] != NULL)
    {
        usrData *newuser = newUsrData();

        newuser->name = malloc(strlen(cmd[1]) + 1);
        newuser->pwd = malloc(strlen(cmd[2]) + 1);
        memset(newuser->name, 0, strlen(cmd[1]) + 1);
        memset(newuser->pwd, 0, strlen(cmd[2]) + 1);
        strcpy(newuser->name, cmd[1]);
        strcpy(newuser->pwd, cmd[2]);
        if (strncmp(cmd[3], "user", 4))
            newuser->spaAccess = USER;
        else if (strncmp(cmd[3], "admin", 5))
            newuser->spaAccess = ADMIN;
        else
        {
            FreeUsrData(newuser);
            ssh_channel_write(sesData->channel,
                              "usage \"createuser user pwd user|admin\"",
                              strlen("usage \"createuser user pwd user|admin\""));
            return;
        }
        SaveUsrData(newuser);
        FreeUsrData(newuser);
    }
    else
    {
        ssh_channel_write(sesData->channel,
                          "usage \"createuser user pwd user|admin\"",
                          strlen("usage \"createuser user pwd user|admin\""));
    }

}

void cmd_deleteuser(char **cmd, sessionData* sesData)
{
    int status;

    if (cmd[0] != NULL && cmd[1] != NULL)
    {
        //username.usr
        status = remove(cmd[1]);
        if (status == 0)
        {
            ssh_channel_write(sesData->channel,
                              "user succesfully deleted\n",
                              strlen("user succesfully deleted\n"));
        }
        else
        {
            ssh_channel_write(sesData->channel,
                              "delete user failure\n",
                              strlen("delete user failure\n"));
        }
    }
}


void cmd_createserver(char **cmd, sessionData* sesData)
{
    //char* name;
    //char* ip;
    //char* port;
 // -> svrData;
    if (cmd[0] != NULL && cmd[1] != NULL && cmd[2] != NULL && cmd[3] != NULL)
    {
        svrData *newsvr = newSvrData();
        newsvr->name = malloc(strlen(cmd[1]) + 1);
        newsvr->ip = malloc(strlen(cmd[2]) + 1);
        newsvr->port = malloc(strlen(cmd[3]) + 1);
        memset(newsvr->name, 0, strlen(cmd[1]) + 1);
        memset(newsvr->ip, 0, strlen(cmd[2]) + 1);
        memset(newsvr->port, 0, strlen(cmd[3]) + 1);
        strcpy(newsvr->name, cmd[1]);
        strcpy(newsvr->ip, cmd[2]);
        strcpy(newsvr->port, cmd[3]);

        SaveSvrData(newsvr);
        FreeSvrData(newsvr);
    }
    else
    {

    }
}

void cmd_deleteserver(char **cmd, sessionData* sesData)
{

}
