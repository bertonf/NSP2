#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "usrFile.h"

usrData* newUsrData()
{
    usrData *ud;

    ud = (usrData*)malloc(sizeof(usrData));
    if (ud == NULL)
        return (NULL);
    ud->name = NULL;
    ud->pwd = NULL;
    ud->sAccess = NULL;
    ud->spaAccess = NONE;
    return (ud);
}

static servAccess* newSvrAccess()
{
    servAccess* sa;

    sa = (servAccess*)malloc(sizeof(servAccess));
    if (sa == NULL)
        return (NULL);
    sa->name = NULL;
    sa->acc = NONE;
    return (sa);
}

static servAccess* addServAccess(servAccess* sAccess, char *var, char *val)
{
    servAccess* sa = newSvrAccess();

    sa->name = (char*)malloc(strlen(var) + 1);
    memset(sa->name, 0, strlen(var) + 1);
    strcpy(sa->name, var);
    if (strncmp("USER", val, 4) == 0)
        sa->acc = USER;
    else if (strncmp("ADMIN", val, 5) == 0)
        sa->acc = ADMIN;
    else
        sa->acc = NONE;
    sa->next = sAccess;
    return (sa);
}

static void LineParser(usrData* ud, char* line, esection s)
{
    char *var;
    char *val;

    var = strtok(line, "=");
    val = strtok(NULL, "=");

    if (var != NULL && val != NULL)
    {
        //printf("var: %s / val: %s\n", var, val);

        switch (s)
        {
        case USR:
            if (strncmp("NAME", var, 4) == 0)
            {
                ud->name = (char*)malloc(strlen(val));
                memset(ud->name, 0, strlen(val));
                strncpy(ud->name, val, strlen(val) - 1);
            }
            else if(strncmp("PWD", var, 3) == 0)
            {
                ud->pwd = (char*)malloc(strlen(val));
                memset(ud->pwd, 0, strlen(val));
                strncpy(ud->pwd, val, strlen(val) - 1);
            }
            break;
        case SVR:
            ud->sAccess = addServAccess(ud->sAccess, var, val);
            break;
        case SPA:
            if (strncmp("ACCESS", var, 6) == 0)
            {
                if (strncmp("USER", val, 4) == 0)
                    ud->spaAccess = USER;
                else if (strncmp("ADMIN", val, 5) == 0)
                    ud->spaAccess = ADMIN;
                else
                    ud->spaAccess = NONE;
            }
            break;
        default:
            break;
        }
    }
}

usrData* GetUsrData(char *usrName)
{
    FILE* fd;
    char* fileName = NULL;
    char* line = NULL;

    size_t len = 0;
    ssize_t read;

    esection s;
    usrData *ud;

    fileName = (char*)malloc(strlen(usrName) + strlen(".usr") + 1);
    if (fileName == NULL)
        return (NULL);
    memset(fileName, 0, strlen(usrName) + strlen(".usr") + 1);
    strcpy(fileName, usrName);
    strcat(fileName, ".usr");

    fd = fopen(fileName, "r");
    if (fd == NULL)
        return (NULL);

    ud = newUsrData();
    if (ud == NULL)
        return (NULL);
    s = NOONE;
    while ((read = getline(&line, &len, fd)) != -1) {
        if(strncmp(line, SECTIONUSR, 9) == 0)
            s = USR;
        else if (strncmp(line, SECTIONSVR, 12) == 0)
            s = SVR;
        else if (strncmp(line, SECTIONSPA, 14) == 0)
            s = SPA;
        else if (read > 2 && s != NOONE)
            LineParser(ud, line, s);
    }
    if (line)
        free(line);
    if (fileName)
        free(fileName);
    fclose(fd);
    return (ud);
}

int SaveUsrData(usrData *uData)
{
    FILE* fd;
    servAccess* tmp;
    char* fileName = NULL;

    fileName = (char*)malloc(strlen(uData->name) + strlen(".usr") + 1);
    if (fileName == NULL)
        return (-1);

    memset(fileName, 0, strlen(uData->name) + strlen(".usr") + 1);
    strcpy(fileName, uData->name);
    strcat(fileName, ".usr");

    fd = fopen(fileName, "w");
    if (fd == NULL)
        return (-1);

    fprintf(fd, "[USRINFO]\n");
    fprintf(fd, "NAME=%s\n", uData->name);
    fprintf(fd, "PWD=%s\n", uData->pwd);

    fprintf(fd, "\n[SERVACCESS]\n");

    tmp = uData->sAccess;
    while (tmp != NULL)
    {
        switch (tmp->acc)
        {
        case USER:
            fprintf(fd, "%s=USER\n", tmp->name);
            break;
        case ADMIN:
            fprintf(fd, "%s=ADMIN\n", tmp->name);
            break;
        default:
            break;
        }
        tmp = tmp->next;
    }

    fprintf(fd, "\n[SPATCHACCESS]\n");

    switch (uData->spaAccess)
    {
    case ADMIN:
        fprintf(fd, "ACCESS=ADMIN\n");
        break;
    default:
        fprintf(fd, "ACCESS=USER\n");
        break;
    }
    if (fileName)
        free(fileName);
    fclose(fd);
    return (0);
}

void DebugTraceUsrData(usrData* usr)
{
    servAccess *sa;

    printf("NAME = [%s]\n", usr->name);
    printf("PWD = [%s]\n", usr->pwd);
    printf("ACCESS | 0 = NONE, 1 = USER, 2 = ADMIN\n");
    printf("SPATCH = %d\n", usr->spaAccess);

    sa = usr->sAccess;
    while (sa != NULL)
    {
        printf("[%s] = %d\n", sa->name, sa->acc);
        sa = sa->next;
    }
}

void FreeUsrData(usrData* usr)
{
    servAccess *sa;

    while (usr->sAccess != NULL)
    {
        sa = usr->sAccess;
        usr->sAccess = usr->sAccess->next;
        free(sa->name);
        free(sa);
    }
    free(usr->name);
    free(usr->pwd);
    free(usr);
}

eaccess isUsrAccess(usrData* uData, char* servName)
{
    servAccess *tmp;

    tmp = uData->sAccess;
    while (tmp != NULL)
    {
        if (strcmp(tmp->name, servName) == 0)
            return (tmp->acc);
        tmp = tmp->next;
    }
    return (NONE);
}

void AddAccess(usrData* uData, char* servName, eaccess access)
{
    switch(access)
    {
    case ADMIN:
        uData->sAccess = addServAccess(uData->sAccess, servName, "ADMIN");
        break;
    default:
        uData->sAccess = addServAccess(uData->sAccess, servName, "USER");
        break;
    }
}

void DeleteAccess(usrData* uData, char* servName)
{
    servAccess* prec = NULL;
    servAccess* tmp = uData->sAccess;

    while (tmp != NULL)
    {
        if (strcmp(tmp->name, servName))
        {
            if (prec != NULL)
                prec->next = tmp->next;
            else
                uData->sAccess = tmp->next;
            free(tmp);
            break;
        }
        prec = tmp;
        tmp = tmp->next;
    }
}
