#ifndef OBJECT_H
#define OBJECT_H

#include <QString>
#include <QVector>
#include <QPair>
#include "endpoints.h"

class Object
{
public:
    Object();
    void clearProperties();
    void clearName();
    void addProperty(QPair<QString,QString>);
    void deleteProperty(QString);
    void addEndpoint(QString endpoint, int httpMethod);

    //setters
    void setName(QString);
    void setProperties(QVector<QPair<QString,QString>>);

    //getters
    QString getName() const;
    QVector<QPair<QString,QString>>& getProperties();
    QStringList getEndpoints(int httpMethod);


private:
    QString name;
    QVector<QPair<QString, QString>> properties;
    Endpoints endpoints;
};

#endif // OBJECT_H
