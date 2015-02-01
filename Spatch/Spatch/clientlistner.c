#include <libssh/libssh.h>
//#include "examples_common.h"
#include <stdio.h>

ssh_session connect_ssh(const char *host, const char *user, int port, int verbosity)
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
    if (ssh_options_set(session, SSH_OPTIONS_PORT, &port) < 0)
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
    auth=authenticate_console(session);
    if(auth==SSH_AUTH_SUCCESS)
    {
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
