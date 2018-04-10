#include "coindatastation.h"

const QHash<QString, CoinPtr> &CoinDataStation::getTrackedCoins() const
{
    return trackedCoins;
}

const QHash<QString, DataObserverPtr> &CoinDataStation::getObservers() const
{
    return observers;
}

QHash<QString, DataObserverPtr> &CoinDataStation::getRefObservers()
{
    return observers;
}

CoinDataStation::CoinDataStation(QObject *parent)
    : QObject(parent),
      networkManager{new QNetworkAccessManager(this)},
      repliesHashTable{},
      repliesMapper{new QSignalMapper(this)},
      replyCount{}
{
    //QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotLastValueOfALLCoins(QNetworkReply*)));
    QObject::connect(repliesMapper, SIGNAL(mapped(QString)), this , SLOT(replyFromServerArrived(QString)));
}

void CoinDataStation::getLastValueOfAllCoins()
{
    QNetworkReply* reply{networkManager->get(QNetworkRequest(API.getLastValueOfAllCoinsUrl()))};
    QString replyKey{QString("%1|%2|%3").arg(QString::number(replyCount++)).arg("ALL").arg("lastvalue")};
    repliesHashTable.insert(replyKey, reply);
    repliesMapper->setMapping(reply, replyKey);
    QObject::connect(reply, SIGNAL(finished()), repliesMapper, SLOT(map()));

}

void CoinDataStation::parseLastValueOfALLCoins(QNetworkReply *replyFromServer)
{
    //qDebug() << replyFromServer->readAll();
    QString jsonString{replyFromServer->readAll()};
    QJsonDocument jsonDoc{QJsonDocument::fromJson(jsonString.toUtf8())};
    if (jsonDoc.isEmpty())
        qDebug() << "Empty";
    else if (jsonDoc.isObject())
        qDebug() << "Object";
    else if (jsonDoc.isArray())
        qDebug() << "Array";
    QJsonArray jsonArray{jsonDoc.array()};
    for (int i{}; i < jsonArray.at(0).toArray().size(); i++)
    {
        // Parse Coin Object and its last Value
        QJsonObject currentObj{jsonArray.at(0).toArray().at(i).toObject()};
        QJsonObject lastValue{currentObj.value("last_values").toObject()};
        // Get the value we want from the key we provide
        QString symbol{currentObj.value("symbol").toString()};
        QString name{currentObj.value("name").toString()};
        long availableSupply{currentObj.value("available_supply").toString().toLong()};

        //qDebug() << currentValue.value("price").toDouble() << currentValue.value("timeStamp").toDouble();
        CoinPtr currentCoin{make_shared<Coin>(name, symbol)};
        currentCoin->setAvailable_supply(availableSupply);
        currentCoin->getLastValue().timeStamp.setTime_t(lastValue.value("timeStamp").toInt());
        currentCoin->getLastValue().price = lastValue.value("price").toDouble();
        currentCoin->getLastValue().marketcap = lastValue.value("marketcap").toDouble();
        currentCoin->getLastValue().volume_24h = lastValue.value("volume24").toString().toDouble();
        //qDebug() << lastValue.value("volume24").toString();
        currentCoin->getLastValue().changedPercent_1h = lastValue.value("change_1h").toString();
        currentCoin->getLastValue().changedPercent_24h = lastValue.value("change_24h").toString();
        trackedCoins.insert(symbol, currentCoin);
        //qDebug() << jsonArray.at(0).toArray().at(i).toObject().value("symbol").toString();
        //
    }
    emit parseLastValueOfAllCoinsCompleted(trackedCoins);
}

void CoinDataStation::getMaxValueIn7DaysOfAllCoins()
{

}

void CoinDataStation::parseMaxValueIn7DaysOfAllCoins(QNetworkReply *replyFromServer)
{
    QString jsonString{replyFromServer->readAll()};
    QJsonDocument jsonDoc{QJsonDocument::fromJson(jsonString.toUtf8())};
    if (jsonDoc.isEmpty())
        qDebug() << "Empty";
    else if (jsonDoc.isObject())
        qDebug() << "Object";
    else if (jsonDoc.isArray())
        qDebug() << "Array";
    QJsonArray jsonArray{jsonDoc.array()};
    for (int i{}; i < jsonArray.at(0).toArray().size(); i++)
    {
        // Parse Coin Object and its last Value
        QJsonObject currentObj{jsonArray.at(0).toArray().at(i).toObject()};
        QJsonObject lastValue{currentObj.value("last_values").toObject()};
        // Get the value we want from the key we provide
        QString symbol{currentObj.value("symbol").toString()};
        QString name{currentObj.value("name").toString()};
        long availableSupply{currentObj.value("available_supply").toString().toLong()};

        //qDebug() << currentValue.value("price").toDouble() << currentValue.value("timeStamp").toDouble();
        CoinPtr currentCoin{make_shared<Coin>(name, symbol)};
        currentCoin->setAvailable_supply(availableSupply);
        currentCoin->getLastValue().timeStamp.setTime_t(lastValue.value("timeStamp").toInt());
        currentCoin->getLastValue().price = lastValue.value("price").toDouble();
        currentCoin->getLastValue().marketcap = lastValue.value("marketcap").toDouble();
        currentCoin->getLastValue().volume_24h = lastValue.value("volume24").toString().toDouble();
        //qDebug() << lastValue.value("volume24").toString();
        currentCoin->getLastValue().changedPercent_1h = lastValue.value("change_1h").toString();
        currentCoin->getLastValue().changedPercent_24h = lastValue.value("change_24h").toString();
        trackedCoins.insert(symbol, currentCoin);
        //qDebug() << jsonArray.at(0).toArray().at(i).toObject().value("symbol").toString();
        //
    }
    emit parseMaxValueIn7DaysOfAllCoinsCompleted(trackedCoins);
}

void CoinDataStation::replyFromServerArrived(const QString& replyDiscription)
{
    parseLastValueOfALLCoins(repliesHashTable.value(replyDiscription));
}

void CoinDataStation::createANewCollectionWith(const QString &collectionName, const QStringList &contents)
{
    CoinCollectionPtr newCollection{make_shared<CoinCollection>(CoinCollection(collectionName))};
    for (const QString& symbol : contents)
    {
        if(trackedCoins.contains(symbol))
            newCollection->addNewTrackedCoin(symbol, trackedCoins.value(symbol));
    }
    registerObserver(newCollection);
    qDebug() << observers.contains(collectionName);
    emit creatingANewCollectionCompleted(collectionName);
}


void CoinDataStation::registerObserver(const DataObserverPtr &observer)
{
    if (!observers.contains(observer->getName()))
        observers.insert(observer->getName(), observer);
}

void CoinDataStation::removeObserver(const DataObserverPtr &observer)
{
}

void CoinDataStation::notifyAllObservers()
{
}
