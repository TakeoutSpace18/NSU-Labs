#ifndef SCENEDATA_H
#define SCENEDATA_H

#include <QVector2D>
#include <QVector3D>
#include <QJsonObject>
#include <QJsonArray>

class SceneData
{
public:
    SceneData();

    const QVector<QVector2D>& anchorPoints() const;
    int generatrixNumber() const;
    int generatrixSegmentsNumber() const;
    int circleSegmentsNumber() const;
    const QVector3D& rotation() const;
    qreal zoom() const;

    void addAnchorPoint(QVector2D point);
    void removeAnchorPoint(size_t index);
    void changeAnchorPoint(size_t index, QVector2D point);

    void setGeneratrixNumber(int value);
    void setGeneratrixSegmentsNumber(int value);
    void setCircleSegmentsNumber(int value);
    void setRotation(const QVector3D& angles);
    void setZoom(qreal value);

    QJsonObject toJson() const;
    static QSharedPointer<SceneData> FromJson(const QJsonObject& obj);

private:
    QVector<QVector2D> m_anchorPoints;
    int m_generatrixNumber;
    int m_generatrixSegmentsNumber;
    int m_circleSegmentsNumber;
    QVector3D m_rotation;
    qreal m_zoom;
};


#endif // !SCENEDATA_H
