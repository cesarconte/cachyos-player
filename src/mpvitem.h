#pragma once

#include <QQuickItem>
#include <mpv/client.h>
#include <mpv/render.h>
#include "mpvcontroller.h"

class MpvItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(MpvController* controller READ controller WRITE setController NOTIFY controllerChanged)

public:
    explicit MpvItem(QQuickItem *parent = nullptr);
    ~MpvItem() override;

    MpvController *controller() const { return m_controller; }
    void setController(MpvController *c);

    mpv_render_context *mpvRenderContext() const { return m_mpvGL; }

Q_SIGNALS:
    void controllerChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private Q_SLOTS:
    void onFrameSwapped();

private:
    static void on_update(void *ctx);

    MpvController *m_controller{nullptr};
    mpv_render_context *m_mpvGL{nullptr};
    bool m_initialized{false};
};
