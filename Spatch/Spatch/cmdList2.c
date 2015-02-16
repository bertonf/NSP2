#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include "cmdlist.h"

void cmd_lsserv(char** cmdtab, sessionData* sesData)
{
    struct dirent *file;
    DIR *rep;
    char *filename;
    char *extfile;
    eaccess acc;
    char *msg;

    if(cmdtab[0] == NULL)
    {
        ssh_channel_write(sesData->channel,
                          "usage \"lsserv\"\n",
                          strlen("usage \"lsserv\"\n"));
        return;
    }
    rep = opendir(".");
    while ((file = readdir(rep)))
    {
        filename = strtok(file->d_name, ".");
        extfile = strtok(NULL, ".");
        if (filename != NULL && extfile != NULL)
        {
            if(strncmp(extfile, "svr", 3) == 0)
            {
                acc = isUsrAccess(sesData->uData, filename);
                switch(acc)
                {
                case NONE:
                    msg = malloc(strlen(filename)+strlen("\t|\tNONE\n") + 1);
                    memset(msg, 0, strlen(filename)+strlen("\t|\tNONE\n") + 1);
                    strcpy(msg, filename);
                    strcat(msg, "\t|\tNONE\n");
                    break;
                case USER:
                    msg = malloc(strlen(filename)+strlen("\t|\tUSER\n") + 1);
                    memset(msg, 0, strlen(filename)+strlen("\t|\tUSER\n") + 1);
                    strcpy(msg, filename);
                    strcat(msg, "\t|\tUSER\n");
                    break;
                case ADMIN:
                    msg = malloc(strlen(filename)+strlen("\t|\tADMIN\n") + 1);
                    memset(msg, 0, strlen(filename)+strlen("\t|\tADMIN\n") + 1);
                    strcpy(msg, filename);
                    strcat(msg, "\t|\tADMIN\n");
                    break;
                }
                ssh_channel_write(sesData->channel, msg, strlen(msg));
                free(msg);
            }
        }
    }
    closedir(rep);
}

void cmd_lsuser(char** cmdtab, sessionData* sesData)
{
    struct dirent *file;
    DIR *rep;
    char *filename;
    char *extfile;
    usrData *uData;
    eaccess acc;
    char *msg;

    if(cmdtab[0] == NULL || cmdtab[1] == NULL)
    {
        ssh_channel_write(sesData->channel,
                          "usage \"lsuser server\"\n",
                          strlen("usage \"lsuser server\"\n"));
        return;
    }
    rep = opendir(".");
    while ((file = readdir(rep)))
    {
        filename = strtok(file->d_name, ".");
        extfile = strtok(NULL, ".");
       if(strncmp(extfile, "usr", 3) == 0)
       {
            uData = GetUsrData(filename);
            acc = isUsrAccess(uData, cmdtab[1]);

            switch(acc)
            {
            case NONE:
                msg = malloc(strlen(uData->name)+strlen("\t|\tNONE\n") + 1);
                memset(msg, 0, strlen(uData->name)+strlen("\t|\tNONE\n") + 1);
                strcpy(msg, uData->name);
                strcat(msg, "\t|\tNONE\n");
                break;
            case USER:
                msg = malloc(strlen(uData->name)+strlen("\t|\tUSER\n") + 1);
                memset(msg, 0, strlen(uData->name)+strlen("\t|\tUSER\n") + 1);
                strcpy(msg, uData->name);
                strcat(msg, "\t|\tUSER\n");
                break;
            case ADMIN:
                msg = malloc(strlen(uData->name)+strlen("\t|\tADMIN\n") + 1);
                memset(msg, 0, strlen(uData->name)+strlen("\t|\tADMIN\n") + 1);
                strcpy(msg, uData->name);
                strcat(msg, "\t|\tADMIN\n");
                break;
            }
            ssh_channel_write(sesData->channel, msg, strlen(msg));
            free(msg);
            FreeUsrData(uData);
        }
    }
    closedir(rep);
}

void cmd_lsaccess(char** cmdtab, sessionData* sesData)
{
    usrData* uData;
    servAccess *sAccess;
    char *msg;

    if(cmdtab[0] == NULL || cmdtab[1] == NULL)
    {
        ssh_channel_write(sesData->channel,
                          "usage \"lsaccess user\"\n",
                          strlen("usage \"lsaccess user\"\n"));
        return;
    }
    uData = GetUsrData(cmdtab[1]);
    if (uData != NULL)
    {
        sAccess = uData->sAccess;
        while (sAccess != NULL)
        {
            switch(sAccess->acc)
            {
            case NONE:
                msg = malloc(strlen(sAccess->name)+strlen("\t|\tNONE\n") + 1);
                memset(msg, 0, strlen(sAccess->name)+strlen("\t|\tNONE\n") + 1);
                strcpy(msg, sAccess->name);
                strcat(msg, "\t|\tNONE\n");
                break;
            case USER:
                msg = malloc(strlen(sAccess->name)+strlen("\t|\tUSER\n") + 1);
                memset(msg, 0, strlen(sAccess->name)+strlen("\t|\tUSER\n") + 1);
                strcpy(msg, sAccess->name);
                strcat(msg, "\t|\tUSER\n");
                break;
            case ADMIN:
                msg = malloc(strlen(sAccess->name)+strlen("\t|\tADMIN\n") + 1);
                memset(msg, 0, strlen(sAccess->name)+strlen("\t|\tADMIN\n") + 1);
                strcpy(msg, sAccess->name);
                strcat(msg, "\t|\tADMIN\n");
                break;
            }
            ssh_channel_write(sesData->channel, msg, strlen(msg));
            free(msg);
            sAccess = sAccess->next;
        }
        FreeUsrData(uData);
    }
}


void cmd_connectto(char** cmdtab, sessionData* sesData)
{
    (void)cmdtab;
    (void)sesData;
}
