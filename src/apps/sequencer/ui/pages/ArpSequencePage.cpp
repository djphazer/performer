#include "ArpSequencePage.h"

#include "ListPage.h"
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

enum class SaveContextAction {
    Load,
    Save,
    SaveAs,
    Last
};

static const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
    { "DUPL" },
    { "ROUTE" },
};

static const ContextMenuModel::Item saveContextMenuItems[] = {
    { "LOAD" },
    { "SAVE" },
    { "SAVE AS"},
};


ArpSequencePage::ArpSequencePage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{}

void ArpSequencePage::enter() {
    _listModel.setSequence(&_project.selectedArpSequence());
}

void ArpSequencePage::exit() {
    _listModel.setSequence(nullptr);
}

void ArpSequencePage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE");
    WindowPainter::drawActiveFunction(canvas, Track::trackModeName(_project.selectedTrack().trackMode()));
    WindowPainter::drawFooter(canvas);

    ListPage::draw(canvas);
}

void ArpSequencePage::updateLeds(Leds &leds) {
    ListPage::updateLeds(leds);
}

void ArpSequencePage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();


    if (key.shiftModifier() && event.count() == 2) {
        saveContextShow();
        event.consume();
        return;
    }

    if (key.isContextMenu()) {
        contextShow();
        event.consume();
        return;
    }

    if (key.pageModifier()) {
        return;
    }

    if (key.is(Key::Encoder) && selectedRow() == 0) {
        _manager.pages().textInput.show("NAME:", _project.selectedArpSequence().name(), ArpSequence::NameLength, [this] (bool result, const char *text) {
            if (result) {
                _project.selectedArpSequence().setName(text);
            }
        });

        return;
    }

    if (!event.consumed()) {
        ListPage::keyPress(event);
    }
    if (key.isEncoder()) {
        auto row = ListPage::selectedRow();
        if (row == 3) {
            _listModel.setSelectedScale(_project.scale());
        }
    }
}

void ArpSequencePage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void ArpSequencePage::saveContextShow() {
    showContextMenu(ContextMenu(
        saveContextMenuItems,
        int(SaveContextAction::Last),
        [&] (int index) { saveContextAction(index); },
        [&] (int index) { return true; }
    ));
}

void ArpSequencePage::contextAction(int index) {
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

void ArpSequencePage::saveContextAction(int index) {
    switch (SaveContextAction(index)) {
    case SaveContextAction::Load:
        loadSequence();
        break;
    case SaveContextAction::Save:
        saveSequence();
        break;
    case SaveContextAction::SaveAs:
        saveAsSequence();
        break;
    case SaveContextAction::Last:
        break;
    }
}

bool ArpSequencePage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteArpSequence();
    case ContextAction::Route:
        return _listModel.routingTarget(selectedRow()) != Routing::Target::None;
    default:
        return true;
    }
}

void ArpSequencePage::initSequence() {
    _project.selectedArpSequence().clear();
    auto &arpEngine = _engine.trackEngine(_project.selectedTrackIndex()).as<ArpTrackEngine>();
    arpEngine.clearNotes();
    showMessage("SEQUENCE INITIALIZED");
}

void ArpSequencePage::copySequence() {
    _model.clipBoard().copyArpSequence(_project.selectedArpSequence());
    showMessage("SEQUENCE COPIED");
}

void ArpSequencePage::pasteSequence() {
    _model.clipBoard().pasteArpSequence(_project.selectedArpSequence());
    showMessage("SEQUENCE PASTED");
}

void ArpSequencePage::duplicateSequence() {
    if (_project.selectedTrack().duplicatePattern(_project.selectedPatternIndex())) {
        showMessage("SEQUENCE DUPLICATED");
    }
}

void ArpSequencePage::initRoute() {
    _manager.pages().top.editRoute(_listModel.routingTarget(selectedRow()), _project.selectedTrackIndex());
}

void ArpSequencePage::loadSequence() {
    _manager.pages().fileSelect.show("LOAD SEQUENCE", FileType::ArpSequence, _project.selectedArpSequence().slotAssigned() ? _project.selectedArpSequence().slot() : 0, false, [this] (bool result, int slot) {
        if (result) {
            _manager.pages().confirmation.show("ARE YOU SURE?", [this, slot] (bool result) {
                if (result) {
                    loadSequenceFromSlot(slot);
                }
            });
        }
    });
}

void ArpSequencePage::saveSequence() {

    if (!_project.selectedArpSequence().slotAssigned() || sizeof(_project.selectedArpSequence().name())==0) {
        saveAsSequence();
        return;
    }

    saveSequenceToSlot(_project.selectedArpSequence().slot());

    showMessage("SEQUENCE SAVED");
}

void ArpSequencePage::saveAsSequence() {
    _manager.pages().fileSelect.show("SAVE SEQUENCE", FileType::ArpSequence, _project.selectedArpSequence().slotAssigned() ? _project.selectedArpSequence().slot() : 0, true, [this] (bool result, int slot) {
        if (result) {
            if (FileManager::slotUsed(FileType::ArpSequence, slot)) {
                _manager.pages().confirmation.show("ARE YOU SURE?", [this, slot] (bool result) {
                    if (result) {
                        saveSequenceToSlot(slot);
                    }
                });
            } else {
                saveSequenceToSlot(slot);
            }
        }
    });
}

void ArpSequencePage::saveSequenceToSlot(int slot) {
    //_engine.suspend();
    _manager.pages().busy.show("SAVING SEQUENCE ...");

    FileManager::task([this, slot] () {
        return FileManager::writeArpSequence(_project.selectedArpSequence(), slot);
    }, [this] (fs::Error result) {
        if (result == fs::OK) {
            showMessage("SEQUENCE SAVED");
        } else {
            showMessage(FixedStringBuilder<32>("FAILED (%s)", fs::errorToString(result)));
        }
        // TODO lock ui mutex
        _manager.pages().busy.close();
        _engine.resume();
    });
}

void ArpSequencePage::loadSequenceFromSlot(int slot) {
    //_engine.suspend();
    _manager.pages().busy.show("LOADING SEQUENCE ...");

    FileManager::task([this, slot] () {
        // TODO this is running in file manager thread but model notification affect ui
        return FileManager::readArpSequence(_project.selectedArpSequence(), slot);
    }, [this] (fs::Error result) {
        if (result == fs::OK) {
            showMessage("SEQUENCE LOADED");
        } else if (result == fs::INVALID_CHECKSUM) {
            showMessage("INVALID SEQUENCE FILE");
        } else {
            showMessage(FixedStringBuilder<32>("FAILED (%s)", fs::errorToString(result)));
        }
        // TODO lock ui mutex
        _manager.pages().busy.close();
        _engine.resume();
    });
}
