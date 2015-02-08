#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "usrFile.h"

static usrData* newUsrData()
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

    sa->name = (char*)malloc(strlen(var));
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
                strncpy(ud->name, val, strlen(val) - 1);
            }
            else if(strncmp("PWD", var, 3) == 0)
            {
                ud->pwd = (char*)malloc(strlen(val));
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
            return;
        }
    }
}

usrData* GetUsrData(char *usrName)
{
    FILE* fp;
    char* fileName = NULL;
    char* line = NULL;

    size_t len = 0;
    ssize_t read;

    esection s;
    usrData *ud;

    fileName = (char*)malloc(strlen(usrName) + strlen(".usr") + 1);
    if (fileName == NULL)
        return (NULL);

    strcpy(fileName, usrName);
    strcat(fileName, ".usr");

    fp = fopen(fileName, "r");
    if (fp == NULL)
        return (NULL);

    ud = newUsrData();
    if (ud == NULL)
        return (NULL);
    s = NOONE;
    while ((read = getline(&line, &len, fp)) != -1) {
        /*printf("Retrieved line of length %zu :\n", read);
        //    sscanf("%s=%s", var, val);
        printf("line : %s\n", line);*/

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

    return (ud);
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
