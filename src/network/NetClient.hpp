#pragma once
/*
 *
 * ����: Ŭ���̾�Ʈ ��Ŷ ó�� WSAEventSelect
 *
 */

#include "NetCommon.hpp"
#include "../core/common/constants/Constants.hpp"

#include <string>
#include <array>
#include <memory>
#include <thread>
#include <span>


class NetClient 
{
public:
    NetClient() = default;
    virtual ~NetClient();

    NetClient(const NetClient&) = delete;
    NetClient& operator=(const NetClient&) = delete;

    // �ʱ�ȭ �� ���� �Լ���
    [[nodiscard]] virtual bool Start(HWND hwnd);
    virtual void Exit();


    [[nodiscard]] bool Connect(std::string_view ip, uint16_t port);
    void Disconnect(bool force = false);

    void SendData(std::span<const char> data);
    [[nodiscard]] bool ProcessRecv(WPARAM wParam, LPARAM lParam);    
    
protected:
    virtual void ProcessPacket(std::span<const char> packet) = 0;
    virtual void ProcessConnectExit() {};

    void EventPollingThreadFunc();
    //void HandleNetworkEvent(SOCKET socket, LONG event_type);

private:
    // ��Ŀ ������ ���� 
    //static unsigned int CALLBACK WorkerThread(void* arg);
    //[[nodiscard]] bool CreateWorkerThread();
    //unsigned int RunWorkerThread();

    // ���� ���� �Լ�
    [[nodiscard]] bool InitSocket();
    void LogError(std::wstring_view msg) const;

private:
    WSASession wsa_session_;
    Socket socket_;
    HANDLE worker_thread_{ nullptr };

    HWND hwnd_;
    bool initialize_;

    std::array<char, Constants::Network::CLIENT_BUF_SIZE * 8> msg_buffer_{};
    uint32_t recv_remain_size_{ 0 };

    std::atomic<bool> is_connected_{ false };

    WSAEVENT event_handle_{ WSA_INVALID_EVENT };

    std::thread event_polling_thread_;
    std::atomic<bool> polling_thread_running_{ false };    
};
