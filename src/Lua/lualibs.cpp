#include "luabridge.h"
#include "lualibs.h"
#include "luaParameterMap.h"
#include "luaValue.h"
#include "luaHelpers.h"
#include "luaGroup.h"
#include "luaPage.h"
#include "luaDevice.h"
#include "luaControl.h"
#include "luaMessage.h"
#include "luaPreset.h"

void loadLuaLibs(void)
{
    static const luaL_Reg ctrlv2libs[] = { { "parameterMap",
                                             luaopen_parameterMap },
                                           { "pages", luaopen_pages },
                                           { "groups", luaopen_groups },
                                           { "devices", luaopen_devices },
                                           { "controls", luaopen_controls },
                                           { "helpers", luaopen_helpers },
                                           { "window", luaopen_window },
                                           { "patch", luaopen_patch },
                                           { "preset", luaopen_preset },
                                           { "info", luaopen_info },
                                           { NULL, NULL } };

    luaLE_openEoslibs(L, ctrlv2libs);
    value_register(L);
    page_register(L);
    group_register(L);
    device_register(L);
    control_register(L);
    message_register(L);

    // Clear the stack
    lua_settop(L, 0);
}

void luaLE_pushDevice(const Device &device)
{
    lua_newtable(L);
    luaLE_pushTableInteger(L, "id", device.getId());
    luaLE_pushTableInteger(L, "port", device.getPort());
    luaLE_pushTableInteger(L, "channel", device.getChannel());
}

bool luaLE_checkBoolean(lua_State *L, int idx)
{
    return (lua_toboolean(L, idx));
}

int luaLE_checkDeviceId(lua_State *L, int idx)
{
    int deviceId = luaL_checkinteger(L, idx);

    if ((deviceId < 1) || (deviceId > 32)) {
        return (luaL_error(L, "invalid deviceId: %d", deviceId));
    }
    return (deviceId);
}

int luaLE_checkInterface(lua_State *L, int idx)
{
    int interface = luaL_checkinteger(L, idx);

    if ((interface < 0) || (interface > 2)) {
        return (luaL_error(L, "invalid interface: %d", interface));
    }
    return (interface);
}

int luaLE_checkPort(lua_State *L, int idx)
{
    int port = luaL_checkinteger(L, idx);

    if ((port < 0) || (port > 2)) {
        return (luaL_error(L, "invalid port: %d", port));
    }
    return (port);
}

int luaLE_checkChannel(lua_State *L, int idx)
{
    int channel = luaL_checkinteger(L, idx);

    if ((channel < 1) || (channel > 16)) {
        return (luaL_error(L, "invalid channel: %d", channel));
    }
    return (channel);
}

int luaLE_checkParameterType(lua_State *L, int idx)
{
    int type = luaL_checkinteger(L, idx);

    if ((type < 0) || (type > 11)) {
        return (luaL_error(L, "invalid parameter type: %d", type));
    }
    return (type);
}

int luaLE_checkParameterNumber(lua_State *L, int idx)
{
    int parameterNumber = luaL_checkinteger(L, idx);

    if ((parameterNumber < 0) || (parameterNumber > 16383)) {
        return (luaL_error(L, "invalid parameterNumber: %d", parameterNumber));
    }
    return (parameterNumber);
}

int luaLE_checkMidiValue(lua_State *L, int idx)
{
    int midiValue = luaL_checkinteger(L, idx);

    if ((midiValue < 0) || (midiValue > 16383)) {
        return (luaL_error(L, "invalid midiValue: %d", midiValue));
    }
    return (midiValue);
}

int luaLE_checkControlId(lua_State *L, int idx)
{
    int controlId = luaL_checkinteger(L, idx);

    if ((controlId < 1) || (controlId > 864)) {
        return (luaL_error(L, "invalid controlId: %d", controlId));
    }
    return (controlId);
}

int luaLE_checkDevicelId(lua_State *L, int idx)
{
    int deviceId = luaL_checkinteger(L, idx);

    if ((deviceId < 1) || (deviceId > 32)) {
        return (luaL_error(L, "invalid deviceId: %d", deviceId));
    }
    return (deviceId);
}

int luaLE_checkPageId(lua_State *L, int idx)
{
    int pageId = luaL_checkinteger(L, idx);

    if ((pageId < 1) || (pageId > 12)) {
        return (luaL_error(L, "invalid pageId: %d", pageId));
    }
    return (pageId);
}

int luaLE_checkControlSetId(lua_State *L, int idx)
{
    int controlSetId = luaL_checkinteger(L, idx);

    if ((controlSetId < 1) || (controlSetId > 3)) {
        return (luaL_error(L, "invalid controlSetId: %d", controlSetId));
    }
    return (controlSetId);
}

int luaLE_checkGroupId(lua_State *L, int idx)
{
    int groupId = luaL_checkinteger(L, idx);

    if ((groupId < 1) || (groupId > 864)) {
        return (luaL_error(L, "invalid groupId: %d", groupId));
    }
    return (groupId);
}

int luaLE_checkValueIndex(lua_State *L, int idx)
{
    int index = luaL_checkinteger(L, idx);

    return (index);
}
