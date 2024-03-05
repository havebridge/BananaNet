#include "Application.h"

Application::Application(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::ApplicationClass;

	client.Connect("127.0.0.1", 8888);

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
	std::cout << "SignUp clicked\n";
	button_type = 1;

	client.SendButtonInfo(button_type);

	ui->stackedWidget->setCurrentIndex(1);
}

void Application::on_SignIn_clicked()
{
	std::cout << "SignIn clicked\n";
	button_type = 2;

	client.SendButtonInfo(button_type);

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
}