#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QFile>

#include "Chunker.h"
using namespace std;


class DatabaseManager : public QObject
    {
    public:
        DatabaseManager(QObject *parent = 0);
      //  ~DatabaseManager();

    public:
        bool openDB();
        void closeDB();
        bool isOpen();
        QSqlError lastError();
        int insertChunk(u_int64_t hash, int offset, int length, QString path);
        bool getChunk(u_int64_t hash, Chunk &chunk);
        int insertChecksum(QString checksum,QString path);
        bool findChecksum(QString checksum, string &value);


    private:
        QSqlDatabase db;
    };

#endif //DBMANAGER_H
