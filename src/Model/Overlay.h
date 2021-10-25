#pragma once

#include "helpers.h"
#include <cstdint>
#include <string>
#include <map>

class Overlay
{
public:
    Overlay() : id(0)
    {
    }

    explicit Overlay(uint8_t newId) : id(newId)
    {
    }

    uint8_t getId(void) const
    {
        return (id);
    }

	void print(void) const
	{
	    logMessage("id: %d", getId());
	    for (const auto &[midiValue, address] : items) {
	        logMessage("    midiValue: %d, address: %s", midiValue, address);
	    }
	}

private:
    uint8_t id;

public:
    std::map<uint16_t, uint32_t> items;
};
