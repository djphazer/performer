#include "Track.h"
#include "Project.h"

void Track::clear() {
    _trackMode = TrackMode::Default;
    _linkTrack = -1;

    initContainer();
}

void Track::clearPattern(int patternIndex) {
    switch (_trackMode) {
    case TrackMode::Note:
        _track.note->sequence(patternIndex).clear();
        break;
    case TrackMode::Curve:
        _track.curve->sequence(patternIndex).clear();
        break;
    case TrackMode::Stochastic:
        _track.stochastic->sequence(patternIndex).clear();
        break;
    case TrackMode::Logic:
        _track.logic->sequence(patternIndex).clear();
        break;
    case TrackMode::Arp:
        _track.arp->sequence(patternIndex).clear();
        break;
    case TrackMode::MidiCv:
        break;
    case TrackMode::Last:
        break;
    }
}

void Track::copyPattern(int src, int dst) {
    switch (_trackMode) {
    case TrackMode::Note:
        _track.note->sequence(dst) = _track.note->sequence(src);
        break;
    case TrackMode::Curve:
        _track.curve->sequence(dst) = _track.curve->sequence(src);
        break;
    case TrackMode::Stochastic:
        _track.stochastic->sequence(dst) = _track.stochastic->sequence(src);
        break;
    case TrackMode::Logic:
        _track.logic->sequence(dst) = _track.logic->sequence(src);
        break;
    case TrackMode::Arp:
        _track.arp->sequence(dst) = _track.arp->sequence(src);
        break;
    case TrackMode::MidiCv:
        break;
    case TrackMode::Last:
        break;
    }
}

bool Track::duplicatePattern(int patternIndex) {
    if (patternIndex < CONFIG_PATTERN_COUNT - 1) {
        copyPattern(patternIndex, patternIndex + 1);
        return true;
    }
    return false;
}

void Track::gateOutputName(int index, StringBuilder &str) const {
    switch (_trackMode) {
    case TrackMode::Note:
    case TrackMode::Curve:
    case TrackMode::Stochastic:
    case TrackMode::Logic:
    case TrackMode::Arp:
        str("Gate");
        break;
    case TrackMode::MidiCv:
        _track.midiCv->gateOutputName(index, str);
        break;
    case TrackMode::Last:
        break;
    }
}

void Track::cvOutputName(int index, StringBuilder &str) const {
    switch (_trackMode) {
    case TrackMode::Note:
    case TrackMode::Curve:
    case TrackMode::Stochastic:
    case TrackMode::Logic:
    case TrackMode::Arp:
        str("CV");
        break;
    case TrackMode::MidiCv:
        _track.midiCv->cvOutputName(index, str);
        break;
    case TrackMode::Last:
        break;
    }
}

void Track::write(VersionedSerializedWriter &writer) const {
    writer.writeEnum(_trackMode, trackModeSerialize);
    writer.write(_linkTrack);

    switch (_trackMode) {
    case TrackMode::Note:
        _track.note->write(writer);
        break;
    case TrackMode::Curve:
        _track.curve->write(writer);
        break;
    case TrackMode::MidiCv:
        _track.midiCv->write(writer);
        break;
    case TrackMode::Stochastic:
        _track.stochastic->write(writer);
        break;
    case TrackMode::Logic:
        _track.logic->write(writer);
        break;
    case TrackMode::Arp:
        _track.arp->write(writer);
        break;
    case TrackMode::Last:
        break;
    }
}

void Track::read(VersionedSerializedReader &reader) {
    reader.readEnum(_trackMode, trackModeSerialize);
    reader.read(_linkTrack);

    initContainer();

    switch (_trackMode) {
    case TrackMode::Note:
        _track.note->read(reader);
        break;
    case TrackMode::Curve:
        _track.curve->read(reader);
        break;
    case TrackMode::MidiCv:
        _track.midiCv->read(reader);
        break;
    case TrackMode::Stochastic:
        _track.stochastic->read(reader);
        break;
    case TrackMode::Logic:
        _track.logic->read(reader);
        break;
    case TrackMode::Arp:
        _track.arp->read(reader);
        break;
    case TrackMode::Last:
        break;
    }
}

void Track::initContainer() {
    _track.note = nullptr;
    _track.curve = nullptr;
    _track.midiCv = nullptr;
    _track.stochastic = nullptr;
    _track.logic = nullptr;
    _track.arp = nullptr;

    switch (_trackMode) {
    case TrackMode::Note:
        _track.note = _container.create<NoteTrack>();
        break;
    case TrackMode::Curve:
        _track.curve = _container.create<CurveTrack>();
        break;
    case TrackMode::MidiCv:
        _track.midiCv = _container.create<MidiCvTrack>();
        break;
    case TrackMode::Stochastic:
        _track.stochastic = _container.create<StochasticTrack>();
        break;
    case TrackMode::Logic:
        _track.logic = _container.create<LogicTrack>();
        break;
    case TrackMode::Arp:
        _track.arp = _container.create<ArpTrack>();
        break;    
    case TrackMode::Last:
        break;
    }

    setContainerTrackIndex(_trackIndex);
}

void Track::setTrackIndex(int trackIndex) {
    _trackIndex = trackIndex;
    setContainerTrackIndex(_trackIndex);
}

void Track::setContainerTrackIndex(int trackIndex) {
    FixedStringBuilder<16> str("TRACK %d", trackIndex+1);
    switch (_trackMode) {
    case TrackMode::Note:
        _track.note->setTrackIndex(trackIndex);
        _track.note->setName(str);
        break;
    case TrackMode::Curve:
        _track.curve->setTrackIndex(trackIndex);
        _track.curve->setName(str);
        break;
    case TrackMode::MidiCv:
        _track.midiCv->setTrackIndex(trackIndex);
        _track.midiCv->setName(str);
        break;
    case TrackMode::Stochastic:
        _track.stochastic->setTrackIndex(trackIndex);
        _track.stochastic->setName(str);
        break;
    case TrackMode::Logic:
        _track.logic->setTrackIndex(trackIndex);
        _track.logic->setName(str);
        break;
     case TrackMode::Arp:
        _track.arp->setTrackIndex(trackIndex);
        _track.arp->setName(str);   
        break;
    case TrackMode::Last:
        break;
    }
}
