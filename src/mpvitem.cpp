#include "mpvitem.h"
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <mpv/render_gl.h>
#include <QDebug>

class MpvFboRenderer : public QQuickFramebufferObject::Renderer
{
public:
    MpvFboRenderer(const MpvItem *item)
        : m_item(item)
    {
    }

    ~MpvFboRenderer() override
    {
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
    {
        return new QOpenGLFramebufferObject(size, QOpenGLFramebufferObject::NoAttachment);
    }

    void render() override
    {
        if (!m_item || !m_item->controller() || !m_item->controller()->mpv()) {
            return;
        }

        // Initialize render context in OpenGL thread if not already created
        if (!m_item->controller()->renderContext()) {
            m_item->controller()->initRenderContext();
        }

        mpv_render_context *ctx = m_item->controller()->renderContext();
        if (!ctx) return;

        QOpenGLFramebufferObject *fbo = framebufferObject();
        if (!fbo) return;

        mpv_opengl_fbo mpvFbo{
            .fbo = static_cast<int>(fbo->handle()),
            .w = fbo->width(),
            .h = fbo->height(),
            .internal_format = 0
        };

        int flip_y = 0;
        mpv_render_param params[] = {
            {MPV_RENDER_PARAM_OPENGL_FBO, &mpvFbo},
            {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
            {MPV_RENDER_PARAM_INVALID, nullptr}
        };

        int err = mpv_render_context_render(ctx, params);
        // qDebug() << "mpv_render_context_render res:" << err;
        int res = mpv_render_context_render(ctx, params);
        if (res < 0) {
            qWarning() << "mpv_render_context_render returned error:" << res;
        }
        mpv_render_context_report_swap(ctx);
    }

private:
    const MpvItem *m_item{nullptr};
};

MpvItem::MpvItem(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
{
    setMirrorVertically(false);
}

MpvItem::~MpvItem()
{
}

QQuickFramebufferObject::Renderer *MpvItem::createRenderer() const
{
    return new MpvFboRenderer(this);
}

void MpvItem::setController(MpvController *c)
{
    if (m_controller == c) return;
    if (m_controller) {
        disconnect(m_controller, &MpvController::mpvRedrawRequested, this, &QQuickItem::update);
    }
    m_controller = c;
    if (m_controller) {
        connect(m_controller, &MpvController::mpvRedrawRequested, this, &QQuickItem::update, Qt::QueuedConnection);
    }
    Q_EMIT controllerChanged();
    update();
}
