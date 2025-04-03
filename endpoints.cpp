#include "endpoints.h"

Endpoints::Endpoints()
{

}

void Endpoints::setEndpointGET(QString endpoint){
    httpGET << endpoint;
}
void Endpoints::setEndpointPOST(QString endpoint){
    httpPOST << endpoint;
}
void Endpoints::setEndpointPUT(QString endpoint){
    httpPUT << endpoint;
}
void Endpoints::setEndpointDELETE(QString endpoint){
    httpDELETE << endpoint;
}

QStringList Endpoints::getEndpoints(int httpMethod){

    switch(httpMethod){
        case 0: return httpGET; break;
        case 1: return httpPOST; break;
        case 2: return httpPUT; break;
        case 3: return httpDELETE; break;
        default:  return {"Error","NoEndpoints"};
    }

}
