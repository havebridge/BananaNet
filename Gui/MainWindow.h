#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "client.h"

namespace App
{
	class MainWindow : public QMainWindow
	{
		Q_OBJECT
	private:
		Ui::MainWindowClass* ui;
		//TCPChat::Client* client;
	public slots:
		void onSignUpClicked();
		void onSignInClicked();

	public:
		MainWindow(QWidget* parent = nullptr);
		~MainWindow();
	};
}