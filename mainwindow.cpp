#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gui/conjugation.h"

#include <QtCore/QCoreApplication>

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
    //mlayout = qobject_cast<QVBoxLayout *>(cent->layout());
    mlayout = new QVBoxLayout;
    btlayout = new QHBoxLayout;

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    mlayout->addWidget(scrollArea);

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
    QObject::connect(btnClear, SIGNAL(clicked()), wordinput, SLOT(clear()));
    QObject::connect(btnClear, SIGNAL(clicked()), labVerb, SLOT(clear()));

    QObject::connect(wordinput, SIGNAL(returnPressed()), this, SLOT(startLookup()));
    QObject::connect(btnLookup, SIGNAL(clicked()), this, SLOT(startLookup()));
}

MainWindow::~MainWindow()
{
    delete ui;
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
        labVerb->setText(QString::fromUtf8(utf8Infinitive.c_str()));
    }
}

void  MainWindow::initverbiste()
{
    langCode = "fr";
}
