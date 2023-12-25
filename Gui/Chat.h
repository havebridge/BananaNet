#pragma once

#include <QWidget>

#include "ui_Chat.h"

class Chat : public QWidget
{
	Q_OBJECT
private:
	Ui::ChatClass* ui;

public:
	Chat(QWidget *parent = nullptr);
	~Chat();

};
