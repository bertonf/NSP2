#include "Spatch.h"
#include "SessionLoop.h"

static sthreadList* newNodeList(sthreadList* list, ssh_session session)
{
    sthreadList *n;
    n = (sthreadList*)malloc(sizeof(sthreadList));
    n->sesData.session = session;
    n->sesData.channel = NULL;
    n->sesData.clientsession = NULL;
    n->sesData.clientchannel = NULL;
    n->sesData.uData = NULL;
    n->next = list;
    return (n);
}

static void cleanList(sthreadList* list)
{
    sthreadList* tmp = NULL;
    do {
        ssh_disconnect(list->sesData.session);
        ssh_free(list->sesData.session);
        tmp = list;
        list = tmp->next;
        free(tmp);
        tmp = NULL;
    } while (list != NULL);

}

int spatch()
{
    ssh_session session = NULL;
    ssh_bind sshbind = NULL;
    sthreadList* list = NULL;

    int port = SERVER_PORT;

    sshbind=ssh_bind_new();
    //session=ssh_new();

    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT, &port);
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_DSAKEY,
                                            KEYS_FOLDER "ssh_host_dsa_key");
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY,
                                            KEYS_FOLDER "ssh_host_rsa_key");



    if (ssh_bind_listen(sshbind)<0)
    {
        printf("Error listening to socket: %s\n", ssh_get_error(sshbind));
        return 1;
    }
    printf("Server start on port %d\n", port);

    while(1)
    {
        session=ssh_new();
        if (ssh_bind_accept(sshbind, session) == SSH_ERROR)
        {
            printf("Error accepting a connection: %s\n", ssh_get_error(sshbind));
            ssh_free(session);
        }
        else
        {
            list = newNodeList(list, session);
            if (pthread_create(&(list->thread), NULL, (void*)NewSessionLoop, (void*)&(list->sesData)) != 0)
            {
                sthreadList* tmp;

                ssh_disconnect(session);
                ssh_free(session);
                tmp = list;
                list = list->next;
                free(tmp);
            }
        }
    }
    if (session)
        ssh_free(session);
    cleanList(list);
    ssh_bind_free(sshbind);
    ssh_finalize();
    return 0;
}
