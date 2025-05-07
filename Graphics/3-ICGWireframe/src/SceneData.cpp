#include "SceneData.h"

#include <stdexcept>

SceneData::SceneData()
    : m_generatrixNumber(2),
      m_generatrixSegmentsNumber(1),
      m_circleSegmentsNumber(1),
      m_rotation(0.0, 0.0, 0.0),
      m_zoom(1.0)
{
    addAnchorPoint(QVector2D(-60, 0));
    addAnchorPoint(QVector2D(-30, 30));
    addAnchorPoint(QVector2D(30, 30));
    addAnchorPoint(QVector2D(60, 0));
}

const QVector<QVector2D>& SceneData::anchorPoints() const
{
    return m_anchorPoints;
}

int SceneData::generatrixNumber() const
{
    return m_generatrixNumber;
}

int SceneData::generatrixSegmentsNumber() const
{
    return m_generatrixSegmentsNumber;
}

int SceneData::circleSegmentsNumber() const
{
    return m_circleSegmentsNumber;
}

const QVector3D& SceneData::rotation() const
{
    return m_rotation;
}

qreal SceneData::zoom() const
{
    return m_zoom;
}

void SceneData::addAnchorPoint(QVector2D point)
{
    m_anchorPoints.push_back(point);
}

void SceneData::removeAnchorPoint(size_t index)
{
    if (m_anchorPoints.size() - 1 < 4) {
        throw std::invalid_argument("Anchor points must be at least 4.");
    }
    m_anchorPoints.removeAt(index);
}

void SceneData::changeAnchorPoint(size_t index, QVector2D point)
{
    m_anchorPoints.replace(index, point);
}

void SceneData::setGeneratrixNumber(int value)
{
    if (value < 2) {
        throw std::invalid_argument("Generatrix number must be at least 2.");
    }
    m_generatrixNumber = value;
}

void SceneData::setGeneratrixSegmentsNumber(int value)
{
    if (value < 1) {
        throw std::invalid_argument(
            "Generatrix interpolation number must be at least 1.");
    }
    m_generatrixSegmentsNumber = value;
}

void SceneData::setCircleSegmentsNumber(int value)
{
    if (value < 1) {
        throw std::invalid_argument(
            "Circle interpolation number must be at least 1.");
    }
    m_circleSegmentsNumber = value;
}

void SceneData::setRotation(const QVector3D& angles)
{
    m_rotation = angles;
}

void SceneData::setZoom(qreal value)
{
    if (value > -0.1) {
        throw std::invalid_argument("Zoom must not be greater than -0.1");
    }
    m_zoom = value;
}

QJsonObject SceneData::toJson() const
{
    QJsonObject obj;

    QJsonArray anchorsArray;
    for (const auto& point : m_anchorPoints) {
        QJsonObject pointObj;
        pointObj["x"] = point.x();
        pointObj["y"] = point.y();
        anchorsArray.append(pointObj);
    }
    obj["anchorPoints"] = anchorsArray;

    obj["generatrixNumber"] = m_generatrixNumber;
    obj["generatrixInterpolationNumber"] = m_generatrixSegmentsNumber;
    obj["circleInterpolationNumber"] = m_circleSegmentsNumber;

    obj["rotationX"] = m_rotation.x();
    obj["rotationY"] = m_rotation.y();
    obj["rotationZ"] = m_rotation.z();

    obj["zoom"] = m_zoom;

    return obj;
}

QSharedPointer<SceneData> SceneData::FromJson(const QJsonObject& obj)
{
    QSharedPointer<SceneData> scene =
        QSharedPointer<SceneData>(new SceneData());
    scene->m_anchorPoints.clear();

    if (!obj.contains("anchorPoints") || !obj["anchorPoints"].isArray()) {
        throw std::invalid_argument("Missing or invalid anchorPoints array.");
    }

    QJsonArray anchorsArray = obj["anchorPoints"].toArray();
    if (anchorsArray.size() < 4) {
        throw std::invalid_argument("Anchor points number should be at least 4");
    }

    for (const auto& value : anchorsArray) {
        if (value.isObject()) {
            QJsonObject pointObj = value.toObject();
            scene->addAnchorPoint(
                QVector2D(pointObj["x"].toDouble(), pointObj["y"].toDouble()));
        }
    }

    scene->setGeneratrixNumber(obj["generatrixNumber"].toInt());
    scene->setGeneratrixSegmentsNumber(
        obj["generatrixInterpolationNumber"].toInt());
    scene->setCircleSegmentsNumber(obj["circleInterpolationNumber"].toInt());

    QVector3D rotation(obj["rotationX"].toDouble(), obj["rotationY"].toDouble(),
                       obj["rotationZ"].toDouble());
    scene->setRotation(rotation);

    scene->setZoom(obj["zoom"].toDouble());

    return scene;
}
