#include "Application.h"

Application::Application(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::ApplicationClass;
	client = &Instance::client;
	client->Connect("127.0.0.1", 8888);

	ui->setupUi(this);
	
	connect(ui->SignUpButton, &QPushButton::clicked, this, &Application::on_SignUp_clicked);
	connect(ui->SignInButton, &QPushButton::clicked, this, &Application::on_SignIn_clicked);

	ui->stackedWidget->insertWidget(1, &SignUpW);
	ui->stackedWidget->insertWidget(2, &SignInW);

	connect(&SignUpW, SIGNAL(HomeClicked()), this, SLOT(MoveHome()));
	connect(&SignInW, SIGNAL(HomeClicked()), this, SLOT(MoveHome()));
}

void Application::on_SignUp_clicked()
{
	qDebug() << "SignUp clicked\n";
	ui->stackedWidget->setCurrentIndex(1);
}

void Application::on_SignIn_clicked()
{
	qDebug() << "SignIn clicked\n";
	ui->stackedWidget->setCurrentIndex(2);
}

void Application::MoveHome()
{
	qDebug() << "Home clicked\n";
	ui->stackedWidget->setCurrentIndex(0);
}

Application::~Application()
{
	delete ui;
	//delete client;
}