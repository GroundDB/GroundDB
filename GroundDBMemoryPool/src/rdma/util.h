#pragma once

namespace mempool{

int sock_connect(const char *serverName, int port);
int poll_completion(const struct connection* conn);
int poll_completion(const struct resources *res, const struct connection* conn);
int post_send(const struct resources *res, const struct memory_region *memregs, const struct connection *conn, const enum ibv_wr_opcode opcode, size_t lofs = 0, size_t size = -1, size_t rofs = 0);
int post_receive(const struct memory_region *memregs, const struct connection *conn, size_t lofs = 0, size_t size = -1);
int post_receive(const struct resources *res, const struct memory_region *memregs, const struct connection *conn, size_t lofs = 0, size_t size = -1);
int resources_create(
    struct resources *res,
    const char *serverName,
    const char *ibDevName,
    const int ibPort);
int register_mr(struct memory_region *&memreg, struct resources *res, const char* buf = nullptr, size_t size = BUFFER_SIZE);
int connect_qp(struct connection *&conn, struct resources *res, struct memory_region *memreg, const char *serverName, const int tcpPort, const int gid_idx, const int ibPort);
int resources_destroy(struct resources *res);

} // namespace mempool