#pragma once

#include <QWidget>
#include <QDebug>

#include "SignUpWidget.h"
#include "SignInWidget.h"
#include "ui_Application.h"

class Application : public QWidget
{
	Q_OBJECT
private:
	Ui::ApplicationClass* ui;
	SignUpWidget SignUpW;
	SignInWidget SignInW;

private slots:
	void on_SignUp_clicked();
	void on_SignIn_clicked();

	void MoveHome();

public:
	Application(QWidget *parent = nullptr);
	~Application();
};
