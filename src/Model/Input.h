#pragma once

#include <cstdint>

class Input
{
public:
    Input() : valueId(0), potId(0)
    {
    }

    Input(uint8_t newValueId, uint8_t newPotId)
        : valueId(newValueId), potId(newPotId)
    {
    }

    virtual ~Input() = default;

    uint8_t getPotId(void) const
    {
        return (potId);
    }

    uint8_t getValueId(void) const
    {
        return (valueId);
    }

    void print(void) const
    {
        logMessage("    potId: %d", getPotId());
        logMessage("    valueId: %d", getValueId());
    }

private:
    uint8_t valueId;
    uint8_t potId;
};