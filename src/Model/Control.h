#pragma once

#include <vector>
#include <map>

#include "ControlValue.h"
#include "Input.h"
#include "Rectangle.h"
#include "Colours.h"
#include "Component.h"

extern const char *valueIdsAdsr[];
extern const char *valueIdsAdr[];
extern const char *valueIdsDx7Env[];

class Control
{
public:
    enum class Type {
        None = 0,
        Fader = 1,
        Vfader = 2,
        List = 3,
        Pad = 4,
        Adsr = 5,
        Adr = 6,
        Dx7envelope = 7,
        Knob = 8,
        Relative = 9
    };

    enum class Mode {
        Default = 0,
        Momentary = 1,
        Toggle = 2,
        Unipolar = 3,
        Bipolar = 4
    };

    enum class Variant { Default = 0, Thin = 2, ValueOnly = 3, Dial = 4 };

    Control();
    Control(uint16_t id,
            uint8_t pageId,
            const char *name,
            const Rectangle &bounds,
            Type type,
            Mode mode,
            uint32_t colour,
            uint8_t controlSetId,
            Variant variant,
            bool visible);

    virtual ~Control() = default;

    bool isValid(void) const;
    void setId(uint16_t newId);
    uint16_t getId(void) const;
    uint8_t getPageId(void) const;
    void setType(Type newType);
    Type getType(void) const;
    Mode getMode(void) const;
    void setColour(uint32_t newColour);
    uint32_t getColour(void) const;
    uint16_t getColour565(void) const;
    void setName(const char *newName);
    const char *getName(void) const;
    void setControlSetId(uint8_t newControlSetId);
    uint8_t getControlSetId(void) const;
    Variant getVariant(void) const;
    void setVisible(bool shouldBeVisible);
    bool isVisible(void) const;
    Rectangle getBounds(void) const;
    void setBounds(const Rectangle &bounds);
    void setValues(std::vector<ControlValue> values);
    ControlValue &getValue(uint8_t index);
    const ControlValue &getValue(uint8_t index) const;
    const ControlValue &getValueByValueId(const char *valueId) const;

    static Type translateType(const char *typeText);
    static Mode translateControlMode(const char *modeText);
    static Variant translateVariant(const char *variantText);

    const char *translateValueId(uint8_t id) const;
    uint8_t translateValueId(const char *handle) const;

    void setComponent(Component *newComponent);
    void resetComponent(void);
    Component *getComponent(void) const;
    void addToParameterMap(ControlValue &value);
    void removeFromParameterMap(ControlValue &value);
    void setDefaultValue(ControlValue &value, bool sendMidiMessages);

    void print(uint8_t logLevel = LOG_ERROR) const;
    void printValues(void) const;
    void printInputs(void) const;

    static uint8_t translateValueToId(Control::Type controlType,
                                      const char *handle);
    static uint8_t constraintValueId(Control::Type controlType,
                                     uint8_t handleId);

private:
    static const int MaxNameLength = 15;

    struct {
        uint16_t id : 10;
        uint8_t pageId : 4;
        uint8_t type : 4;
        uint8_t mode : 3;
        uint8_t controlSetId : 3;
        uint8_t variant : 4;
        uint8_t visible : 1;
        uint32_t colour : 24;
    };

    Rectangle bounds;
    char name[MaxNameLength + 1];

    Component *component;

public:
    std::vector<ControlValue> values;
    std::vector<Input> inputs;
};

typedef std::map<uint16_t, Control> Controls;
