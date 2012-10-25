#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QScrollArea>
#include <QtGui/QLabel>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    // Setup UI by coding, not by QtDesigner
    void setupcodedUI();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

public slots:
    void startLookup();

private:
    Ui::MainWindow *ui;
    QWidget     *cent;               // Central widget
    QVBoxLayout *mlayout;            // Main layout
    QHBoxLayout *btlayout;           // Layout to pack the functional buttons
    QPushButton *btnClear;           // Clear button
    QLineEdit   *wordinput;          //  Word input
    QPushButton *btnLookup;          // Lookup button
    QLabel      *labVerb;
};

#endif // MAINWINDOW_H
