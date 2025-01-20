#pragma once

#include "LaunchpadDevice.h"

#include "core/midi/MidiMessage.h"

// Compatible with Launchpad Mini Mk3 and Launchpad X
class LaunchpadMk3Device : public LaunchpadDevice {
public:
    LaunchpadMk3Device();

    void initialize() override;

    void recvMidi(uint8_t cable, const MidiMessage &message) override;

    void setLed(int row, int col, Color color, int style) override {
        _ledState[row * Cols + col] = mapColor(color.red, color.green, style);
    }

    void setLed(int row, int col, int red, int green, int style) override {
        _ledState[row * Cols + col] = mapColor(red, green, style);;
    }

    void setCustomLed(int row, int col, Color color, int style = 0) override{
        _ledState[row * Cols + col] = mapCustomColor(color.red, color.green, style);
    }

    void syncLeds() override;

private:
    static constexpr uint8_t Cable = 1;

    inline uint8_t mapColor(int red, int green, int style) const {
        static const uint8_t map[] = {
        //  g0 g1 g2 g3
            0, 23, 25, 21, // r0
            7, 2, 18, 21, // r1
            6, 10, 14, 17, // r2
            5,  5,  9, 13, // r3
        };

        static const uint8_t mapBlue[] = {
        //  g0 g1 g2 g3
            0, 43, 78, 79, // r0
            71, 2, 39, 79, // r1
            2, 118, 55, 41, // r2
            3,  3,  2, 82, // r3
        };
        if (style == 0) {
            return map[(red & 0x3) * 4 + (green & 0x3)];
        } else {
            return mapBlue[(red & 0x3) * 4 + (green & 0x3)];
        }     
    }

    inline uint8_t mapCustomColor(int x, int y, int style) const {
        static const uint8_t map[] = {
        //  g0 g1 g2 g3
            0, 1, 2, 3, // r0
            32, 36, 40, 44, // r1
            48, 52, 56, 60, // r2
            5,  5,  9, 3, // r3
        };
        return map[(x & 0x3) * 4 + (y & 0x3)];
    }
    
};


