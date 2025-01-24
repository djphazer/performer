#pragma once

#include "BasePage.h"

#include "model/Routing.h"

class HomePage : public BasePage {
public:
    HomePage(PageManager &manager, PageContext &context);

    void init();

    void editRoute(Routing::Target target, int trackIndex);

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

    void setView(Key::Code code) {
        //_manager.setView(mode);
        setMode(Mode(code));
    }

private:
    enum Mode : uint8_t {
        // main modes
        Project         = Key::Project,
        Layout          = Key::Layout,
        Track           = Key::Track,
        Sequence        = Key::Sequence,
        SequenceEdit    = Key::SequenceEdit,
        Song            = Key::Song,
        Routing         = Key::Routing,
        MidiOutput      = Key::MidiOutput,
        Pattern         = Key::Pattern,
        Performer       = Key::Performer,
        Overview        = Key::Overview,
        Clock           = Key::Clock,

        // aux modes
        UserScale       = Key::UserScale,
        Monitor         = Key::Monitor,
        System          = Key::System,

        Last,
    };
    enum Function : uint8_t {
        F1_Overview,
        F2_Project,
        F3_Track,
        F4_UserScales,
        F5_SystemMonitor,

        FUNCTION_COUNT
    };
    const Mode FunctionModeMap[FUNCTION_COUNT] = {
        Overview, Project, Track, UserScale, Monitor
    };

    void setMode(Mode mode);
    void setMainPage(Page &page);

    void setSequencePage();
    void setSequenceEditPage();

    Mode _mode;
    Mode _lastMode;
};
