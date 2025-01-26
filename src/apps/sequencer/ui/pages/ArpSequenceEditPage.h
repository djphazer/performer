#pragma once

#include "BasePage.h"

#include "ui/StepSelection.h"
#include "ui/model/ArpSequenceListModel.h"
#include "ui/model/ArpTrackListModel.h"

#include "engine/generators/SequenceBuilder.h"
#include "ui/KeyPressEventTracker.h"

#include "core/utils/Container.h"

class ArpSequenceEditPage : public BasePage {
public:
    ArpSequenceEditPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;
    virtual void midi(MidiEvent &event) override;

private:
    typedef ArpSequence::Layer Layer;

    static const int StepCount = 16;

    int stepOffset() const { return _section * StepCount; }

    void switchLayer(int functionKey, bool shift);
    int activeFunctionKey();

    void updateMonitorStep();
    void drawDetail(Canvas &canvas, const ArpSequence::Step &step);

    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;

    void initSequence();
    void copySequence();
    void pasteSequence();
    void duplicateSequence();
    void tieNotes();
    void generateSequence();

    void quickEdit(int index);

    bool allSelectedStepsActive() const;
    void setSelectedStepsGate(bool gate);

    void setSectionTracking(bool track);

    ArpSequence::Layer layer() const { return _project.selectedArpSequenceLayer(); };
    void setLayer(ArpSequence::Layer layer) { _project.setSelectedArpSequenceLayer(layer); }

    int _section = 0;
    bool _sectionTracking = false;
    bool _showDetail;
    uint32_t _showDetailTicks;

    KeyPressEventTracker _keyPressEventTracker;


    ArpSequenceListModel _listModel;
    ArpTrackListModel _trackListModel;

    StepSelection<CONFIG_STEP_COUNT> _stepSelection;

    Container<ArpSequenceBuilder> _builderContainer;

};
