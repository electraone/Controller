#pragma once

#include <cstdint>
#include <cstring>
#include <string>

#include "ElectraMessage.h"
#include "Overlay.h"
#include "Message.h"
#include "Macros.h"
#include "helpers.h"
#include "luahooks.h"

class Control;

// Define a list of all supported value Ids
#define MAX_NUM_VALUE_IDS 13
static const char *valueIds[MAX_NUM_VALUE_IDS] = {
    "value", "attack", "decay", "sustain", "release", "l1", "r1",
    "l2",    "r2",     "l3",    "r3",      "l4",      "r4"
};

class ControlValue
{
public:
    ControlValue()
        : handle(0),
          index(0),
          defaultValue(0),
          min(0),
          max(NOT_SET),
          overlayId(0),
          control(nullptr),
          overlay(nullptr)
    {
    }

    ControlValue(Control *newControl,
                 const char *newValueId,
                 uint8_t newIndex,
                 int16_t newDefaultValue,
                 int16_t newMin,
                 int16_t newMax,
                 uint8_t newOverlayId,
                 Message(newMessage),
                 const char *newFormatter,
                 const char *newFunction,
                 Overlay *newOverlay)
        : control(newControl),
          index(newIndex),
          defaultValue(newDefaultValue),
          min(newMin),
          max(newMax),
          overlayId(newOverlayId),
          message(newMessage),
          overlay(newOverlay)
    {
        if (newFormatter) {
            formatter = newFormatter;
        }

        if (newFunction) {
            function = newFunction;
        }

        // translate the valueId to the numeric handle
        handle = translateId(newValueId);
    }

    virtual ~ControlValue() = default;

    Control *getControl(void) const
    {
        return (control);
    }

    int16_t getHandle(void) const
    {
        return (handle);
    }

    int16_t getIndex(void) const
    {
        return (index);
    }

    const char *getId(void) const
    {
        return (translateId(handle));
    }

    void setDefault(int16_t newDefaultValue)
    {
        defaultValue = newDefaultValue;
    }

    int16_t getDefault(void) const
    {
        return (defaultValue);
    }

    void setMin(int16_t newMin)
    {
        min = newMin;
    }

    int16_t getMin(void) const
    {
        return (min);
    }

    void setMax(int16_t newMax)
    {
        max = newMax;
    }

    int16_t getMax(void) const
    {
        return (max);
    }

    uint8_t getOverlayId(void) const
    {
        return (overlayId);
    }

    Overlay *getOverlay(void) const
    {
        return (overlay);
    }

    uint16_t getNumSteps(void) const
    {
        return (max - min);
    }

    const char *getFunction(void) const
    {
        return (function.c_str());
    }

    std::string getFormatter(void) const
    {
        return formatter;
    }

    void callFormatter(int16_t value, char *buffer, size_t length) const
    {
        if (L != nullptr && !formatter.empty()) {
            runFormatter(formatter.c_str(), this, value, buffer, length);
        }
    }

    void callFunction(int16_t value) const
    {
        if (L != nullptr && !function.empty()) {
            runFunction(function.c_str(), this, value);
        }
    }

    static const char *translateId(uint8_t id)
    {
        if ((0 <= id) && (id < MAX_NUM_VALUE_IDS)) {
            return (valueIds[id]);
        }
        return ("value");
    }

    static uint8_t translateId(const char *handle)
    {
        for (uint8_t i = 0; i < MAX_NUM_VALUE_IDS; i++) {
            if (strcmp(handle, valueIds[i]) == 0) {
                return (i);
            }
        }
        return (0);
    }

    void print(void) const
    {
        logMessage("    id: %s", getId());
        logMessage("    handle: %d", getHandle());
        logMessage("    index: %d", getIndex());
        logMessage("    default: %d", getDefault());
        logMessage("    min: %d", getMin());
        logMessage("    max: %d", getMax());
        logMessage("    overlayId: %d", getOverlayId());
        logMessage("    function: %s", getFunction());
        logMessage("    formatter: %s", getFormatter());
        logMessage("    overlay: %x", getOverlay());
        logMessage("    control: %x", getControl());
        message.print();
    }

    /*
	 * attributes
	 */
private:
    uint8_t handle;
    uint8_t index;
    int16_t defaultValue;
    int16_t min;
    int16_t max;
    uint8_t overlayId;
    std::string formatter;
    std::string function;
    Control *control;
    Overlay *overlay;

public:
    Message message;
};
