#include "include/dbprovider.h"

DBProvider* DBProvider::getInstance(QString dbName)
{
    static DBProvider instance(dbName);
    return &instance;
}

DBProvider::DBProvider(QString dbName)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if(!db.open()) if (!db.open()) qWarning() << db.lastError().text();
    QFile file(":/db/scheme.sql");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << file.errorString();
        return;
    }

    QTextStream in(&file);

    QSqlQuery create(db);
    QString currentStatement;
    bool insideTrigger = false;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.isEmpty() || line.startsWith("--")) {
            continue;
        }

        if (line.startsWith("CREATE TRIGGER", Qt::CaseInsensitive)) {
            insideTrigger = true;
        }

        currentStatement += line + " ";

        if (insideTrigger) {
            if (line.toUpper().contains("END;")) {
                insideTrigger = false;
                if (!create.exec(currentStatement)) {
                    qWarning() << "SQL Execute Error (TRIGGER):" << create.lastError().text();
                    qWarning() << "SQL:" << currentStatement;
                    return;
                }
                currentStatement.clear();
            }
        }
        else if (line.endsWith(";")) {
            if (!create.exec(currentStatement)) {
                qWarning() << "SQL: Execute Error" << create.lastError().text();
                qWarning() << "SQL:" << currentStatement;
                return;
            }
            currentStatement.clear();
        }
    }

    file.close();
}