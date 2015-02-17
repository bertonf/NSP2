#include <libssh/libssh.h>
//#include "examples_common.h"
#include <stdio.h>
#include "authentication.h"

ssh_session connect_ssh(const char *host,  char* port, const char *user, char* password, int verbosity)
{
    ssh_session session;
    int auth=0;
    session=ssh_new();
    if (session == NULL)
    {
        return NULL;
    }
    if(user != NULL){
        if (ssh_options_set(session, SSH_OPTIONS_USER, user) < 0)
        {
            ssh_disconnect(session);
            return NULL;
        }
    }
    if (ssh_options_set(session, SSH_OPTIONS_HOST, host) < 0)
    {
        return NULL;
    }
    if (ssh_options_set(session, SSH_OPTIONS_PORT_STR, port) < 0)
    {
        return NULL;
    }
    ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    if(ssh_connect(session))
    {
        fprintf(stderr,"Connection failed : %s\n",ssh_get_error(session));
        ssh_disconnect(session);
        return NULL;
    }
    if(verify_knownhost(session)<0)
    {
        ssh_disconnect(session);
        return NULL;
    }
    auth=authenticate_console(session, password);
    if(auth==SSH_AUTH_SUCCESS)
    {
        printf("Connection reussi\n");
        //show_remote_processes(session);
        return session;
    }
    else if(auth==SSH_AUTH_DENIED)
    {
        fprintf(stderr,"Authentication failed\n");
    }
    else
    {
        fprintf(stderr,"Error while authenticating : %s\n",ssh_get_error(session));
    }

    ssh_disconnect(session);
    return NULL;
}
