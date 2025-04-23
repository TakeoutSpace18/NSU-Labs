#ifndef BSPLINEEDITOR_H
#define BSPLINEEDITOR_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QList>
#include <QPointF>
#include <QMouseEvent>
#include <QWheelEvent>

namespace Ui {
class BSplineEditor;
}

class AnchorPointItem : public QGraphicsEllipseItem {
public:
    AnchorPointItem(qreal x, qreal y, int index, QGraphicsItem *parent = nullptr);
    int getIndex() const { return m_index; }
    void setIndex(int index) { m_index = index; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    int m_index;
    QPointF m_startPos;
};

class BSplineEditor : public QDialog
{
    Q_OBJECT

public:
    explicit BSplineEditor(QWidget *parent = nullptr);
    ~BSplineEditor();

private slots:
    void on_addAnchorButton_clicked();
    void on_removeAnchorButton_clicked();
    void on_pointsListWidget_currentRowChanged(int currentRow);
    void on_nSegmentsSpinBox_valueChanged(int value);
    void on_xSpinBox_valueChanged(double value);
    void on_ySpinBox_valueChanged(double value);
    void on_zoomInButton_clicked();
    void on_zoomOutButton_clicked();
    void on_resetViewButton_clicked();
    void on_autoApplyCheckBox_stateChanged(int state);
    void on_applyButton_clicked();
    void on_okButton_clicked();

    void updateAnchorPointDisplay();
    void updateSplineDisplay();

public slots:
    void anchorPointMoved(AnchorPointItem* item);

private:
    Ui::BSplineEditor *ui;
    QGraphicsScene *m_scene;
    QList<AnchorPointItem*> m_anchorPoints;
    QList<QGraphicsLineItem*> m_splineSegments;
    QGraphicsLineItem *m_xAxis;
    QGraphicsLineItem *m_yAxis;
    bool m_autoApply;

    void setupScene();
    void drawAxes();
    void addAnchorPoint(qreal x, qreal y);
    void removeAnchorPoint(int index);
    void setupConnections();
};

#endif // BSPLINEEDITOR_H
