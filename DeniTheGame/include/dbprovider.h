#pragma once

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QMap>
#include <QFile>
#include <QTextStream>

class DBProvider
{
public:
    DBProvider(DBProvider& other) = delete;
    void operator=(DBProvider other) = delete;
    static DBProvider* getInstance(QString dbName = "deni.db");
    QSqlDatabase& getDB() { return db; }
    ~DBProvider() { db.close(); }

private:
    DBProvider(QString dbName);
    QSqlDatabase db;
};