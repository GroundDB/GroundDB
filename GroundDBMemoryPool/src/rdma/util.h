#pragma once

int sock_connect(const char *serverName, int port);
int poll_completion(struct resources *res);
int post_send(struct resources *res, enum ibv_wr_opcode opcode);
int post_receive(struct resources *res);
void resources_init(struct resources *res);
int resources_create(
    struct resources *res,
    const char *serverName,
    int tcpPort,
    const char *ibDevName,
    int ibPort);
int connect_qp(struct resources *res, const char *serverName, const int gid_idx, const int ibPort);
int resources_destroy(struct resources *res);