#include <QDir>
#include "DBmanager.h"
#include <QSqlQuery>
#include <QVariant>
#include <iostream>


DatabaseManager::DatabaseManager(QObject *parent){

}

bool DatabaseManager::openDB()
    {
    // Find QSLite driver
    db = QSqlDatabase::addDatabase("QSQLITE");

    #ifdef Q_OS_LINUX
    // NOTE: We have to store database file into user home folder in Linux
    QString path(QDir::home().path());
    path.append(QDir::separator()).append("my.db.sqlite");
    path = QDir::toNativeSeparators(path);
    db.setDatabaseName(path);
    #else
    // NOTE: File exists in the application private folder, in Symbian Qt implementation
    db.setDatabaseName("my.db.sqlite");
    #endif

    // Open databasee
    return db.open();
    }

void DatabaseManager::closeDB()
{
    if(db.isOpen())
        db.close();

}

bool DatabaseManager::isOpen()
{
  return db.isOpen();

}

QSqlError DatabaseManager::lastError()
    {
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return db.lastError();
    }


int DatabaseManager::insertChunk(u_int64_t hash, int offset, int length, QString path)
    {
    int newId = -1;
    bool ret = false;

    if (db.isOpen())
        {
        //http://www.sqlite.org/autoinc.html
        // NULL = is the keyword for the autoincrement to generate next value
//cout << hash <<offset<<length<<path.toStdString() <<endl;
        //QSqlQuery query;
       // ret = query.exec(QString("insert into chunk values(NULL,'%1','%2',%3)")
       // .arg(offset).arg(length).arg(path));

QSqlQuery query;
ret = query.exec(QString("insert into chunk values(%1,%2,%3,'%4')")
.arg(hash).arg(length).arg(offset).arg(path));


        // Get database given autoincrement value
        if (ret)
            {
            // http://www.sqlite.org/c3ref/last_insert_rowid.html
            newId = query.lastInsertId().toInt();
            }

        }
    return newId;
    }

bool DatabaseManager::getChunk(u_int64_t hash, Chunk &chunk)
{

    bool ret = false;
     if (db.isOpen()){
    QSqlQuery query(QString("select * from chunk where id = %1").arg(hash));
    if (query.next())
        {
        chunk.setHash(query.value(0).toLongLong());
        chunk.setOffset(query.value(2).toInt());
        chunk.setLength(query.value(1).toInt());

        QString str1 = query.value(3).toString();
          QByteArray ba = str1.toLatin1();
          const char *path = ba.data();
        chunk.setPath(path);
        ret = true;
        }
//cout << chunk.getHash() << chunk.getOffset() << chunk.getLength() << chunk.getPath() << endl;
     }
    return ret;
}

int DatabaseManager::insertChecksum(QString checksum, QString path)
    {
    int newId = -1;
    bool ret = false;

    if (db.isOpen())
        {

QSqlQuery query;
ret = query.exec(QString("insert into checksum values('%1','%2')").arg(checksum).arg(path));


        // Get database given autoincrement value
        if (ret)
            {
            // http://www.sqlite.org/c3ref/last_insert_rowid.html
            newId = query.lastInsertId().toInt();
            }

        }
    return newId;
    }
bool DatabaseManager::findChecksum(QString checksum,string &value)
{

    bool ret = false;
     if (db.isOpen()){
    QSqlQuery query(QString("select * from checksum where checksum = '%1'").arg(checksum));
    if (query.next())
        {
        QString str1 = query.value(1).toString();
          QByteArray ba = str1.toLatin1();
          value = ba.data();


        ret = true;
        }

     }
    return ret;
}
