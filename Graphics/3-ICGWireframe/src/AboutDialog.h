#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QSvgWidget>
#include <QVBoxLayout>

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowTitle("About ICGWireframe");
        setFixedSize(450, 400);

        QVBoxLayout *layout = new QVBoxLayout(this);

        QSvgWidget *logo =
            new QSvgWidget(":resources/icons/sphere.svg", this);
        logo->setFixedSize(100, 100);

        QLabel *titleLabel = new QLabel("<h2>ICGWireframe</h2>");
        QLabel *infoLabel = new QLabel(
            "<p>An application for rendering rotation figures with b-spline generatix</p>"
            "<p><b>Version:</b> 1.0</p>"
            "<p><b>Features:</b></p>"
            "<ul>"
            "<li>B-Spline editor</li>"
            "<li>3D viewport with zoom and figure rotation</li>"
            "<li>Scene export and import</li>"
            "<li></li>"
            "</ul>"
            "<p><b>Made by:</b> Pavel Urdin, NSU FIT 2025, Group 22201</p>");
        infoLabel->setWordWrap(true);

        QPushButton *closeButton = new QPushButton("Close");
        connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

        layout->addWidget(logo);
        layout->addWidget(titleLabel);
        layout->addWidget(infoLabel);
        layout->addWidget(closeButton, 0, Qt::AlignCenter);
    }
};

#endif  // !ABOUT_DIALOG_H
