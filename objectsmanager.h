#ifndef OBJECTSMANAGER_H
#define OBJECTSMANAGER_H

#include "object.h"

class ObjectsManager
{
public:
    ObjectsManager();
    int countObjects();
    void addObject(std::shared_ptr<Object>);
    void addEndpointToObject(QString objectName, QString newEndpoint, int httpMethod);
    std::shared_ptr<Object> createObject();

    //getters
    std::shared_ptr<Object> getObject(QString objectName);
    std::shared_ptr<Object> getObjectInCreation();

    //setters
    void setObjectInCreation(std::shared_ptr<Object>);

private:
    QVector<std::shared_ptr<Object>> objects;
    std::shared_ptr<Object> objectInCreation;

};

#endif // OBJECTSMANAGER_H
