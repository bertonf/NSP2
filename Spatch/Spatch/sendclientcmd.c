#include <libssh/libssh.h>

ssh_channel open_client_channel(ssh_session session)
{
    ssh_channel channel;
    int rc;

    channel = ssh_channel_new(session);
    if (channel == NULL)
        return NULL;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return NULL;
    }
    return (channel);
}

int close_client_channel(ssh_channel channel)
{
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_OK;
}

int send_cmd_to_ssh(ssh_channel chanusr, ssh_channel chansvr, char *buff)
{
    int rc;
    unsigned int nbytes;
    char buffer[256];

    rc = ssh_channel_request_exec(chansvr, buff);
    if (rc != SSH_OK)
    {
        ssh_channel_close(chansvr);
        ssh_channel_free(chansvr);
        return rc;
    }
    nbytes = ssh_channel_read(chansvr, buffer, sizeof(buffer), 0);
    while (nbytes > 0)
    {
        ssh_channel_write(chanusr, buffer, nbytes);
        /*if (write(fd, buffer, nbytes) != nbytes)
        {
            ssh_channel_close(chansvr);
            ssh_channel_free(chansvr);
            return SSH_ERROR;
        }*/
        nbytes = ssh_channel_read(chansvr, buffer, sizeof(buffer), 0);
    }
    return rc;
}

/*
int show_remote_processes(ssh_session session)
{
    ssh_channel channel;
    int rc;
    char buffer[256];
    unsigned int nbytes;

    channel = open_client_channel(session);
    if (channel == NULL)
        return SSH_ERROR;

    while (1)
    {
        char buff[2048];
        printf("->");
        scanf("%s", buff);
        rc = ssh_channel_request_exec(channel, buff);
        if (rc != SSH_OK)
        {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            return rc;
        }
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
        while (nbytes > 0)
        {
            if (write(1, buffer, nbytes) != nbytes)
            {
                ssh_channel_close(channel);
                ssh_channel_free(channel);
                return SSH_ERROR;
            }
            nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
        }
        if (nbytes < 0)
        {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            return SSH_ERROR;
        }
    }
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_OK;
}
*/
