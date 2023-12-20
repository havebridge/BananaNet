#pragma once

#include <QWidget>
#include "ui_SignUpWidget.h"

class SignUpWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::SignUpWidgetClass* ui;

private slots:
	void on_HomeButton_clicked();

signals:
	void HomeClicked();

public:
	SignUpWidget(QWidget *parent = nullptr);
	~SignUpWidget();
};