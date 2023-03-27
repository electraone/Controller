#include "Presets.h"
#include "MidiOutput.h"
#include "MidiCallbacks.h"
#include "lualibs.h"
#include "luabridge.h"
#include "luaPreset.h"

Preset *luaPreset = nullptr;

Presets::Presets(const char *newAppSandbox,
                 const bool &shouldKeepPresetState,
                 const bool &shouldLoadPresetStateOnStartup)
    : appSandbox(newAppSandbox),
      currentSlot(0),
      currentBankNumber(0),
      readyForPresetSwitch(true),
      keepPresetState(shouldKeepPresetState),
      loadPresetStateOnStartup(shouldLoadPresetStateOnStartup)
{
}

void Presets::assignPresetNames(void)
{
    for (uint16_t i = 0; i < NumSlots; i++) {
        char filename[MAX_FILENAME_LENGTH + 1];
        snprintf(filename, MAX_FILENAME_LENGTH, "%s/p%03d.epr", appSandbox, i);

        if (File file = Hardware::sdcard.createInputStream(filename)) {
            char presetName[Preset::MaxNameLength + 1];
            Preset::getPresetName(file, presetName, Preset::MaxNameLength);
            presetSlot[i].setPresetName(presetName);

            char projectId[Preset::MaxProjectIdLength + 1];
            Preset::getPresetProjectId(
                file, projectId, Preset::MaxProjectIdLength);
            presetSlot[i].setProjectId(projectId);
            System::logger.write(LOG_ERROR,
                                 "setting a preset name: %s, id=%d",
                                 presetSlot[i].getPresetName(),
                                 i);
            file.close();
        }
    }
}

void Presets::sendList(uint8_t port)
{
    char filenameList[MAX_FILENAME_LENGTH + 1];
    snprintf(filenameList,
             MAX_FILENAME_LENGTH,
             "%s/%08ld.tmp",
             appSandbox,
             millis());

    System::sysExBusy = true;

    File presetListFile = Hardware::sdcard.createOutputStream(
        filenameList, FILE_WRITE | O_CREAT | O_TRUNC);

    if (!presetListFile) {
        System::logger.write(LOG_ERROR,
                             "Presets::sendList: cannot open transfer file: %s",
                             filenameList);
        System::sysExBusy = false;
        return;
    }

    bool firstRecord = true;
    char buf[256]; // TODO: fix buffer overflow issue

    presetListFile.print("{\"version\":1,\"presets\":[");

    for (uint8_t i = 0; i < 72; i++) {
        if (strlen(presetSlot[i].getPresetName()) > 0) {
            sprintf(
                buf,
                "%s{\"slot\":%d,\"bankNumber\":%d,\"name\":\"%s\",\"projectId\":\"%s\"}",
                (firstRecord) ? "" : ",",
                i % 12,
                i / 12,
                presetSlot[i].getPresetName(),
                presetSlot[i].getProjectId());
            presetListFile.write(buf, strlen(buf));
            firstRecord = false;
        }
    }

    presetListFile.print("]}");
    presetListFile.close();

    MidiOutput::sendSysExFile(
        port, filenameList, ElectraCommand::Object::PresetList);

    if (!Hardware::sdcard.deleteFile(filenameList)) {
        System::logger.write(
            LOG_ERROR,
            "Presets::sendList: cannot remove temporary file: %s",
            filenameList);
    }
    System::sysExBusy = false;
}

/** Load preset.
 *  Read preset file and initialize all data structures to display and run the preset.
 */
bool Presets::loadPreset(LocalFile file)
{
    const char *presetFile = file.getFilepath();

    // clear all entries in the frame buffer
    parameterMap.disable();
    System::tasks.disableRepaintGraphics();
    System::tasks.clearRepaintGraphics();

    // Free current preset
    reset();

    if (Hardware::sdcard.exists(presetFile)) {
        if (preset.load(presetFile) == true) {
            System::logger.write(
                LOG_INFO, "Default preset loaded: filename=%s", presetFile);
        }

        // Display the preset if valid.
        if (preset.isValid()) {
            // Initialise the parameterMap
            for (auto &[id, control] : preset.controls) {
                for (auto &value : control.values) {
                    control.setDefaultValue(value, false);
                }
            }

            parameterMap.setProjectId(preset.getProjectId());
            parameterMap.enable();
            uint8_t presetId = (currentBankNumber * NumBanks) + currentSlot;

            if (!loadPresetStateOnStartup
                && !presetSlot[presetId].hasBeenAlreadyLoaded()) {
                parameterMap.forget();
            }
            if (keepPresetState) {
                parameterMap.recall();
            }

            // mark reset as loaded in this session
            presetSlot[presetId].setAlreadyLoaded(true);
        } else {
            System::logger.write(LOG_ERROR,
                                 "Presets::loadPreset: Invalid preset: file=%s",
                                 presetFile);
            preset.reset();
        }
    }
    System::tasks.enableRepaintGraphics();
    return (true);
}

/** Mark preset slot
 *  Mark given preset slot as unused.
 */
void Presets::removePreset(uint8_t slotId)
{
    presetSlot[slotId].clear();
}

/** Reset preset.
 *  Re-initialize preset so that it is completely empty and ready for loading a new preset.
 */
void Presets::reset(void)
{
    // Disable the ParameterMap sync
    parameterMap.disable();
    if (keepPresetState) {
        parameterMap.keep();
    }

    // trigger Lua onLoad function
    if (L) {
        preset_onExit();
    }

    // Reset Lua callbacks
    resetMidiCallbacks();

    // Reset Lua
    closeLua();

    // Reset preset
    preset.reset();

    // Reset parameterMap
    parameterMap.reset();

    System::logger.write(LOG_TRACE,
                         "Controller::reset: preset memory deallocated");
    monitorFreeMemory();
}

/** Load preset identified with a preset Id
 *
 */
bool Presets::loadPresetById(uint8_t presetId)
{
    if (!readyForPresetSwitch) {
        System::logger.write(
            LOG_ERROR,
            "Controller::loadPresetById: still busy with swicthing previous preset");
        return (false);
    } else {
        readyForPresetSwitch = false;
    }

    if (presetId > 71) {
        presetId = 0;
    }

    setBankNumberAndSlot(presetId);
    LocalFile file(System::context.getCurrentPresetFile());

    // Remember the preset for the next startup
    System::runtimeInfo.setLastActivePreset(presetId);

    // Try to load the preset
    if (loadPreset(file)) {
        System::logger.write(LOG_INFO,
                             "loadPresetById: preset loaded: name='%s'",
                             preset.getName());

        // Re-set Lua state and execute
        runPresetLuaScript();
    } else {
        System::logger.write(LOG_ERROR,
                             "loadPresetById: preset loading failed: id=%d",
                             presetId);
    }

    readyForPresetSwitch = true;

    return (true);
}

/** Run lua script
 *
 */
void Presets::runPresetLuaScript(void)
{
    closeLua();

    luaPreset = &preset;

    if (isLuaValid(System::context.getCurrentLuaFile())) {
        initLua();
        loadLuaLibs();

        executeElectraLua(System::context.getCurrentLuaFile());

        // assign Lua callbacks
        assignLuaCallbacks();

        // trigger Lua onLoad function
        preset_onLoad();
    }
}

void Presets::setBankNumberAndSlot(uint8_t presetId)
{
    currentBankNumber = presetId / NumPresetsInBank;
    currentSlot = presetId % NumPresetsInBank;
    setDefaultFiles(currentBankNumber, currentSlot);
}

void Presets::setBankNumberAndSlot(uint8_t newBankNumber, uint8_t newSlot)
{
    currentBankNumber = newBankNumber;
    currentSlot = newSlot;
    setDefaultFiles(currentBankNumber, currentSlot);
}

uint8_t Presets::getPresetId(void) const
{
    return (currentBankNumber * NumPresetsInBank + currentSlot);
}

void Presets::setCurrentSlot(uint8_t newSlot)
{
    currentSlot = newSlot;
    setDefaultFiles(currentBankNumber, currentSlot);
}

uint8_t Presets::getCurrentSlot(void) const
{
    return (currentSlot);
}

void Presets::setCurrentBankNumber(uint8_t newBankNumber)
{
    currentBankNumber = newBankNumber;
    setDefaultFiles(currentBankNumber, currentSlot);
}

uint8_t Presets::getCurrentBankNumber(void) const
{
    return (currentBankNumber);
}

void Presets::setDefaultFiles(uint8_t newBankNumber, uint8_t newSlot)
{
    System::context.setCurrentFile(newBankNumber * NumPresetsInBank + newSlot);
}

const char *Presets::getPresetName(uint8_t slotId) const
{
    return presetSlot[slotId].getPresetName();
}
