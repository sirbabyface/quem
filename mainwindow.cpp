#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QTimer>
#include <QDir>
#include <QSound>
#include <algorithm>
#include <ctime>
#include <QKeyEvent>

#include "mainwindow.h"

ImageViewer::ImageViewer()
{
    imageLabel = new QLabel(this);
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->setFont(QFont( "Arial", 120, QFont::Bold));
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setText(tr("Get ready Dino!"));


    setCentralWidget(imageLabel);

    createTimer();
    createSounds();
    readImages();

    setWindowTitle(tr("Quem é Quem"));
    resize(500, 400);

    imageCorrect = false;
    evaluatedImage = true;
    finished = false;

    n_yes = n_no = 0;
}


void ImageViewer::changeImage()
{
    if(noImages) {
        imageLabel->clear();
        imageLabel->setFont(QFont( "Arial", 30, QFont::Bold));
        imageLabel->setText(tr("No images found!\nA folder \"images\" is required.\n" ));
        return;
    }

    if(evaluatedImage)
    {
        if(currentImage == images.end())
        {
            // finish
            imageLabel->clear();
            imageLabel->setText("Correct: " + QString::number(n_yes) + "\nIncorrect: " + QString::number(n_no));
            finished = true;
            return;
        }

        // Move to the next image
        QImage image((*currentImage));
        if (image.isNull()) {
            imageLabel->clear();
            imageLabel->setText(tr("problem opening image ." ));
        }
        else {
            imageLabel->setPixmap(QPixmap::fromImage(image));
        }
        currentImage++; // iterate to next image
        evaluatedImage = false;
        timer->start(imageDelay);
    }
    else
    {
        QString filename;
        if(imageCorrect) {
            filename = ":yes";
            n_yes++;
            correct->play();
        }
        else {
            filename = ":no";
            n_no++;
            wrong->play();
        }
        QImage image(filename);
        if (image.isNull()) {
            imageLabel->clear();
            imageLabel->setText(tr("problem opening image ok image ." ));
        }
        else {
            imageLabel->setPixmap(QPixmap::fromImage(image));
        }
        evaluatedImage = true;
        imageCorrect = false;
        timer->start(resultDelay);
    }
}

void ImageViewer::keyPressEvent(QKeyEvent* e)
{
    switch(e->key())
    {
        case Qt::Key_S:
          imageCorrect = true;
          changeImage();
          break;
        case Qt::Key_N:
            imageCorrect = false;
            changeImage();
            break;
        case Qt::Key_Escape:
            if(!finished) {
                finished = true;
                evaluatedImage = true;
                currentImage = images.end();
                changeImage();
            }
            break;
        case Qt::Key_R:
            if(finished) {
                currentImage = images.begin();
                evaluatedImage = true;
                finished = false;
                imageCorrect = false;
                changeImage();
                n_no = n_yes = 0;
            }
            break;
    }
    if(!timer->isActive()){
        changeImage();
    }
};

void ImageViewer::createSounds()
{
    wrong = new QSound(":/wrong");
    correct = new QSound(":/correct");
}


void ImageViewer::createTimer()
{
    resultDelay = 2000;
    imageDelay = 4000;
    timer = new QTimer(this);
    timer->setInterval(imageDelay);
    timer->setSingleShot(true);

    connect(timer, SIGNAL(timeout()), this, SLOT(changeImage()));
}

void ImageViewer::readImages()
{
    QStringList filters;
    filters << "*.png" << "*.jpg";
    QDir dir = QDir::current();
    if(dir.cd("images")){
        dir.setNameFilters(filters);
        dir.setSorting(QDir::Name);

        images = dir.entryList();

        srand(time(NULL));
        std::random_shuffle(images.begin(), images.end());

        currentImage = images.begin();
        noImages = false;
    }
    else {
        noImages = true;
    }
}
