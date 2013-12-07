#include <QDir>
#include "databasehandler.h"
#include <QSqlQuery>
#include <QVariant>
#include <iostream>


DatabaseManager::DatabaseManager(QObject *parent){

}

bool DatabaseManager::openDB()
    {
     db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("localhost");
        db.setDatabaseName("cloudsync");
        db.setUserName("root");
        db.setPassword("root");

        return db.open();
    }

bool DatabaseManager::isOpen(){
    if(db.isOpen())
        return true;
    else
        return false;
}


QSqlError DatabaseManager::lastError()
    {
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return db.lastError();
    }


int DatabaseManager::insertChunk(u_int64_t hash, int offset, int length, int fileid)
    {
    int newId = -1;
    bool ret = false;

    if (db.isOpen())
        {

        QSqlQuery query;
            query.prepare("INSERT INTO chunk_data (hash,length,offset,fileid) VALUES(:hash,:length,:offset,:fileid)");
                query.bindValue(":hash", (qulonglong)hash);
                query.bindValue(":length", length);
                query.bindValue(":offset", offset);
                query.bindValue(":fileid", fileid);
              ret=  query.exec();

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
    //QSqlQuery query(QString("select * from chunk_data where hash = %1").arg(hash));
    QSqlQuery query(QString("SELECT offset , length, path FROM chunk_data AS A JOIN file AS B ON A.fileid = B.id where hash='%1'").arg(hash));
    //SELECT OFFSET , length, path FROM chunk_data AS A JOIN file AS B ON A.fileid = B.id
    if (query.next())
        {
       // chunk.setHash(query.value(0).toLongLong());
        chunk.setHash(hash);
        chunk.setOffset(query.value(0).toInt());
        chunk.setLength(query.value(1).toInt());

        QString str1 = query.value(2).toString();
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
query.prepare("insert into file (checksum,path) values(:checksum,:path)");
query.bindValue(":checksum", checksum);
query.bindValue(":path", path);
ret = query.exec();


        // Get database given autoincrement value
        if (ret)
            {
            // http://www.sqlite.org/c3ref/last_insert_rowid.html
            newId = query.lastInsertId().toInt();
            }

        }
    return newId;
    }
bool DatabaseManager::findChecksum(QString checksum,string *value)
{
    bool ret = false;
     if (!db.isOpen()){this->openDB();}
    QSqlQuery query(QString("select path from file where checksum = '%1'").arg(checksum));
    if (query.next())
        {
        QString str1 = query.value(0).toString();
          QByteArray ba = str1.toLatin1();
          *value = ba.data();
        ret = true;
        }


    return ret;
}

bool DatabaseManager::deleteChunkTableEntries(){
    if(db.open()){
        QSqlQuery query(QString("delete from chunk_data"));
        bool result=query.exec();
        if(result){
          //  cout<<"Chunk entries removed successfully!!\n";
            return true;
        }
        else{
            //cout<<"Error when deleting chunk entries\n";
            return false;
        }
    }
}

bool DatabaseManager::deleteChecksumTableEntries(){
    if(db.open()){
        QSqlQuery query(QString("delete from file"));
        bool result=query.exec();
        if(result){
          //  cout<<"Checksum entries removed successfully!!\n";
            return true;
        }
        else{
           // cout<<"Error when deleting checksum entries\n";
            return false;
        }
    }
}

bool DatabaseManager::deleteFileChunkEntries(QString path){
    //delete from chunk where path="/home/mayuresan/Project/SourceTemp/LICENSE_1_0.txt~
    if(db.open()){
        QSqlQuery query(QString("delete from chunk_data where path = '%1'").arg(path));
        bool result=query.exec();
        if(result){
            cout<<"Chunk entries removed successfully!!\n";
            return true;
        }
        else{
            cout<<"Error when deleting chunk entries\n";
            return false;
        }
    }
}


//file move and file renamed
void DatabaseManager::moveFile(QString oldpath,QString newpath){

    if(db.isOpen()){
        QSqlQuery query(QString("update file set path='%1' where path = '%2'").arg(newpath).arg(oldpath));
        bool result=query.exec();
        if(result){
            cout<<"File is moved/renamed successfully!!\n";

        }
        else{
            cout<<"Error when File is moved/renamed\n";

        }

    }

}

//directory move / rename
void DatabaseManager::moveDirectory(QString oldpath,QString newpath){
    QString oldpathlike=oldpath + "%";
    cout<<"Old Path:"<<oldpath.toStdString()<<endl;
    cout<<"New Path:"<<newpath.toStdString()<<endl;
    if(!db.isOpen()){
        this->openDB();
    }
        QSqlQuery query;
            query.prepare("SELECT * FROM file WHERE path LIKE :path ");
            query.bindValue(":path", oldpathlike);
            query.exec();
            cout<<"Result size:"<<query.size()<<endl;
            while(query.next()){
                string str1 = query.value(2).toString().toStdString();
                string str2 = str1.substr(oldpath.size());
                string updatedpath=newpath.toStdString().append(str2);
                QString qstr = QString::fromStdString(updatedpath);

                QSqlQuery subquery;
                    subquery.prepare("UPDATE file SET path=:path WHERE id=:id");
                    subquery.bindValue(":path", qstr);
                     subquery.bindValue(":id", query.value(0));
                    subquery.exec();
            }


}

//delete file

void DatabaseManager::deleteFile(QString path){    
    if(!db.isOpen()){
        this->openDB();
    }
        QSqlQuery query(QString("delete from file where path = '%1'").arg(path));
        bool result=query.exec();
        if(result){
            cout<<"file entries removed successfully!!\n";

        }
        else{
            cout<<"Error when deleting file entries\n";

        }

}

//delete directory
void DatabaseManager::deleteDirectory(QString path){
    if(db.open()){
        QSqlQuery query(QString("delete from file where path = '%1'").arg(path+"%"));
        bool result=query.exec();
        if(result){
            cout<<"file entries removed successfully!!\n";

        }
        else{
            cout<<"Error when deleting file entries\n";

        }
    }
}
