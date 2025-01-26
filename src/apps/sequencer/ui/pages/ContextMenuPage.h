#pragma once

#include "BasePage.h"

#include "ui/model/ContextMenuModel.h"

class ContextMenuPage : public BasePage {
public:
    ContextMenuPage(PageManager &manager, PageContext &context);

    using ResultCallback = std::function<void(int)>;

    using BasePage::show;
    void show(ContextMenuModel &contextMenuModel, ResultCallback callback);

    virtual void draw(Canvas &canvas) override;

    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

protected:
    void closeAndCallback(int index);

    ContextMenuModel *_contextMenuModel;
    ResultCallback _callback;
};
