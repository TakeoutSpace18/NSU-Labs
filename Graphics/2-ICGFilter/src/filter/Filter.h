#ifndef FILTER_H
#define FILTER_H

#include <QAction>
#include <QImage>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QWidget>

class Filter
{
public:
    Filter(const QString& displayName) : displayName(displayName) {}

    virtual ~Filter() = default;

    virtual void apply(const QImage& input, QImage& output) const = 0;

    const QString& getDisplayName() const { return displayName; }

    void setParameter(const QString& name, const QVariant& value)
    {
        parameters[name] = value;
    }

    QVariant getParameter(const QString& name) const
    {
        return parameters.value(name);
    }

    QMap<QString, QVariant> getAllParameters() const { return parameters; }

    void setAllParameters(const QMap<QString, QVariant>& params)
    {
        parameters = params;
    }

    struct ParameterInfo
    {
        QString name;
        QString displayName;
        QVariant defaultValue;
        QVariant minValue;
        QVariant maxValue;
        QString type;         // "int", "double", "bool", "choice"
        QStringList choices;  // Only used if type is "choice"
    };

    virtual QList<ParameterInfo> getParameterInfoList() const = 0;

    virtual QAction* createAction(QObject* parent = nullptr) = 0;

protected:
    QMap<QString, QVariant> parameters;
    QString displayName;
};

#endif  // FILTER_H
