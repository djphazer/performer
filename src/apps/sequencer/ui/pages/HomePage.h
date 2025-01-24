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
        // TODO: move logic to PageManager
        //_manager.setView(mode);
        setMode(PageView(code));
    }

private:
    enum PageView : uint8_t {
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

    const char *functionNames[5] = { "Overview", "Project", "Track", "Scales", "System" };
    const PageView FunctionModeMap[5] = {
        Overview, Project, Track, UserScale, Monitor
    };

    void setMode(PageView mode);
    void setMainPage(Page &page);

    void setSequencePage();
    void setSequenceEditPage();

    PageView _mode;
    PageView _lastMode;
};
