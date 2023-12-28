#pragma once

#include <QWidget>
#include <QMessageBox>

#include "ui_SignInWidget.h"
#include "Instance.h"
#include "Chat.h"
#include "client.h"
#include "RecieveUserInfoThread.h"

class SignInWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::SignInWidgetClass* ui;

	Chat* ChatWidget;

	TCPChat::Client& client = TCPChat::Client::GetInstance();

	//RecieveUserInfoThread* thread;
private slots:
	void on_HomeButton_clicked();
	void on_SignInToButton_clicked();
	//void on_DataRecieved();

signals:
	void HomeClicked();
	//void DataRecieved();

//public:
//	void StartReceiveDataThread();
public:
	SignInWidget(QWidget *parent = nullptr);
	~SignInWidget();
};