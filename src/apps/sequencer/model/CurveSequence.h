#pragma once

#include "Sequence.h"

#include "Curve.h"

class CurveSequence : public Sequence {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    using Shape = UnsignedValue<6>;
    using ShapeVariationProbability = UnsignedValue<4>;
    using Min = UnsignedValue<8>;
    using Max = UnsignedValue<8>;
    using Gate = UnsignedValue<4>;
    using GateProbability = UnsignedValue<3>;

    enum class Layer {
        Shape,
        ShapeVariation,
        ShapeVariationProbability,
        Min,
        Max,
        Gate,
        GateProbability,
        Last
    };

    static const char *layerName(Layer layer) {
        switch (layer) {
        case Layer::Shape:                      return "SHAPE";
        case Layer::ShapeVariation:             return "SHAPE VAR";
        case Layer::ShapeVariationProbability:  return "SHAPE PROB";
        case Layer::Min:                        return "MIN";
        case Layer::Max:                        return "MAX";
        case Layer::Gate:                       return "GATE";
        case Layer::GateProbability:            return "GATE PROB";
        case Layer::Last:                       break;
        }
        return nullptr;
    }

    static Types::LayerRange layerRange(Layer layer);
    static int layerDefaultValue(Layer layer);

    class Step {
    public:
        //----------------------------------------
        // Properties
        //----------------------------------------

        // shape

        int shape() const { return _data0.shape; }
        void setShape(int shape) {
            _data0.shape = clamp(shape, 0, int(Curve::Last) - 1);
        }

        // shapeVariation

        int shapeVariation() const { return _data0.shapeVariation; }
        void setShapeVariation(int shapeVariation) {
            _data0.shapeVariation = clamp(shapeVariation, 0, int(Curve::Last) - 1);
        }

        // shapeVariationProbability

        int shapeVariationProbability() const { return _data0.shapeVariationProbability; }
        void setShapeVariationProbability(int shapeVariationProbability) {
            _data0.shapeVariationProbability = clamp(shapeVariationProbability, 0, 8);
        }

        // min

        int min() const { return _data0.min; }
        void setMin(int min) {
            _data0.min = Min::clamp(min);
            _data0.max = std::max(max(), this->min());
        }

        float minNormalized() const { return float(min()) / Min::Max; }
        void setMinNormalized(float min) {
            setMin(int(std::round(min * Min::Max)));
        }

        // max

        int max() const { return _data0.max; }
        void setMax(int max) {
            _data0.max = Max::clamp(max);
            _data0.min = std::min(min(), this->max());
        }

        float maxNormalized() const { return float(max()) / Max::Max; }
        void setMaxNormalized(float max) {
            setMax(int(std::round(max * Max::Max)));
        }

        // gate

        int gate() const { return _data1.gate; }
        void setGate(int gate) {
            _data1.gate = Gate::clamp(gate);
        }

        // gateProbability

        int gateProbability() const { return _data1.gateProbability; }
        void setGateProbability(int gateProbability) {
            _data1.gateProbability = GateProbability::clamp(gateProbability);
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
            return _data0.raw == other._data0.raw;
        }

        bool operator!=(const Step &other) const {
            return !(*this == other);
        }

    private:
        union {
            uint32_t raw;
            BitField<uint32_t, 0, Shape::Bits> shape;
            BitField<uint32_t, 6, Shape::Bits> shapeVariation;
            BitField<uint32_t, 12, ShapeVariationProbability::Bits> shapeVariationProbability;
            BitField<uint32_t, 16, Min::Bits> min;
            BitField<uint32_t, 24, Max::Bits> max;
        } _data0;
        union {
            uint16_t raw;
            BitField<uint16_t, 0, Gate::Bits> gate;
            BitField<uint16_t, 4, GateProbability::Bits> gateProbability;
            // 9 bits left
        } _data1;
    };

    using StepArray = std::array<Step, CONFIG_STEP_COUNT>;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // range

    Types::VoltageRange range() const { return _range; }
    void setRange(Types::VoltageRange range) {
        _range = ModelUtils::clampedEnum(range);
    }

    void editRange(int value, bool shift) {
        setRange(ModelUtils::adjustedEnum(range(), value));
    }

    void printRange(StringBuilder &str) const {
        str(Types::voltageRangeName(range()));
    }

    // steps

    const StepArray &steps() const { return _steps; }
          StepArray &steps()       { return _steps; }

    const Step &step(int index) const { return _steps[index]; }
          Step &step(int index)       { return _steps[index]; }

    virtual void writeRouted(Routing::Target target, int intValue, float floatValue) override;

    //----------------------------------------
    // Methods
    //----------------------------------------

    CurveSequence() { clear(); }

    void clear();
    void clearSteps();

    bool isEdited() const;

    void setShapes(std::initializer_list<int> shapes);

    void shiftSteps(const std::bitset<CONFIG_STEP_COUNT> &selected, int direction);

    void duplicateSteps();

    void write(VersionedSerializedWriter &writer) const;
    void read(VersionedSerializedReader &reader);

private:
    Types::VoltageRange _range;

    StepArray _steps;

    friend class CurveTrack;
};
