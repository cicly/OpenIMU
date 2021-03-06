#include "ApplicationMenubar.h"

ApplicationMenuBar::ApplicationMenuBar(QWidget *parent) : QMenuBar(parent)
{
    this->setStyleSheet("background-color:rgba(230, 233, 239,0.6);");
    this->setMinimumWidth(parent->width());
    parent = parent;
    fichier = new QMenu(tr("&Fichier"));

    QAction* actionAjouterEnregistrement = new QAction(tr("&Ajouter Enregistrement"), fichier);
    actionAjouterEnregistrement->setShortcut(QKeySequence("Ctrl+R"));

    QAction* actionOuvrir = new QAction(tr("&Charger Enregistrement"),fichier);
    actionOuvrir->setShortcut(QKeySequence("Ctrl+O"));

    QAction* actionQuitter = new QAction(tr("&Quitter"),fichier);
    actionQuitter->setShortcut(QKeySequence("Ctrl+Q"));

    fichier->addAction(actionAjouterEnregistrement);
    fichier->addAction(actionOuvrir);
    fichier->addSeparator();
    fichier->addAction(actionQuitter);

    vue = new QMenu("&Vue");
    QAction* actionDonneeBrutes = new QAction(tr("&Données brutes"),vue);
    actionDonneeBrutes->setShortcut(QKeySequence("Ctrl+D"));
    vue->addAction(actionDonneeBrutes);

    aide = new QMenu("&Aide");
    QAction* actionAPropos = new QAction(tr("À &propos"),aide);
    actionAPropos->setShortcut(QKeySequence("Ctrl+A"));
    QAction* actionAide = new QAction(tr("&Aide"),aide);
    actionAide->setShortcut(QKeySequence("Ctrl+H"));
    aide->addAction(actionAPropos);
    aide->addAction(actionAide);

    actionEnglish = new QAction(tr("&English"),aide);
    actionFrench = new QAction(tr("&French"),aide);

    actionEnglish->setCheckable(true);
    actionFrench->setCheckable(true);

    preference = new QMenu(tr("&Preferences"));
    preferenceLangue = preference->addMenu(tr("&Langue"));
    preferenceLangue->addAction(actionEnglish);
    preferenceLangue->addAction(actionFrench);

    this->addMenu(fichier);
    this->addMenu(vue);
    this->addMenu(preference);
    this->addMenu(aide);

    //connect(actionDonneeBrutes, SIGNAL(triggered()), parent, SLOT(displayRawAccData()));
    connect(actionOuvrir, SIGNAL(triggered()), parent, SLOT(openFile()));
    connect(actionAjouterEnregistrement, SIGNAL(triggered()), parent, SLOT(openRecordDialog()));
    connect(actionQuitter,SIGNAL(triggered()),parent,SLOT(closeWindow()));
    connect(actionAPropos,SIGNAL(triggered()),parent,SLOT(openAbout()));
    connect(actionAide,SIGNAL(triggered()),parent,SLOT(openHelp()));
    connect(actionEnglish,SIGNAL(triggered()),parent,SLOT(setApplicationInEnglish()));
    connect(actionFrench,SIGNAL(triggered()),parent,SLOT(setApplicationInFrench()));
}

void ApplicationMenuBar::setUncheck(const QString uncheck)
{
    if(uncheck == "French")
    {
        actionFrench->setChecked(false);
    }
    if(uncheck == "English")
    {
         actionEnglish->setChecked(false);
    }
}
