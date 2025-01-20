#pragma once

#include "SystemConfig.h"
#include <bitset>

namespace MatrixMap {

    // index = col * 8 + row

    //                              ROW
    //          0     1     2     3     4     5     6     7
    //        -----------------------------------------------
    //     5 | F-0 | F-1 | F-2 | F-3 | F-4 |                 |
    //  C  4 | xxx | xxx | xxx | xxx | xxx | xxx | xxx | xxx |
    //  O  2 | T-0 | T-1 | T-2 | T-3 | T-4 | T-5 | T-6 | T-7 |
    //  L  1 | S-0 | S-1 | S-2 | S-3 | S-4 | S-5 | S-6 | S-7 |
    //     0 | S-8 | S-9 | S10 | S11 | S12 | S13 | S14 | S15 |
    //        -----------------------------------------------

    static constexpr int fromStep(int step) {
        return step < 8 ? (8 + step) : (step - 8);
    }

    static constexpr bool isStep(int index) {
        return index >= fromStep(8) && index <= fromStep(7);
    }

    static constexpr int toStep(int index) {
        return index >= fromStep(0) ? (index - fromStep(0)) : (index - fromStep(8) + 8);
    }

    static constexpr int fromTrack(int track) {
        return 16 + track;
    }

    static constexpr bool isTrack(int index) {
        return index >= fromTrack(0) && index <= fromTrack(7);
    }

    static constexpr int toTrack(int index) {
        return index - fromTrack(0);
    }

    static constexpr int fromFunction(int function) {
        return 32 + function;
    }

    static constexpr int isFunction(int index) {
        return index >= fromFunction(0) && index <= fromFunction(4);
    }

    static constexpr int toFunction(int index) {
        return index - fromFunction(0);
    }

} // namespace MatrixMap

using KeyState = std::bitset<CONFIG_BLM_ROWS * CONFIG_BLM_COLS_BUTTON + 1>;

class Key {
public:
    // key codes
    enum Code {
        None = -1,
        Track0 = MatrixMap::fromTrack(0),
        Track1 = MatrixMap::fromTrack(1),
        Track2 = MatrixMap::fromTrack(2),
        Track3 = MatrixMap::fromTrack(3),
        Track4 = MatrixMap::fromTrack(4),
        Track5 = MatrixMap::fromTrack(5),
        Track6 = MatrixMap::fromTrack(6),
        Track7 = MatrixMap::fromTrack(7),
        Step0 = MatrixMap::fromStep(0),
        Step1 = MatrixMap::fromStep(1),
        Step2 = MatrixMap::fromStep(2),
        Step3 = MatrixMap::fromStep(3),
        Step4 = MatrixMap::fromStep(4),
        Step5 = MatrixMap::fromStep(5),
        Step6 = MatrixMap::fromStep(6),
        Step7 = MatrixMap::fromStep(7),
        Step8 = MatrixMap::fromStep(8),
        Step9 = MatrixMap::fromStep(9),
        Step10 = MatrixMap::fromStep(10),
        Step11 = MatrixMap::fromStep(11),
        Step12 = MatrixMap::fromStep(12),
        Step13 = MatrixMap::fromStep(13),
        Step14 = MatrixMap::fromStep(14),
        Step15 = MatrixMap::fromStep(15),
        Global0 = 24,
        Global1 = 25,
        Global2 = 26,
        Global3 = 27,
        Global4 = 28,
        Global5 = 29,
        Global6 = 30,
        Global7 = 31,
        Play = Global0,
        Tempo = Global1,
        Pattern = Global2,
        Performer = Global3,
        Left = Global4,
        Right = Global5,
        Shift = Global6,
        Page = Global7,
        Home = Global7,
        F0 = MatrixMap::fromFunction(0),
        F1 = MatrixMap::fromFunction(1),
        F2 = MatrixMap::fromFunction(2),
        F3 = MatrixMap::fromFunction(3),
        F4 = MatrixMap::fromFunction(4),
        Encoder = 40,
    };

    // page select key aliases
    enum PageKey {
        Clock           = Tempo,
        //Pattern         = Key::Pattern,
        //Performer       = Key::Performer,
        Overview        = Left,

        Project         = Track0,
        Layout          = Track1,
        Routing         = Track2,
        MidiOutput      = Track3,
        UserScale       = Track4,

        SequenceEdit    = Step0,
        Sequence        = Step1,
        Track           = Step2,
        Song            = Step3,

        System          = Track7,
        Monitor         = Step7,
    };

    static bool isPageKey(int code) {
        switch (code) {
        case Clock:
        case Pattern:
        case Performer:
        case Overview:

        case Project:
        case Layout:
        case Routing:
        case MidiOutput:
        case UserScale:

        case SequenceEdit:
        case Sequence:
        case Track:
        case Song:

        case System:
        case Monitor:
            return true;
        default:
            break;
        }

        return false;
    }

    Key(int code, const KeyState &state) : _code(code), _state(state) {}

    int code() const { return _code; }

    const KeyState &state() const { return _state; }
    bool state(int code) const { return _state[code]; }

    bool shiftModifier() const { return _state[Shift]; }
    bool pageModifier() const { return _state[Page]; }

    bool is(int code) const { return _code == code; }

    // individual buttons

    bool isPlay() const { return _code == Play; }
    bool isTempo() const { return _code == Tempo; }
    bool isPattern() const { return _code == Pattern; }
    bool isPerformer() const { return _code == Performer; }
    bool isLeft() const { return _code == Left; }
    bool isRight() const { return _code == Right; }
    bool isPage() const { return _code == Page; };
    bool isHome() const { return _code == Home; };
    bool isShift() const { return _code == Shift; }

    // button groups

    bool isTrack() const { return MatrixMap::isTrack(_code); }
    int track() const { return MatrixMap::toTrack(_code); }

    bool isStep() const { return MatrixMap::isStep(_code); }
    int step() const { return MatrixMap::toStep(_code); }

    bool isFunction() const { return MatrixMap::isFunction(_code); }
    int function() const { return MatrixMap::toFunction(_code); }

    // combined buttons

    bool isTrackSelect() const { return !pageModifier() && isTrack(); }
    int trackSelect() const { return track(); }

    bool isPageSelect() const { return pageModifier() && isPageKey(_code); }
    int pageSelect() const {
        if (isTrack()) {
            return track();
        } else if (isStep()) {
            return step() + 8;
        }
        return -1;
    }

    bool isQuickEdit() const { return pageModifier() && isStep() && step() >= 8; }
    int quickEdit() const { return step() - 8; }

    bool isContextMenu() const { return shiftModifier() && isShift(); }

    bool isEncoder() const { return is(Encoder); }

    bool isGlobal() const { return isPlay() || isTempo(); }

private:
    int _code;
    const KeyState &_state;
};
