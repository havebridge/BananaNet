#pragma once

#include <QWidget>
#include <QPushButton>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

#include <vector>

#include "ui_Chat.h"
#include "client.h"

class Chat : public QWidget
{
	Q_OBJECT
private:
	Ui::ChatClass* ui;
	//std::thread recieved_thread;

	TCPChat::Client& client = TCPChat::Client::GetInstance();
	std::string last_chat_name;
	std::string client_login;
	bool is_add_list_widget = false;

private slots:
	void CreateListWidget();
	void MoveBack();
	void SearchUsers(const QString& name);
	void on_Message_SendButton_clicked();

public:
	void addButtonsFromVector(const std::vector<std::string>& buttonNames);
	void OpenChatWithUser(const std::string name);
	void addMessage(const QString& message, bool alignRight);
	Chat(QWidget *parent = nullptr);
	~Chat();
};
