#include "objectsmanager.h"
#include "qDebug"
ObjectsManager::ObjectsManager()
{

}

void ObjectsManager::addObject(Object newObject){

    objects << newObject;

}

Object& ObjectsManager::getObject(QString nameObject){

    for(int i = 0; i<objects.size(); i++){

        if(objects[i].getName() == nameObject){
            return objects[i];
        }

    }

    throw std::runtime_error("Objeto no encontrado");

}
