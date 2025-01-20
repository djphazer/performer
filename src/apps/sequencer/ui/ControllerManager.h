#pragma once

#include "Controller.h"
#include "controllers/launchpad/LaunchpadController.h"

#include "model/Model.h"

#include "engine/Engine.h"
#include "engine/MidiPort.h"

#include "core/midi/MidiMessage.h"
#include "core/utils/Container.h"

    static const ControllerInfo controllerInfos[] = {
        { 0x1235, 0x0020, ControllerInfo::Type::Launchpad },    // Novation Launchpad S
        { 0x1235, 0x0036, ControllerInfo::Type::Launchpad },    // Novation Launchpad Mini Mk1
        { 0x1235, 0x0037, ControllerInfo::Type::Launchpad },    // Novation Launchpad Mini Mk2
        { 0x1235, 0x0069, ControllerInfo::Type::Launchpad },    // Novation Launchpad Mk2
        { 0x1235, 0x0051, ControllerInfo::Type::Launchpad },    // Novation Launchpad Pro
        { 0x1235, 0x0113, ControllerInfo::Type::Launchpad },    // Novation Launchpad Mini Mk3
        { 0x1235, 0x0103, ControllerInfo::Type::Launchpad },    // Novation Launchpad X
        { 0x1235, 0x0123, ControllerInfo::Type::Launchpad },    // Novation Launchpad Pro Mk3
    };

class ControllerManager {



public:
    ControllerManager(Model &model, Engine &engine);

    void connect(uint16_t vendorId, uint16_t productId);
    void disconnect();

    bool isConnected() { return _controller != nullptr; }

    void update();

    int fps() const { return 50; }

    bool recvMidi(MidiPort port, uint8_t cable, const MidiMessage &message);

    static const ControllerInfo *findController(uint16_t vendorId, uint16_t productId) {
        for (size_t i = 0; i < sizeof(controllerInfos) / sizeof(controllerInfos[0]); ++i) {
            auto info = &controllerInfos[i];
            if (info->vendorId == vendorId && info->productId == productId) {
                return info;
            }
        }
        return nullptr;
    }

private:
    bool sendMidi(uint8_t cable, const MidiMessage &message);

    Model &_model;
    Engine &_engine;
    MidiPort _port;
    Container<LaunchpadController> _controllerContainer;
    Controller *_controller = nullptr;

    friend class Controller;
};
