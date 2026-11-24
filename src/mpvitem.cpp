#include "mpvitem.h"
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <mpv/render_gl.h>
#include <QDebug>

static void *get_proc_address_mpv(void *ctx, const char *name)
{
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx) return nullptr;
    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

class MpvFboRenderer : public QQuickFramebufferObject::Renderer
{
public:
    MpvFboRenderer(const MpvItem *item)
        : m_item(item)
    {
    }

    ~MpvFboRenderer() override
    {
        if (m_mpvGL) {
            mpv_render_context_free(m_mpvGL);
            m_mpvGL = nullptr;
        }
    }

    static void on_update(void *ctx)
    {
        MpvFboRenderer *r = static_cast<MpvFboRenderer *>(ctx);
        QMetaObject::invokeMethod(const_cast<MpvItem *>(r->m_item), "update", Qt::QueuedConnection);
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

        if (!m_mpvGL) {
            mpv_opengl_init_params gl_init_params{get_proc_address_mpv, nullptr};
            mpv_render_param params[]{
                {MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
                {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
                {MPV_RENDER_PARAM_INVALID, nullptr}
            };

            if (mpv_render_context_create(&m_mpvGL, m_item->controller()->mpv(), params) < 0) {
                qWarning() << "Failed to initialize mpv render context in FBO";
                return;
            }

            mpv_render_context_set_update_callback(m_mpvGL, on_update, this);
        }

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

        mpv_render_context_render(m_mpvGL, params);
        mpv_render_context_report_swap(m_mpvGL);
    }

private:
    const MpvItem *m_item{nullptr};
    mpv_render_context *m_mpvGL{nullptr};
};

MpvItem::MpvItem(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
{
    setMirrorVertically(true);
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
    m_controller = c;
    Q_EMIT controllerChanged();
    update();
}
