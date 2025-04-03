#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QUrl>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initButtonsId();
    manager = new QNetworkAccessManager(this);
    connect(ui->buttonGroupHttpMethods, &QButtonGroup::buttonClicked, this, &MainWindow::buttonGroupClicked);
    ui->tableWidgetProperties->hideColumn(2);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initButtonsId(){

    ui->buttonGroupHttpMethods->setId(ui->pushButtonGET, 0);
    ui->buttonGroupHttpMethods->setId(ui->pushButtonPOST, 1);
    ui->buttonGroupHttpMethods->setId(ui->pushButtonPUT, 2);
    ui->buttonGroupHttpMethods->setId(ui->pushButtonDELETE, 3);

}

void MainWindow::on_lineEditClassName_textChanged(const QString &arg1)
{
    ui->listWidgetShowClass->item(0)->setText(arg1+"{");
    if(arg1.size() == 0){
        ui->listWidgetShowClass->item(0)->setText("ClassName{");
    }
}

void MainWindow::on_pushButtonAddProperties_clicked()
{
    if(checkEmptyFields()){
        qDebug()<<"Campos vacios!";
        return;
    }

    QString propertyName = ui->lineEditPropertyName->text();
    QString dataType = ui->lineEditDataType->text();
    setPropertyOnListWidget(propertyName, dataType);
    currentObject.addProperty(QPair<QString, QString>(propertyName, dataType));

    qDebug()<<currentObject.getProperties();


}
void MainWindow::setPropertyOnListWidget(QString& propertyName, QString& dataType){

    QString propertyText = propertyName + ": " + dataType;

    QListWidgetItem* item = new QListWidgetItem(propertyText);
    ui->listWidgetShowClass->insertItem(ui->listWidgetShowClass->count()-1, item);

    ui->lineEditPropertyName->setText("");
    ui->lineEditDataType->setText("");
    ui->lineEditPropertyName->setFocus();

}

void MainWindow::on_pushButtonAddClass_clicked()
{
    QString className = ui->lineEditClassName->text();
    ui->comboBoxClasses->addItem(className);
    ui->lineEditClassName->setText("");

    currentObject.setName(className);
    objectsManager.addObject(currentObject);
    currentObject.clearName();
    currentObject.clearProperties();

    ui->listWidgetShowClass->clear();
    ui->listWidgetShowClass->addItem("ClassName{");
    ui->listWidgetShowClass->addItem("}");

    ui->comboBoxClassEndpoints->addItem(className);

}

bool MainWindow::checkEmptyFields(){

    return ui->lineEditPropertyName->text().isEmpty() || ui->lineEditDataType->text().isEmpty();

}

void MainWindow::buttonGroupClicked(QAbstractButton* button){

    setEndpointsOnListWidget();

}

void MainWindow::on_pushButtonAddEndpoint_clicked()
{
    QLineEdit* lineEditEndpoint= ui->lineEditEndpointText;
    QString newEndpoint = lineEditEndpoint->text();
    QString strHttpMethod = ui->comboBoxHttpMethods->currentText();
    QString objectName = ui->comboBoxClasses->currentText();

    if(objectName != "" && newEndpoint.startsWith(strHttpMethod)){

        Object &object = objectsManager.getObject(objectName);
        int httpMethod = ui->comboBoxHttpMethods->currentIndex();
        object.addEndpoint(newEndpoint, httpMethod);

    }

    setEndpointsOnListWidget();
    lineEditEndpoint->setText(ui->comboBoxHttpMethods->currentText());
    ui->comboBoxProperties->setCurrentIndex(0);
    ui->lineEditEndpointText->setFocus();

}

void MainWindow::setEndpointsOnListWidget(){

    QString objectName = ui->comboBoxClassEndpoints->currentText();
    Object &object = objectsManager.getObject(objectName);
    int idHttpMethod = getIdHttpMethod();
    QStringList endpoints = object.getEndpoints(idHttpMethod);

    ui->listWidgetEndpoints->clear();
    ui->listWidgetEndpoints->addItems(endpoints);

}

void MainWindow::on_comboBoxHttpMethods_currentTextChanged(const QString &arg1)
{
    ui->lineEditEndpointText->setText(arg1);
    ui->comboBoxClasses->setCurrentIndex(0);
    ui->comboBoxProperties->setCurrentIndex(0);
}

void MainWindow::on_comboBoxClasses_currentTextChanged(const QString &arg1)
{
    QString currentHttpMethod = ui->comboBoxHttpMethods->currentText();
    ui->lineEditEndpointText->setText(currentHttpMethod + " /" + arg1);

    if(!arg1.isEmpty()){

        QString className = ui->comboBoxClasses->currentText();
        Object object = objectsManager.getObject(className);
        setClassOnListWidget(object);
        setPropertiesOnComboBox(object);

    } else {
        currentObject.setName("ClassName{");
        setClassOnListWidget(currentObject);
        setPropertiesOnComboBox(currentObject);
    }

    ui->lineEditEndpointText->setFocus();

}

void MainWindow::setPropertiesOnComboBox(Object &object){

    ui->comboBoxProperties->clear();
    ui->comboBoxProperties->addItem("");

    for(const QPair<QString,QString> &property : object.getProperties()){

        QString propertyName = property.first;

        ui->comboBoxProperties->addItem(propertyName);

    }

}

void MainWindow::setClassOnListWidget(Object &object){

    QListWidget* list = ui->listWidgetShowClass;
    list->clear();
    list->addItem(object.getName() + "{");

    for(const QPair<QString,QString>& propertie : object.getProperties()){

        QString listItem = propertie.first + ": " + propertie.second;
        list->addItem(listItem);

    }

    list->addItem("}");

}

void MainWindow::on_comboBoxProperties_currentTextChanged(const QString &arg1)
{
    QString currentHttpMethod = ui->comboBoxHttpMethods->currentText();
    QString currentClassName = ui->comboBoxClasses->currentText();
    ui->lineEditEndpointText->setText(currentHttpMethod + " /" + currentClassName + "/" + arg1);
}

void MainWindow::on_pushButtonSendHttp_clicked()
{
    int idHttpMethod = getIdHttpMethod();

    switch(idHttpMethod){
        case 0: doGetRequest(); break;
        case 1: doPostRequest(); break;
        case 2: doPutRequest(); break;
        case 3: doDeleteRequest(); break;
    }

}

void MainWindow::doGetRequest(){

    QUrl url = ui->textEditFinalUrl->toPlainText();
    qDebug()<<"url: "<<makeUrl();
    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::getReplyFinished);

}
void MainWindow::getReplyFinished(){

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if(reply->error() == QNetworkReply::NoError){

        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QString values;

        if(jsonDoc.isNull()){
            qDebug()<<"json vacio";
            return;
        }

        if(jsonDoc.isArray()){

            QJsonArray jsonArray = jsonDoc.array();
            for(const QJsonValue& value : jsonArray){
                if(value.isObject()){
                    QJsonObject jsonObject = value.toObject();
                    handleJsonDocAsObject(jsonObject, values);
                }
            }

        } else {

            QJsonObject jsonObject = jsonDoc.object();
            handleJsonDocAsObject(jsonObject, values);

        }

        ui->textEdit_2->setText(values);

    } else {

        QByteArray response = reply->readAll();
        QString textResponse = QString::fromUtf8(response);
        ui->textEdit_2->setText(textResponse);

    }

}

void MainWindow::handleJsonDocAsObject(QJsonObject& jsonObject, QString &values){

    QString objectName = ui->comboBoxClassEndpoints->currentText();
    Object &object = objectsManager.getObject(objectName);
    QVector<QPair<QString,QString>> &properties = object.getProperties();

    for(const QPair<QString, QString> &property : properties){

        QString key = property.first;
        QString dataType = property.second;

        if(dataType == "int"){

            int value = jsonObject[key].toInt();
            values += QString::number(value) + ", ";

        } else if(dataType == "str"){

            QString value = jsonObject[key].toString();
            values += value + ", ";

        }

    }

    values += "\n";

}

void MainWindow::doPostRequest(){

    QUrl url = ui->textEditFinalUrl->toPlainText();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = manager->post(request, requestBody);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::postReplyFinished);

}
void MainWindow::postReplyFinished(){

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QByteArray response = reply->readAll();
    QString textResponse = QString::fromUtf8(response);
    ui->textEdit_2->setText(textResponse);

}

void MainWindow::doPutRequest(){

    QUrl url = ui->textEditFinalUrl->toPlainText();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = manager->put(request, requestBody);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::putReplyFinished);

}
void MainWindow::putReplyFinished(){

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QByteArray response = reply->readAll();
    QString textResponse = QString::fromUtf8(response);
    ui->textEdit_2->setText(textResponse);

}

void MainWindow::doDeleteRequest(){

    QUrl url = ui->textEditFinalUrl->toPlainText();
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::deleteReplyFinished);


}
void MainWindow::deleteReplyFinished(){

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if(reply->error() == QNetworkReply::NoError){

        QByteArray response = reply->readAll();
        QString textResponse = QString::fromUtf8(response);
        ui->textEdit_2->setText(textResponse);

    }

}

QString MainWindow::makeUrl(){

    QString url = ui->lineEditUrl->text();
    QString httpMethod = ui->buttonGroupHttpMethods->checkedButton()->text();
    QString endpoint = ui->listWidgetEndpoints->currentItem()->text();
    endpoint.remove(httpMethod);
    QString finalUrl = url + endpoint;
    finalUrl.remove(' ');


    return finalUrl;

}

void MainWindow::on_listWidgetEndpoints_itemPressed(QListWidgetItem *item)
{

    QString url = makeUrl();
    ui->textEditFinalUrl->setText(url);

    setPropertiesOnTableWidget();

}

void MainWindow::setPropertiesOnTableWidget(){

    QString objectName = ui->comboBoxClassEndpoints->currentText();
    Object &object = objectsManager.getObject(objectName);
    QVector<QPair<QString,QString>> &properties = object.getProperties();
    QVector<QPair<QString,QString>> filteredProperties = getFilteredProperties(properties);

    QTableWidget* table = ui->tableWidgetProperties;
    table->setRowCount(0);
    table->setRowCount(filteredProperties.size());

    for(int i = 0; i<filteredProperties.size(); i++){

        QString propertyName = filteredProperties[i].first;
        QString propertyType = filteredProperties[i].second;

        QTableWidgetItem *property = new QTableWidgetItem(propertyName);
        QTableWidgetItem *value= new QTableWidgetItem("");
        QTableWidgetItem *type = new QTableWidgetItem(propertyType);

        table->setItem(i,0, property);
        table->setItem(i,1, value);
        table->setItem(i,2, type);

    }

}

QVector<QPair<QString,QString>> MainWindow::getFilteredProperties(QVector<QPair<QString,QString>> &properties){

    int idHttpMethod = getIdHttpMethod();
    QString currentEndpoint = ui->listWidgetEndpoints->currentItem()->text();
    QVector<QPair<QString,QString>> filteredProperties;

    if(idHttpMethod == 1 || idHttpMethod == 2) return properties;

    for(int i = 0; i<properties.size(); i++){

        QString propertyName = properties[i].first;

        if(currentEndpoint.endsWith(propertyName)){
            filteredProperties <<  properties[i];
            break;
        }

    }

    return filteredProperties;

}

int MainWindow::getIdHttpMethod(){

    QAbstractButton* button = ui->buttonGroupHttpMethods->checkedButton();
    return ui->buttonGroupHttpMethods->id(button);

}

void MainWindow::on_comboBoxClassEndpoints_currentTextChanged(const QString &arg1)
{
}


void MainWindow::on_comboBoxClassEndpoints_activated(int index)
{
    setEndpointsOnListWidget();
    ui->tableWidgetProperties->setRowCount(0);
}


void MainWindow::on_tableWidgetProperties_itemDoubleClicked(QTableWidgetItem *item)
{
    int idHttpMethod = getIdHttpMethod();
    if(item->column() == 1){
        ui->tableWidgetProperties->editItem(item);
    } else {
        return;
    }
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(ui->tableWidgetProperties->cellWidget(item->row(), item->column()));

    if(idHttpMethod == 0 || idHttpMethod == 3){

        if(lineEdit){
            connect(lineEdit, &QLineEdit::returnPressed, this, &MainWindow::cellPropertyReturnPressed);
        } else {
            qDebug()<<"LineEdit no valido";
        }

    }

}

void MainWindow::cellPropertyReturnPressed(){

    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());
    QString currentUrl = ui->textEditFinalUrl->toPlainText();
    QString pathParameter = lineEdit->text();
    QString finalUrl = currentUrl + "=" + pathParameter;

    ui->textEditFinalUrl->setText(finalUrl);

}


void MainWindow::on_pushButtonSetBodyRequest_clicked()
{

    QTableWidget* table = ui->tableWidgetProperties;
    QJsonObject jsonObject;

    for(int i = 0; i<table->rowCount(); i++){

        QString property = table->item(i,0)->text();
        QString value = table->item(i,1)->text();
        QString type = table->item(i,2)->text();

        if(type == "int"){
            jsonObject[property] = value.toInt();
        } else if(type == "str"){
            jsonObject[property] = value;
        }

    }

    QJsonDocument doc(jsonObject);
    requestBody = doc.toJson(QJsonDocument::Compact);

}

