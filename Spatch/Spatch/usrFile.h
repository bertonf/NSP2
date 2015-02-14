#ifndef USRFILE_H
#define USRFILE_H

/* FILE EXAMPLE  !!SAUT DE LIGNE OBLIGATOIRE ENTRE CHAQUE SECTION

[USRINFO]
NAME=toto
PWD=titi

[SERVACCESS]
SERV01=USER
SERV02=ADMIN

[SPATCHACCESS]
ACCESS=USER

*/

#define SECTIONUSR "[USRINFO]"
#define SECTIONSVR "[SERVACCESS]"
#define SECTIONSPA "[SPATCHACCESS]"

typedef enum e_section
{
    NOONE,
    USR,
    SVR,
    SPA
}esection;

typedef enum e_access
{
    NONE,
    USER,
    ADMIN
}eaccess;

typedef struct s_servAccess
{
    char* name;
    eaccess acc;
    struct s_servAccess* next;
}servAccess;

typedef struct s_usrData
{
    char* name;
    char* pwd;
    servAccess *sAccess;
    eaccess spaAccess;
}usrData;

usrData* GetUsrData(char *usrName);
int SaveUsrData(usrData *uData);
void DebugTraceUsrData(usrData* usr);
void FreeUsrData(usrData* usr);

#endif // USRFILE_H