#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "endpoints.h"
#include "objectsmanager.h"
#include <QAbstractButton>
#include <QListWidgetItem>
#include <QtNetwork/QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void buttonGroupClicked(QAbstractButton*);
    void getReplyFinished();
    void postReplyFinished();
    void putReplyFinished();
    void deleteReplyFinished();
    void deletePropertyItem(const QPoint &pos);
    void itemLineEditReturnPressed();

    void on_pushButtonAddProperties_clicked();

    void on_lineEditClassName_textChanged(const QString &arg1);

    void on_pushButtonAddEndpoint_clicked();

    void on_pushButtonSendHttp_clicked();

    void on_pushButtonAddClass_clicked();

    void on_listWidgetEndpoints_itemPressed(QListWidgetItem *item);

    void on_comboBoxClassEndpoints_currentTextChanged(const QString &arg1);

    void on_comboBoxClassEndpoints_activated(int index);

    void on_pushButtonSetBodyRequest_clicked();

    void on_comboBoxHttpMethods_activated(int index);

    void on_comboBoxClasses_activated(int index);

    void on_comboBoxProperties_activated(int index);

    void on_tableWidgetProperties_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager* manager;
    Endpoints endpoints;
    ObjectsManager objectsManager;
    std::shared_ptr<Object> currentObject;
    std::shared_ptr<Object> currentRequestObject;
    QByteArray requestBody;

    void setClassOnListWidget(Object &);
    void setPropertiesOnComboBox(Object &);
    void setPropertiesOnTableWidget();
    int getIdHttpMethod();
    bool checkEmptyFields();
    void initButtonsId();
    void setPropertyOnListWidget(QString &propertyName, QString &dataType);
    void setEndpointsOnListWidget();
    QVector<QPair<QString,QString>> getFilteredProperties(QVector<QPair<QString,QString>>& properties);
    QString makeUrl();
    void doGetRequest();
    void doPostRequest();
    void doPutRequest();
    void doDeleteRequest();

    void handleJsonDocAsObject(QJsonObject& jsonDoc, QString &values);
    void setValuesOnTableWidget(QJsonObject& jsonDoc);
    void restartUiInformation();
    bool setBodyRequestError();
    void showWarningDialog(QString);
    void connectLineEditReturnPressedSignal(int row);

};
#endif // MAINWINDOW_H
