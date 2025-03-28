#pragma once
#include <cstdint>


enum class PacketType : uint16_t 
{
    None = 0,

    // ����/�ʱ�ȭ ���� (1-99)
    GiveId = 1,
    ConnectLobby = 2,

    //�÷��̾� ����
    RemovePlayer = 50,
    PlayerInfo = 51,
    AddPlayer = 52,
    RemovePlayerInRoom = 53,

    // ä�� ���� (100-199)
    ChatMessage = 100,
    ChatMessageServer = 101,

    // ĳ���� ���� ���� (200-299)
    StartCharSelect = 200,
    ChangeCharSelect = 201,
    DecideCharSelect = 202,

    // ���� ����/�ʱ�ȭ ���� (300-399)
    StartGame = 300,
    InitializeGame = 301,
    InitializePlayer = 302,
    RestartGame = 303,
    GameOver = 304,

    // ��� ���� ���� (400-499)
    AddNewBlock = 400,
    UpdateBlockState = 401,
    UpdateBlockMove = 402,
    UpdateBlockRotate = 403,
    CheckBlockState = 404,
    UpdateBlockPos = 405,
    UpdateBlockFalling = 406,
    ChangeBlockState = 407,
    PushBlockInGame = 408,
    SyncBlockPositionY = 409,

    // ����/��� ���� (500-599)
    AttackInterruptBlock = 500,
    DefenseInterruptBlock = 501,
    AddInterruptBlock = 502,
    StopComboAttack = 503,
    DefenseResultInterruptBlockCount = 504,
    AttackResultPlayerInterruptBlocCount = 505,

    // 500����� ���� �����̹Ƿ� �ű⿡ ��ġ
    ComboUpdate = 550,        

    // ���� ���� ���� (600-699)
    LoseGame = 600,

    Max
};

// ��Ŷ Ÿ�� ��ȿ�� �˻� ���� �Լ�
[[nodiscard]] constexpr bool IsValidPacketType(PacketType type) 
{
    return type > PacketType::None && type < PacketType::Max;
}

// ��Ŷ ���� Ȯ�� ���� �Լ���
[[nodiscard]] constexpr bool IsConnectionPacket(PacketType type) 
{
    return type >= PacketType::GiveId && type < PacketType::ChatMessage;
}

[[nodiscard]] constexpr bool IsChatPacket(PacketType type) 
{
    return type >= PacketType::ChatMessage && type < PacketType::StartCharSelect;
}

[[nodiscard]] constexpr bool IsCharacterSelectPacket(PacketType type) 
{
    return type >= PacketType::StartCharSelect && type < PacketType::StartGame;
}

[[nodiscard]] constexpr bool IsGameInitPacket(PacketType type) 
{
    return type >= PacketType::StartGame && type < PacketType::AddNewBlock;
}

[[nodiscard]] constexpr bool IsBlockOperationPacket(PacketType type) 
{
    return type >= PacketType::AddNewBlock && type < PacketType::AttackInterruptBlock;
}

[[nodiscard]] constexpr bool IsCombatPacket(PacketType type) 
{
    return type >= PacketType::AttackInterruptBlock && type < PacketType::LoseGame;
}
