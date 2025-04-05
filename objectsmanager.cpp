#include "objectsmanager.h"
#include "qDebug"
ObjectsManager::ObjectsManager()
{
    objectInCreation = nullptr;
}

int ObjectsManager::countObjects(){
    return objects.size();
}

void ObjectsManager::addObject(std::shared_ptr<Object> newObject){

    objects << newObject;

}

void ObjectsManager::addEndpointToObject(QString objectName, QString newEndpoint, int httpMethod){

    for(auto &object : objects){

        if(object->getName() == objectName){
            object->addEndpoint(newEndpoint, httpMethod);
        }

    }

}


std::shared_ptr<Object> ObjectsManager::createObject(){

    std::shared_ptr<Object> newObject = std::make_shared<Object>();
    return newObject;

}

//getters
std::shared_ptr<Object> ObjectsManager::getObject(QString nameObject){

    for(int i = 0; i<objects.size(); i++){

        if(objects[i]->getName() == nameObject){
            return objects[i];
        }

    }

    return nullptr;

}

std::shared_ptr<Object> ObjectsManager::getObjectInCreation(){
    return objectInCreation;
}


//setters
void ObjectsManager::setObjectInCreation(std::shared_ptr<Object> object){
    objectInCreation = object;
}
