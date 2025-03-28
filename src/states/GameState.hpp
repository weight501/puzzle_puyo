#pragma once
/*
 *
 * ����: ���� ���� ���� ������
 *
 */

#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include <span>
#include <string_view>
#include <array>

#include "BaseState.hpp"
#include "../core/common/types/GameTypes.hpp"
#include "../network/PacketProcessor.hpp"
#include "../network/NetworkController.hpp"
#include "../game/block/Block.hpp"
#include "../core/common/constants/Constants.hpp"
#include "../game/event/IPlayerEventListener.hpp"

class EditBox;
class Button;
class GameBackground;
class InterruptBlockView;
class ComboView;
class ResultView;
class LocalPlayer;
class RemotePlayer;
class Player;
class NetworkController;
struct ClientInfo;
class OnPlayerEvent;

namespace GameStateDetail
{
    constexpr Constants::Direction GetOppositeDirection(Constants::Direction dir)
    {
        switch (dir)
        {
        case Constants::Direction::Left:
            return Constants::Direction::Right;
        case Constants::Direction::Right:
            return Constants::Direction::Left;
        case Constants::Direction::Top:
            return Constants::Direction::Bottom;
        case Constants::Direction::Bottom:
            return Constants::Direction::Top;
        default:
            return Constants::Direction::None;
        }
    }
}

// ���� ���� ������
enum class GamePhase
{
    Standing,   // ��� ����
    Playing,    // ���� ���� ��
    Shattering, // ��� �ı� ��
    IceBlocking,// ���� ��� ���� ��
    GameOver    // ���� ����
};

// ��� ��ġ ��Ŀ ����ü
struct BlockPositionMarker
{
    float xPos;
    float yPos;
    BlockType type;
};



class GameState final : public BaseState, public IPlayerEventListener
{
public:
    GameState();
    ~GameState() override;

    // ����/�̵� ������ ����
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;
    GameState(GameState&&) = delete;
    GameState& operator=(GameState&&) = delete;

    // BaseState �������̽� ����
    bool Init() override;
    void Enter() override;
    void Leave() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Release() override;
    void HandleEvent(const SDL_Event& event) override;
    void HandleNetworkMessage(uint8_t connectionId, std::span<const char> data, uint32_t length) override;

    // ���� ���� ����
    [[nodiscard]] std::string_view GetStateName() const override { return "Game"; }
    [[nodiscard]] const std::shared_ptr<LocalPlayer>& GetLocalPlayer() const { return local_player_; }
    [[nodiscard]] const std::shared_ptr<RemotePlayer>& GetRemotePlayer() const { return remote_player_; }
    [[nodiscard]] GameBackground* GetBackGround() const { return background_.get(); }
    [[nodiscard]] Block* (*GetGameBlocks(uint8_t playerId))[Constants::Board::BOARD_X_COUNT];

    // ���� Flow ����
    bool GameRestart();
    bool GameExit();
    void GameQuit();
    void CreateGamePlayer(const std::span<const uint8_t>& blockType1, const std::span<const uint8_t>& blockType2,
        uint8_t playerIdx, uint16_t characterIdx);

    void ScheduleGameStart();


private:
    // �ʱ�ȭ ����
    bool LoadResources();
    bool CreateUI();
    bool CreatePlayers();
    void Reset();

    // �̺�Ʈ �ڵ鸵
    void HandleMouseInput(const SDL_Event& event);
    void HandleKeyboardInput(const SDL_Event& event);
    void HandleKeyboardState();
    void HandleSystemEvent(const SDL_Event& event);

    // ������ ����
    void RenderUI();
#ifdef _DEBUG
    void RenderDebugInfo();
    void RenderDebugGrid();
#endif

    // ��Ŷ ���μ��� ����
    void InitializePacketHandlers();

    // ��Ŷ �ڵ鷯
    void HandleGameInitialize(uint8_t connectionId, const GameInitPacket* packet);
    void HandleAddNewBlock(uint8_t connectionId, const AddNewBlockPacket* packet);
    void HandleUpdateBlockMove(uint8_t connectionId, const MoveBlockPacket* packet);
    void HandleBlockRotate(uint8_t connectionId, const RotateBlockPacket* packet);
    void HandleStartGame();
    void HandleCheckBlockState(uint8_t connectionId, const CheckBlockStatePacket* packet);
    void HandleChangeBlockState(uint8_t connectionId, const ChangeBlockStatePacket* packet);
    void HandlePushBlockInGame(uint8_t connectionId, const PushBlockPacket* packet);
    void HandleStopCombo(uint8_t connectionId, const StopComboPacket* packet);
    void HandleLose(uint8_t connectionId, const LoseGamePacket* packet);
    void HandleAttackInterrupt(uint8_t connectionId, const AttackInterruptPacket* packet);
    void HandleAddInterruptBlock(uint8_t connectionId, const AddInterruptBlockPacket* packet);
    void HandleRestart(uint8_t connectionId, const RestartGamePacket* packet);
    void HandleDefenseInterrupt(uint8_t connectionId, const DefenseInterruptPacket* packet);
    void HandleDefenseResultInterruptBlockCount(uint8_t connectionId, const DefenseResultInterruptBlockCountPacket* packet);
    void HandleAttackResultPlayerInterruptBlocCount(uint8_t connectionId, const AttackResultPlayerInterruptBlocCountPacket* packet);
    void HandleSyncBlockPositionY(uint8_t connectionId, const SyncBlockPositionYPacket* packet);    
    void HandleGameOver();

    // �̺�Ʈ �ڵ鷯
    void OnPlayerEvent(const std::shared_ptr<BasePlayerEvent>& event) override;
    void HandlePlayerGameOver(const std::shared_ptr<GameOverEvent>& event);
    void HandleGameRestart(const std::shared_ptr<GameRestartEvent>& event);
    void HandleAddInterruptBlock(const std::shared_ptr<AddInterruptBlockEvent>& event);
    void HandleAttackInterruptBlock(const std::shared_ptr<AttackInterruptBlockEvent>& event);
    void HandleDefenseBlock(const std::shared_ptr<DefenseBlockEvent>& event);

private:
    // �÷��̾� ���� ���
    std::shared_ptr<LocalPlayer> local_player_;
    std::shared_ptr<RemotePlayer> remote_player_;
    std::shared_ptr<GameBackground> background_;

    // UI ������Ʈ
    std::shared_ptr<Button> restart_button_;
    std::shared_ptr<Button> exit_button_;

    // ���� ����
    uint64_t lastInputTime_{ 0 };
    bool initialized_{ false };
    bool is_network_game_{ false };
    uint8_t local_player_id_{ 0 };
    bool should_quit_{ false };

    // ��Ŷ ���μ���
    PacketProcessor packet_processor_{};
};