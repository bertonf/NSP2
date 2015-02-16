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
                              "usage \"createuser user pwd user|admin\"\n",
                              strlen("usage \"createuser user pwd user|admin\"\n"));
            return;
        }
        SaveUsrData(newuser);
        FreeUsrData(newuser);
    }
    else
    {
        ssh_channel_write(sesData->channel,
                          "usage \"createuser user pwd user|admin\"\n",
                          strlen("usage \"createuser user pwd user|admin\"\n"));
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

        ssh_channel_write(sesData->channel,
                          "server succesfully added\n",
                          strlen("server succesfully added\n"));
    }
    else
    {
        ssh_channel_write(sesData->channel,
                          "server add failure\n",
                          strlen("server add failure\n"));
    }
}

void cmd_deleteserver(char **cmd, sessionData* sesData)
{
    int status;

    if (cmd[0] != NULL && cmd[1] != NULL)
    {
        status = remove(cmd[1]);
        if (status == 0)
        {
            ssh_channel_write(sesData->channel,
                              "server succesfully deleted\n",
                              strlen("server succesfully deleted\n"));
        }
        else
        {
            ssh_channel_write(sesData->channel,
                              "delete server failure\n",
                              strlen("delete server failure\n"));
        }
    }
}


void cmd_createaccess(char **cmd, sessionData* sesData)
{
    //cmd user server right
    // eaccess isuseraccess(*usrData servername)
    //addaccess(*usrData servername eaccess)
    if (cmd[0] != NULL && cmd[1] != NULL && cmd[2] != NULL && cmd[3] != NULL)
    {
        GetSvrData(cmd[2]);
        usrData *usr = GetUsrData(cmd[1]);
        if (GetSvrData(cmd[2]) == NULL)
        {
            ssh_channel_write(sesData->channel,
                              "no server found with this name\n",
                              strlen("no server found with this name\n"));
            return;
        }
        if (isUsrAccess(usr, cmd[2]) == NONE)
        {
            if (strncmp("user", cmd[3], 4))
            {
                //TODO add user on serv ssh
                AddAccess(usr, cmd[2], USER);
            }
            else if (strncmp("admin", cmd[3], 5))
            {
                //TODO add user on serv ssh
                AddAccess(usr, cmd[2], ADMIN);
            }
            else
                ssh_channel_write(sesData->channel,
                                  "usage \"createaccess username servername user|admin\"\n",
                                  strlen("usage \"createaccess username servername user|admin\"\n"));
        }
        else
        {
            ssh_channel_write(sesData->channel,
                              "user already have right on this server\n",
                              strlen("user already have right on this server\n"));
        }
    }
    else
    {
        ssh_channel_write(sesData->channel,
                          "usage \"createaccess username servername user|admin\"\n",
                          strlen("usage \"createaccess username servername user|admin\"\n"));
    }
}

void cmd_deleteaccess(char **cmd, sessionData *sesData)
{

}

