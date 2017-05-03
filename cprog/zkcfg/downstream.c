#include <zookeeper/zookeeper.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "definitions.h"

#define _LL_CAST_ (long long)

static zhandle_t *zh;
static clientid_t myid;
static const char *clientIdFile = 0;

static
void my_data_completion(int rc, const char *value, int value_len,
        const struct Stat *stat, const void *data);

static const char* state2String(int state)
{
    if (state == 0)
        return "CLOSED_STATE";
    if (state == ZOO_CONNECTING_STATE)
        return "CONNECTING_STATE";
    if (state == ZOO_ASSOCIATING_STATE)
        return "ASSOCIATING_STATE";
    if (state == ZOO_CONNECTED_STATE)
        return "CONNECTED_STATE";
    if (state == ZOO_EXPIRED_SESSION_STATE)
        return "EXPIRED_SESSION_STATE";
    if (state == ZOO_AUTH_FAILED_STATE)
        return "AUTH_FAILED_STATE";

    return "INVALID_STATE";
}

static const char* type2String(int state)
{
    if (state == ZOO_CREATED_EVENT)
        return "CREATED_EVENT";
    if (state == ZOO_DELETED_EVENT)
        return "DELETED_EVENT";
    if (state == ZOO_CHANGED_EVENT)
        return "CHANGED_EVENT";
    if (state == ZOO_CHILD_EVENT)
        return "CHILD_EVENT";
    if (state == ZOO_SESSION_EVENT)
        return "SESSION_EVENT";
    if (state == ZOO_NOTWATCHING_EVENT)
        return "NOTWATCHING_EVENT";

    return "UNKNOWN_EVENT_TYPE";
}

void watcher(zhandle_t *zzh, int type, int state, const char *path,
             void* context)
{
    /* Be careful using zh here rather than zzh - as this may be mt code
     * the client lib may call the watcher before zookeeper_init returns */

    fprintf(stderr, "Watcher %s state = %s", type2String(type), state2String(state));
    if (path && strlen(path) > 0) {
        fprintf(stderr, " for path %s", path);
    }

    fprintf(stderr, "\n");
    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_CONNECTED_STATE) {
            const clientid_t *id = zoo_client_id(zzh);
            if (myid.client_id == 0 || myid.client_id != id->client_id) {
                myid = *id;
                fprintf(stderr, "Got a new session id: 0x%llx\n",
                        _LL_CAST_ myid.client_id);
            }
        } else if (state == ZOO_AUTH_FAILED_STATE) {
            fprintf(stderr, "Authentication failure. Shutting down...\n");
            zookeeper_close(zzh);
            zh=0;
        } else if (state == ZOO_EXPIRED_SESSION_STATE) {
            fprintf(stderr, "Session expired. Shutting down...\n");
            zookeeper_close(zzh);
            zh=0;
        }
    } else if (type == ZOO_CREATED_EVENT) {
        zoo_aget(zh, path, 0, my_data_completion, strdup(path));
    } else if (type == ZOO_CHANGED_EVENT) {
        zoo_aget(zh, path, 0, my_data_completion, strdup(path));
    }
}

static
void my_data_completion(int rc, const char *value, int value_len,
        const struct Stat *stat, const void *data)
{
    const char* path = (const char*) data;
    if (rc != ZOK) {
        fprintf(stderr, "Failed to get data of %s: %d\n", path, rc);
    } else {
        fprintf(stdout, "path=%s, version=%d, data=%s\n", path, stat->version, value);
    }

    free((void*) data);
}

static
void my_stat_completion(int rc, const struct Stat *stat,
        const void *data)
{
    const char* path = (const char*) data;
    if (rc != ZOK) {
        fprintf(stderr, "Failed to get data of %s: %d\n", path, rc);
    } else {
        fprintf(stdout, "path=%s, version=%d\n", path, stat->version);
    }

    free((void*) data);
}

static void
get_all_config(const char* location)
{
    const char* marketinfo = "/mds/zp/marketinfo.xml";
    if (zoo_aexists(zh, marketinfo, 1, my_stat_completion, strdup(marketinfo))) {
        fprintf(stderr, "Failed to check exist: %s\n", UNIVERSAL_ID_PATH);
    }

    if (zoo_aget(zh, UNIVERSAL_ID_PATH, 1, my_data_completion, strdup(UNIVERSAL_ID_PATH))) {
        fprintf(stderr, "Failed to aget %s\n", UNIVERSAL_ID_PATH);
    }

    char* path = get_config_path(location, ID_CONFIG);
    if (zoo_aget(zh, path, 1, my_data_completion, path)) {
        fprintf(stderr, "Failed to aget %s\n", UNIVERSAL_ID_PATH);
    }
}

int main(int argc, char* argv[])
{
    char address[128];
    int flags = 0;
    fprintf(stdout, "%ld\n", _POSIX_C_SOURCE);
    zoo_deterministic_conn_order(1); // enable deterministic order
    if (argc > 2) {
        snprintf(address, sizeof(address), "%s:%s", argv[1], argv[2]);
    } else {
        sprintf(address, "127.0.0.1:2181");
    }

    zh = zookeeper_init(address, watcher, 30000, &myid, NULL, flags);
    if (zh) {
        sigset_t signals;
        get_all_config("sh");
        sigfillset(&signals);
        sigwait(&signals, NULL);
        zookeeper_close(zh);
    }

    exit(EXIT_SUCCESS);
}

