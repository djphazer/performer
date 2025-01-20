#include "NoteSequencePage.h"

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


NoteSequencePage::NoteSequencePage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{}

void NoteSequencePage::enter() {
    _listModel.setSequence(&_project.selectedNoteSequence());
}

void NoteSequencePage::exit() {
    _listModel.setSequence(nullptr);
}

void NoteSequencePage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE");
    WindowPainter::drawActiveFunction(canvas, Track::trackModeName(_project.selectedTrack().trackMode()));
    WindowPainter::drawFooter(canvas);

    ListPage::draw(canvas);
}

void NoteSequencePage::updateLeds(Leds &leds) {
    ListPage::updateLeds(leds);
}

void NoteSequencePage::keyPress(KeyPressEvent &event) {
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
        _manager.pages().textInput.show("NAME:", _project.selectedNoteSequence().name(), NoteSequence::NameLength, [this] (bool result, const char *text) {
            if (result) {
                _project.selectedNoteSequence().setName(text);
            }
        });

        return;
    }

    // We lock the playback parameters when logic track is enabled
    /*if (key.is(Key::Encoder) && selectedRow() > 0 && selectedRow() < 6) {
        if (_project.selectedTrack().noteTrack().logicTrack() != -1) {
            return;
        }
    }*/

    if (!event.consumed()) {
        ListPage::keyPress(event);
    }
    if (key.isEncoder()) {
        auto row = ListPage::selectedRow();
        if (row == 6) {
            _listModel.setSelectedScale(_project.scale());
        }
    }
}

void NoteSequencePage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void NoteSequencePage::saveContextShow() {
    showContextMenu(ContextMenu(
        saveContextMenuItems,
        int(SaveContextAction::Last),
        [&] (int index) { saveContextAction(index); },
        [&] (int index) { return true; }
    ));
}

void NoteSequencePage::contextAction(int index) {
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

void NoteSequencePage::saveContextAction(int index) {
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

bool NoteSequencePage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteNoteSequence();
    case ContextAction::Route:
        return _listModel.routingTarget(selectedRow()) != Routing::Target::None;
    default:
        return true;
    }
}

void NoteSequencePage::initSequence() {
    _project.selectedNoteSequence().clear();
    showMessage("SEQUENCE INITIALIZED");
}

void NoteSequencePage::copySequence() {
    _model.clipBoard().copyNoteSequence(_project.selectedNoteSequence());
    showMessage("SEQUENCE COPIED");
}

void NoteSequencePage::pasteSequence() {
    _model.clipBoard().pasteNoteSequence(_project.selectedNoteSequence());
    showMessage("SEQUENCE PASTED");
}

void NoteSequencePage::duplicateSequence() {
    if (_project.selectedTrack().duplicatePattern(_project.selectedPatternIndex())) {
        showMessage("SEQUENCE DUPLICATED");
    }
}

void NoteSequencePage::initRoute() {
    _manager.pages().top.editRoute(_listModel.routingTarget(selectedRow()), _project.selectedTrackIndex());
}

void NoteSequencePage::loadSequence() {
    _manager.pages().fileSelect.show("LOAD SEQUENCE", FileType::NoteSequence, _project.selectedNoteSequence().slotAssigned() ? _project.selectedNoteSequence().slot() : 0, false, [this] (bool result, int slot) {
        if (result) {
            _manager.pages().confirmation.show("ARE YOU SURE?", [this, slot] (bool result) {
                if (result) {
                    loadSequenceFromSlot(slot);
                }
            });
        }
    });
}

void NoteSequencePage::saveSequence() {

    if (!_project.selectedNoteSequence().slotAssigned() || sizeof(_project.selectedNoteSequence().name())==0) {
        saveAsSequence();
        return;
    }

    saveSequenceToSlot(_project.selectedNoteSequence().slot());

    showMessage("SEQUENCE SAVED");
}

void NoteSequencePage::saveAsSequence() {
    _manager.pages().fileSelect.show("SAVE SEQUENCE", FileType::NoteSequence, _project.selectedNoteSequence().slotAssigned() ? _project.selectedNoteSequence().slot() : 0, true, [this] (bool result, int slot) {
        if (result) {
            if (FileManager::slotUsed(FileType::NoteSequence, slot)) {
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

void NoteSequencePage::saveSequenceToSlot(int slot) {
    //_engine.suspend();
    _manager.pages().busy.show("SAVING SEQUENCE ...");

    FileManager::task([this, slot] () {
        return FileManager::writeNoteSequence(_project.selectedNoteSequence(), slot);
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

void NoteSequencePage::loadSequenceFromSlot(int slot) {
    //_engine.suspend();
    _manager.pages().busy.show("LOADING SEQUENCE ...");

    FileManager::task([this, slot] () {
        // TODO this is running in file manager thread but model notification affect ui
        return FileManager::readNoteSequence(_project.selectedNoteSequence(), slot);
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
