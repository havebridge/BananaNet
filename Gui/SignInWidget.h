#pragma once

#include <QWidget>
#include "ui_SignInWidget.h"

class SignInWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::SignInWidgetClass* ui;

private slots:
	void on_HomeButton_clicked();

signals:
	void HomeClicked();

public:
	SignInWidget(QWidget *parent = nullptr);
	~SignInWidget();
};