#pragma once
/**
 *
 * ����: PacketBase ��Ŷ�� ���� ó���� ����ϴ� �߻�ȭ �������̽�
 *
 */

#include <memory>
#include "../PacketBase.hpp"

class IPacketProcessor 
{
public:
    virtual ~IPacketProcessor() = default;
    virtual void Initialize() = 0;
    virtual void Process(const PacketBase& packet, struct ClientInfo* client) = 0;
    virtual void Release() = 0;
    [[nodiscard]] virtual PacketType GetPacketType() const = 0;
};
