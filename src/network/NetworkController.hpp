#pragma once
/*
 *
 * ����: Ŭ�� ���� ���뿡 ���� ���� ó�� �������̽�
 *
 */
#include <memory>
#include <string>
#include <string_view>
#include <span>
#include <concepts>


#include "packets/PacketBase.hpp"
#include "GameClient.hpp"
#include "NetCommon.hpp"
#include <Windows.h>

enum class NetworkRole 
{
    None,
    Server,
    Client
};

struct ClientInfo;
class GameServer;

class NetworkController 
{
public:
    static NetworkController& Instance();

    void Initialize(NetworkRole role = NetworkRole::None);
    void Shutdown();

    // ��Ʈ��ũ ����
    bool Start();
    void Stop();
    void Update();

    // ����/Ŭ���̾�Ʈ ���� Ȯ��
    [[nodiscard]] bool IsServer() const { return role_ == NetworkRole::Server; }
    [[nodiscard]] bool IsClient() const { return role_ == NetworkRole::Client; }
    [[nodiscard]] bool IsRunning() const { return is_running_; }

    // IP �ּ� ����
    void SetAddress(std::string_view ip) { ip_address_ = ip; }
    [[nodiscard]] std::string_view GetAddress() const { return ip_address_; }

    void SendData(std::span<const char> data);

    // ���� ó��
    template<typename T> requires std::is_base_of_v<PacketBase, T>
    void SendData(const T& packet);
    bool SendToClient(ClientInfo* client, const PacketBase& packet);


    // ���� ���� ���� �Լ���
    void StartCharacterSelect();
    void StartGame();
    void GameInitialize(std::span<const uint8_t> block1, std::span<const uint8_t> block2);
    void ChangeCharSelect(uint8_t x, uint8_t y);
    void DecideCharacter(uint8_t x, uint8_t y);

    // ��� ���� �Լ���
    void AddNewBlock(std::span<const uint8_t> block);
    void MoveBlock(uint8_t move_type, float position);
    void RotateBlock(uint8_t rotate_type, bool horizontal_moving);
    void CheckBlockState();
    void UpdateGameBlockPos(float pos1, float pos2);
    void RequireFallingBlock(uint8_t falling_idx, bool falling);
    void ChangeBlockState(uint8_t state);
    void PushBlockInGame(std::span<const float> pos1, std::span<const float> pos2);
    void SyncPositionY(float positionY, float velocity);

    // ���� ���� �Լ���
    void AttackInterruptBlock(int16_t count, float x, float y, uint8_t type);
    void DefenseInterruptBlock(int16_t count, float x, float y, uint8_t type);
    void AddInterruptBlock(uint8_t y_row_count, uint8_t x_count, std::span<const uint8_t> xIdx);
    void StopComboAttack();
    void LoseGame();
    void ReStartGame(std::span<const uint8_t> block1, std::span<const uint8_t> block2);

    // ä��
    void ChatMessage(std::string_view msg);    

    bool ProcessRecv(WPARAM wParam, LPARAM lParam);    

private:
    NetworkController() = default;
    ~NetworkController() = default;

    NetworkController(const NetworkController&) = delete;
    NetworkController& operator=(const NetworkController&) = delete;

private:
    std::unique_ptr<GameServer> server_;
    std::unique_ptr<GameClient> client_;

    NetworkRole role_{ NetworkRole::None };
    bool is_running_{ false };
    std::string ip_address_;
};

template<typename T> requires std::is_base_of_v<PacketBase, T>
void NetworkController::SendData(const T& packet)
{
    if (role_ == NetworkRole::Client)
    {
        if (client_)
        {
            auto packetBytes = packet.ToBytes();
            client_->SendData(std::span<const char>(packetBytes.data(), packetBytes.size()));
        }
    }
}



#define NETWORK NetworkController::Instance()

