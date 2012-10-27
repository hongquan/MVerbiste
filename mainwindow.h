#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#include <QtGui/QTableWidget>

/* Verbiste */
#include <iostream>
#include <vector>
#include <string.h>
#include <verbiste/FrenchVerbDictionary.h>
#include "gui/conjugation.h"
using namespace verbiste;

namespace Ui {
    class MainWindow;
    class ResultPage;
}

class ResultPage
{
public:
    QScrollArea *page;
    QGridLayout *grid;

    ResultPage();
    // No destructor because this object does not own the two widgets.
    void packContent();

private:
    QWidget *immediate;
};

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
    void initverbiste();
    QVBoxLayout* makeResultCell(const VVS &tenseIterator,
                                     const std::string &tenseName,
                                     const std::string &inputWord,
                                     FrenchVerbDictionary *verbDict);

public slots:
    void startLookup();

private:
    Ui::MainWindow *ui;
    QWidget     *cent;               // Central widget
    QVBoxLayout *mlayout;            // Main layout
    QTabWidget  *resultPages;
    QHBoxLayout *btlayout;           // Layout to pack the functional buttons
    QPushButton *btnClear;           // Clear button
    QLineEdit   *wordinput;          //  Word input
    QPushButton *btnLookup;          // Lookup button
    std::string langCode;
    FrenchVerbDictionary *freVerbDic;

    ResultPage* addResultPage(const std::string &labelText);

private slots:
    void clearResults();
    void startAgain();
};


#endif // MAINWINDOW_H
