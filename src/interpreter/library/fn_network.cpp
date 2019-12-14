#include "library.h"
#include "ogm/interpreter/Variable.hpp"
#include "ogm/common/error.hpp"
#include "ogm/common/util.hpp"
#include "ogm/interpreter/Executor.hpp"
#include "ogm/interpreter/execute.hpp"
#include "ogm/interpreter/display/Display.hpp"

#include <string>
#include "ogm/common/error.hpp"
#include <locale>
#include <cctype>
#include <cstdlib>

using namespace ogm::interpreter;
using namespace ogm::interpreter::fn;

#define frame staticExecutor.m_frame

void ogm::interpreter::fn::network_create_server_raw(VO out, V type, V port, V max_client)
{
    out = frame.m_network.create_server_socket(
        true,
        static_cast<NetworkProtocol>(type.castCoerce<size_t>()),
        port.castCoerce<port_t>(),
        max_client.castCoerce<size_t>(),
        staticExecutor.m_self->m_data.m_id
    );
}

void ogm::interpreter::fn::network_create_socket(VO out, V type)
{
    out = frame.m_network.create_socket(
        true,
        static_cast<NetworkProtocol>(type.castCoerce<size_t>()),
        staticExecutor.m_self->m_data.m_id
    );
}

void ogm::interpreter::fn::network_create_socket_ext(VO out, V type, V port)
{
    out = frame.m_network.create_socket(
        true,
        static_cast<NetworkProtocol>(type.castCoerce<size_t>()),
        staticExecutor.m_self->m_data.m_id,
        port.castCoerce<port_t>()
    );
}

void ogm::interpreter::fn::network_connect_raw(VO out, V socket, V url, V port)
{
    out = frame.m_network.connect_socket(
        socket.castCoerce<size_t>(),
        url.castCoerce<string_t>().c_str(),
        port.castCoerce<port_t>()
    );
}

void ogm::interpreter::fn::network_send_raw(VO out, V socket, V buffer, V size)
{
    size_t s = size.castCoerce<size_t>();
    Buffer& b = frame.m_buffers.get_buffer(buffer.castCoerce<size_t>());
    char* c = new char[s];
    size_t read = b.peek_n(c, s);
    size_t sent = frame.m_network.send_raw(
        socket.castCoerce<size_t>(),
        read,
        c
    );
    b.read_n(c, sent);

    delete[] c;
}

void ogm::interpreter::fn::network_send_udp_raw(VO out, V socket, V url, V port, V buffer, V size)
{
    size_t s = size.castCoerce<size_t>();
    Buffer& b = frame.m_buffers.get_buffer(buffer.castCoerce<size_t>());
    char* c = new char[s];
    size_t read = b.peek_n(c, s);
    size_t sent = frame.m_network.send_raw(
        socket.castCoerce<size_t>(),
        read,
        c,
        url.castCoerce<string_t>().c_str(),
        port.castCoerce<port_t>()
    );
    b.read_n(c, sent);

    delete[] c;
}

void ogm::interpreter::fn::network_destroy(VO out, V socket)
{
    frame.m_network.destroy_socket(
        socket.castCoerce<size_t>()
    );
}