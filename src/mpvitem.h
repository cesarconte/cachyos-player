#pragma once

#include <QQuickFramebufferObject>
#include <mpv/client.h>
#include <mpv/render.h>
#include "mpvcontroller.h"

class MpvItem : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(MpvController* controller READ controller WRITE setController NOTIFY controllerChanged)

public:
    explicit MpvItem(QQuickItem *parent = nullptr);
    ~MpvItem() override;

    Renderer *createRenderer() const override;

    MpvController *controller() const { return m_controller; }
    void setController(MpvController *c);

Q_SIGNALS:
    void controllerChanged();

private:
    MpvController *m_controller{nullptr};
};
