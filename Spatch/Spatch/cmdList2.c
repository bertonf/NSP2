#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include "cmdlist.h"

void cmd_lsserv(char** cmdtab, sessionData* sesData)
{
    struct dirent *file;
    DIR *rep;
    rep = opendir(".");
    while ((file = readdir(rep)))
    {
        /*
       if(strncmp(&(file->d_name[strlen(file->d_name)-strlen(".svr")]), ".svr", 4) == 0)
           ssh_channel_write(sesData->channel, strtok(file->d_name, "."),);
           */
    }
    closedir(rep);
}

void cmd_lsuser(char** cmdtab, sessionData* sesData)
{
    struct dirent *file;
    DIR *rep;
    rep = opendir(".");
    char *filename;
    char *extfile;
    usrData *uData;
    eaccess acc;
    char *msg;

    while ((file = readdir(rep)))
    {
        filename = strtok(file->d_name, ".");
        extfile = strtok(NULL, ".");
       if(strncmp(extfile, ".usr", 4) == 0)
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
       }
    }
    closedir(rep);
}

void cmd_lsaccess(char** cmdtab, sessionData* sesData)
{


}

void cmd_connectto(char** cmdtab, sessionData* sesData)
{


}
