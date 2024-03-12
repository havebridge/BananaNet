#pragma once

#include <QWidget>
#include <QPushButton>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

#include <vector>
#include <condition_variable>

#include "ui_Chat.h"
#include "client.h"

class Chat : public QWidget
{
	Q_OBJECT
private:
	Ui::ChatClass* ui;

	std::thread reciever_thread;
	std::mutex reciever_thread_mtx;
	std::condition_variable reciever_thread_cv;
	std::atomic<bool> stop_thread = false;

	TCPChat::Client& client = TCPChat::Client::GetInstance();
	std::string last_chat_name;
	std::string client_login;
	bool is_add_list_widget = false;


private slots:
	void CreateListWidget();
	void MoveBack();
	void MoveBackFromChat();
	void SearchUsers(const QString& name);
	void on_Message_SendButton_clicked();

public:
	void addButtonsFromVector(const std::vector<std::string>& buttonNames);
	void OpenChatWithUser(const std::string name);
	void addMessage();
	Chat(QWidget *parent = nullptr);
	~Chat();
};
