#pragma once

#include "BasePage.h"

#include "model/Routing.h"

class TopPage : public BasePage {
public:
    TopPage(PageManager &manager, PageContext &context);

    void init();

    void editRoute(Routing::Target target, int trackIndex);

    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

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

    void setMode(Mode mode);
    void setMainPage(Page &page);

    void setSequencePage();
    void setSequenceEditPage();

    Mode _mode;
    Mode _lastMode;
};
