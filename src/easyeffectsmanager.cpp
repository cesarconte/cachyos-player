#include "easyeffectsmanager.h"
#include <QStandardPaths>
#include <QRegularExpression>
#include <QDebug>

EasyEffectsManager::EasyEffectsManager(QObject *parent)
    : QObject(parent)
{
    refresh();
}

void EasyEffectsManager::refresh()
{
    checkAvailability();
    if (m_isAvailable) {
        parsePresets();
        detectCurrentPreset();
    }
}

void EasyEffectsManager::checkAvailability()
{
    QString exe = QStandardPaths::findExecutable(QStringLiteral("easyeffects"));
    bool avail = !exe.isEmpty();
    if (m_isAvailable != avail) {
        m_isAvailable = avail;
        Q_EMIT availabilityChanged();
    }
}

void EasyEffectsManager::parsePresets()
{
    QProcess proc;
    proc.start(QStringLiteral("easyeffects"), {QStringLiteral("-p")});
    if (proc.waitForFinished(1000)) {
        QString out = QString::fromUtf8(proc.readAllStandardOutput());
        QStringList lines = out.split(QLatin1Char('\n'));
        QStringList list;
        bool inOutputSection = false;
        for (const auto &line : lines) {
            QString trimmed = line.trimmed();
            if (trimmed.startsWith(QStringLiteral("Perfiles de salida")) || trimmed.startsWith(QStringLiteral("Output presets"))) {
                inOutputSection = true;
                continue;
            }
            if (trimmed.startsWith(QStringLiteral("Perfiles de entrada")) || trimmed.startsWith(QStringLiteral("Input presets"))) {
                inOutputSection = false;
                break;
            }
            if (inOutputSection && !trimmed.isEmpty()) {
                if (trimmed.startsWith(QStringLiteral("No hay")) || trimmed.startsWith(QStringLiteral("No input"))) {
                    continue;
                }
                // Format: "1\tPresetName" or "1 PresetName"
                static QRegularExpression re(QStringLiteral("^\\d+\\s+(.+)"));
                auto match = re.match(trimmed);
                if (match.hasMatch()) {
                    list << match.captured(1).trimmed();
                } else if (!trimmed.contains(QLatin1Char(':'))) {
                    list << trimmed;
                }
            }
        }
        if (m_presets != list) {
            m_presets = list;
            Q_EMIT presetsChanged();
        }
    }
}

void EasyEffectsManager::detectCurrentPreset()
{
    QProcess proc;
    proc.start(QStringLiteral("easyeffects"), {QStringLiteral("-s")});
    if (proc.waitForFinished(1000)) {
        QString out = QString::fromUtf8(proc.readAllStandardOutput());
        QStringList lines = out.split(QLatin1Char('\n'));
        for (const auto &line : lines) {
            QString trimmed = line.trimmed();
            if (trimmed.startsWith(QStringLiteral("output:")) || trimmed.startsWith(QStringLiteral("salida:"))) {
                QString val = trimmed.section(QLatin1Char(':'), 1).trimmed();
                if (m_currentPreset != val) {
                    m_currentPreset = val;
                    Q_EMIT currentPresetChanged();
                }
                break;
            }
        }
    }
}

void EasyEffectsManager::loadPreset(const QString &presetName)
{
    if (presetName.isEmpty()) return;
    QProcess::startDetached(QStringLiteral("easyeffects"), {QStringLiteral("-l"), presetName});
    m_currentPreset = presetName;
    Q_EMIT currentPresetChanged();
}
