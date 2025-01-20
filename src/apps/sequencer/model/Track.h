#pragma once

#include "Config.h"
#include "Types.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "NoteTrack.h"
#include "CurveTrack.h"
#include "MidiCvTrack.h"
#include "StochasticTrack.h"
#include "LogicTrack.h"
#include "ArpTrack.h"

#include "core/Debug.h"
#include "core/math/Math.h"
#include "core/utils/StringUtils.h"
#include "core/utils/Container.h"

#include <cstdint>
#include <cstring>

#if CONFIG_ENABLE_SANITIZE
# define SANITIZE_TRACK_MODE(_actual_, _expected_) ASSERT(_actual_ == _expected_, "invalid track mode");
#else // CONFIG_ENABLE_SANITIZE
# define SANITIZE_TRACK_MODE(_actual_, _expected_) {}
#endif // CONFIG_ENABLE_SANITIZE

class Project;

class Track {
public:
    //----------------------------------------
    // Types
    //----------------------------------------
    static constexpr size_t NameLength = FileHeader::NameLength; 

    enum class TrackMode : uint8_t {
        Note,
        Curve,
        MidiCv,
        Stochastic,
        Arp,
        Logic,
        Last,
        Default = Note
    };

    static const char *trackModeName(TrackMode trackMode) {
        switch (trackMode) {
        case TrackMode::Note:   return "Note";
        case TrackMode::Curve:  return "Curve";
        case TrackMode::MidiCv: return "MIDI/CV";
        case TrackMode::Stochastic: return "Stochastic";
        case TrackMode::Logic:  return "Logic";
        case TrackMode::Arp:    return "Arp";
        case TrackMode::Last:   break;
        }
        return nullptr;
    }

    static uint8_t trackModeSerialize(TrackMode trackMode) {
        switch (trackMode) {
        case TrackMode::Note:   return 0;
        case TrackMode::Curve:  return 1;
        case TrackMode::MidiCv: return 2;
        case TrackMode::Stochastic: return 3;
        case TrackMode::Logic:  return 4;
        case TrackMode::Arp:   return 5;
        case TrackMode::Last:   break;
        }
        return 0;
    }


    //----------------------------------------
    // Properties
    //----------------------------------------

    // trackIndex

    int trackIndex() const { return _trackIndex; }

    // trackMode

    TrackMode trackMode() const { return _trackMode; }

    void printTrackMode(StringBuilder &str) const {
        str(trackModeName(trackMode()));
    }

    // linkTrack

    int linkTrack() const { return _linkTrack; }
    void setLinkTrack(int linkTrack) {
        _linkTrack = clamp(linkTrack, -1, _trackIndex - 1);
    }

    void editLinkTrack(int value, bool shift) {
        setLinkTrack(linkTrack() + value);
    }

    void printLinkTrack(StringBuilder &str) const {
        if (linkTrack() == -1) {
            str("None");
        } else {
            str("Track%d", linkTrack() + 1);
        }
    }

    // noteTrack

    const NoteTrack &noteTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Note); return *_track.note; }
          NoteTrack &noteTrack()       { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Note); return *_track.note; }

    // curveTrack

    const CurveTrack &curveTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Curve); return *_track.curve; }
          CurveTrack &curveTrack()       { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Curve); return *_track.curve; }

    // midiCvTrack

    const MidiCvTrack &midiCvTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::MidiCv); return *_track.midiCv; }
          MidiCvTrack &midiCvTrack()       { SANITIZE_TRACK_MODE(_trackMode, TrackMode::MidiCv); return *_track.midiCv; }

    const StochasticTrack &stochasticTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Stochastic); return *_track.stochastic; }
          StochasticTrack &stochasticTrack()       { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Stochastic); return *_track.stochastic; }

    const LogicTrack &logicTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Logic); return *_track.logic; }
          LogicTrack &logicTrack()       { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Logic); return *_track.logic; }

    const ArpTrack &arpTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Arp); return *_track.arp; }
          ArpTrack &arpTrack()       { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Arp); return *_track.arp; }          

    //----------------------------------------
    // Methods
    //----------------------------------------

    Track() {
        initContainer();
    }

    void clear();
    void clearPattern(int patternIndex);
    void copyPattern(int src, int dst);
    bool duplicatePattern(int patternIndex);

    void gateOutputName(int index, StringBuilder &str) const;
    void cvOutputName(int index, StringBuilder &str) const;

    void write(VersionedSerializedWriter &writer) const;
    void read(VersionedSerializedReader &reader);

    Track &operator=(const Track &other) {
        ASSERT(_trackMode == other._trackMode, "invalid track mode");
        _linkTrack = other._linkTrack;
        _container = other._container;
        setContainerTrackIndex(_trackIndex);
        return *this;
    }

private:
    void setTrackIndex(int trackIndex);
    void setContainerTrackIndex(int trackIndex);

    // Note: always call through Project::setTrackMode
    void setTrackMode(TrackMode trackMode) {
        trackMode = ModelUtils::clampedEnum(trackMode);
        if (trackMode != _trackMode) {
            _trackMode = trackMode;
            initContainer();
        }
    }

    void initContainer();

    uint8_t _trackIndex = -1;
    TrackMode _trackMode;
    int8_t _linkTrack;

    Container<NoteTrack, CurveTrack, MidiCvTrack, StochasticTrack, LogicTrack, ArpTrack> _container;
    union {
        NoteTrack *note;
        CurveTrack *curve;
        MidiCvTrack *midiCv;
        StochasticTrack *stochastic;
        LogicTrack *logic;
        ArpTrack *arp;
    } _track;

    friend class Project;
    friend class ClipBoard;
};

#undef SANITIZE_TRACK_MODE
