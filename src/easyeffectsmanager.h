#pragma once

#include <QObject>
#include <QStringList>
#include <QProcess>

class EasyEffectsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList presets READ presets NOTIFY presetsChanged)
    Q_PROPERTY(QString currentPreset READ currentPreset NOTIFY currentPresetChanged)
    Q_PROPERTY(bool isAvailable READ isAvailable NOTIFY availabilityChanged)

public:
    explicit EasyEffectsManager(QObject *parent = nullptr);

    QStringList presets() const { return m_presets; }
    QString currentPreset() const { return m_currentPreset; }
    bool isAvailable() const { return m_isAvailable; }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void loadPreset(const QString &presetName);

Q_SIGNALS:
    void presetsChanged();
    void currentPresetChanged();
    void availabilityChanged();

private:
    void checkAvailability();
    void parsePresets();
    void detectCurrentPreset();

    QStringList m_presets;
    QString m_currentPreset;
    bool m_isAvailable{false};
};
