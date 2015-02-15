#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "svrFile.h"

static svrData* newSvrData()
{
    svrData *sd;

    sd = (svrData*)malloc(sizeof(svrData));
    if (sd == NULL)
        return (NULL);
    sd->name = NULL;
    sd->port = NULL;
    sd->ip = NULL;
    return (sd);
}


static void LineParser(svrData* sd, char* line)
{
    char *var;
    char *val;

    var = strtok(line, "=");
    val = strtok(NULL, "=");

    if (var != NULL && val != NULL)
    {
        //printf("var: %s / val: %s\n", var, val);

        if (strncmp("NAME", var, 4) == 0)
        {
            sd->name = (char*)malloc(strlen(val));
            memset(sd->name, 0, strlen(val));
            strncpy(sd->name, val, strlen(val) - 1);
        }
        else if(strncmp("IP", var, 2) == 0)
        {
            sd->ip = (char*)malloc(strlen(val));
            memset(sd->ip, 0, strlen(val));
            strncpy(sd->ip, val, strlen(val) - 1);
        }
        else if(strncmp("PORT", var, 4) == 0)
        {
            sd->port = (char*)malloc(strlen(val));
            memset(sd->port, 0, strlen(val));
            strncpy(sd->port, val, strlen(val) - 1);
        }
    }
}



svrData* GetSvrData(char *svrName)
{
    FILE* fd;
    char* fileName = NULL;
    char* line = NULL;

    size_t len = 0;
    ssize_t read;

    int servsection;
    svrData *sd;

    fileName = (char*)malloc(strlen(svrName) + strlen(".svr") + 1);
    if (fileName == NULL)
        return (NULL);
    memset(fileName, 0, strlen(svrName) + strlen(".svr") + 1);
    strcpy(fileName, svrName);
    strcat(fileName, ".svr");

    fd = fopen(fileName, "r");
    if (fd == NULL)
        return (NULL);

    sd = newSvrData();
    if (sd == NULL)
        return (NULL);
    servsection = 0;
    while ((read = getline(&line, &len, fd)) != -1) {
        if(strncmp(line, SECTIONSERV, 9) == 0)
            servsection = 1;
        else if (read > 2 && servsection == 1)
            LineParser(sd, line);
    }
    if (line)
        free(line);
    if (fileName)
        free(fileName);
    fclose(fd);
    return (sd);

}

int SaveSvrData(svrData *sData)
{
    FILE* fd;
    char* fileName = NULL;

    fileName = (char*)malloc(strlen(sData->name) + strlen(".svr") + 1);
    if (fileName == NULL)
        return (-1);

    memset(fileName, 0, strlen(sData->name) + strlen(".svr") + 1);
    strcpy(fileName, sData->name);
    strcat(fileName, ".svr");

    fd = fopen(fileName, "w");
    if (fd == NULL)
        return (-1);

    fprintf(fd, "[SERVINFO]\n");
    fprintf(fd, "NAME=%s\n", sData->name);
    fprintf(fd, "IP=%s\n", sData->ip);
    fprintf(fd, "PORT=%s\n", sData->port);

    if (fileName)
        free(fileName);
    fclose(fd);
    return (0);

}

void DebugTraceSvrData(svrData* svr)
{
    printf("NAME = [%s]\n", svr->name);
    printf("IP = [%s]\n", svr->ip);
    printf("PORT = [%s]\n", svr->port);
}


void FreeSvrData(svrData* svr)
{
    free(svr->ip);
    free(svr->name);
    free(svr->port);
    free(svr);
}
