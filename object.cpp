#include "object.h"

Object::Object()
{
    name = "ClassName";
}

void Object::clearName(){
    name = "";
}

void Object::clearProperties(){
    properties.clear();
}

void Object::addProperty(QPair<QString,QString> property){
    properties << property;
}

void Object::setName(QString _name){
    name = _name;
}

void Object::setProperties(QVector<QPair<QString,QString>> _properties){
    properties = _properties;
}

void Object::addEndpoint(QString endpoint, int httpMethod){


    switch(httpMethod){
        case 0: endpoints.setEndpointGET(endpoint); break;
        case 1: endpoints.setEndpointPOST(endpoint); break;
        case 2: endpoints.setEndpointPUT(endpoint); break;
        case 3: endpoints.setEndpointDELETE(endpoint); break;

    }

}

QString Object::getName() const{
    return name;
}

QVector<QPair<QString,QString>>& Object::getProperties(){
    return properties;
}

QStringList Object::getEndpoints(int httpMethod){
    return endpoints.getEndpoints(httpMethod);
}
