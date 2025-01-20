#pragma once

#include "TrackEngine.h"
#include "SequenceState.h"
#include "SortedQueue.h"
#include "Groove.h"
#include "RecordHistory.h"
#include "model/StochasticSequence.h"
#include "StepRecorder.h"
#include <valarray>
#include <vector>

class StochasticStep {
        public:

            StochasticStep() {}
            StochasticStep(int index, int probability) {
                _index = index;
                _probability = probability;
            }

            int index() {
                return _index;
            }

            int probability() const {
                return _probability;
            }

        private:
            int _index;
            int _probability;

    };

class StochasticLoopStep {
    public:
        StochasticLoopStep() {}
        StochasticLoopStep(int index, bool gate, StochasticSequence::Step step, float noteValue, uint32_t stepLength, int stepRetrigger ) {
            _index = index;
            _gate = gate;
            _step = step;
            _noteValue = noteValue;
            _stepLength = stepLength;
            _stepRetrigger = stepRetrigger;
        }

        int index() {
            return _index;
        }

        bool gate() {
            return _gate;
        }

        StochasticSequence::Step step() {
            return _step;
        }

        float noteValue() {
            return _noteValue;
        }

        uint32_t stepLength() {
            return _stepLength;
        }

        int stepRetrigger() {
            return _stepRetrigger;
        }


    private:
        int _index;
        bool _gate;
        StochasticSequence::Step _step;
        float _noteValue;
        uint32_t _stepLength;
        int _stepRetrigger;
};

class StochasticEngine : public TrackEngine {
public:
    StochasticEngine(Engine &engine, Model &model, Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(engine, model, track, linkedTrackEngine),
        _stochasticTrack(track.stochasticTrack())
    {
        reset();
    }

    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::Stochastic; }

    virtual void reset() override;
    virtual void restart() override;
    virtual TickResult tick(uint32_t tick) override;
    virtual void update(float dt) override;

    virtual void changePattern() override;

    virtual void monitorMidi(uint32_t tick, const MidiMessage &message) override;
    virtual void clearMidiMonitoring() override;

    virtual const TrackLinkData *linkData() const override { return &_linkData; }

    virtual bool activity() const override { return _activity; }
    virtual bool gateOutput(int index) const override { return _gateOutput; }
    virtual float cvOutput(int index) const override { return _cvOutput; }
    virtual float sequenceProgress() const override {
        return _currentStep < 0 ? 0.f : float(_currentStep - _sequence->firstStep()) / (_sequence->lastStep() - _sequence->firstStep());
    }

    const StochasticSequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const StochasticSequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _currentStep; }
    int currentRecordStep() const { return _stepRecorder.stepIndex(); }

    int currentIndex() const { return _index; }

    void setMonitorStep(int index);
    Types::PlayMode playMode() const { return _stochasticTrack.playMode(); }


    int getNextWeightedPitch(std::vector<StochasticStep> distr, int notesPerOctave = 12);
    int evalRestProbability(StochasticSequence &sequence);

    std::vector<StochasticLoopStep> lockedSteps() {
        return _lockedSteps;
    }




private:
    void triggerStep(uint32_t tick, uint32_t divisor, bool nextStep);
    void triggerStep(uint32_t tick, uint32_t divisor);
    void recordStep(uint32_t tick, uint32_t divisor);
    int noteFromMidiNote(uint8_t midiNote) const;

    bool fill() const {
        return (_stochasticTrack.fillMuted() || !TrackEngine::mute()) ? TrackEngine::fill() : false;
    }

    std::vector<StochasticLoopStep> slicing(std::vector<StochasticLoopStep> &arr, int X, int Y)
    {
    
        // Starting and Ending iterators
        auto start = arr.begin() + X;
        auto end = arr.begin() + Y + 1;
    
        // To store the sliced vector
        std::vector<StochasticLoopStep> result(Y - X + 1);
    
        // Copy vector using copy function()
        copy(start, end, result.begin());
    
        // Return the final sliced vector
        return result;
    }

    StochasticTrack &_stochasticTrack;

    TrackLinkData _linkData;

    StochasticSequence *_sequence;
    const StochasticSequence *_fillSequence;

    uint32_t _freeRelativeTick;
    SequenceState _sequenceState;
    int _currentStep;
    int _index;
    bool _prevCondition;

    int _monitorStepIndex = -1;

    RecordHistory _recordHistory;
    bool _monitorOverrideActive = false;
    StepRecorder _stepRecorder;

    bool _activity;
    bool _gateOutput;
    float _cvOutput;
    float _cvOutputTarget;
    bool _slideActive;
    unsigned int _currentStageRepeat;

    int _skips;

    std::vector<StochasticLoopStep> _lockedSteps;

    struct Gate {
        uint32_t tick;
        bool gate;
    };

    struct GateCompare {
        bool operator()(const Gate &a, const Gate &b) {
            return a.tick < b.tick;
        }
    };

    SortedQueue<Gate, 16, GateCompare> _gateQueue;

    struct Cv {
        uint32_t tick;
        float cv;
        bool slide;
    };

    struct CvCompare {
        bool operator()(const Cv &a, const Cv &b) {
            return a.tick < b.tick;
        }
    };

    SortedQueue<Cv, 16, CvCompare> _cvQueue;
};
