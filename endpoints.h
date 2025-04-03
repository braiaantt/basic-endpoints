#ifndef ENDPOINTS_H
#define ENDPOINTS_H

#include<QString>
#include<QList>

class Endpoints
{
private:

    QStringList httpGET;
    QStringList httpPOST;
    QStringList httpPUT;
    QStringList httpDELETE;

public:
    Endpoints();

    //setters
    void setEndpointGET(QString);
    void setEndpointPOST(QString);
    void setEndpointPUT(QString);
    void setEndpointDELETE(QString);

    QStringList getEndpoints(int httpMethod);

};

#endif // ENDPOINTS_H
