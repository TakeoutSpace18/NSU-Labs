#include "BSplineEditor.h"
#include "ui_BSplineEditor.h"
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>

AnchorPointItem::AnchorPointItem(qreal x, qreal y, int index, QGraphicsItem *parent)
    : QGraphicsEllipseItem(-5, -5, 10, 10, parent), m_index(index)
{
    setPos(x, y);
    setBrush(QBrush(Qt::blue));
    setPen(QPen(Qt::black));
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

void AnchorPointItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_startPos = pos();
    QGraphicsEllipseItem::mousePressEvent(event);
}

void AnchorPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsEllipseItem::mouseMoveEvent(event);
}

void AnchorPointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsEllipseItem::mouseReleaseEvent(event);
    
    // Notify parent that we've been moved
    if (pos() != m_startPos) {
        BSplineEditor* editor = qobject_cast<BSplineEditor*>(scene()->parent());
        if (editor) {
            editor->anchorPointMoved(this);
        }
    }
}

QVariant AnchorPointItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        // Update the position display in real-time
        BSplineEditor* editor = qobject_cast<BSplineEditor*>(scene()->parent());
        if (editor) {
            editor->anchorPointMoved(this);
        }
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}

BSplineEditor::BSplineEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BSplineEditor),
    m_autoApply(false)
{
    ui->setupUi(this);
    setupScene();
    setupConnections();
    
    addAnchorPoint(-50, 0);
    addAnchorPoint(50, 0);
    
    updateAnchorPointDisplay();
}

BSplineEditor::~BSplineEditor()
{
    delete ui;
}

void BSplineEditor::setupScene()
{
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-200, -200, 400, 400);
    ui->graphicsView->setScene(m_scene);
    
    drawAxes();
    
    ui->graphicsView->viewport()->setMouseTracking(true);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    
    ui->graphicsView->scale(1.0, 1.0);
}

void BSplineEditor::drawAxes()
{
    m_xAxis = m_scene->addLine(-1000, 0, 1000, 0, QPen(Qt::gray, 1, Qt::DashLine));
    m_yAxis = m_scene->addLine(0, -1000, 0, 1000, QPen(Qt::gray, 1, Qt::DashLine));
    
    for (int i = -1000; i <= 1000; i += 50) {
        if (i == 0) continue;
        
        m_scene->addLine(-1000, i, 1000, i, QPen(Qt::lightGray, 0.5, Qt::DotLine));
        m_scene->addLine(i, -1000, i, 1000, QPen(Qt::lightGray, 0.5, Qt::DotLine));
    }
}

void BSplineEditor::setupConnections()
{
    // Connect UI elements
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void BSplineEditor::on_addAnchorButton_clicked()
{
    double x = ui->xSpinBox->value();
    double y = ui->ySpinBox->value();
    addAnchorPoint(x, y);
    updateAnchorPointDisplay();
    updateSplineDisplay();
}

void BSplineEditor::addAnchorPoint(qreal x, qreal y)
{
    int index = m_anchorPoints.size();
    AnchorPointItem* point = new AnchorPointItem(x, y, index);
    m_scene->addItem(point);
    m_anchorPoints.append(point);
    
    // Add to the list widget
    QString pointText = QString("Point %1 (%2, %3)").arg(index).arg(x).arg(y);
    ui->pointsListWidget->addItem(pointText);
}

void BSplineEditor::on_removeAnchorButton_clicked()
{
    int currentRow = ui->pointsListWidget->currentRow();
    if (currentRow >= 0 && currentRow < m_anchorPoints.size()) {
        removeAnchorPoint(currentRow);
        updateAnchorPointDisplay();
        updateSplineDisplay();
    } else {
        QMessageBox::warning(this, "Warning", "Please select a point to remove");
    }
}

void BSplineEditor::removeAnchorPoint(int index)
{
    if (index >= 0 && index < m_anchorPoints.size()) {
        // Remove from scene and delete
        AnchorPointItem* point = m_anchorPoints.at(index);
        m_scene->removeItem(point);
        delete point;
        
        // Remove from list
        m_anchorPoints.removeAt(index);
        delete ui->pointsListWidget->takeItem(index);
        
        // Renumber remaining points
        for (int i = 0; i < m_anchorPoints.size(); i++) {
            m_anchorPoints[i]->setIndex(i);
            QString pointText = QString("Point %1 (%2, %3)")
                .arg(i)
                .arg(m_anchorPoints[i]->pos().x())
                .arg(m_anchorPoints[i]->pos().y());
            ui->pointsListWidget->item(i)->setText(pointText);
        }
    }
}

void BSplineEditor::updateAnchorPointDisplay()
{
    // Update the list widget with current anchor points
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

void BSplineEditor::on_pointsListWidget_currentRowChanged(int currentRow)
{
    if (currentRow >= 0 && currentRow < m_anchorPoints.size()) {
        AnchorPointItem* point = m_anchorPoints.at(currentRow);
        ui->xSpinBox->setValue(point->pos().x());
        ui->ySpinBox->setValue(point->pos().y());
        
        // Highlight the selected point
        for (auto* p : m_anchorPoints) {
            p->setBrush(QBrush(Qt::blue));
        }
        point->setBrush(QBrush(Qt::red));
    }
}

void BSplineEditor::on_nSegmentsSpinBox_valueChanged(int value)
{
    if (m_autoApply) {
        updateSplineDisplay();
    }
}

void BSplineEditor::on_xSpinBox_valueChanged(double value)
{
    int currentRow = ui->pointsListWidget->currentRow();
    if (currentRow >= 0 && currentRow < m_anchorPoints.size()) {
        AnchorPointItem* point = m_anchorPoints.at(currentRow);
        point->setPos(value, point->pos().y());
        updateAnchorPointDisplay();
        
        if (m_autoApply) {
            updateSplineDisplay();
        }
    }
}

void BSplineEditor::on_ySpinBox_valueChanged(double value)
{
    int currentRow = ui->pointsListWidget->currentRow();
    if (currentRow >= 0 && currentRow < m_anchorPoints.size()) {
        AnchorPointItem* point = m_anchorPoints.at(currentRow);
        point->setPos(point->pos().x(), value);
        updateAnchorPointDisplay();
        
        if (m_autoApply) {
            updateSplineDisplay();
        }
    }
}

void BSplineEditor::anchorPointMoved(AnchorPointItem* item)
{
    int index = item->getIndex();
    if (index >= 0 && index < m_anchorPoints.size()) {
        // Update list widget
        QString pointText = QString("Point %1 (%2, %3)")
            .arg(index)
            .arg(item->pos().x())
            .arg(item->pos().y());
        ui->pointsListWidget->item(index)->setText(pointText);
        
        // If this is the currently selected point, update the spinboxes
        if (ui->pointsListWidget->currentRow() == index) {
            ui->xSpinBox->setValue(item->pos().x());
            ui->ySpinBox->setValue(item->pos().y());
        }
        
        // Update the spline if auto-apply is enabled
        if (m_autoApply) {
            updateSplineDisplay();
        }
    }
}

void BSplineEditor::updateSplineDisplay()
{
    // Remove existing spline segments
    for (auto* segment : m_splineSegments) {
        m_scene->removeItem(segment);
        delete segment;
    }
    m_splineSegments.clear();
    
    // Here you would implement the actual B-spline calculation
    // For now, we'll just connect the points with straight lines
    if (m_anchorPoints.size() >= 2) {
        for (int i = 0; i < m_anchorPoints.size() - 1; i++) {
            QPointF p1 = m_anchorPoints.at(i)->pos();
            QPointF p2 = m_anchorPoints.at(i+1)->pos();
            QGraphicsLineItem* line = m_scene->addLine(
                p1.x(), p1.y(), p2.x(), p2.y(),
                QPen(Qt::red, 2)
            );
            m_splineSegments.append(line);
        }
    }
    
    // The actual B-spline implementation would be added here
    // using the N segments parameter from ui->nSegmentsSpinBox->value()
}

void BSplineEditor::on_zoomInButton_clicked()
{
    ui->graphicsView->scale(1.2, 1.2);
}

void BSplineEditor::on_zoomOutButton_clicked()
{
    ui->graphicsView->scale(0.8, 0.8);
}

void BSplineEditor::on_resetViewButton_clicked()
{
    ui->graphicsView->resetTransform();
}

void BSplineEditor::on_autoApplyCheckBox_stateChanged(int state)
{
    m_autoApply = (state == Qt::Checked);
    ui->applyButton->setEnabled(!m_autoApply);
    
    if (m_autoApply) {
        updateSplineDisplay();
    }
}

void BSplineEditor::on_applyButton_clicked()
{
    updateSplineDisplay();
}

void BSplineEditor::on_okButton_clicked()
{
    // Apply changes and close the dialog
    updateSplineDisplay();
    accept();
}
