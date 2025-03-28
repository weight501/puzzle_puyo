#pragma once
/*
 *
 * ����: ���� ���ݿ� ���� ������ ���غ��
 *
 */

#include "Block.hpp"

class IceBlock : public Block 
{
public:
    IceBlock() = default;
    ~IceBlock() override = default;

    IceBlock(const IceBlock&) = delete;
    IceBlock& operator=(const IceBlock&) = delete;
    IceBlock(IceBlock&&) noexcept = delete;
    IceBlock& operator=(IceBlock&&) noexcept = delete;

    void Update(float deltaTime) override;
    void Render() override;
    void SetState(BlockState state) override;

private:

    void UpdateDestroying(float deltaTime);
    void UpdateDownMoving(float deltaTime);

private:
    float alpha_{ 255.0f };
    bool is_initialized_{ false };
};