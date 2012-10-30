#ifndef ABOUT_H
#define ABOUT_H

#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>

class AboutDialog: public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(const char *iconFile, const QString &title);

    void setIntro(const QString &text);
    void addAuthor(const QString &name);

private:
    QVBoxLayout *rlayout;
};

#endif // ABOUT_H
