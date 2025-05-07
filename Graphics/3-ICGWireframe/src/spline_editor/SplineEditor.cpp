#include "SplineEditor.h"

#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>

#include "BSpline.h"
#include "SceneData.h"
#include "ui_SplineEditor.h"

SplineEditor::SplineEditor(QSharedPointer<SceneData> sceneData, QWidget* parent)
    : QDialog(parent),
      ui(new Ui::BSplineEditor),
      m_autoApply(true),
      m_sceneData(sceneData)
{
    ui->setupUi(this);
    ui->applyButton->setDisabled(true);

    setupScene();
    setupConnections();

    setSceneData(sceneData);
}

SplineEditor::SplineEditor(QWidget* parent)
    : SplineEditor(QSharedPointer<SceneData>(new SceneData()), parent)
{}

SplineEditor::~SplineEditor()
{
    delete ui;
}

void SplineEditor::setupScene()
{
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-30, -30, 30, 30);
    ui->graphicsView->setScene(m_scene);

    connect(m_scene, &QGraphicsScene::changed, this,
            [this]() { m_scene->setSceneRect(m_scene->itemsBoundingRect()); });

    drawAxes();

    ui->graphicsView->viewport()->setMouseTracking(true);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    ui->graphicsView->scale(1.0, 1.0);
}

void SplineEditor::drawAxes()
{
    m_xAxis =
        m_scene->addLine(-5000, 0, 5000, 0, QPen(Qt::black, 1, Qt::DashLine));
    m_yAxis =
        m_scene->addLine(0, -5000, 0, 5000, QPen(Qt::gray, 1, Qt::DashLine));

    for (int i = -5000; i <= 5000; i += 50) {
        if (i == 0) {
            continue;
        }

        m_scene->addLine(-5000, i, 5000, i,
                         QPen(Qt::lightGray, 0.5, Qt::DotLine));
        m_scene->addLine(i, -5000, i, 5000,
                         QPen(Qt::lightGray, 0.5, Qt::DotLine));
    }
}

void SplineEditor::setupConnections()
{
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void SplineEditor::addAnchorPoint()
{
    auto points = m_sceneData->anchorPoints();
    QVector2D a = points.at(points.size() - 2);
    QVector2D b = points.at(points.size() - 1);

    QVector2D direction = (b - a).normalized();

    qreal offset = 50;
    QVector2D newPoint = b + offset * direction;

    addAnchorPoint(newPoint);
}

void SplineEditor::addAnchorPoint(QVector2D point)
{
    m_sceneData->addAnchorPoint(point);
    addAnchorPointItem(point);

    if (m_autoApply) {
        emit apply(m_sceneData);
    }
}

void SplineEditor::addAnchorPointItem(QVector2D pos)
{
    int index = m_anchorPoints.size();
    AnchorPointItem* point = new AnchorPointItem(pos, index);
    m_scene->addItem(point);
    m_anchorPoints.append(point);

    updateAnchorPointDisplay();
    updateSplineDisplay();
}

void SplineEditor::removeAnchorPoint(int index)
{
    if (m_sceneData->anchorPoints().size() == 4) {
        return;
    }

    m_selectedAnchorPointsIndices.remove(index);
    QSet<int> newSet;
    for (auto i : m_selectedAnchorPointsIndices) {
        newSet.insert(i < index ? i : i - 1);
    }
    m_selectedAnchorPointsIndices = newSet;

    AnchorPointItem* point = m_anchorPoints.at(index);
    m_scene->removeItem(point);
    delete point;

    m_anchorPoints.removeAt(index);

    m_sceneData->removeAnchorPoint(index);

    ui->pointsSpinBox->setValue(m_sceneData->anchorPoints().size());

    updateAnchorPointDisplay();
    updateSplineDisplay();

    if (m_autoApply) {
        emit apply(m_sceneData);
    }
}

void SplineEditor::updateAnchorPointDisplay()
{
    ui->pointsListWidget->clear();
    for (int i = 0; i < m_anchorPoints.size(); i++) {
        AnchorPointItem* point = m_anchorPoints.at(i);
        QString pointText = QString("Point %1 (%2, %3)")
                                .arg(i)
                                .arg(point->pos().x())
                                .arg(point->pos().y());
        ui->pointsListWidget->addItem(pointText);
    }
}

void SplineEditor::on_pointsListWidget_currentRowChanged(int currentRow)
{
    if (currentRow >= 0 && currentRow < m_anchorPoints.size()) {
        AnchorPointItem* point = m_anchorPoints.at(currentRow);
        point->setSelected(true);

        // Highlight the selected point
        for (auto* p : m_anchorPoints) {
            p->setBrush(QBrush(Qt::blue));
        }
        point->setBrush(QBrush(Qt::red));
    }
}

void SplineEditor::anchorPointMoved(AnchorPointItem* item)
{
    int index = m_anchorPoints.indexOf(item);
    QString pointText = QString("Point %1 (%2, %3)")
                            .arg(index)
                            .arg(item->pos().x())
                            .arg(item->pos().y());
    ui->pointsListWidget->item(index)->setText(pointText);
    m_sceneData->changeAnchorPoint(index,
                                   QVector2D(item->pos().x(), item->pos().y()));

    updateSplineDisplay();

    if (m_autoApply) {
        emit apply(m_sceneData);
    }
}

void SplineEditor::updateSplineDisplay()
{
    // Remove existing spline segments
    for (auto* segment : m_splineSegments) {
        m_scene->removeItem(segment);
        delete segment;
    }
    m_splineSegments.clear();

    auto points = BSpline::Generate(m_sceneData->anchorPoints(),
                                    m_sceneData->generatrixSegmentsNumber());

    if (points.size() >= 2) {
        for (int i = 0; i < points.size() - 1; i++) {
            QVector2D p1 = points.at(i);
            QVector2D p2 = points.at(i + 1);

            QGraphicsLineItem* line = m_scene->addLine(
                p1.x(), p1.y(), p2.x(), p2.y(), QPen(Qt::black, 2));
            line->setZValue(-1);
            m_splineSegments.append(line);
        }
    }
}

void SplineEditor::on_autoApplyCheckBox_stateChanged(int state)
{
    m_autoApply = (state == Qt::Checked);
    ui->applyButton->setEnabled(!m_autoApply);

    if (m_autoApply) {
        // TODO: update scene
    }
}

void SplineEditor::on_applyButton_clicked()
{
    updateSplineDisplay();
    emit apply(m_sceneData);
}

void SplineEditor::on_okButton_clicked()
{
    // Apply changes and close the dialog
    updateSplineDisplay();
    emit apply(m_sceneData);
    accept();
}

void SplineEditor::removeSelectedAnchorPoints()
{
    while (!m_selectedAnchorPointsIndices.isEmpty()) {
        if (m_sceneData->anchorPoints().size() == 4) {
            break;
        }

        int idx = m_selectedAnchorPointsIndices.values().front();
        removeAnchorPoint(idx);
    }
}

void SplineEditor::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Up:
            for (int idx : m_selectedAnchorPointsIndices) {
                m_anchorPoints.at(idx)->moveBy(0, -20);
            }
            break;
        case Qt::Key_Down:
            for (int idx : m_selectedAnchorPointsIndices) {
                m_anchorPoints.at(idx)->moveBy(0, 20);
            }
            break;
        case Qt::Key_Left:
            for (int idx : m_selectedAnchorPointsIndices) {
                m_anchorPoints.at(idx)->moveBy(-20, 0);
            }
            break;
        case Qt::Key_Right:
            for (int idx : m_selectedAnchorPointsIndices) {
                m_anchorPoints.at(idx)->moveBy(20, 0);
            }
            break;
        case Qt::Key_Delete:
            removeSelectedAnchorPoints();
            break;
        case Qt::Key_Enter:
            accept();
            break;
    }
}

void SplineEditor::anchorPointSelectChanged(AnchorPointItem* item,
                                            bool selected)
{
    if (selected) {
        m_selectedAnchorPointsIndices.insert(m_anchorPoints.indexOf(item));
    }
    else {
        m_selectedAnchorPointsIndices.remove(m_anchorPoints.indexOf(item));
    }
}

void SplineEditor::on_pointsSpinBox_valueChanged(int value)
{
    qsizetype oldValue = m_sceneData->anchorPoints().size();
    qsizetype count = qAbs(oldValue - value);

    if (value > oldValue) {
        for (qsizetype i = 0; i < count; ++i) {
            addAnchorPoint();
        }
    }
    else if (value < oldValue) {
        for (qsizetype i = 0; i < count; ++i) {
            removeAnchorPoint(oldValue - i - 1);
        }
    }
}

QSharedPointer<const SceneData> SplineEditor::sceneData() const
{
    return m_sceneData;
}

void SplineEditor::setSceneData(QSharedPointer<SceneData> sceneData)
{
    m_sceneData = sceneData;

    m_selectedAnchorPointsIndices = QSet<int>();
    m_anchorPoints = QList<AnchorPointItem *>();

    for (const auto point : sceneData->anchorPoints()) {
        addAnchorPointItem(point);
    }

    ui->pointsSpinBox->setValue(sceneData->anchorPoints().size());
    ui->generatixNumberSpinBox->setValue(sceneData->generatrixNumber());
    ui->circleSegmentsSpinBox->setValue(sceneData->circleSegmentsNumber());
    ui->generatixSegmentsSpinBox->setValue(
        sceneData->generatrixSegmentsNumber());

    updateSplineDisplay();
    updateAnchorPointDisplay();
}

void SplineEditor::on_generatixNumberSpinBox_valueChanged(int value)
{
    m_sceneData->setGeneratrixNumber(value);

    if (m_autoApply) {
        emit apply(m_sceneData);
    }
}

void SplineEditor::on_generatixSegmentsSpinBox_valueChanged(int value)
{
    m_sceneData->setGeneratrixSegmentsNumber(value);
    updateSplineDisplay();

    if (m_autoApply) {
        emit apply(m_sceneData);
    }
}

void SplineEditor::on_circleSegmentsSpinBox_valueChanged(int value)
{
    m_sceneData->setCircleSegmentsNumber(value);

    if (m_autoApply) {
        emit apply(m_sceneData);
    }
}
