#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QUrl>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QMessageBox>
#include <QMenu>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initButtonsId();
    manager = new QNetworkAccessManager(this);
    connect(ui->buttonGroupHttpMethods, &QButtonGroup::buttonClicked, this, &MainWindow::buttonGroupClicked);
    connect(ui->listWidgetShowClass, &QListWidget::customContextMenuRequested, this, &MainWindow::deletePropertyItem);
    ui->tableWidgetProperties->hideColumn(2);
    currentObject = objectsManager.createObject();

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
        currentObject->setName("ClassName");
        ui->listWidgetShowClass->item(0)->setText(currentObject->getName() + "{");
    }
}

void MainWindow::on_pushButtonAddProperties_clicked()
{
    if(checkEmptyFields()){
        showWarningDialog("Campos vacíos!");
        return;
    }

    QString propertyName = ui->lineEditPropertyName->text();
    QString dataType = ui->lineEditDataType->text();
    setPropertyOnListWidget(propertyName, dataType);
    currentObject->addProperty(QPair<QString, QString>(propertyName, dataType));

    if(!ui->comboBoxClasses->currentText().isEmpty()){
        ui->comboBoxProperties->addItem(propertyName);
    }

}
void MainWindow::setPropertyOnListWidget(QString& propertyName, QString& dataType){

    QString propertyText = propertyName + ": " + dataType;

    QListWidgetItem* item = new QListWidgetItem(propertyText);
    ui->listWidgetShowClass->insertItem(ui->listWidgetShowClass->count()-1, item);

    ui->lineEditPropertyName->setText("");
    ui->lineEditDataType->setText("");
    ui->lineEditPropertyName->setFocus();

}

void MainWindow::deletePropertyItem(const QPoint &pos){

    QListWidget* list = ui->listWidgetShowClass;
    QListWidgetItem* item = list->itemAt(pos);
    if(item == nullptr) return;

    int itemRow = list->row(item);
    if(itemRow == 0 || itemRow == list->count()-1)  return;

    QMenu menu(this);

    QAction* deleteItem = menu.addAction("Eliminar propiedad");

    QAction* selectedAction = menu.exec(list->viewport()->mapToGlobal(pos));
    if(selectedAction == deleteItem){
        QString propertyName = item->text().split(":").value(0);
        currentObject->deleteProperty(propertyName);
        ui->comboBoxProperties->removeItem(itemRow);
        delete item;
        list->clearFocus();
    }

}

void MainWindow::on_pushButtonAddClass_clicked()
{
    QString className = ui->lineEditClassName->text();

    if(className == ""){
        showWarningDialog("La clase debe tener un nombre!");
        return;
    }

    ui->comboBoxClasses->addItem(className);
    ui->lineEditClassName->setText("");

    currentObject->setName(className);
    objectsManager.addObject(currentObject);
    currentRequestObject = currentObject;

    currentObject = objectsManager.createObject();
    objectsManager.setObjectInCreation(currentObject);

    ui->listWidgetShowClass->clear();
    ui->listWidgetShowClass->addItem(currentObject->getName()+"{");
    ui->listWidgetShowClass->addItem("}");

    ui->comboBoxClassEndpoints->addItem(className);

    if(objectsManager.countObjects() == 1){
        ui->pushButtonGET->setEnabled(true);
        ui->pushButtonPOST->setEnabled(true);
        ui->pushButtonPUT->setEnabled(true);
        ui->pushButtonDELETE->setEnabled(true);
    }

}

bool MainWindow::checkEmptyFields(){

    return ui->lineEditPropertyName->text().isEmpty() || ui->lineEditDataType->text().isEmpty();

}

void MainWindow::buttonGroupClicked(QAbstractButton* button){

    setEndpointsOnListWidget();
    restartUiInformation();


    int idHttpMethod = getIdHttpMethod();
    if(idHttpMethod == 1 || idHttpMethod == 2){
        ui->pushButtonSetBodyRequest->setEnabled(true);
    } else{
        ui->pushButtonSetBodyRequest->setEnabled(false);
    }

}

void MainWindow::restartUiInformation(){

    ui->labelBodyRequestState->setEnabled(false);
    ui->labelBodyRequestState->setText("Sin cuerpo");
    ui->textEdit_2->setText("");
    ui->tableWidgetProperties->setRowCount(0);

}

void MainWindow::on_pushButtonAddEndpoint_clicked()
{
    QLineEdit* lineEditEndpoint= ui->lineEditEndpointText;
    QString newEndpoint = lineEditEndpoint->text();
    QString strHttpMethod = ui->comboBoxHttpMethods->currentText();
    QString objectName = ui->comboBoxClasses->currentText();

    if(objectName != "" && newEndpoint.startsWith(strHttpMethod)){

        int httpMethod = ui->comboBoxHttpMethods->currentIndex();
        objectsManager.addEndpointToObject(objectName, newEndpoint, httpMethod);

    }

    setEndpointsOnListWidget();

    ui->comboBoxProperties->setCurrentIndex(0);

    QString text = ui->comboBoxHttpMethods->currentText() + " /" + ui->comboBoxClasses->currentText();
    lineEditEndpoint->setText(text);
    lineEditEndpoint->setFocus();

}

void MainWindow::setEndpointsOnListWidget(){

    QString objectName = ui->comboBoxClassEndpoints->currentText();

    currentRequestObject = objectsManager.getObject(objectName);
    int idHttpMethod = getIdHttpMethod();
    QStringList endpoints = currentRequestObject->getEndpoints(idHttpMethod);

    ui->listWidgetEndpoints->clear();
    ui->listWidgetEndpoints->addItems(endpoints);

}

void MainWindow::on_comboBoxHttpMethods_activated(int index)
{
    ui->lineEditEndpointText->setText(ui->comboBoxHttpMethods->currentText());
    ui->comboBoxClasses->setCurrentIndex(0);
    ui->comboBoxProperties->setCurrentIndex(0);
}

void MainWindow::on_comboBoxClasses_activated(int index)
{

    QString currentHttpMethod = ui->comboBoxHttpMethods->currentText();
    QString currentClass = ui->comboBoxClasses->currentText();
    ui->lineEditEndpointText->setText(currentHttpMethod + " /" + currentClass);

    if(!currentClass.isEmpty()){

        QString objectName = ui->comboBoxClasses->currentText();

        currentObject = objectsManager.getObject(objectName);
        setClassOnListWidget(*currentObject);
        setPropertiesOnComboBox(*currentObject);

        ui->lineEditClassName->setEnabled(false);
        ui->pushButtonAddClass->setEnabled(false);

    } else {

        currentObject = objectsManager.getObjectInCreation();
        if(!ui->lineEditClassName->text().isEmpty())currentObject->setName(ui->lineEditClassName->text());
        setClassOnListWidget(*currentObject);
        setPropertiesOnComboBox(*currentObject);

        ui->lineEditClassName->setEnabled(true);
        ui->pushButtonAddClass->setEnabled(true);

    }

    ui->lineEditEndpointText->setFocus();
}

void MainWindow::on_comboBoxProperties_activated(int index)
{
    if(ui->comboBoxProperties->currentIndex() == 0) return;
    QString currentHttpMethod = ui->comboBoxHttpMethods->currentText();
    QString currentClassName = ui->comboBoxClasses->currentText();
    QString currentProperty = ui->comboBoxProperties->currentText();
    ui->lineEditEndpointText->setText(currentHttpMethod + " /" + currentClassName + "/" + currentProperty);
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

void MainWindow::on_pushButtonSendHttp_clicked()
{
    int idHttpMethod = getIdHttpMethod();

    if((idHttpMethod == 1 || idHttpMethod == 2) && !ui->labelBodyRequestState->isEnabled()){
        showWarningDialog("Este metodo http necesita de un cuerpo!");
        return;
    }

    switch(idHttpMethod){
        case 0: doGetRequest(); break;
        case 1: doPostRequest(); break;
        case 2: doPutRequest(); break;
        case 3: doDeleteRequest(); break;
    }

}

void MainWindow::doGetRequest(){

    QUrl url = ui->textEditFinalUrl->toPlainText();
    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::getReplyFinished);

}
void MainWindow::getReplyFinished(){

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QString values;

    QJsonObject object = jsonDoc.object();
    bool success = object["success"].toBool();

    if(success){

        if(object["data"].isArray()){
            QJsonArray array = object["data"].toArray();
            for(int i = 0; i<array.size(); i++){

                QJsonObject data = array[i].toObject();
                handleJsonDocAsObject(data, values);
            }

        } else {

            QJsonObject data = object["data"].toObject();

            if(ui->pushButtonPUT->isChecked()){
                setValuesOnTableWidget(data);
                return;
            } else {
                handleJsonDocAsObject(data, values);
            }

        }

    } else {

        values = object["detail"].toString();

    }

    ui->textEdit_2->setText(values);

}

void MainWindow::handleJsonDocAsObject(QJsonObject& jsonObject, QString &values){

    QVector<QPair<QString,QString>> &properties = currentRequestObject->getProperties();

    for(const QPair<QString, QString> &property : properties){

        QString key = property.first;
        QString dataType = property.second;

        if(dataType == "int"){

            int intValue = jsonObject[key].toInt();
            values.append(QString::number(intValue) + ", ");

        } else if(dataType == "str"){

            QString stringValue = jsonObject[key].toString();
            values.append(stringValue + ", ");

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
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObject = jsonDoc.object();

    QString textResponse = jsonObject["message"].toString();
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
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObject = jsonDoc.object();
    QString textResponse = jsonDoc["message"].toString();

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
    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObject = jsonDoc.object();

    QString textResponse = jsonObject["message"].toString();
    ui->textEdit_2->setText(textResponse);


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

    QVector<QPair<QString,QString>> &properties = currentRequestObject->getProperties();
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
    currentRequestObject = objectsManager.getObject(ui->comboBoxClassEndpoints->currentText());
    setEndpointsOnListWidget();
    ui->tableWidgetProperties->setRowCount(0);
}

void MainWindow::on_pushButtonSetBodyRequest_clicked()
{

    QTableWidget* table = ui->tableWidgetProperties;
    QJsonObject jsonObject;

    if(setBodyRequestError()) return;

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
    ui->labelBodyRequestState->setEnabled(true);
    ui->labelBodyRequestState->setText("Con cuerpo");

}

bool MainWindow::setBodyRequestError(){

    QTableWidget* table = ui->tableWidgetProperties;

    if(table->rowCount() == 0){
        showWarningDialog("No se ha seleccionado ningun endpoint");
        return true;
    }

    for(int i = 0; i<table->rowCount(); i++){

        QTableWidgetItem* item = table->item(i,1);
        if(item->text() == ""){
            showWarningDialog("Campos vacíos en el tableWidget!");
            return true;
        }

    }

    return false;

}

void MainWindow::setValuesOnTableWidget(QJsonObject& jsonObject){

    QVector<QPair<QString,QString>> &properties = currentRequestObject->getProperties();
    QTableWidget *table = ui->tableWidgetProperties;

    for(int i = 1; i<table->rowCount(); i++){

        QString key = properties[i].first;
        QString dataType = properties[i].second;
        QString value;

        if(dataType == "int"){

            int intValue = jsonObject[key].toInt();
            value.append(QString::number(intValue) + ", ");

        } else if(dataType == "str"){

            QString stringValue = jsonObject[key].toString();
            value.append(stringValue + ", ");

        }

        table->item(i,1)->setText(value);

    }

}

void MainWindow::showWarningDialog(QString warningText){

    QMessageBox::warning(this, "Advertencia", warningText);

}

void MainWindow::on_tableWidgetProperties_itemDoubleClicked(QTableWidgetItem *item)
{
    if(item->column() == 1){
        ui->tableWidgetProperties->setCurrentCell(item->row(),item->column());
        ui->tableWidgetProperties->editItem(item);
        connectLineEditReturnPressedSignal(item->row());
    }
}

void MainWindow::connectLineEditReturnPressedSignal(int row){

    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(ui->tableWidgetProperties->cellWidget(row, 1));

    if(lineEdit){
        disconnect(lineEdit, &QLineEdit::returnPressed, this, &MainWindow::itemLineEditReturnPressed);
        connect(lineEdit, &QLineEdit::returnPressed, this, &MainWindow::itemLineEditReturnPressed);
    }

}

void MainWindow::itemLineEditReturnPressed(){

    int newRow = ui->tableWidgetProperties->currentRow() + 1;


    QTimer::singleShot(0,this,[this, newRow](){

        if(ui->tableWidgetProperties->currentRow() == 0 && !ui->pushButtonPOST->isChecked() && !ui->pushButtonPUT->isChecked()){
            QString key = ui->tableWidgetProperties->item(newRow-1, 0)->text();
            QString value = ui->tableWidgetProperties->item(newRow-1, 1)->text();
            QString url = makeUrl();
            url = url.split(key).value(0);
            url.append(value);
            ui->textEditFinalUrl->setText(url);
        }

    });

    if(newRow < ui->tableWidgetProperties->rowCount()){

        QTimer::singleShot(0, this, [this, newRow](){

            ui->tableWidgetProperties->setCurrentCell(newRow,1);
            ui->tableWidgetProperties->editItem(ui->tableWidgetProperties->item(newRow,1));

            connectLineEditReturnPressedSignal(newRow);

        });

    }

}
