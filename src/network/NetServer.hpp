#pragma once
/*
 *
 * ����: TCP ����� IOCP�� ����Ͽ� �񵿱� I/O ����
 *  1. ��Ŀ ������� ���� �����带 ���� �۾��� �и�.
 *
 */

#include "NetCommon.hpp"
#include "RingBuffer.hpp"

#include <array>
#include <concurrent_queue.h>
#include <memory>
#include <span>

struct SendQueueData
{
    std::vector<char> buffer;

    SendQueueData(std::span<const char> data) : buffer(data.begin(), data.end()) {}
};


struct ClientInfo 
{
    Socket socket;
    OverlappedEx recv_overlapped;
    OverlappedEx send_overlapped;

    RingBuffer  recv_buffer;
    Concurrency::concurrent_queue<std::shared_ptr<SendQueueData>> send_queue;

    ClientInfo() 
    {
        recv_overlapped.operation = OperationType::Receive;
        send_overlapped.operation = OperationType::Send;

        recv_buffer.Create(Constants::Network::MAX_RINGBUFSIZE);
    }
};

class NetServer 
{
public:
    NetServer();
    virtual ~NetServer();
    NetServer(const NetServer&) = delete;
    NetServer& operator=(const NetServer&) = delete;

    [[nodiscard]] bool StartServer();
    bool ExitServer();

    [[nodiscard]] bool SendMsg(ClientInfo* client, std::span<const char> msg);

protected:
    virtual bool ConnectProcess(ClientInfo* client) = 0;
    virtual bool DisconnectProcess(ClientInfo* client) = 0;
    virtual bool PacketProcess(ClientInfo* client, std::span<const char> packet) = 0;    

    void CloseSocket(ClientInfo* client, bool force = false);

private:
    static constexpr size_t MAX_WORKER_THREAD_COUNT = 1;

    // ���� ó�� ����
    [[nodiscard]] bool InitSocket();
    [[nodiscard]] bool BindAndListen(uint16_t port);

    // IOCP �� ������ ����
    static unsigned int CALLBACK CallWorkerThread(void* arg);
    static unsigned int CALLBACK CallAccepterThread(void* arg);

    [[nodiscard]] bool BindIOCP(ClientInfo* client);
    [[nodiscard]] bool CreateThreadAndIOCP();
    [[nodiscard]] bool CreateWorkerThread();
    [[nodiscard]] bool CreateAccepterThread();
    unsigned int WorkerThread();
    unsigned int AccepterThread();
    void DestroyThread();

    // ������ �ۼ��� ó��
    [[nodiscard]] bool BindRecv(ClientInfo* client, char* processed_pos, int remain_size);
    void ProcessRecv(ClientInfo* client, OverlappedEx* overlapped, DWORD bytes);
    void ProcessSend(ClientInfo* client, OverlappedEx* overlapped, DWORD bytes);

    [[nodiscard]] ClientInfo* GetEmptyClientInfo();
    void LogError(std::wstring_view msg) const;

private:
    WSASession wsa_session_;
    Socket listen_socket_;

    HANDLE iocp_handle_{ nullptr };
    HANDLE accepter_thread_{ nullptr };
    std::array<HANDLE, MAX_WORKER_THREAD_COUNT> worker_threads_{};

    std::unique_ptr<ClientInfo[]> clients_;
    size_t client_count_{ 0 };

    std::atomic<bool> worker_running_{ false };
    std::atomic<bool> accepter_running_{ false };
};

