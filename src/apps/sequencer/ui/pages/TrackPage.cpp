#include "TrackPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

enum class ContextAction {
    Init,
    Copy,
    Paste,
    Route,
    Last
};

static const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
    { "ROUTE" },
};

static const char *functionNames[] = { "Edit", "Sequence", nullptr, nullptr, "Song" };
static const Key::Code FunctionModeMap[] = { Key::SequenceEdit, Key::Sequence, Key::None, Key::None, Key::Song };

TrackPage::TrackPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _noteTrackListModel)
{}

void TrackPage::enter() {
    setTrack(_project.selectedTrack());
}

void TrackPage::exit() {
}

void TrackPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "TRACK");
    WindowPainter::drawActiveFunction(canvas, Track::trackModeName(_project.selectedTrack().trackMode()));
    WindowPainter::drawFooter(canvas, functionNames, globalKeyState(), -1);

    ListPage::draw(canvas);
}

void TrackPage::updateLeds(Leds &leds) {
    ListPage::updateLeds(leds);
}

void TrackPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isContextMenu()) {
        contextShow();
        event.consume();
        return;
    }

    if (key.isLeft() || key.isRight()) {
        _manager.setView(key.isLeft()? Key::Sequence : Key::Song);
        event.consume();
        return;
    }
    if (key.isFunction()) {
        _manager.setView(FunctionModeMap[key.function()]);
        event.consume();
    }

    if (key.isTrackSelect()) {
        _project.setSelectedTrackIndex(key.trackSelect());
        setTrack(_project.selectedTrack());
    }

    ListPage::keyPress(event);
}

void TrackPage::setTrack(Track &track) {
    ListModel *newListModel = nullptr;

    switch (track.trackMode()) {
    case Track::TrackMode::Note:
        _noteTrackListModel.setTrack(track.noteTrack());
        newListModel = &_noteTrackListModel;
        break;
    case Track::TrackMode::Curve:
        _curveTrackListModel.setTrack(track.curveTrack());
        newListModel = &_curveTrackListModel;
        break;
    case Track::TrackMode::MidiCv:
        _midiCvTrackListModel.setTrack(track.midiCvTrack());
        newListModel = &_midiCvTrackListModel;
        break;
    case Track::TrackMode::Last:
    default:
        ASSERT(false, "invalid track mode");
        break;
    }

    if (newListModel)
        setListModel(*newListModel);
}

void TrackPage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void TrackPage::contextAction(int index) {
    switch (ContextAction(index)) {
    case ContextAction::Init:
        initTrackSetup();
        break;
    case ContextAction::Copy:
        copyTrackSetup();
        break;
    case ContextAction::Paste:
        pasteTrackSetup();
        break;
    case ContextAction::Route:
        initRoute();
        break;
    case ContextAction::Last:
        break;
    }
}

bool TrackPage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteTrack();
    case ContextAction::Route:
        return getListModel().routingTarget(selectedRow()) != Routing::Target::None;
    default:
        return true;
    }
}

void TrackPage::initTrackSetup() {
    _project.selectedTrack().clear();
    setTrack(_project.selectedTrack());
    showMessage("TRACK INITIALIZED");
}

void TrackPage::copyTrackSetup() {
    _model.clipBoard().copyTrack(_project.selectedTrack());
    showMessage("TRACK COPIED");
}

void TrackPage::pasteTrackSetup() {
    // we are about to change track engines -> lock the engine to avoid inconsistent state
    _engine.lock();
    _model.clipBoard().pasteTrack(_project.selectedTrack());
    _engine.unlock();
    setTrack(_project.selectedTrack());
    showMessage("TRACK PASTED");
}

void TrackPage::initRoute() {
    _manager.pages().home.editRoute(getListModel().routingTarget(selectedRow()), _project.selectedTrackIndex());
}
