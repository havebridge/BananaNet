#pragma once

#include <QWidget>
#include <QDebug>

#include "SignUpWidget.h"
#include "SignInWidget.h"
#include "ui_Application.h"
#include "client.h"

class Application : public QWidget
{
	Q_OBJECT
private:
	Ui::ApplicationClass* ui;

	SignUpWidget SignUpW;
	SignInWidget SignInW;

	TCPChat::Client& client = TCPChat::Client::GetInstance();
	int button_type;

private slots:
	void on_SignUp_clicked();
	void on_SignIn_clicked();

	void MoveHome();

public:
	Application(QWidget *parent = nullptr);
	~Application();
};
