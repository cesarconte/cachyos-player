#include "mpvitem.h"
#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <mpv/render_gl.h>
#include <QDebug>

static void *get_proc_address_mpv(void *ctx, const char *name)
{
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx) return nullptr;
    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

void MpvItem::on_update(void *ctx)
{
    MpvItem *item = static_cast<MpvItem *>(ctx);
    QMetaObject::invokeMethod(item, [item]() {
        item->update();
    }, Qt::QueuedConnection);
}

MpvItem::MpvItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
}

MpvItem::~MpvItem()
{
    if (m_mpvGL) {
        mpv_render_context_free(m_mpvGL);
        m_mpvGL = nullptr;
    }
}

void MpvItem::setController(MpvController *c)
{
    if (m_controller == c) return;
    m_controller = c;
    Q_EMIT controllerChanged();
    update();
}

void MpvItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    update();
}

void MpvItem::onFrameSwapped()
{
    if (m_mpvGL) {
        mpv_render_context_report_swap(m_mpvGL);
    }
}

QSGNode *MpvItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    if (!m_controller || !m_controller->mpv() || width() <= 0 || height() <= 0) {
        delete oldNode;
        return nullptr;
    }

    if (!m_initialized) {
        mpv_opengl_init_params gl_init_params{get_proc_address_mpv, nullptr};
        mpv_render_param params[]{
            {MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
            {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
            {MPV_RENDER_PARAM_INVALID, nullptr}
        };

        if (mpv_render_context_create(&m_mpvGL, m_controller->mpv(), params) < 0) {
            qWarning() << "Failed to initialize mpv render context";
            return oldNode;
        }

        mpv_render_context_set_update_callback(m_mpvGL, on_update, this);
        connect(window(), &QQuickWindow::frameSwapped, this, &MpvItem::onFrameSwapped, Qt::DirectConnection);
        m_initialized = true;
    }

    // Render texture using Qt Quick scene graph
    GLint defaultFbo = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFbo);

    mpv_opengl_fbo fbo{
        .fbo = defaultFbo,
        .w = static_cast<int>(width() * window()->devicePixelRatio()),
        .h = static_cast<int>(height() * window()->devicePixelRatio()),
        .internal_format = 0
    };

    int flip_y = 1;
    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_OPENGL_FBO, &fbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    mpv_render_context_render(m_mpvGL, params);

    // Return empty node to let Qt Quick continue composition
    QSGNode *node = oldNode;
    if (!node) {
        node = new QSGNode();
    }
    return node;
}
