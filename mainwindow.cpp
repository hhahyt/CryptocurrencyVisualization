#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_collectioncoinmanagementdialog.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dataStation{new CoinDataStation(this)},
    collectionManagerDialog{new CollectionCoinManagementDialog(this)}
{
    ui->setupUi(this);
    //------------------------------------------------------
    QObject::connect(ui->toolButtonAddNewList, SIGNAL(clicked(bool)), collectionManagerDialog, SLOT(showAddNewCollectionCoin()));
    QObject::connect(ui->toolButtonAddNewList, SIGNAL(clicked(bool)), this , SLOT(disableAllToolButtonRelativeWithCollectionList()));

    QObject::connect(ui->toolButtonConfigList, SIGNAL(clicked(bool)), collectionManagerDialog, SLOT(showConfigureCurrentCollectionCoin()));
    QObject::connect(ui->toolButtonConfigList, SIGNAL(clicked(bool)), this, SLOT(disableAllToolButtonRelativeWithCollectionList()));
    //QObject::connect(ui->toolButtonAddNewList, SIGNAL(clicked(bool)), dataStation, SLOT(getLastValueOfAllCoins()));
    QObject::connect(dataStation, &CoinDataStation::parseLastValueOfAllCoinsCompleted, collectionManagerDialog, &CollectionCoinManagementDialog::getAvailableCoins);
    QObject::connect(collectionManagerDialog, &CollectionCoinManagementDialog::finishedPreloadAvailableCoins,  this, &MainWindow::show);
    QObject::connect(collectionManagerDialog, SIGNAL(finishedCurrentAction()), this, SLOT(enableAllToolButtonRelativeWithCollectionList()));

    // First action
    dataStation->getLastValueOfAllCoins();
    QObject::connect(collectionManagerDialog, &CollectionCoinManagementDialog::requestToCreateANewCollectionWith, dataStation, &CoinDataStation::createANewCollectionWith);
    QObject::connect(dataStation, &CoinDataStation::creatingANewCollectionCompleted, this, &MainWindow::loadNewCollectionContents);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::disableAllToolButtonRelativeWithCollectionList()
{
    ui->toolButtonAddNewList->setDisabled(true);
    ui->toolButtonConfigList->setDisabled(true);
    ui->toolButtonRemoveList->setDisabled(true);
}

void MainWindow::enableAllToolButtonRelativeWithCollectionList()
{
    ui->toolButtonAddNewList->setEnabled(true);
    ui->toolButtonConfigList->setEnabled(true);
    ui->toolButtonRemoveList->setEnabled(true);
}

void MainWindow::loadNewCollectionContents(const QString &collectionName)
{
    ui->comboBoxCryptoList->addItem(QIcon(":/ico/collection.ico"), collectionName);
    for (int index{}; index < collectionManagerDialog->getUi()->listWidgetTrackedCoins->count(); index++)
    {
        QListWidgetItem* item{new QListWidgetItem(collectionManagerDialog->getUi()->listWidgetTrackedCoins->item(index)->icon(), collectionManagerDialog->getUi()->listWidgetTrackedCoins->item(index)->text())};
        ui->listWidgetTrackedCoins->addItem(item);
        //ui->listWidgetTrackedCoins->addItem("FERER");
        qDebug() << index;
    }
    ui->listWidgetTrackedCoins->show();
}
