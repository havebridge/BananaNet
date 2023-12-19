#include "MainWindow.h"

#include <QInputDialog>
#include <QDebug>
#include <format>


namespace App
{
	MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
	{
		ui = new Ui::MainWindowClass;
		//client = new TCPChat::Client;
		ui->setupUi(this);

		connect(ui->SignUp, &QPushButton::clicked, this, &MainWindow::onSignUpClicked);
		connect(ui->SignIn, &QPushButton::clicked, this, &MainWindow::onSignInClicked);
	}

	void MainWindow::onSignUpClicked()
	{
		QString qusername = QInputDialog::getText(this, tr("Enter Username"), tr("username:"));
		QString qlogin = QInputDialog::getText(this, tr("Enter Login"), tr("login:"));
		QString qpassword = QInputDialog::getText(this, tr("Enter Password"), tr("password:"), QLineEdit::Password);

		QWidget* widgetToShow = ui->stackedWidget->widget(0);
		//QLayout* layout = ui->centralWidget->layout();
		

		std::string username = qusername.toStdString();
		std::string login = qlogin.toStdString();
		std::string password = qpassword.toStdString();

		std::cout << username << login << password;
	}
	void MainWindow::onSignInClicked()
	{
		std::cout << "SingIn";
	}

	MainWindow::~MainWindow()
	{
		delete ui;
		//delete client;
	}
}