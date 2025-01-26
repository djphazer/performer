#include "RandomGenerator.h"

#include "core/utils/Random.h"
#include <cstddef>
#include <ctime>

RandomGenerator::RandomGenerator(SequenceBuilder &builder, Params &params, SelectedSteps &selected) :
    Generator(builder),
    _params(params),
    _selected(selected)
{
    update();
}

const char *RandomGenerator::paramName(int index) const {
    switch (Param(index)) {
    case Param::Seed:   return "Seed";
    case Param::Smooth: return "Smooth";
    case Param::Bias:   return "Bias";
    case Param::Scale:  return "Scale";
    case Param::Last:   break;
    }
    return nullptr;
}

void RandomGenerator::editParam(int index, int value, bool shift) {
    switch (Param(index)) {
    case Param::Seed:   setSeed(seed() + value); break;
    case Param::Smooth: setSmooth(smooth() + value); break;
    case Param::Bias:   setBias(bias() + value); break;
    case Param::Scale:  setScale(scale() + value); break;
    case Param::Last:   break;
    }
}

void RandomGenerator::printParam(int index, StringBuilder &str) const {
    switch (Param(index)) {
    case Param::Seed:   str("%d", seed()); break;
    case Param::Smooth: str("%d", smooth()); break;
    case Param::Bias:   str("%d", bias()); break;
    case Param::Scale:  str("%d", scale()); break;
    case Param::Last:   break;
    }
}

void RandomGenerator::init() {
    _params = Params();
    randomizeSeed();
    update();
}

void RandomGenerator::randomizeSeed() {
    srand((unsigned int)time(NULL));
    _params.seed = 0 + ( std::rand() % ( 999 - 0 + 1 ) );
}

void RandomGenerator::update() {
    Random rng(_params.seed);

    int size = _pattern.size();

    for (int i = 0; i < size; ++i) {
        if (_selected[i]) {
            _pattern[i] = rng.nextRange(255);
        } else {
            _pattern[i] = 0;
        }
    }

    for (int iteration = 0; iteration < _params.smooth; ++iteration) {
        for (int i = 0; i < size; ++i) {
            if (_selected[i]) {
                _pattern[i] = (4 * _pattern[i] + _pattern[(i - 1 + size) % size] + _pattern[(i + 1) % size] + 3) / 6;
            }
        }
    }

    int bias = (_params.bias * 255) / 10;
    int scale = _params.scale;

    for (int i = 0; i < size; ++i) {
        if (_selected[i]) {
            int value = _pattern[i];
            // value = ((value - 127) * scale) / 10 + 127 + bias;
            value = ((value + bias - 127) * scale) / 10 + 127;
            _pattern[i] = clamp(value, 0, 255);
        }
    }

    for (size_t i = 0; i < _pattern.size(); ++i) {
        if (_selected[i]) {
            _builder.setValue(i, _pattern[i] * (1.f / 255.f));
        }
    }
}
