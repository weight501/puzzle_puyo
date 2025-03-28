#include "StateManager.hpp"
#include "../../states/LoginState.hpp"
#include "../../states/RoomState.hpp"
#include "../../states/CharacterSelectState.hpp"
#include "../../states/GameState.hpp"
#include "../../utils/Logger.hpp"
#include <format>

StateManager::StateManager()
    : current_state_id_(StateID::Max)
    , current_state_(nullptr)
    , initialized_(false)
    , paused_(false)
{
}

bool StateManager::Initialize()
{
    if (initialized_) 
    {
        SDL_LOG_WARN(SDL_LOG_CATEGORY_ERROR, "StateManager is already initialized");
        return true;
    }

    try 
    {
        InitializeStates();
        initialized_ = true;
        return true;
    }
    catch (const std::exception& e) 
    {
        LOGGER.Error("Failed to initialize StateManager: {}", e.what());
        return false;
    }
}

void StateManager::InitializeStates()
{
    // �� ���� ���� �� �ʱ�ȭ
    states_[StateID::Login] = std::make_shared<LoginState>();
    states_[StateID::Room] = std::make_shared<RoomState>();
    states_[StateID::CharSelect] = std::make_shared<CharacterSelectState>();
    states_[StateID::Game] = std::make_shared<GameState>();

    // �ʱ� ���� ����
    ChangeState(StateID::Login);
}

void StateManager::Update(float deltaTime)
{
    if (!initialized_ || paused_) 
    {
        return;
    }

    // ���� ���� ��û ó��
    ProcessStateChangeRequests();

    // ���� ���� ������Ʈ
    if (current_state_) 
    {
        current_state_->Update(deltaTime);
    }
}

void StateManager::ProcessStateChangeRequests()
{
    while (!state_change_queue_.empty()) 
    {
        StateID newState = state_change_queue_.front();
        ChangeState(newState);
        state_change_queue_.pop();
    }
}

void StateManager::Render()
{
    if (current_state_ != nullptr)
    {
        current_state_->Render();
    }
}

void StateManager::HandleEvent(const SDL_Event& event)
{
    if (!initialized_ || paused_ || !current_state_) 
    {
        return;
    }

    current_state_->HandleEvent(event);
}

void StateManager::HandleNetworkMessage(uint8_t connectionId, std::span<const char> message, uint32_t length)
{
    if (!initialized_ || paused_ || !current_state_) 
    {
        return;
    }

    current_state_->HandleNetworkMessage(connectionId, message, length);
}

void StateManager::RequestStateChange(StateID newState)
{
    if (current_state_id_ == newState) 
    {
        return;
    }

    state_change_queue_.push(newState);
}

void StateManager::ChangeState(StateID newState)
{
    auto it = states_.find(newState);

    if (it == states_.end()) 
    {
        throw std::runtime_error(std::format("Invalid state requested: {}", static_cast<int>(newState)));
    }

    if (current_state_) 
    {
        current_state_->Leave();
    }

    current_state_id_ = newState;
    current_state_ = it->second;

    if (current_state_->isInitialized() == false)
    {
        current_state_->Init();
    }
    
    current_state_->Enter();

    LOGGER.Info("State changed to: {}", static_cast<int>(newState));
}

void StateManager::PauseCurrentState()
{
    if (!paused_ && current_state_) 
    {
        paused_ = true;
    }
}

void StateManager::ResumeCurrentState()
{
    if (paused_ && current_state_) 
    {
        paused_ = false;
    }
}

void StateManager::Release()
{
    if (!initialized_) 
    {
        return;
    }

    if (current_state_) 
    {
        current_state_->Leave();
    }

    // ��� ���µ��� ���ҽ� ����
    for (auto& [id, state] : states_) 
    {
        if (state) 
        {
            state->Leave();
            state->Release();
        }
    }

    // �����̳ʵ� ����
    states_.clear();
    while (!state_change_queue_.empty()) 
    {
        state_change_queue_.pop();
    }

    current_state_.reset();
    current_state_id_ = StateID::Max;
    initialized_ = false;
    paused_ = false;
}