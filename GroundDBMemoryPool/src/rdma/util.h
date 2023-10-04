#pragma once

int sock_connect(const char *serverName, int port);
int poll_completion(const struct resources *res);
int post_send(const struct resources *res, const enum ibv_wr_opcode opcode);
int post_receive(const struct resources *res);
int resources_create(
    struct resources *res,
    const char *serverName,
    const int tcpPort,
    const char *ibDevName,
    const int ibPort);
int connect_qp(struct resources *res, const char *serverName, const int gid_idx, const int ibPort);
int resources_destroy(struct resources *res);