#pragma once
#include "PacketType.hpp"
#include <cstdint>
#include <vector>
#include <string_view>

#pragma pack(push, 1)
struct PacketBase
{
    uint32_t size;      // ��Ŷ ��ü ũ��
    uint16_t type;      // ��Ŷ Ÿ�� (PacketType�� uint16_t�� ó��)

    // ��Ŷ �����͸� ����Ʈ �迭�� ��ȯ
    std::vector<char> ToBytes() const
    {
        std::vector<char> bytes(size);
        std::memcpy(bytes.data(), this, size);
        return bytes;
    }
};

#pragma pack(pop)