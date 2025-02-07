#include "ClockSetupPage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

ClockSetupPage::ClockSetupPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel),
    _listModel(_project.clockSetup())
{
}

void ClockSetupPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "CLOCK");
    WindowPainter::drawFooter(canvas);

    ListPage::draw(canvas);
}

void ClockSetupPage::keyPress(KeyPressEvent &event) {
    if (event.key().isRight()) {
        _manager.setView(Key::Project);
        event.consume();
        return;
    }
    ListPage::keyPress(event);
}
