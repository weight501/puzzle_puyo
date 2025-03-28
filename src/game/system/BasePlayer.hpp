#pragma once
/**
 *
 * ����: ���� �÷��̾� �⺻ Ŭ���� (���� ��� ����)
 *
 */
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <memory>
#include <array>
#include <span>
#include "../RenderableObject.hpp"
#include "../../core/common/constants/Constants.hpp"
#include "../../core/common/types/GameTypes.hpp"
#include "../../states/GameState.hpp"
#include "../event/PlayerEvent.hpp"

class Block;
class GameBackground;
class GroupBlock;
class GameGroupBlock;
class IceBlock;
class BulletEffect;
class GameBoard;
class InterruptBlockView;
class ComboView;
class ResultView;
class ImageTexture;
class IPlayerEventListener;
enum class BoardState;

class BasePlayer : public RenderableObject {
public:

    // ��� �׷� ���� Ÿ�� ����
    using BlockVector = std::vector<Block*>;

    BasePlayer();
    virtual ~BasePlayer();

    void Update(float deltaTime) override;
    void Render() override;
    virtual void Reset();
    virtual void Release();

    template<typename Container>
    void ReleaseContainer(Container& container);

    // �ʱ�ȭ �� �����
    virtual bool Initialize(const std::span<const uint8_t>& blocktype1,
        const std::span<const uint8_t>& blocktype2,
        uint8_t playerIdx,
        uint16_t characterIdx,
        const std::shared_ptr<GameBackground>& background) = 0;
    virtual bool Restart(const std::span<const uint8_t>& blockType1 = {}, const std::span<const uint8_t>& blockType2 = {}) = 0;

    // ��� ����
    virtual void CreateNextBlock() = 0;
    virtual void PlayNextBlock() = 0;
    virtual bool CheckGameBlockState() = 0;

    // ��� ����
    virtual void MoveBlock(uint8_t moveType, float position) = 0;
    virtual void RotateBlock(uint8_t rotateType, bool horizontalMoving) = 0;
    virtual void UpdateBlockPosition(float pos1, float pos2) = 0;
    virtual void UpdateFallingBlock(uint8_t fallingIdx, bool falling) = 0;
    virtual void ChangeBlockState(uint8_t state) = 0;

    // ���� ��� ����
    virtual void AddInterruptBlock(int16_t count);
    virtual void AttackInterruptBlock(float x, float y, uint8_t type) = 0;
    virtual void DefenseInterruptBlockCount(int16_t count, float x, float y, uint8_t type) = 0;
    virtual void UpdateInterruptBlock(int16_t count);
    virtual void CollectRemoveIceBlocks();

    // ���� ���� ����
    virtual void LoseGame(bool isWin);
    virtual void SetGameQuit() { is_game_quit_ = true; }

    // ���� ��ȸ
    GamePhase GetGameState() const { return state_info_.current_phase; }
    uint8_t GetPlayerID() const { return player_id_; }
    int16_t GetTotalInterruptBlockCount() const { return score_info_.total_interrupt_block_count; }
    int16_t GetTotalEnemyInterruptBlockCount() const { return score_info_.total_enemy_interrupt_block_count; }
    std::shared_ptr<GameBoard> GetGameBoard() const { return game_board_; }
    Block* (*GetGameBlocks())[Constants::Board::BOARD_X_COUNT] { return board_blocks_; }

    bool IsRunning() const { return state_info_.is_running; }
    void SetRunning(bool running) { state_info_.is_running = running; }
    [[nodiscard]] bool IsPossibleMove(int xIdx);
    [[nodiscard]] const std::shared_ptr<InterruptBlockView>& GetInterruptView() { return interrupt_view_; }

    // ������ ���/���� �޼���
    void AddEventListener(IPlayerEventListener* listener);
    void RemoveEventListener(IPlayerEventListener* listener);
    
    // ������Ʈ ����
    void SetInterruptView(const std::shared_ptr<InterruptBlockView>& view) { interrupt_view_ = view; }
    void SetComboView(const std::shared_ptr<ComboView>& view) { combo_view_ = view; }
    void SetResultView(const std::shared_ptr<ResultView>& view) { result_view_ = view; }
    void SetGameBoard(const std::shared_ptr<GameBoard>& board) { game_board_ = board; }
    void SetBackGround(const std::shared_ptr<GameBackground>& backGround) { background_ = backGround; }
    void SetGameBoardState(BoardState bordState);

    void SetComboAttackState(bool enable) { state_info_.is_combo_attack = enable; }
    void SetTotalInterruptBlockCount(uint16_t count) 
    { 
        score_info_.total_interrupt_block_count = count; 
        state_info_.has_ice_block = count > 0;
    }


protected:
    // �ʱ�ȭ ���� �޼���
    virtual bool InitializeViews();
    virtual bool InitializeGameBoard(float posX, float posY);
    virtual bool InitializeControlBlock();

    // ��� ���� ���� �޼���
    void UpdateLinkState(Block* block);
    void CreateBlockClearEffect(const std::shared_ptr<Block>& block);

    // ��� ���� ���� �޼���
    void RemoveBlock(Block* block, const SDL_Point& pos_idx);
    void UpdateFallingBlocks(const std::list<SDL_Point>& x_index_list);
    void UpdateBlockLinks();

    // ���� ��� ���� �޼���
    int16_t GetComboConstant(uint8_t combo_count) const;
    uint8_t GetLinkBonus(size_t link_count) const;
    uint8_t GetTypeBonus(size_t count) const;
    uint8_t GetMargin() const;

    // �Ѿ� �� ����Ʈ ���� �޼���
    virtual void CreateBullet(Block* block) = 0;
    void UpdateBullets(float delta_time);
    void NotifyEvent(const std::shared_ptr<BasePlayerEvent>& event);


    // ��� ���Ϸ� ���� ����
    void CreateBlocksFromFile();

    // ���� ���� �ڵ鸵 (���ø� �޼��� ����)
    virtual bool FindMatchedBlocks(std::list<BlockVector>& matchedGroups);
    virtual short RecursionCheckBlock(short x, short y, Constants::Direction direction, std::vector<Block*>& matchedBlocks);    
    virtual void UpdateComboState();
    virtual void ResetComboState();

    // ���� ��� ���� ���� �޼���
    virtual void GenerateIceBlocks();
    virtual void GenerateLargeIceBlockGroup(const std::shared_ptr<ImageTexture>& texture, uint8_t playerID);
    virtual void GenerateSmallIceBlockGroup(const std::shared_ptr<ImageTexture>& texture, uint8_t playerID,
        const std::span<const uint8_t>& xIdxList = {});
    virtual void InitializeIceBlock(IceBlock* block, const std::shared_ptr<ImageTexture>& texture,
        int x, int y, uint8_t playerID);
    virtual void RemoveIceBlocks(std::list<SDL_Point>& x_index_list);
    virtual void UpdateShatteringPhase(float deltaTime);
    virtual void HandleClearedBlockGroup(std::list<BlockVector>::iterator& group_it, std::list<SDL_Point>& x_index_list);
    virtual void UpdateAfterBlocksCleared();
    virtual void UpdateAllBlocksState(BlockState state);
    virtual bool IsGameOver() const;
    virtual bool ProcessGameOver();

    // ���� ���� �޼���
    virtual void SetGamePhase(GamePhase newPhase);


    // ���� ���� ���� ����ü
    struct GameStateInfo 
{
        GamePhase current_phase{ GamePhase::Standing };
        GamePhase previous_phase{ GamePhase::Standing };
        float play_time{ 0.0f };
        bool is_running{ false };
        bool is_defending{ false };
        bool is_attacked{ false };
        bool is_combo_attack{ false };
        bool should_quit{ false };
        uint8_t defense_count{ 0 };
        bool has_ice_block{ false };  // ���� ��� ���� ����
    };

    // ���� ���� ����ü
    struct ScoreInfo 
    {
        uint32_t total_score{ 0 };
        uint32_t rest_score{ 0 };
        uint8_t combo_count{ 0 };
        int16_t total_interrupt_block_count{ 0 };
        int16_t total_enemy_interrupt_block_count{ 0 };
        int16_t add_interrupt_block_count{ 0 };

        void reset() {
            total_score = 0;
            rest_score = 0;
            combo_count = 0;
            total_interrupt_block_count = 0;
            total_enemy_interrupt_block_count = 0;
            add_interrupt_block_count = 0;
        }
    };

protected:
    // ���� ���� ����
    uint8_t player_id_{ 0 };
    int16_t character_id_{ 0 };
    bool is_game_quit_{ false };   

    // ���� ����
    GameStateInfo state_info_;
    ScoreInfo score_info_;

    // ������Ʈ
    std::shared_ptr<GameBoard> game_board_;
    std::shared_ptr<InterruptBlockView> interrupt_view_;
    std::shared_ptr<ComboView> combo_view_;
    std::shared_ptr<ResultView> result_view_;
    std::shared_ptr<GameGroupBlock> control_block_;
    std::shared_ptr<GameBackground> background_;

    // ���� ������
    Block* board_blocks_[Constants::Board::BOARD_Y_COUNT][Constants::Board::BOARD_X_COUNT]{ nullptr };
    
    std::vector<RenderableObject*> draw_objects_;
    std::vector<IPlayerEventListener*> event_listeners_;

    std::list<std::shared_ptr<Block>> block_list_;
    std::list<std::shared_ptr<BulletEffect>> bullet_list_;
    std::list<BlockVector> matched_blocks_;
    
    std::set<std::shared_ptr<IceBlock>> ice_blocks_;
    std::deque<std::shared_ptr<GroupBlock>> next_blocks_;    
};

template<typename Container>
void BasePlayer::ReleaseContainer(Container& container) 
{
    for (auto& item : container) 
    {
        if (item) {
            item->Release();
        }
    }
    container.clear();
}