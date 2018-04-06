#include "collectioncoinmanagementdialog.h"
#include "ui_collectioncoinmanagementdialog.h"

CollectionCoinManagementDialog::CollectionCoinManagementDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CollectionCoinManagementDialog)
{
    ui->setupUi(this);
    this->setToolTip("GTK");
    ui->lineEditCollectionName->setToolTip("Collection name must not be empty");
    ui->listWidgetAvailableCoins->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listWidgetTrackedCoins->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QObject::connect(ui->pushButtonMoveItemsToRight, &QPushButton::clicked, this, &CollectionCoinManagementDialog::moveSelectedCoinsToTrackedCoins);
    QObject::connect(ui->pushButtonMoveItemToLeft, &QPushButton::clicked, this, &CollectionCoinManagementDialog::moveSelectedCoinsToAvailableCoins);
    QObject::connect(ui->pushButtonMoveAllToRight, &QPushButton::clicked, this, &CollectionCoinManagementDialog::moveAllCoinsToTrackedCoins);
    QObject::connect(ui->pushButtonMoveAllToLeft, &QPushButton::clicked, this, &CollectionCoinManagementDialog::moveAllCoinsToAvailableCoins);

    QObject::connect(ui->buttonControllerBox, &QDialogButtonBox::accepted, this, &CollectionCoinManagementDialog::checkNewCollectionContents);
    QObject::connect(this, &CollectionCoinManagementDialog::accepted, this, &CollectionCoinManagementDialog::finishedCurrentAction);
    QObject::connect(this, &CollectionCoinManagementDialog::rejected, this, &CollectionCoinManagementDialog::finishedCurrentAction);
}

CollectionCoinManagementDialog::~CollectionCoinManagementDialog()
{
    delete ui;
}

void CollectionCoinManagementDialog::showAddNewCollectionCoin()
{
    this->setWindowTitle("Add new collection");
    this->setWindowIcon(QIcon(":/ico/collectionAdd.ico"));
    QHashIterator<QString, CoinPtr> iterQHash{trackedCoins};
    while (ui->listWidgetAvailableCoins->count() > 0)
    {
        ui->listWidgetAvailableCoins->takeItem(0);
    }

    while (ui->listWidgetTrackedCoins->count() > 0)
    {
        ui->listWidgetTrackedCoins->takeItem(0);
    }
    while(iterQHash.hasNext())
    {
        iterQHash.next();
        ui->listWidgetAvailableCoins->addItem(iterQHash.value()->getDisplayItem().get());
    }
    this->show();
    this->raise();
}

void CollectionCoinManagementDialog::showConfigureCurrentCollectionCoin()
{
    this->setWindowTitle("Configure collection");
    this->setWindowIcon(QIcon(":/ico/collectionSetting.ico"));
    this->show();
    this->raise();
}

void CollectionCoinManagementDialog::getAvailableCoins(const QHash<QString, CoinPtr> &availableCoins)
{
    trackedCoins = availableCoins;
    emit finishedPreloadAvailableCoins();
    /*QHashIterator<QString, CoinPtr> iterQHash{availableCoins};
    while(iterQHash.hasNext())
    {
        iterQHash.next();
        ui->listWidgetAvailableCoins->addItem(iterQHash.value()->getDisplayItem().get());
    }*/
    //ui->listWidgetAvailableCoins->show();
    //showAddNewCollectionCoin();
}

void CollectionCoinManagementDialog::loadAvailableCoinsAndTrackedCoins(const CoinDataStation &coinDataStation, const CoinCollection &coinCollection)
{
    QHash<QString, CoinPtr> availableCoinsFromServer{coinDataStation.getTrackedCoins()};
    QHash<QString, CoinPtr> trackedCoinFromCurrentCollection{coinCollection.getTrackedCoins()};
    // Removed old Items
    ui->listWidgetAvailableCoins->clear();
    ui->listWidgetTrackedCoins->clear();
    // Let's put all Coins in the right position
    QHashIterator<QString, CoinPtr> iterQHash{availableCoinsFromServer};
    while(iterQHash.hasNext())
    {
        iterQHash.next();
        if (trackedCoinFromCurrentCollection.contains(iterQHash.key()))
            ui->listWidgetAvailableCoins->addItem(iterQHash.value()->getDisplayItem().get());
        else
            ui->listWidgetTrackedCoins->addItem(iterQHash.value()->getDisplayItem().get());
    }

}

void CollectionCoinManagementDialog::moveSelectedCoinsToAvailableCoins()
{
    if(ui->listWidgetTrackedCoins->selectedItems().count() < 1)
        qDebug() << "Cannot move anything";
    else
    {
        QList<QListWidgetItem*> currentSelectedDisplayItems{ui->listWidgetTrackedCoins->selectedItems()};
        for (const QListWidgetItem* item : currentSelectedDisplayItems)
        {
            int itemRow{ui->listWidgetTrackedCoins->row(item)};
            ui->listWidgetAvailableCoins->addItem(ui->listWidgetTrackedCoins->takeItem(itemRow));
        }
    }
}

void CollectionCoinManagementDialog::moveAllCoinsToTrackedCoins()
{
    //qDebug() << ui->listWidgetAvailableCoins->count();
    while (ui->listWidgetAvailableCoins->count() > 0)
    {
        ui->listWidgetTrackedCoins->addItem(ui->listWidgetAvailableCoins->takeItem(0));
    }
}

void CollectionCoinManagementDialog::moveAllCoinsToAvailableCoins()
{
    while (ui->listWidgetTrackedCoins->count() > 0)
    {
        ui->listWidgetAvailableCoins->addItem(ui->listWidgetTrackedCoins->takeItem(0));
    }
}

void CollectionCoinManagementDialog::checkNewCollectionContents()
{
    if (ui->lineEditCollectionName->text().isEmpty())
        QMessageBox::warning(this, "Cannot create new collection", "Collection name must not be empty");
    else if (ui->listWidgetTrackedCoins->count() == 0)
        QMessageBox::warning(this, "Cannot create new collection", "You must choose at least one item for collection contents");
    else
        this->accept();

}

void CollectionCoinManagementDialog::moveSelectedCoinsToTrackedCoins()
{
    if(ui->listWidgetAvailableCoins->selectedItems().count() < 1)
        qDebug() << "Cannot move anything";
    else
    {
        QList<QListWidgetItem*> currentSelectedDisplayItems{ui->listWidgetAvailableCoins->selectedItems()};
        for (const QListWidgetItem* item : currentSelectedDisplayItems)
        {
            int itemRow{ui->listWidgetAvailableCoins->row(item)};
            ui->listWidgetTrackedCoins->addItem(ui->listWidgetAvailableCoins->takeItem(itemRow));
        }
    }

}


/*void CollectionCoinManagementDialog::focusInEvent(QFocusEvent *event)
{
    QHelpEvent* hevent{new QHelpEvent(QEvent::ToolTip, QPoint(ui->lineEditCollectionName->pos().x(), ui->lineEditCollectionName->pos().y()), QPoint(QCursor::pos().x(), QCursor::pos().y()))};
    QApplication::postEvent(this, hevent);
    QDialog::focusInEvent(event);
}*/
