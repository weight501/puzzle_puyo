#pragma once
/*
 *
 * ����: ������Ʈ render �߻�ȭ Interface
 * 1. �׷����� ����� �� Ŭ������ �� Interface�� ��� ����
 *
 */


#include <string>
#include <string_view>

class IResource 
{
public:
    IResource() = default;
    virtual ~IResource() = default;

    virtual void Unload() = 0;
    [[nodiscard]] virtual bool Load(const std::string& path) = 0;    
    [[nodiscard]] virtual bool IsLoaded() const = 0;
    [[nodiscard]] virtual std::string_view GetResourcePath() const = 0;
    
};