#include "MidiLearn.h"

void MidiLearn::process(const MidiInput &midiInput,
                        const MidiMessage &midiMessage)
{
    Device device =
        preset.getDevice(midiInput.getPort(), midiMessage.getChannel());

    MidiMessage::Type midiType = midiMessage.getType();
    uint8_t midiChannel = midiMessage.getChannel();
    uint8_t midiPort = midiInput.getPort();

    Device midiLearnDevice(MidiLearnDeviceId, "ml", midiPort, midiChannel, 0);

    logMessage(
        "ElectraMidi::processMidiLearn: getting a device: port=%d, channel=%d",
        midiPort,
        midiChannel);

    if (midiType == MidiMessage::Type::ControlChange) {
        uint8_t midiParameterId = midiMessage.getData1();
        uint8_t midiValue = midiMessage.getData2();
        bool midiLearnSentAlready = false;

        // handle RPN / NRPN
        MidiRpnMessage midiRpnMessage;

        if (rpnDetector.parseControllerMessage(
                device.getId(), midiParameterId, midiValue, midiRpnMessage)) {
            logMessage(
                "ElectraMidi::processMidiLearn: RPN detected: parameter=%d, "
                "value=%d, isNrpn=%d, is14bit=%d",
                midiRpnMessage.parameterNumber,
                midiRpnMessage.value,
                midiRpnMessage.isNrpn,
                midiRpnMessage.is14BitValue);

            Message::Type messageType = (midiRpnMessage.isNrpn)
                                            ? Message::Type::nrpn
                                            : Message::Type::rpn;

            if (messageType == Message::Type::nrpn) {
                MidiOutput::sendMidiLearn(MidiInterface::Type::MidiUsbDev,
                                          USB_MIDI_PORT_CTRL,
                                          "nrpn",
                                          midiPort + 1,
                                          midiChannel,
                                          midiRpnMessage.parameterNumber,
                                          midiRpnMessage.value);
            } else {
                MidiOutput::sendMidiLearn(MidiInterface::Type::MidiUsbDev,
                                          USB_MIDI_PORT_CTRL,
                                          "rpn",
                                          midiPort + 1,
                                          midiChannel,
                                          midiRpnMessage.parameterNumber,
                                          midiRpnMessage.value);
            }
            midiLearnSentAlready = true;
        }

        // handle CC14
        MidiCc14Message midiCc14Message;

        if (cc14Detector.parseControllerMessage(
                device.getId(), midiParameterId, midiValue, midiCc14Message)) {
            logMessage(
                "ElectraMidi::processMidiLearn: CC14 detected: parameter=%d, value=%d",
                midiCc14Message.parameterNumber,
                midiCc14Message.value);
            MidiOutput::sendMidiLearn(MidiInterface::Type::MidiUsbDev,
                                      USB_MIDI_PORT_CTRL,
                                      "cc14",
                                      midiPort + 1,
                                      midiChannel,
                                      midiCc14Message.parameterNumber,
                                      midiCc14Message.value);
            midiLearnSentAlready = true;
        }

        if (midiLearnSentAlready != true) {
            MidiOutput::sendMidiLearn(MidiInterface::Type::MidiUsbDev,
                                      USB_MIDI_PORT_CTRL,
                                      "cc7",
                                      midiPort + 1,
                                      midiChannel,
                                      midiParameterId,
                                      midiValue);
        }
        return;
    } else if ((midiType == MidiMessage::Type::NoteOn)
               || (midiType == MidiMessage::Type::NoteOff)) {
        uint8_t midiNoteNr = midiMessage.getData1();
        uint8_t value = (midiType == MidiMessage::Type::NoteOn) ? 127 : 0;
        logMessage("ElectraMidi::processMidiLearn: note message: note=%d",
                   midiNoteNr);
        MidiOutput::sendMidiLearn(MidiInterface::Type::MidiUsbDev,
                                  USB_MIDI_PORT_CTRL,
                                  "note",
                                  midiPort + 1,
                                  midiChannel,
                                  midiNoteNr,
                                  value);
        return;
    } else if (midiType == MidiMessage::Type::ProgramChange) {
        uint8_t programNumber = midiMessage.getData1();
        logMessage("ElectraMidi::processMidiLearn: program message: program=%d",
                   programNumber);
        MidiOutput::sendMidiLearn(MidiInterface::Type::MidiUsbDev,
                                  USB_MIDI_PORT_CTRL,
                                  "program",
                                  midiPort + 1,
                                  midiChannel,
                                  programNumber,
                                  0);
        return;
    } else if (midiType == MidiMessage::Type::SystemExclusive) {
        MidiOutput::sendMidiLearnSysex(MidiInterface::Type::MidiUsbDev,
                                       USB_MIDI_PORT_CTRL,
                                       midiPort + 1,
                                       midiMessage.getSysExBlock());
    } else {
        logMessage("ElectraMidi::processMidiLearn: learn not supported");
    }
}
