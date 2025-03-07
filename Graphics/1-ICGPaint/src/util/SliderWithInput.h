#ifndef SLIDER_WITH_INPUT_H
#define SLIDER_WITH_INPUT_H

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>

class SliderWithInput : public QWidget {
    Q_OBJECT

public:
    explicit SliderWithInput(int minValue = 1, int maxValue = 100,
                             QWidget *parent = nullptr);
    int value() const;
    void setValue(int newValue);

signals:
    void valueChanged(int newValue);

private:
    QSlider *m_slider;
    QLineEdit *m_inputField;
};

#endif // SLIDER_WITH_INPUT_H

