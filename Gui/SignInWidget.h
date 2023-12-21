#pragma once

#include <QWidget>
#include <QMessageBox>

#include "ui_SignInWidget.h"
#include "Instance.h"
#include "Chat.h"
#include "client.h"

class SignInWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::SignInWidgetClass* ui;
	Chat* ChatWidget;
	TCPChat::Client* client;

private slots:
	void on_HomeButton_clicked();
	void on_SignInToButton_clicked();

signals:
	void HomeClicked();

public:
	SignInWidget(QWidget *parent = nullptr);
	~SignInWidget();
};