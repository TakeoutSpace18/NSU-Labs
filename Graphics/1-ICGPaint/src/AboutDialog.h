#ifndef ABOUT_DIALOG
#define ABOUT_DIALOG

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>

class AboutDialog : public QDialog {
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("About ICGPaint");
        setFixedSize(450, 400);

        QVBoxLayout *layout = new QVBoxLayout(this);
        
        QLabel *logoLabel = new QLabel(this);
        QPixmap logo(":/resources/icons/icgpaint.svg");
        logoLabel->setPixmap(logo.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logoLabel->setAlignment(Qt::AlignCenter);

        QLabel *titleLabel = new QLabel("<h2>ICGPaint</h2>");
        QLabel *infoLabel = new QLabel(
            "<p>A simple paint application with various drawing tools.</p>"
            "<p><b>Version:</b> 1.0</p>"
            "<p><b>Features:</b></p>"
            "<ul>"
            "<li>Brush Tool - Freehand drawing</li>"
            "<li>Line Tool - Draw straight lines</li>"
            "<li>Fill Tool - Fill areas with color</li>"
            "<li>Stamp Tool - Draw polygons and stars with adjustable parameters</li>"
            "</ul>"
            "<p><b>Made by:</b> Pavel Urdin, NSU FIT 2025, Group 22201</p>"
        );
        infoLabel->setWordWrap(true);

        QPushButton *closeButton = new QPushButton("Close");
        connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

        layout->addWidget(logoLabel);
        layout->addWidget(titleLabel);
        layout->addWidget(infoLabel);
        layout->addWidget(closeButton, 0, Qt::AlignCenter);
    }
};

#endif // !ABOUT_DIALOG
