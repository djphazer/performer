#pragma once

#include "BaseTrackPatternFollow.h"
#include "Config.h"
#include "Types.h"
#include "ArpSequence.h"
#include "Serialize.h"
#include "Routing.h"
#include "FileDefs.h"
#include "core/utils/StringUtils.h"
#include "BaseTrack.h"
#include "Arpeggiator.h"
#include <cstdint>


class ArpTrack : public BaseTrack, public BaseTrackPatternFollow {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef std::array<ArpSequence, CONFIG_PATTERN_COUNT + CONFIG_SNAPSHOT_COUNT> ArpSequenceArray;

    // FillMode

    enum class FillMode : uint8_t {
        None,
        Gates,
        NextPattern,
        Condition,
        Last
    };

    static const char *fillModeName(FillMode fillMode) {
        switch (fillMode) {
        case FillMode::None:        return "None";
        case FillMode::Gates:       return "Gates";
        case FillMode::NextPattern: return "Next Pattern";
        case FillMode::Condition:   return "Condition";
        case FillMode::Last:        break;
        }
        return nullptr;
    }

    // CvUpdateMode

    enum class CvUpdateMode : uint8_t {
        Gate,
        Always,
        Last
    };

    static const char *cvUpdateModeName(CvUpdateMode mode) {
        switch (mode) {
        case CvUpdateMode::Gate:    return "Gate";
        case CvUpdateMode::Always:  return "Always";
        case CvUpdateMode::Last:    break;
        }
        return nullptr;
    }

    //----------------------------------------
    // Properties
    //----------------------------------------

    const int trackIndex() const { return _trackIndex;}

    // playMode

    Types::PlayMode playMode() const { return _playMode; }
    void setPlayMode(Types::PlayMode playMode) {
        _playMode = ModelUtils::clampedEnum(playMode);
    }

    void editPlayMode(int value, bool shift) {
        setPlayMode(ModelUtils::adjustedEnum(playMode(), value));
    }

    void printPlayMode(StringBuilder &str) const {
        str(Types::playModeName(playMode()));
    }

    // fillMode

    FillMode fillMode() const { return _fillMode; }
    void setFillMode(FillMode fillMode) {
        _fillMode = ModelUtils::clampedEnum(fillMode);
    }

    void editFillMode(int value, bool shift) {
        setFillMode(ModelUtils::adjustedEnum(fillMode(), value));
    }

    void printFillMode(StringBuilder &str) const {
        str(fillModeName(fillMode()));
    }

    // fillMuted

    bool fillMuted() const { return _fillMuted; }
    void setFillMuted(bool fillMuted) {
        _fillMuted = fillMuted;
    }

    void editFillMuted(int value, bool shift) {
        setFillMuted(value > 0);
    }

    void printFillMuted(StringBuilder &str) const {
        ModelUtils::printYesNo(str, fillMuted());
    }

    // cvUpdateMode

    CvUpdateMode cvUpdateMode() const { return _cvUpdateMode; }
    void setCvUpdateMode(CvUpdateMode cvUpdateMode) {
        _cvUpdateMode = ModelUtils::clampedEnum(cvUpdateMode);
    }

    void editCvUpdateMode(int value, bool shift) {
        setCvUpdateMode(ModelUtils::adjustedEnum(cvUpdateMode(), value));
    }

    void printCvUpdateMode(StringBuilder &str) const {
        str(cvUpdateModeName(cvUpdateMode()));
    }

    // slideTime

    int slideTime() const { return _slideTime.get(isRouted(Routing::Target::SlideTime)); }
    void setSlideTime(int slideTime, bool routed = false) {
        _slideTime.set(clamp(slideTime, 0, 100), routed);
    }

    void editSlideTime(int value, bool shift) {
        if (!isRouted(Routing::Target::SlideTime)) {
            setSlideTime(ModelUtils::adjustedByStep(slideTime(), value, 5, !shift));
        }
    }

    void printSlideTime(StringBuilder &str) const {
        printRouted(str, Routing::Target::SlideTime);
        str("%d%%", slideTime());
    }

    // octave

    int octave() const { return _octave.get(isRouted(Routing::Target::Octave)); }
    void setOctave(int octave, bool routed = false) {
        _octave.set(clamp(octave, -10, 10), routed);
    }

    void editOctave(int value, bool shift) {
        if (!isRouted(Routing::Target::Octave)) {
            setOctave(octave() + value);
        }
    }

    void printOctave(StringBuilder &str) const {
        printRouted(str, Routing::Target::Octave);
        str("%+d", octave());
    }

    // transpose

    int transpose() const { return _transpose.get(isRouted(Routing::Target::Transpose)); }
    void setTranspose(int transpose, bool routed = false) {
        _transpose.set(clamp(transpose, -100, 100), routed);
    }

    void editTranspose(int value, bool shift) {
        if (!isRouted(Routing::Target::Transpose)) {
            setTranspose(transpose() + value);
        }
    }

    void printTranspose(StringBuilder &str) const {
        printRouted(str, Routing::Target::Transpose);
        str("%+d", transpose());
    }

    // gateProbabilityBias

    int gateProbabilityBias() const { return _gateProbabilityBias.get(isRouted(Routing::Target::GateProbabilityBias)); }
    void setGateProbabilityBias(int gateProbabilityBias, bool routed = false) {
        _gateProbabilityBias.set(clamp(gateProbabilityBias, -ArpSequence::GateProbability::Range, ArpSequence::GateProbability::Range), routed);
    }

    void editGateProbabilityBias(int value, bool shift) {
        if (!isRouted(Routing::Target::GateProbabilityBias)) {
            setGateProbabilityBias(gateProbabilityBias() + value);
        }
    }

    void printGateProbabilityBias(StringBuilder &str) const {
        printRouted(str, Routing::Target::GateProbabilityBias);
        str("%+.1f%%", gateProbabilityBias() * 12.5f);
    }

    // retriggerProbabilityBias

    int retriggerProbabilityBias() const { return _retriggerProbabilityBias.get(isRouted(Routing::Target::RetriggerProbabilityBias)); }
    void setRetriggerProbabilityBias(int retriggerProbabilityBias, bool routed = false) {
        _retriggerProbabilityBias.set(clamp(retriggerProbabilityBias, -ArpSequence::RetriggerProbability::Range, ArpSequence::RetriggerProbability::Range), routed);
    }

    void editRetriggerProbabilityBias(int value, bool shift) {
        if (!isRouted(Routing::Target::RetriggerProbabilityBias)) {
            setRetriggerProbabilityBias(retriggerProbabilityBias() + value);
        }
    }

    void printRetriggerProbabilityBias(StringBuilder &str) const {
        printRouted(str, Routing::Target::RetriggerProbabilityBias);
        str("%+.1f%%", retriggerProbabilityBias() * 12.5f);
    }

    // lengthBias

    int lengthBias() const { return _lengthBias.get(isRouted(Routing::Target::LengthBias)); }
    void setLengthBias(int lengthBias, bool routed = false) {
        _lengthBias.set(clamp(lengthBias, -ArpSequence::Length::Range, ArpSequence::Length::Range), routed);
    }

    void editLengthBias(int value, bool shift) {
        if (!isRouted(Routing::Target::LengthBias)) {
            setLengthBias(lengthBias() + value);
        }
    }

    void printLengthBias(StringBuilder &str) const {
        printRouted(str, Routing::Target::LengthBias);
        str("%+.1f%%", lengthBias() * 12.5f);
    }

    // noteProbabilityBias

    int noteProbabilityBias() const { return _noteProbabilityBias.get(isRouted(Routing::Target::NoteProbabilityBias)); }
    void setNoteProbabilityBias(int noteProbabilityBias, bool routed = false) {
        _noteProbabilityBias.set(clamp(noteProbabilityBias, -ArpSequence::NoteVariationProbability::Range, ArpSequence::NoteVariationProbability::Range), routed);
    }

    void editNoteProbabilityBias(int value, bool shift) {
        if (!isRouted(Routing::Target::NoteProbabilityBias)) {
            setNoteProbabilityBias(noteProbabilityBias() + value);
        }
    }

    void printNoteProbabilityBias(StringBuilder &str) const {
        printRouted(str, Routing::Target::NoteProbabilityBias);
        str("%+.1f%%", noteProbabilityBias() * 12.5f);
    }

    // midi keyboard
    const bool midiKeyboard() const {
        return _midiKeyboard;
    }

    void toggleMidiKeybaord() {
        _midiKeyboard = !_midiKeyboard;
    }

    // sequences

    const ArpSequenceArray &sequences() const { return _sequences; }
          ArpSequenceArray &sequences()       { return _sequences; }

    const ArpSequence &sequence(int index) const { return _sequences[index]; }
          ArpSequence &sequence(int index)       { return _sequences[index]; }

    void setSequence(int index, ArpSequence seq) {
        _sequences[index] = seq;
    }

    const Arpeggiator &arpeggiator() const { return _arpeggiator; }
    Arpeggiator &arpeggiator()       { return _arpeggiator; }

    //----------------------------------------
    // Routing
    //----------------------------------------

    inline bool isRouted(Routing::Target target) const { return Routing::isRouted(target, _trackIndex); }
    inline void printRouted(StringBuilder &str, Routing::Target target) const { Routing::printRouted(str, target, _trackIndex); }
    void writeRouted(Routing::Target target, int intValue, float floatValue);

    //----------------------------------------
    // Methods
    //----------------------------------------

    ArpTrack() { clear(); }

    void clear();

    void write(VersionedSerializedWriter &writer) const;
    void read(VersionedSerializedReader &reader);

private:
    void setTrackIndex(int trackIndex) {
        _trackIndex = trackIndex;
        for (auto &sequence : _sequences) {
            sequence.setTrackIndex(trackIndex);
        }
    }

    int8_t _trackIndex = -1;
    Types::PlayMode _playMode;
    FillMode _fillMode;
    bool _fillMuted;
    CvUpdateMode _cvUpdateMode;
    Routable<uint8_t> _slideTime;
    Routable<int8_t> _octave;
    Routable<int8_t> _transpose;
    Routable<int8_t> _gateProbabilityBias;
    Routable<int8_t> _retriggerProbabilityBias;
    Routable<int8_t> _lengthBias;
    Routable<int8_t> _noteProbabilityBias;

    ArpSequenceArray _sequences;

    Arpeggiator _arpeggiator;

    bool _midiKeyboard = false;

    friend class Track;
};
