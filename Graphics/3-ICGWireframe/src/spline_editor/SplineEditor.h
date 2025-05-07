#ifndef BSPLINEEDITOR_H
#define BSPLINEEDITOR_H

#include <qevent.h>
#include <qsharedpointer.h>

#include <QDialog>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include <QMouseEvent>
#include <QPointF>
#include <QWheelEvent>

#include "AnchorPointItem.h"
#include "SceneData.h"

namespace Ui
{
class BSplineEditor;
}

class SplineEditor : public QDialog
{
    Q_OBJECT

public:
    SplineEditor(QSharedPointer<SceneData> sceneData,
                 QWidget *parent = nullptr);

    explicit SplineEditor(QWidget *parent = nullptr);

    ~SplineEditor();

    QSharedPointer<const SceneData> sceneData() const;

private slots:
    void on_pointsListWidget_currentRowChanged(int currentRow);
    void on_autoApplyCheckBox_stateChanged(int state);
    void on_applyButton_clicked();
    void on_okButton_clicked();
    void on_pointsSpinBox_valueChanged(int value);
    void on_generatixNumberSpinBox_valueChanged(int value);
    void on_generatixSegmentsSpinBox_valueChanged(int value);
    void on_circleSegmentsSpinBox_valueChanged(int value);

    void updateAnchorPointDisplay();
    void updateSplineDisplay();

public slots:
    void anchorPointMoved(AnchorPointItem *item);
    void anchorPointSelectChanged(AnchorPointItem *item, bool selected);

signals:
    void apply(QSharedPointer<SceneData> sceneData);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupScene();
    void setSceneData(QSharedPointer<SceneData> sceneData);
    void drawAxes();
    void removeAnchorPoint(int index);
    void removeSelectedAnchorPoints();
    void setupConnections();

    void addAnchorPoint();
    void addAnchorPoint(QVector2D point);
    void addAnchorPointItem(QVector2D point);

    Ui::BSplineEditor *ui;
    QGraphicsScene *m_scene;

    QList<AnchorPointItem *> m_anchorPoints;
    QList<QGraphicsLineItem *> m_splineSegments;

    QSet<int> m_selectedAnchorPointsIndices;

    QGraphicsLineItem *m_xAxis;
    QGraphicsLineItem *m_yAxis;

    bool m_autoApply;

    QSharedPointer<SceneData> m_sceneData;
};

#endif  // BSPLINEEDITOR_H
