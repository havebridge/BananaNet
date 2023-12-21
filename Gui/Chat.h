#pragma once

#include <QWidget>
#include "ui_Chat.h"

class Chat : public QWidget
{
	Q_OBJECT

public:
	Chat(QWidget *parent = nullptr);
	~Chat();

private:
	Ui::ChatClass* ui;
};
