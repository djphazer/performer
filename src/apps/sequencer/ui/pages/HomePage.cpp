#include "HomePage.h"

#include "Pages.h"

#include "ui/model/NoteSequenceListModel.h"
#include "ui/model/CurveSequenceListModel.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

HomePage::HomePage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{
}

void HomePage::init() {
    setMode(PageView::Project);
    //setMode(PageView::Overview);

    _context.model.project().watch([this] (Project::Event event) {
        auto &pages = _manager.pages();
        switch (event) {
        case Project::Event::ProjectCleared:
        case Project::Event::ProjectRead:
            // reset local state in pages
            pages.routing.reset();
            pages.midiOutput.reset();
            pages.song.reset();
            setMode(_mode);
            break;
        case Project::Event::TrackModeChanged:
        case Project::Event::SelectedTrackIndexChanged:
        case Project::Event::SelectedPatternIndexChanged:
            setMode(_mode);
            break;
        }
    });
}

void HomePage::editRoute(Routing::Target target, int trackIndex) {
    auto &routing = _project.routing();

    if (target == Routing::Target::None) {
        return;
    }

    int routeIndex = routing.findRoute(target, trackIndex);
    if (routeIndex >= 0) {
        setMode(PageView::Routing);
        _manager.pages().routing.showRoute(routeIndex);
        return;
    }

    routeIndex = routing.findEmptyRoute();
    if (routeIndex >= 0) {
        routing.route(routeIndex).clear();
        Routing::Route initRoute;
        initRoute.setTarget(target);
        initRoute.setTracks(1<<trackIndex);
        setMode(PageView::Routing);
        _manager.pages().routing.showRoute(routeIndex, &initRoute);
    } else {
        showMessage("All routes are used!");
    }
}

void HomePage::draw(Canvas &canvas) {
    if (!isTop()) return;

    WindowPainter::clear(canvas);

    uint32_t currentTicks = os::ticks();
    float dt = float(currentTicks - _lastTicks) / os::time::ms(1000);
    _lastTicks = currentTicks;
    _intro.update(dt, _engine.gateOutput());
    _intro.draw(canvas);

    WindowPainter::drawHeader(canvas, _model, _engine, "HOME");
    //WindowPainter::drawActiveFunction(canvas, "whateven");
    WindowPainter::drawFooter(canvas, functionNames, globalKeyState(), -1);

    // TODO: put something more useful here...
    //canvas.setFont(Font::Small);
    //canvas.drawTextCentered(0, 20, Width, 16, "Home Page :)");
}

void HomePage::updateLeds(Leds &leds) {
    bool clockTick = _engine.clockRunning() && _engine.tick() % CONFIG_PPQN < (CONFIG_PPQN / 8);

    leds.set(
        Key::Play,
        _engine.recording() && !clockTick,
        clockTick
    );

    LedPainter::drawTrackGatesAndSelectedTrack(leds, _engine, _project.playState(), _project.selectedTrackIndex());
}

void HomePage::keyDown(KeyEvent &event) {
    event.consume();
}

void HomePage::keyUp(KeyEvent &event) {
    event.consume();
}

void HomePage::keyPress(KeyPressEvent &event) {
    auto &pages = _manager.pages();
    const auto &key = event.key();

    if (isTop()) {
        // if we're on top, every key navigates to somewhere...
        if (key.isHome()) {
            setMode(_mode); // go back where we came from
        } else if (key.isTrackSelect()) {
            // Track keys jump to Edit page
            _project.setSelectedTrackIndex(key.trackSelect());
            setMode(PageView::SequenceEdit);
        } else if (key.isFunction()) {
            // Nav shortcuts on F-keys
            setMode( FunctionModeMap[key.function()] );
        } else {
            setMode(PageView(key.code()));
        }

        event.consume();
        return;
    }

    // The rest of this is top-level key handling that should be handled in PageManager
    if (key.isTrack() && event.count() == 2) {
        setMode(PageView::SequenceEdit);
        event.consume();
        return;
    }
    if (key.isTrackSelect()) {
        _project.setSelectedTrackIndex(key.trackSelect());
        event.consume();
        return;
    }

    if (key.isPattern() && _mode != PageView::Pattern) {
        pages.pattern.setModal(true);
        pages.pattern.show();
        event.consume();
    }
    if (key.isPerformer() && _mode != PageView::Performer) {
        pages.performer.setModal(true);
        pages.performer.show();
        event.consume();
    }

    if (key.isPlay()) {
        if (key.pageModifier()) {
            _engine.toggleRecording();
        } else {
            _engine.togglePlay(key.shiftModifier());
        }
        event.consume();
    }

    if (key.isTempo()) {
        if (!key.pageModifier()) {
            // tempo page
            pages.tempo.show();
        }
    }

    event.consume();
}

void HomePage::encoder(EncoderEvent &event) {
    event.consume();
}

void HomePage::setMode(PageView mode) {
    auto &pages = _manager.pages();

    _lastMode = _mode;

    switch (mode) {
    case PageView::Project:
        setMainPage(pages.project);
        break;
    case PageView::Layout:
        setMainPage(pages.layout);
        break;
    case PageView::Track:
        setMainPage(pages.track);
        break;
    case PageView::Sequence:
        setSequencePage();
        break;
    case PageView::SequenceEdit:
        setSequenceEditPage();
        break;
    case PageView::Pattern:
        pages.pattern.setModal(false);
        // do not re-enter pattern page when its already the selected page
        // the reason for this is that when changing a pattern in latched mode, we don't want to loose the latch
        // state on the page
        if (_manager.top() != &pages.pattern) {
            setMainPage(pages.pattern);
        }
        break;
    case PageView::Performer:
        pages.performer.setModal(false);
        setMainPage(pages.performer);
        break;
    case PageView::Overview:
        setMainPage(pages.overview);
        break;
    case PageView::Clock:
        setMainPage(pages.clockSetup);
        break;
    case PageView::Song:
        setMainPage(pages.song);
        break;
    case PageView::Routing:
        setMainPage(pages.routing);
        break;
    case PageView::MidiOutput:
        setMainPage(pages.midiOutput);
        break;
    case PageView::UserScale:
        setMainPage(pages.userScale);
        break;
    case PageView::Monitor:
        setMainPage(pages.monitor);
        break;
    case PageView::System:
        if (mode != _lastMode) {
            _manager.pages().confirmation.show("DO YOU REALLY WANT TO ENTER SYSTEM PAGE?", [this] (bool result) {
                if (result) {
                    setMainPage(_manager.pages().system);
                } else {
                    setMode(_lastMode);
                }
            });
        }
        break;
    default:
        return;
    }

    _mode = mode;
}

void HomePage::setMainPage(Page &page) {
    if (_manager.stackSize() < 2) {
        _manager.push(&page);
    } else {
        _manager.replace(1, &page);
    }
}

void HomePage::setSequencePage() {
    auto &pages = _manager.pages();

    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
    case Track::TrackMode::Curve:
        setMainPage(pages.sequence);
        break;
    case Track::TrackMode::MidiCv:
        setMainPage(pages.track);
        break;
    case Track::TrackMode::Last:
        break;
    }
}

void HomePage::setSequenceEditPage() {
    auto &pages = _manager.pages();

    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        setMainPage(pages.noteSequenceEdit);
        break;
    case Track::TrackMode::Curve:
        setMainPage(pages.curveSequenceEdit);
        break;
    case Track::TrackMode::MidiCv:
        setMainPage(pages.track);
        break;
    case Track::TrackMode::Last:
        break;
    }
}
