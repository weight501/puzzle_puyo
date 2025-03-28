#pragma once

#include <winSock2.h>
#include <WS2tcpip.h>
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <stdexcept>


#define WM_SOCKET (WM_USER + 1)

enum class OperationType : uint8_t
{
    None,
    Receive,
    Send,
};

struct OverlappedEx 
{
    WSAOVERLAPPED overlapped;
    WSABUF wsa_buf;
    OperationType operation;
    int receive_size;      // ���� ������ ũ��
    int packet_size;       // ��Ŷ ũ��
    char* begin_buf;       // ���� ���� ��ġ
    int remain_size;       // ���� ������ ũ��

    OverlappedEx() :
        operation(OperationType::None),
        receive_size(0),
        packet_size(0),
        begin_buf(nullptr),
        remain_size(0)
    {
        ZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
        wsa_buf.buf = nullptr;
        wsa_buf.len = 0;
    }
};

// RAII ���� ���� Ŭ����
class Socket
{
    SOCKET socket_{ INVALID_SOCKET };

public:
    Socket() = default;
    explicit Socket(SOCKET socket) : socket_(socket) {}
    ~Socket() 
    {
        if (is_valid()) {
            closesocket(socket_);
        }
    }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept : socket_(other.socket_) 
    {
        other.socket_ = INVALID_SOCKET;
    }

    Socket& operator=(Socket&& other) noexcept 
    {
        if (this != &other) 
        {
            if (is_valid())
            {
                closesocket(socket_);
            }
            socket_ = other.socket_;
            other.socket_ = INVALID_SOCKET;
        }
        return *this;
    }

    [[nodiscard]] SOCKET get() const { return socket_; }
    [[nodiscard]] bool is_valid() const { return socket_ != INVALID_SOCKET; }

    void close()
    {
        if (is_valid())
        {
            closesocket(socket_);
            socket_ = INVALID_SOCKET;
        }
    }
};

// ��Ʈ��ũ ���� ó���� ���� ���� Ŭ����
class NetworkException : public std::runtime_error
{
public:
    explicit NetworkException(const std::string& message)
        : std::runtime_error(message) {}
};


class WSASession
{
public:
    WSASession()
    {
        WSADATA wsa_data;
        const int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
        if (result != 0) 
        {
            throw NetworkException("WSAStartup Failed");
        }
    }

    ~WSASession()
    {
        WSACleanup();
    }

    WSASession(const WSASession&) = delete;
    WSASession& operator=(const WSASession&) = delete;
};

// Result Ÿ��
template<typename T>
class Result
{
    bool success_;
    std::string error_message_;
    T value_;

public:
    explicit Result(T value) : success_(true), value_(std::move(value)) {}
    explicit Result(std::string error) : success_(false), error_message_(std::move(error)) {}

    [[nodiscard]] bool is_success() const { return success_; }
    [[nodiscard]] const std::string& get_error() const { return error_message_; }
    [[nodiscard]] const T& get_value() const { return value_; }
};
