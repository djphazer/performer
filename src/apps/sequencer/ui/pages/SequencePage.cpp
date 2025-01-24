#include "SequencePage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

enum class ContextAction {
    Init,
    Copy,
    Paste,
    Duplicate,
    Route,
    Last
};

static const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
    { "DUPL" },
    { "ROUTE" },
};

static const char *functionNames[] = { "Edit", "Sequence", "Track", nullptr, "Song" };
static const Key::Code FunctionModeMap[] = { Key::SequenceEdit, Key::Sequence, Key::Track, Key::None, Key::Song };

SequencePage::SequencePage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _noteList)
{}

void SequencePage::enter() {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        _noteList.setSequence(&_project.selectedNoteSequence());
        setListModel(_noteList);
        break;
    case Track::TrackMode::Curve:
        _curveList.setSequence(&_project.selectedCurveSequence());
        setListModel(_curveList);
        break;

    default:
        ASSERT(false, "Invalid track type for Sequence Page!");
        return;
    }
}

void SequencePage::exit() {
}

void SequencePage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE");
    WindowPainter::drawActiveFunction(canvas, Track::trackModeName(_project.selectedTrack().trackMode()));
    WindowPainter::drawFooter(canvas, functionNames, globalKeyState(), -1);

    ListPage::draw(canvas);
}

void SequencePage::updateLeds(Leds &leds) {
    ListPage::updateLeds(leds);
}

void SequencePage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isContextMenu()) {
        contextShow();
        event.consume();
        return;
    }

    if (key.isFunction()) {
        _manager.setView(FunctionModeMap[key.function()]);
        event.consume();
    }

    if (!event.consumed()) {
        ListPage::keyPress(event);
    }
}

void SequencePage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void SequencePage::contextAction(int index) {
    switch (ContextAction(index)) {
    case ContextAction::Init:
        initSequence();
        break;
    case ContextAction::Copy:
        copySequence();
        break;
    case ContextAction::Paste:
        pasteSequence();
        break;
    case ContextAction::Duplicate:
        duplicateSequence();
        break;
    case ContextAction::Route:
        initRoute();
        break;
    case ContextAction::Last:
        break;
    }
}

bool SequencePage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        if (_project.selectedTrack().trackMode() == Track::TrackMode::Note)
            return _model.clipBoard().canPasteNoteSequence();
        if (_project.selectedTrack().trackMode() == Track::TrackMode::Curve)
            return _model.clipBoard().canPasteCurveSequence();
        return false;
    case ContextAction::Route:
        return getListModel().routingTarget(selectedRow()) != Routing::Target::None;
    default:
        return true;
    }
}

void SequencePage::initSequence() {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        _project.selectedNoteSequence().clear();
        break;
    case Track::TrackMode::Curve:
        _project.selectedCurveSequence().clear();
        break;
    default: return;
    }
    showMessage("SEQUENCE INITIALIZED");
}

void SequencePage::copySequence() {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        _model.clipBoard().copyNoteSequence(_project.selectedNoteSequence());
        break;
    case Track::TrackMode::Curve:
        _model.clipBoard().copyCurveSequence(_project.selectedCurveSequence());
        break;
    default: return;
    }
    showMessage("SEQUENCE COPIED");
}

void SequencePage::pasteSequence() {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        _model.clipBoard().pasteNoteSequence(_project.selectedNoteSequence());
        break;
    case Track::TrackMode::Curve:
        _model.clipBoard().pasteCurveSequence(_project.selectedCurveSequence());
        break;
    default: return;
    }
    showMessage("SEQUENCE PASTED");
}

void SequencePage::duplicateSequence() {
    if (_project.selectedTrack().duplicatePattern(_project.selectedPatternIndex())) {
        showMessage("SEQUENCE DUPLICATED");
    }
}

void SequencePage::initRoute() {
    _manager.pages().home.editRoute(getListModel().routingTarget(selectedRow()), _project.selectedTrackIndex());
}
