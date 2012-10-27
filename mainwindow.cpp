#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gui/conjugation.h"

#include <QtCore/QCoreApplication>
#ifdef DEBUG
#include <QDebug>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
#ifdef Q_WS_MAEMO_5
    this->setAttribute(Qt::WA_Maemo5StackedWindow);
    this->setWindowFlags(Qt::Window);
#endif
    ui->setupUi(this);
    setupcodedUI();
    initverbiste();
}

void MainWindow::setupcodedUI()
{
    cent = centralWidget();
    mlayout = new QVBoxLayout;
    btlayout = new QHBoxLayout;

    resultPages = new QTabWidget;
    resultPages->setTabPosition(QTabWidget::West);
    mlayout->addWidget(resultPages);

    btnClear = new QPushButton;
    btnClear->setIcon(QIcon("/usr/share/icons/hicolor/64x64/hildon/general_delete.png"));
    wordinput = new QLineEdit;
    labVerb = new QLabel();
    labVerb->setMinimumWidth(250);
    btlayout->addWidget(btnClear);
    btlayout->addWidget(labVerb);
    btlayout->addWidget(wordinput);
    btnLookup = new QPushButton;  // Lookup button
    btnLookup->setIcon(QIcon("/usr/share/icons/hicolor/64x64/hildon/general_search.png"));
    btlayout->addWidget(btnLookup);

    mlayout->addLayout(btlayout);
    cent->setLayout(mlayout);

    // Clear the word input when Clear button is tapped
    connect(btnClear, SIGNAL(clicked()), this, SLOT(clearResults()));

    connect(wordinput, SIGNAL(returnPressed()), this, SLOT(startLookup()));
    connect(btnLookup, SIGNAL(clicked()), this, SLOT(startLookup()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete freVerbDic;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
    wordinput->setFocus();
}

void  MainWindow::initverbiste()
{
    langCode = "fr";
}

void MainWindow::startLookup()
{
    QString input = wordinput->text();

    FrenchVerbDictionary::Language lang = FrenchVerbDictionary::parseLanguageCode(langCode);
    if (lang != FrenchVerbDictionary::FRENCH)
    {
        // TODO: If lang code is not supported?
    }

    /* Create verb dictionary, accept non-accent input */
    freVerbDic = new FrenchVerbDictionary(true);

    /* Get input word to look up */
    const std::string word = input.toLower().toUtf8().constData();

    /*
     *  For each possible deconjugation, take the infinitive form and
     *  obtain its complete conjugation.
     */
    std::vector<InflectionDesc> v;
    bool includePronouns = FALSE;    // TODO: Will get this value from external
    bool isItalian = FALSE;          // TODO: Will get this value from external

    freVerbDic->deconjugate(word, v);

    std::string prevUTF8Infinitive, prevTemplateName;
    for (std::vector<InflectionDesc>::const_iterator it = v.begin();
         it != v.end(); it++)
    {
        const InflectionDesc &d = *it;
        VVVS conjug;
        getConjugation(freVerbDic, d.infinitive, d.templateName, conjug, includePronouns);

        if (conjug.size() == 0           // if no tenses
            || conjug[0].size() == 0     // if no infinitive tense
            || conjug[0][0].size() == 0  // if no person in inf. tense
            || conjug[0][0][0].empty())  // if infinitive string empty
        {
            continue;
        }

        std::string utf8Infinitive = conjug[0][0][0];
        if (utf8Infinitive == prevUTF8Infinitive && d.templateName == prevTemplateName)
            // This result is duplicated
            continue;

        /* Show on GUI */
        ResultPage *rsp = addResultPage(utf8Infinitive);
        //QString infVerb = QString::fromUtf8(utf8Infinitive.c_str());
        //labVerb->setText(infVerb);

        /* Get modes and tenses of the verb */
        int i = 0;
        for (VVVS::const_iterator t = conjug.begin();
             t != conjug.end(); t++, i++)
        {
            if (i == 1)
                i = 4;
            else if (i == 11)
                i = 12;
            assert(i >= 0 && i < 16);

            int row = i / 4;
            int col = i % 4;

            std::string utf8TenseName = getTenseNameForTableCell(row, col, isItalian);
            if (utf8TenseName.empty())
                continue;

#ifdef DEBUG
            qDebug() << utf8TenseName.c_str();
#endif
            QVBoxLayout *cell = makeResultCell(*t, utf8TenseName, word, freVerbDic);
            rsp->grid->addLayout(cell, row, col);
#ifdef DEBUG
            qDebug() << "Add cell to " << row << col;
#endif
        }
        rsp->packContent();
        prevUTF8Infinitive = utf8Infinitive;
        prevTemplateName = d.templateName;
    }
}

ResultPage* MainWindow::addResultPage(const std::string &labelText)
{
    ResultPage *rp = new ResultPage();
    QString label = QString::fromUtf8(labelText.c_str());
    resultPages->addTab(rp->page, label);
    return rp;
}

void MainWindow::clearResults()
{
    wordinput->clear();
    labVerb->clear();

    while (resultPages->count()) {
        int lastIndex = resultPages->count() - 1;
        resultPages->widget(lastIndex)->deleteLater();
        resultPages->removeTab(lastIndex);
    }
    wordinput->setFocus();
}

QVBoxLayout* MainWindow::makeResultCell(const VVS &tenseIterator,
                                        const std::string &tenseName,
                                        const std::string &inputWord,
                                        FrenchVerbDictionary *verbDict)
{
    QLabel *tenseLabel = new QLabel();
    tenseLabel->setText(QString::fromUtf8(tenseName.c_str()));
    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(tenseLabel);
    std::string conjugated = createTableCellText(
                                    *verbDict,
                                    tenseIterator,
                                    inputWord,
                                    "",
                                    "");
    QLabel *conjResult = new QLabel();
    conjResult->setText(QString::fromUtf8(conjugated.c_str()));
    vbox->addWidget(conjResult);
    return vbox;
}

/**** For ResultPage class ****/
ResultPage::ResultPage()
    : page(new QScrollArea),
      grid(new QGridLayout)
{
}

void ResultPage::packContent()
{
    QWidget *immediate = new QWidget();
    immediate->setLayout(grid);
    page->setWidget(immediate);
}

