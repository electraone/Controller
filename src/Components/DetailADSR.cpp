/*
* Electra One MIDI Controller Firmware
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Electra One Project (http://electra.one/).
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.
*/

#include "DetailADSR.h"

DetailADSR::DetailADSR(const Control &control, MainDelegate &newDelegate)
    : Detail(control, newDelegate),
      attackKnob(nullptr),
      decayKnob(nullptr),
      sustainKnob(nullptr),
      releaseKnob(nullptr)
{
    // \todo to be deduplicated
    attackValues.push_back(control.getValue(0));
    attack.setId(500);
    attack.setType(Control::Type::Knob);
    attack.setName("ATTACK");
    attack.setColour(control.getColour565());
    attack.setValues(attackValues);
    attackValues[0].setControl(&attack);
    attack.addToParameterMap(attackValues[0]);
    attackKnob = ControlComponent::createControlComponent(attack, delegate);
    attackKnob->updateValueFromParameterMap();
    attackKnob->assignPot(1, attack.values[0].getNumSteps());
    addAndMakeVisible(attackKnob);

    decayValues.push_back(control.getValue(1));
    decay.setId(501);
    decay.setType(Control::Type::Knob);
    decay.setName("DECAY");
    decay.setColour(control.getColour565());
    decay.setValues(decayValues);
    decayValues[0].setControl(&decay);
    decay.addToParameterMap(decayValues[0]);
    decayKnob = ControlComponent::createControlComponent(decay, delegate);
    decayKnob->updateValueFromParameterMap();
    decayKnob->assignPot(2, decay.values[0].getNumSteps());
    addAndMakeVisible(decayKnob);

    sustainValues.push_back(control.getValue(2));
    sustain.setId(502);
    sustain.setType(Control::Type::Knob);
    sustain.setName("SUSTAIN");
    sustain.setColour(control.getColour565());
    sustain.setValues(sustainValues);
    sustainValues[0].setControl(&sustain);
    sustain.addToParameterMap(sustainValues[0]);
    sustainKnob = ControlComponent::createControlComponent(sustain, delegate);
    sustainKnob->updateValueFromParameterMap();
    sustainKnob->assignPot(3, sustain.values[0].getNumSteps());
    addAndMakeVisible(sustainKnob);

    releaseValues.push_back(control.getValue(3));
    release.setId(503);
    release.setType(Control::Type::Knob);
    release.setName("RELEASE");
    release.setColour(control.getColour565());
    release.setValues(releaseValues);
    releaseValues[0].setControl(&release);
    release.addToParameterMap(releaseValues[0]);
    releaseKnob = ControlComponent::createControlComponent(release, delegate);
    releaseKnob->updateValueFromParameterMap();
    releaseKnob->assignPot(4, release.values[0].getNumSteps());
    addAndMakeVisible(releaseKnob);
    Detail::setLocked();
}

DetailADSR::~DetailADSR()
{
    attack.removeFromParameterMap(attackValues[0]);
    decay.removeFromParameterMap(decayValues[0]);
    sustain.removeFromParameterMap(sustainValues[0]);
    release.removeFromParameterMap(releaseValues[0]);
    attackKnob->releasePot();
    decayKnob->releasePot();
    sustainKnob->releasePot();
    releaseKnob->releasePot();
}

void DetailADSR::paint(Graphics &g)
{
    g.setColour(Colours565::black);
    g.fillRect(0, 0, getWidth() - 120, getHeight());
    g.setColour(Colours565::darkgrey);
    g.drawRect(0, 0, getWidth() - 120, getHeight());
    g.dim(getWidth() - 120, 0, 120, getHeight(), Colours565::black);
}

void DetailADSR::resized()
{
    component->setBounds(50, 50, 608, 150);
    lock->setBounds(getWidth() - 100, 25, 75, 75);
    dimmedSection->setBounds(getWidth() - 120, 100, 120, getHeight() - 100);
    attackKnob->setBounds(63, getHeight() - 130, 75, 100);
    decayKnob->setBounds(228, getHeight() - 130, 75, 100);
    sustainKnob->setBounds(394, getHeight() - 130, 75, 100);
    releaseKnob->setBounds(560, getHeight() - 130, 80, 100);
}
