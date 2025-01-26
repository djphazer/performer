#pragma once

#include "Config.h"
#include "Bitfield.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "Types.h"
#include "Scale.h"
#include "Routing.h"
#include "FileDefs.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"
#include "core/utils/StringUtils.h"

#include <array>
#include <bitset>
#include <cstdint>
#include <initializer_list>

class LogicSequence {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef UnsignedValue<4> GateProbability;
    typedef SignedValue<4> GateOffset;
    typedef UnsignedValue<3> Retrigger;
    typedef UnsignedValue<4> RetriggerProbability;
    typedef UnsignedValue<4> Length;
    typedef SignedValue<4> LengthVariationRange;
    typedef UnsignedValue<4> LengthVariationProbability;
    typedef SignedValue<3> GateLogic;
    typedef SignedValue<7> Note;
    typedef SignedValue<3> NoteLogic;
    typedef SignedValue<7> NoteVariationRange;
    typedef UnsignedValue<4> NoteVariationProbability;
    typedef UnsignedValue<7> Condition;
    typedef UnsignedValue<3> StageRepeats;
    typedef UnsignedValue<3> StageRepeatsMode;

    static_assert(int(Types::Condition::Last) <= Condition::Max + 1, "Condition enum does not fit");

    enum class Layer {
        Gate,
        GateLogic,
        GateProbability,
        GateOffset,
        Retrigger,
        RetriggerProbability,
        StageRepeats,
        StageRepeatsMode,
        Length,
        LengthVariationRange,
        LengthVariationProbability,
        NoteLogic,
        NoteVariationRange,
        NoteVariationProbability,
        Slide,
        Condition,
        Last
    };

    static const char *layerName(Layer layer) {
        switch (layer) {
            case Layer::Gate:                       return "GATE";
            case Layer::GateLogic:                  return "GATE LOGIC";
            case Layer::GateProbability:            return "GATE PROB";
            case Layer::GateOffset:                 return "GATE OFFSET";

            case Layer::Retrigger:                  return "RETRIG";
            case Layer::RetriggerProbability:       return "RETRIG PROB";

            case Layer::Length:                     return "LENGTH";
            case Layer::LengthVariationRange:       return "LENGTH RANGE";
            case Layer::LengthVariationProbability: return "LENGTH PROB";
            
            case Layer::NoteLogic:                  return "NOTE LOGIC";
            case Layer::NoteVariationProbability:   return "NOTE PROB";
            case Layer::NoteVariationRange:         return "NOTE RANGE";
            case Layer::Slide:                      return "SLIDE";

            case Layer::Condition:                  return "CONDITION";

            case Layer::StageRepeats:               return "REPEAT";
            case Layer::StageRepeatsMode:           return "REPEAT MODE";
            case Layer::Last:                       break;
        }
        return nullptr;
    }

    static Types::LayerRange layerRange(Layer layer);
    static int layerDefaultValue(Layer layer);


    enum GateLogicMode {
        One,
        Two,
        And,
        Or,
        Xor,
        Nand,
        RandomInput,
        RandomLogic
    };

    enum NoteLogicMode {
        NOne,
        NTwo,
        Min,
        Max,
        Sum,
        Avg,
        NRandomInput,
        NRandomLogic
    };

    static constexpr size_t NameLength = FileHeader::NameLength;


    class Step {

    public:
        //----------------------------------------
        // Properties
        //----------------------------------------

        // stage
        void setStageRepeats(int repeats) {
            _data1.stageRepeats = StageRepeats::clamp(repeats);
        }
        unsigned int stageRepeats() const { return _data1.stageRepeats; }

        void setStageRepeatsMode(Types::StageRepeatMode mode) {
            _data1.stageRepeatMode = mode;
        }

        Types::StageRepeatMode stageRepeatMode() const {
            int value = _data1.stageRepeatMode;
            return static_cast<Types::StageRepeatMode>(value);
        }

        // gate

        bool gate() const { return _data0.gate ? true : false; }
        void setGate(bool gate) { _data0.gate = gate; }
        void toggleGate() { setGate(!gate()); }

        // gateProbability

        int gateProbability() const { return _data1.gateProbability; }
        void setGateProbability(int gateProbability) {
            _data1.gateProbability = GateProbability::clamp(gateProbability);
        }

        // gateOffset

        int gateOffset() const { return GateOffset::Min + (int) _data1.gateOffset; }
        void setGateOffset(int gateOffset) {
            _data1.gateOffset = GateOffset::clamp(gateOffset) - GateOffset::Min;
        }

        // gateLogic

        GateLogicMode gateLogic() const { 
            int value = _data0.gateLogic;
            return static_cast<GateLogicMode>(value); 
        }
        void setGateLogic(GateLogicMode gateLogic) {
            _data0.gateLogic = gateLogic;
        }

        // noteLogic
        NoteLogicMode noteLogic() const { 
            int value = _data0.noteLogic;
            return static_cast<NoteLogicMode>(value); 
        }
        void setNoteLogic(NoteLogicMode noteLogic) {
            _data0.noteLogic = noteLogic;
        }


        // slide

        bool slide() const { return _data0.slide ? true : false; }
        void setSlide(bool slide) {
            _data0.slide = slide;
        }
        void toggleSlide() {
            setSlide(!slide());
        }

        // retrigger

        int retrigger() const { return _data1.retrigger; }
        void setRetrigger(int retrigger) {
            _data1.retrigger = Retrigger::clamp(retrigger);
        }

        // retriggerProbability

        int retriggerProbability() const { return _data1.retriggerProbability; }
        void setRetriggerProbability(int retriggerProbability) {
            _data1.retriggerProbability = RetriggerProbability::clamp(retriggerProbability);
        }

        // length

        int length() const { return _data0.length; }
        void setLength(int length) {
            _data0.length = Length::clamp(length);
        }

        // lengthVariationRange

        int lengthVariationRange() const { return LengthVariationRange::Min + _data0.lengthVariationRange; }
        void setLengthVariationRange(int lengthVariationRange) {
            _data0.lengthVariationRange = LengthVariationRange::clamp(lengthVariationRange) - LengthVariationRange::Min;
        }

        // lengthVariationProbability

        int lengthVariationProbability() const { return _data0.lengthVariationProbability; }
        void setLengthVariationProbability(int lengthVariationProbability) {
            _data0.lengthVariationProbability = LengthVariationProbability::clamp(lengthVariationProbability);
        }

        // note

        int note() const { return 0; }
        void setNote(int note) {
            //
        }

        // noteVariationRange

        int noteVariationRange() const { return NoteVariationRange::Min + _data0.noteVariationRange; }
        void setNoteVariationRange(int noteVariationRange) {
            _data0.noteVariationRange = NoteVariationRange::clamp(noteVariationRange) - NoteVariationRange::Min;
        }

        // noteVariationProbability

        int noteVariationProbability() const { return _data0.noteVariationProbability; }
        void setNoteVariationProbability(int noteVariationProbability) {
            _data0.noteVariationProbability = NoteVariationProbability::clamp(noteVariationProbability);
        }

        // condition

        Types::Condition condition() const { return Types::Condition(int(_data1.condition)); }
        void setCondition(Types::Condition condition) {
            _data1.condition = int(ModelUtils::clampedEnum(condition));
        }

        int layerValue(Layer layer) const;
        void setLayerValue(Layer layer, int value);

        //----------------------------------------
        // Methods
        //----------------------------------------

        Step() { clear(); }

        void clear();

        void write(VersionedSerializedWriter &writer) const;
        void read(VersionedSerializedReader &reader);

        bool operator==(const Step &other) const {
            return _data0.raw == other._data0.raw && _data1.raw == other._data1.raw;
        }

        bool operator!=(const Step &other) const {
            return !(*this == other);
        }

    private:
        union {
            uint32_t raw;
            BitField<uint32_t, 0, 1> gate;
            BitField<uint32_t, 1, 1> slide;
            BitField<uint32_t, 2, Length::Bits> length;
            BitField<uint32_t, 6, LengthVariationRange::Bits> lengthVariationRange;
            BitField<uint32_t, 10, LengthVariationProbability::Bits> lengthVariationProbability;
            BitField<uint32_t, 14, GateLogic::Bits> gateLogic;
            BitField<uint32_t, 17, NoteVariationRange::Bits> noteVariationRange;
            BitField<uint32_t, 24, NoteVariationProbability::Bits> noteVariationProbability;
            BitField<uint32_t, 28, NoteLogic::Bits> noteLogic;

        } _data0;
        union {
            uint32_t raw;
            BitField<uint32_t, 0, Retrigger::Bits> retrigger;
            BitField<uint32_t, 3, GateProbability::Bits> gateProbability;
            BitField<uint32_t, 7, RetriggerProbability::Bits> retriggerProbability;
            BitField<uint32_t, 11, GateOffset::Bits> gateOffset;
            BitField<uint32_t, 15, Condition::Bits> condition;
            BitField<uint32_t, 22, StageRepeats::Bits> stageRepeats;
            BitField<uint32_t, 25, StageRepeatsMode::Bits> stageRepeatMode;
        } _data1;

    };

    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;



    //----------------------------------------
    // Properties
    //----------------------------------------

    // slot

    int slot() const { return _slot; }
    void setSlot(int slot) {
        _slot = slot;
    }
    bool slotAssigned() const {
        return _slot != uint8_t(-1);
    }

    // name

    const char *name() const { return _name; }
    void setName(const char *name) {
        StringUtils::copy(_name, name, sizeof(_name));
    }

    // trackIndex

    int trackIndex() const { return _trackIndex; }

    // scale

    int scale() const { return _scale.get(isRouted(Routing::Target::Scale)); }
    void setScale(int s, bool routed = false, int defaultScale = 0) {

        auto &pScale = selectedScale(defaultScale);

        _scale.set(clamp(s, -1, Scale::Count - 1), routed);

        auto &aScale = selectedScale(s);

        if (pScale == aScale) {
            return;
        }

        if (s != -1 && aScale.isChromatic() && pScale.isChromatic() > 0) {
            for (int i = 0; i < 64; ++i) {

                auto pStep = _steps[i];

                int rN = pScale.noteIndex(pStep.note(), selectedRootNote(0));
                if (rN > 0) {
                    if (aScale.isNotePresent(rN)) {
                        int pNoteIndex = aScale.getNoteIndex(rN);
                        step(i).setNote(pNoteIndex);
                    } else {
                        // search nearest note
                        while (!aScale.isNotePresent(rN)) {
                            rN--;
                        }
                        int pNoteIndex = aScale.getNoteIndex(rN);
                        step(i).setNote(pNoteIndex);
                    }
                }


            }
        }

    }

    int indexedScale() const { return scale() + 1; }
    void setIndexedScale(int index) {
        setScale(index - 1);
    }

    void editScale(int value, bool shift, int defaultScale = 0) {
        if (!isRouted(Routing::Target::Scale)) {
            setScale(value, false, defaultScale);
        }
    }

    void printScale(StringBuilder &str) const {
        printRouted(str, Routing::Target::Scale);
        str(scale() < 0 ? "Default" : Scale::name(scale()));
    }

    const Scale &selectedScale(int defaultScale) const {
        return Scale::get(scale() < 0 ? defaultScale : scale());
    }

    // rootNote

    int rootNote() const { return _rootNote.get(isRouted(Routing::Target::RootNote)); }
    void setRootNote(int rootNote, bool routed = false) {
        _rootNote.set(clamp(rootNote, -1, 11), routed);
    }

    int indexedRootNote() const { return rootNote() + 1; }
    void setIndexedRootNote(int index) {
        setRootNote(index - 1);
    }

    void editRootNote(int value, bool shift) {
        if (!isRouted(Routing::Target::RootNote)) {
            setRootNote(rootNote() + value);
        }
    }

    void printRootNote(StringBuilder &str) const {
        printRouted(str, Routing::Target::RootNote);
        if (rootNote() < 0) {
            str("Default");
        } else {
            Types::printNote(str, rootNote());
        }
    }

    int selectedRootNote(int defaultRootNote) const {
        return rootNote() < 0 ? defaultRootNote : rootNote();
    }

    // divisor

    int divisor() const { return _divisor.get(isRouted(Routing::Target::Divisor)); }
    void setDivisor(int divisor, bool routed = false) {
        _divisor.set(ModelUtils::clampDivisor(divisor), routed);
    }

    int indexedDivisor() const { return ModelUtils::divisorToIndex(divisor()); }
    void setIndexedDivisor(int index) {
        int divisor = ModelUtils::indexToDivisor(index);
        if (divisor > 0) {
            setDivisor(divisor);
        }
    }

    void editDivisor(int value, bool shift) {
        if (!isRouted(Routing::Target::Divisor)) {
            setDivisor(ModelUtils::adjustedByDivisor(divisor(), value, shift));
        }
    }

    void printDivisor(StringBuilder &str) const {
        printRouted(str, Routing::Target::Divisor);
        ModelUtils::printDivisor(str, divisor());
    }

    // resetMeasure

    int resetMeasure() const { return _resetMeasure; }
    void setResetMeasure(int resetMeasure) {
        _resetMeasure = clamp(resetMeasure, 0, 128);
    }

    void editResetMeasure(int value, bool shift) {
        setResetMeasure(ModelUtils::adjustedByPowerOfTwo(resetMeasure(), value, shift));
    }

    void printResetMeasure(StringBuilder &str) const {
        if (resetMeasure() == 0) {
            str("off");
        } else {
            str("%d %s", resetMeasure(), resetMeasure() > 1 ? "bars" : "bar");
        }
    }

    // runMode

    Types::RunMode runMode() const { return _runMode.get(isRouted(Routing::Target::RunMode)); }
    void setRunMode(Types::RunMode runMode, bool routed = false) {
        _runMode.set(ModelUtils::clampedEnum(runMode), routed);
    }

    void editRunMode(int value, bool shift) {
        if (!isRouted(Routing::Target::RunMode)) {
            setRunMode(ModelUtils::adjustedEnum(runMode(), value));
        }
    }

    void printRunMode(StringBuilder &str) const {
        printRouted(str, Routing::Target::RunMode);
        str(Types::runModeName(runMode()));
    }

    // firstStep

    int firstStep() const {
        return _firstStep.get(isRouted(Routing::Target::FirstStep));
    }

    void setFirstStep(int firstStep, bool routed = false) {
        _firstStep.set(clamp(firstStep, 0, lastStep()), routed);
    }

    void editFirstStep(int value, bool shift) {
        if (shift) {
            offsetFirstAndLastStep(value);
        } else if (!isRouted(Routing::Target::FirstStep)) {
            setFirstStep(firstStep() + value);
        }
    }

    void printFirstStep(StringBuilder &str) const {
        printRouted(str, Routing::Target::FirstStep);
        str("%d", firstStep() + 1);
    }

    // lastStep

    int lastStep() const {
        // make sure last step is always >= first step even if stored value is invalid (due to routing changes)
        return std::max(firstStep(), int(_lastStep.get(isRouted(Routing::Target::LastStep))));
    }

    void setLastStep(int lastStep, bool routed = false) {
        _lastStep.set(clamp(lastStep, firstStep(), CONFIG_STEP_COUNT - 1), routed);
    }

    void editLastStep(int value, bool shift) {
        if (shift) {
            offsetFirstAndLastStep(value);
        } else if (!isRouted(Routing::Target::LastStep)) {
            setLastStep(lastStep() + value);
        }
    }

    void printLastStep(StringBuilder &str) const {
        printRouted(str, Routing::Target::LastStep);
        str("%d", lastStep() + 1);
    }

    // steps

    const StepArray &steps() const { return _steps; }
          StepArray &steps()       { return _steps; }

    const Step &step(int index) const { return _steps[index]; }
          Step &step(int index)       { return _steps[index]; }

    //----------------------------------------
    // Routing
    //----------------------------------------

    inline bool isRouted(Routing::Target target) const { return Routing::isRouted(target, _trackIndex); }
    inline void printRouted(StringBuilder &str, Routing::Target target) const { Routing::printRouted(str, target, _trackIndex); }
    void writeRouted(Routing::Target target, int intValue, float floatValue);

    //----------------------------------------
    // Methods
    //----------------------------------------

    LogicSequence() { clear(); }

    void clear();
    void clearSteps();
    void clearStepsSelected(const std::bitset<CONFIG_STEP_COUNT> &selected);

    bool isEdited() const;

    void setGates(std::initializer_list<int> gates);
    void setNotes(std::initializer_list<int> notes);

    void shiftSteps(const std::bitset<CONFIG_STEP_COUNT> &selected, int direction);

    void duplicateSteps();

    void write(VersionedSerializedWriter &writer) const;
    bool read(VersionedSerializedReader &reader);

    int trackIndex() {
        return _trackIndex;
    }

    int section() { return _section; }
    const int section() const { return _section; }

    void setSecion(int section) {
        _section = section;
    }

private:
    void setTrackIndex(int trackIndex) { _trackIndex = trackIndex; }

    void offsetFirstAndLastStep(int value) {
        value = clamp(value, -firstStep(), CONFIG_STEP_COUNT - 1 - lastStep());
        if (value > 0) {
            editLastStep(value, false);
            editFirstStep(value, false);
        } else {
            editFirstStep(value, false);
            editLastStep(value, false);
        }
    }

    uint8_t _slot = uint8_t(-1);
    char _name[NameLength + 1];
    int8_t _trackIndex = -1;
    Routable<int8_t> _scale;

    int _previousScale;

    Routable<int8_t> _rootNote;
    Routable<uint16_t> _divisor;
    uint8_t _resetMeasure;
    Routable<Types::RunMode> _runMode;
    Routable<uint8_t> _firstStep;
    Routable<uint8_t> _lastStep;

    StepArray _steps;

    uint8_t _edited;

    int _section = 0;

    friend class LogicTrack;
};
