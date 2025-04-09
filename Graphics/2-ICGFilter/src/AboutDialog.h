#ifndef ABOUT_DIALOG
#define ABOUT_DIALOG

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
        setWindowTitle("About ICGPaint");
        setFixedSize(450, 500);

        QVBoxLayout *layout = new QVBoxLayout(this);

        QSvgWidget *logo =
            new QSvgWidget(":resources/icons/image-edit.svg", this);
        logo->setFixedSize(100, 100);

        QLabel *titleLabel = new QLabel("<h2>ICGPaint</h2>");
        QLabel *infoLabel = new QLabel(
            "<p>An application for filtering images.</p>"
            "<p><b>Version:</b> 1.0</p>"
            "<p><b>Features:</b></p>"
            "<ul>"
            "<li>Conversion to grayscale image</li>"
            "<li>Inversion</li>"
            "<li>Gaussian blur</li>"
            "<li>Chromatic aberration</li>"
            "<li>Dithering (Floyd-Steinberg & Ordered)</li>"
            "<li>Edge detection (Sobel & Roberts filters)</li>"
            "<li>Emboss</li>"
            "<li>Gamma correction</li>"
            "<li>Sharpen filter</li>"
            "<li>Watercolor filter</li>"
            "<li>Rotation</li>"
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

#endif  // !ABOUT_DIALOG
