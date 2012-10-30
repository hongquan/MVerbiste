# Add files and directories to ship with the application 
# by adapting the examples below.
# file1.source = myfile
# dir1.source = mydir
DEPLOYMENTFOLDERS = xmldata icon # file1 dir1
xmldata.source = data
icon.source = icons

symbian:TARGET.UID3 = 0xE214283E

# Smart Installer package's UID
# This UID is from the protected range 
# and therefore the package will fail to install if self-signed
# By default qmake uses the unprotected range value if unprotected UID is defined for the application
# and 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
# CONFIG += mobility
# MOBILITY +=

SOURCES += main.cpp mainwindow.cpp \
    verbiste/Trie.cpp \
    verbiste/misc-types.cpp \
    verbiste/FrenchVerbDictionary.cpp \
    verbiste/c-api.cpp \
    gui/conjugation.cpp \
    about.cpp
HEADERS += mainwindow.h \
    verbiste/Trie.h \
    verbiste/misc-types.h \
    verbiste/FrenchVerbDictionary.h \
    verbiste/c-api.h \
    gui/conjugation.h \
    about.h
FORMS += mainwindow.ui

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    qtc_packaging/debian_fremantle/rules \
    qtc_packaging/debian_fremantle/README \
    qtc_packaging/debian_fremantle/copyright \
    qtc_packaging/debian_fremantle/control \
    qtc_packaging/debian_fremantle/compat \
    qtc_packaging/debian_fremantle/changelog \
    verbiste/verbiste.dox \
    verbiste/Makefile.in \
    verbiste/Makefile.am \
    gui/Makefile.in \
    gui/Makefile.am

# To build verbiste
unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += libxml-2.0

DEFINES += VERSTR=\\\"1.0\\\"

# For verbiste
DEFINES += ICONV_CONST=

simulator {    # Build to run on simulator.
    DEFINES += LIBDATADIR=\\\"$$PWD/data\\\"
    DEFINES +=ICONFILE=\\\"$$PWD/icons/mverbiste160.png\\\"
}
else {
    # installPrefix must be explicitly exported from deployment.pri first
    DEFINES += LIBDATADIR=\\\"$${installPrefix}/data\\\"
    DEFINES +=ICONFILE=\\\"$${installPrefix}/icons/mverbiste160.png\\\"
}
