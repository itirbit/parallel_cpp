#include <uv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uv_tcp_t server;
uv_loop_t *loop;

typedef struct
{
	uv_write_t req;
	uv_buf_t buf;
} write_req_t;

void read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{	
	uv_write_t *req = (uv_write_t*) malloc(sizeof(uv_write_t));
	if (nread > 0)
	{
		write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
		req->buf = uv_buf_init(buf->base, nread);
		uv_write((uv_write_t*)req, stream, &req->buf, 1, NULL);
		return;
	}
	if (nread < 0)
	{
		if (nread != UV_EOF) 
		{
			fprintf(stderr, "Read error %s\n", uv_err_name(nread));
		}
		uv_close((uv_handle_t*)stream, NULL);
	}
	free(buf->base);
}

void alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t* buf)
{
	buf->base = malloc(size);
	buf->len = size;
}

void connection_cb(uv_stream_t *server, int status)
{
	uv_tcp_t *client = malloc(sizeof(uv_tcp_t));

	uv_tcp_init(loop, client);
	uv_accept(server, (uv_stream_t*) client);
	uv_read_start((uv_stream_t*)client, alloc_buffer, read_cb);
}

int main(int argc, char** argv)
{
	loop = uv_default_loop();
	struct sockaddr_in addr; 
	uv_ip4_addr("127.0.0.1", 12345, &addr);

	uv_tcp_init(loop, &server);
	uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
	uv_listen((uv_stream_t*)&server,128, connection_cb);

	return uv_run(loop, UV_RUN_DEFAULT);
}
