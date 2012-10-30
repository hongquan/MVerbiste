#include "about.h"

AboutDialog::AboutDialog(const char *iconFile, const char *title)
    : QDialog()
{
    setWindowTitle(title);
    QHBoxLayout *mlayout = new QHBoxLayout();
    QLabel *iconshow = new QLabel();
    iconshow->setPixmap(QPixmap(iconFile));

    rlayout = new QVBoxLayout();
    mlayout->addWidget(iconshow);
    mlayout->addLayout(rlayout);
    setLayout(mlayout);
}

void AboutDialog::setIntro(const QString &text)
{
    // Place to the first item of rlayout
    if (rlayout->count() == 0) {
        // No item yet, add one
        rlayout->addWidget(new QLabel(text));
    }
    else {
        // First item has existed, replace it.
        QWidget *child = (QWidget *)rlayout->itemAt(0);
        rlayout->removeWidget(child);
        rlayout->insertWidget(0, new QLabel(text));
    }
}

void AboutDialog::addAuthor(const QString &name)
{
    // Place from the second item of rlayout
    if (rlayout->count() == 0) {
        // No first item, add empty one.
        rlayout->addWidget(new QLabel(""));
    }
    // Add from the second
    rlayout->addWidget(new QLabel(name));
}
