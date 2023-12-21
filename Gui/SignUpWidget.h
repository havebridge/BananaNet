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
	TCPChat::Client* client;

private slots:
	void on_HomeButton_clicked();
	void on_SignUpToButton_clicked();

signals:
	void HomeClicked();

public:
	SignUpWidget(QWidget *parent = nullptr);
	~SignUpWidget();
};