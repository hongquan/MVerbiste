#include "about.h"

AboutDialog::AboutDialog(const char *iconFile, const QString &title)
    : QDialog()
{
    setWindowTitle("About");
    QHBoxLayout *mlayout = new QHBoxLayout();
    QLabel *iconshow = new QLabel();
    iconshow->setPixmap(QPixmap(iconFile));
    mlayout->addWidget(iconshow);

    rlayout = new QVBoxLayout();
    QLabel *tit = new QLabel(title);
    tit->setStyleSheet("QLabel {font-size: 40px}");
    rlayout->addWidget(tit);
    mlayout->addLayout(rlayout);
    setLayout(mlayout);
}

void AboutDialog::setIntro(const QString &text)
{
    // Place to the second item of rlayout
    if (rlayout->count() == 1) {
        // No item yet, add one
        rlayout->addWidget(new QLabel(text));
    }
    else {
        // Second item has existed, replace it.
        QWidget *child = (QWidget *)rlayout->takeAt(0);
        delete child;
        rlayout->insertWidget(0, new QLabel(text));
    }
}

void AboutDialog::addAuthor(const QString &name)
{
    // Place from the third item of rlayout
    if (rlayout->count() == 1) {
        // No second item, add empty one.
        rlayout->addWidget(new QLabel(""));
    }
    // Add from the third
    rlayout->addWidget(new QLabel(name));
}
