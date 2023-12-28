#pragma once

#include <QWidget>
#include <QMessageBox>

#include "ui_SignUpWidget.h"
#include "Instance.h"
#include "Chat.h"
#include "client.h"

class SignUpWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::SignUpWidgetClass* ui;

	Chat* ChatWidget;

	TCPChat::Client& client = TCPChat::Client::GetInstance();

private slots:
	void on_HomeButton_clicked();
	void on_SignUpToButton_clicked();

signals:
	void HomeClicked();
	void SignUpClicked();

public:
	SignUpWidget(QWidget *parent = nullptr);
	~SignUpWidget();
};