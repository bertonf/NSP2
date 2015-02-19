#include <libssh/libssh.h>
#include <stdlib.h>
#include <string.h>
#include "Spatch.h"
#include "usrFile.h"
#include "cmdlist.h"
#include "svrFile.h"
#include "clientlistner.h"
#include "CmdAnalyser.h"
#include "sendclientcmd.h"

void cmd_createuser(char **cmd, sessionData* sesData)
{
    if (cmd[0] != NULL && cmd[1] != NULL && cmd[2] != NULL && cmd[3] != NULL)
    {
        usrData *newuser = newUsrData();

        newuser->name = malloc(strlen(cmd[1]) + 1);
        newuser->pwd = malloc(strlen(cmd[2]) + 1);
        memset(newuser->name, 0, strlen(cmd[1]) + 1);
        memset(newuser->pwd, 0, strlen(cmd[2]) + 1);
        strcpy(newuser->name, cmd[1]);
        strcpy(newuser->pwd, cmd[2]);
        if (strncmp(cmd[3], "user", 4) == 0)
            newuser->spaAccess = USER;
        else if (strncmp(cmd[3], "admin", 5) == 0)
            newuser->spaAccess = ADMIN;
        else
        {
            FreeUsrData(newuser);
            ssh_channel_write(sesData->channel,
                              "usage \"createuser user pwd user|admin\"\r\n",
                              strlen("usage \"createuser user pwd user|admin\"\r\n"));
            return;
        }
        SaveUsrData(newuser);
        FreeUsrData(newuser);
    }
    else
    {
        ssh_channel_write(sesData->channel,
                          "usage \"createuser user pwd user|admin\"\r\n",
                          strlen("usage \"createuser user pwd user|admin\"\r\n"));
    }

}

void cmd_deleteuser(char **cmd, sessionData* sesData)
{
    int status;
    char* filename;

    if (cmd[0] != NULL && cmd[1] != NULL)
    {

        filename = malloc(strlen(cmd[1]) + 5);
        memset(filename, 0, strlen(cmd[1]) + 5);
        strcpy(filename, cmd[1]);
        strcat(filename, ".usr");
        status = remove(filename);
        if (status == 0)
        {
            ssh_channel_write(sesData->channel,
                              "user succesfully deleted\r\n",
                              strlen("user succesfully deleted\r\n"));
        }
        else
        {
            ssh_channel_write(sesData->channel,
                              "delete user failure\r\n",
                              strlen("delete user failure\r\n"));
        }
    }
    else
    {
        ssh_channel_write(sesData->channel,
                          "usage \"deleteuser user\"\r\n",
                          strlen("usage \"deleteuser\"\r\n"));
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
                          "server succesfully added\r\n",
                          strlen("server succesfully added\r\n"));
    }
    else
    {
        ssh_channel_write(sesData->channel,
                          "server add failure\r\nusage createserver servername ip port\r\n",
                          strlen("server add failure\r\nusage createserver servername ip port\r\n"));
    }
}

void cmd_deleteserver(char **cmd, sessionData* sesData)
{
    int status;
    char* filename;

    if (cmd[0] != NULL && cmd[1] != NULL)
    {
        filename = malloc(strlen(cmd[1]) + 5);
        memset(filename, 0, strlen(cmd[1]) + 5);
        strcpy(filename, cmd[1]);
        strcat(filename, ".svr");
        status = remove(filename);
        if (status == 0)
        {
            ssh_channel_write(sesData->channel,
                              "server succesfully deleted\r\n",
                              strlen("server succesfully deleted\r\n"));
        }
        else
        {
            ssh_channel_write(sesData->channel,
                              "delete server failure\r\n",
                              strlen("delete server failure\r\n"));
        }
    }
    else
    {
        ssh_channel_write(sesData->channel,
                          "usage \"deleteserv name\"\r\n",
                          strlen("usage \"deleteserv name\"\r\n"));
    }
}


void cmd_createaccess(char **cmd, sessionData* sesData)
{
    if (cmd[0] != NULL && cmd[1] != NULL && cmd[2] != NULL && cmd[3] != NULL)
    {
        svrData *svr = GetSvrData(cmd[2]);
        usrData *usr = GetUsrData(cmd[1]);
        if (GetSvrData(cmd[2]) == NULL)
        {
            ssh_channel_write(sesData->channel,
                              "no server found with this name\r\n",
                              strlen("no server found with this name\r\n"));
            return;
        }
        if (isUsrAccess(sesData->uData, cmd[2]) != ADMIN)
        {
            ssh_channel_write(sesData->channel,
                              "must have admin right on this server\r\n",
                              strlen("must have admin right on this server\r\n"));
            return;
        }
        if (isUsrAccess(usr, cmd[2]) == NONE)
        {
            if (strncmp("user", cmd[3], 4) == 0)
            {
                ssh_session session = connect_ssh(svr->ip, svr->port, sesData->uData->name, sesData->uData->pwd, 0);
                ssh_channel svrChannel = open_client_channel(session);

                char *cmdbuffer;

                int cmdbuffsize = (strlen(usr->name) + strlen(usr->pwd) + strlen("`\" ") + strlen("useradd -m -p \"`mkpasswd ") + 1);
                cmdbuffer = malloc(cmdbuffsize);
                memset(cmdbuffer, 0, cmdbuffsize);
                strcat(cmdbuffer, "useradd -m -p \"`mkpasswd ");
                strcat(cmdbuffer, usr->pwd);
                strcat(cmdbuffer, "`\" ");
                strcat(cmdbuffer, usr->name);
                printf("%s\n", cmdbuffer);
                send_cmd_to_ssh(sesData->channel, svrChannel, cmdbuffer);
                AddAccess(usr, cmd[2], USER);
                SaveUsrData(usr);
                if (svrChannel != NULL)
                    close_client_channel(svrChannel);
                ssh_disconnect(session);
                ssh_free(session);
            }
            else if (strncmp("admin", cmd[3], 5) == 0)
            {
                ssh_session session = connect_ssh(svr->ip, svr->port, sesData->uData->name, sesData->uData->pwd, 0);
                ssh_channel svrChannel = open_client_channel(session);
                char *cmdbuffer;
                int cmdbuffsize = (strlen(usr->name) + strlen(usr->pwd) + strlen("`\" ") + strlen("useradd -m --gid sudo -p \"`mkpasswd ") + 1);
                cmdbuffer = malloc(cmdbuffsize);
                memset(cmdbuffer, 0, cmdbuffsize);
                strcat(cmdbuffer, "useradd -m --gid sudo -p \"`mkpasswd ");
                strcat(cmdbuffer, usr->pwd);
                strcat(cmdbuffer, "`\" ");
                strcat(cmdbuffer, usr->name);
                printf("%s\n", cmdbuffer);
                send_cmd_to_ssh(sesData->channel, svrChannel, cmdbuffer);

                AddAccess(usr, cmd[2], ADMIN);
                SaveUsrData(usr);
                if (svrChannel != NULL)
                    close_client_channel(svrChannel);
                ssh_disconnect(session);
                ssh_free(session);
            }
            else
                ssh_channel_write(sesData->channel,
                                  "usage \"createaccess username servername user|admin\"\r\n",
                                  strlen("usage \"createaccess username servername user|admin\"\r\n"));
        }
        else
        {
            ssh_channel_write(sesData->channel,
                              "user already have right on this server\r\n",
                              strlen("user already have right on this server\r\n"));
        }
        FreeUsrData(usr);
        FreeSvrData(svr);
    }
    else
    {
        ssh_channel_write(sesData->channel,
                          "usage \"createaccess username servername user|admin\"\r\n",
                          strlen("usage \"createaccess username servername user|admin\"\r\n"));
    }
}

void cmd_deleteaccess(char **cmd, sessionData *sesData)
{
    if (cmd[0] != NULL && cmd[1] != NULL && cmd[2] != NULL)
    {
        usrData *usr = GetUsrData(cmd[1]);
        if (isUsrAccess(usr, cmd[2]) == NONE)
        {
            ssh_channel_write(sesData->channel,
                              "user doesnt have access right on this server\r\n",
                              strlen("user doesnt have access right on this server\r\n"));
            return;
        }
        DeleteAccess(usr, cmd[2]);
        SaveUsrData(usr);
        FreeUsrData(usr);
    }
    else
    {
        ssh_channel_write(sesData->channel,
                          "usage \"deleteaccess username servername\"\r\n",
                          strlen("usage \"deleteaccess username servername\"\r\n"));
    }

}
