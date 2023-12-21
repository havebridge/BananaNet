#include "Chat.h"

Chat::Chat(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::ChatClass;
	ui->setupUi(this);
}

Chat::~Chat()
{
	delete ui;
}