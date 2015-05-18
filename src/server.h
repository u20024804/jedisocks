#ifndef SERVER_H_
#define SERVER_H_
#include "c_map.h"

#define BUF_SIZE 2048
#define MAX_PKT_SIZE 8192
#define HDRLEN (ID_LEN + RSV_LEN + DATALEN_LEN)
#define EXP_TO_RECV_LEN (ID_LEN + RSV_LEN + DATALEN_LEN)
#define ID_LEN 4
#define RSV_LEN 1
#define DATALEN_LEN 2
#define ATYP_LEN 1
#define ADDRLEN_LEN  1
#define PORT_LEN 2
#define FULLPKT 1
#define SMALLPKT 0
#define CTL_CLOSE 0x04
#define CTL_INIT 0x01
#define CTL_NORMAL 0
#define CTL_CLOSE_ACK 0x03

#define packet_payload_alloc(packet, flag) \
do { \
if (flag) \
    packet.payloadlen = packet.datalen - ATYP_LEN + ADDRLEN_LEN + packet.addrlen + PORT_LEN; \
else \
    packet.payloadlen = packet.datalen; \
packet.data = calloc(1, packet.payloadlen); \
} while (0)

#define addrinfo_hints_init(hints)  \
do {                                \
hints.ai_family = PF_INET;          \
hints.ai_socktype = SOCK_STREAM;    \
hints.ai_protocol = IPPROTO_TCP;    \
hints.ai_flags = 0;                 \
} while (0)

// reset packet buf
// reset packet structure
#define packetnbuf_reset(ctx)                   \
do {                                            \
    ctx->buf_len = 0;                           \
    ctx->stage = 0;                             \
    memset(ctx->packet_buf, 0, MAX_PKT_SIZE);   \
    memset(&ctx->packet, 0, sizeof(packet_t));  \
    ctx->reset = 1;                             \
} while (0)

#define UV_WRITE_CHECK(r, wr, handle, cb)                           \
do {                                                                \
    if (r) {                                                        \
        if (wr) {                                                   \
            free(wr);                                               \
            free((wr)->buf.base);                                   \
        }                                                           \
        if (!uv_is_closing((uv_handle_t*) handle)) {                \
            uv_read_stop((uv_stream_t *) handle);                   \
            uv_close((uv_handle_t*) handle, cb);                    \
        }                                                           \
    }                                                               \
} while (0)

#define HANDLECLOSE(handle, cb) do {                            \
    if (!uv_is_closing((uv_handle_t*) handle)) {                \
        uv_read_stop((uv_stream_t *) handle);                   \
        uv_close((uv_handle_t*) handle, cb);                    \
    }                                                           \
} while (0)

// debug
#define SHOWPKTDEBUG(remote_ctx) LOGD("session_id=%d, rsv=%d, datalen=%d, atyp=%d, addrlen=%d, host=%s, port=%d, data=\n%s",remote_ctx->packet->session_id, remote_ctx->packet->rsv, remote_ctx->packet->datalen, remote_ctx->packet->atyp, remote_ctx->packet->addrlen, remote_ctx->packet->host, ntohs(*(uint16_t*)remote_ctx->packet->port), remote_ctx->packet->data)

#define SHOWPKTDEBUGWODATA(remote_ctx) LOGD("session_id=%d, rsv=%d, datalen=%d, atyp=%d, addrlen=%d, host=%s, port=%d",remote_ctx->packet->session_id, remote_ctx->packet->rsv, remote_ctx->packet->datalen, remote_ctx->packet->atyp, remote_ctx->packet->addrlen, remote_ctx->packet->host, ntohs(*(uint16_t*)remote_ctx->packet->port))

#define TCP_HANDLE_BASIC    \
uv_tcp_t handle;

int compare_id (void* left, void* right) {
    if (*(uint32_t*)left == *(uint32_t*)right)
        return 0;
    return *(uint32_t*)left < *(uint32_t*)right? -1:1;
}

typedef struct packet {
    uint32_t session_id;
    uint8_t rsv;
    uint16_t datalen;
    uint16_t payloadlen;
    uint8_t atyp;
    uint8_t addrlen;
    char host[257];
    char port[2];
    char* data;
    int offset;
    struct packet *prev;
    struct packet *next;
} packet_t;

typedef struct send_queue{
    packet_t head;
} queue_t;

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

typedef struct listener {
    TCP_HANDLE_BASIC
} listener_t;

typedef struct {
	TCP_HANDLE_BASIC
    struct clib_map* idfd_map;  // for mapping session id with remote fd
    packet_t packet;
    queue_t send_queue;
    char packet_buf[MAX_PKT_SIZE];
    char recv_buffer[MAX_PKT_SIZE];
    int buf_len;
    int reset;
	int stage;
    int expect_to_recv;
} server_ctx_t;


typedef struct {
    TCP_HANDLE_BASIC
    int session_id;
    server_ctx_t* server_ctx;
    char host[257];
    char port[2];
    packet_t* packet;
    queue_t send_queue;
    int resolved;
    int connected;
    char addrlen;
    int stage;
    int closing;
    int ctl_cmd;
    uv_timer_t http_timeout;
} remote_ctx_t;

#endif