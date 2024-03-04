#include "SignInWidget.h"

SignInWidget::SignInWidget(QWidget* parent)
	: QWidget(parent)
{
	ui = new Ui::SignInWidgetClass;

	ui->setupUi(this);

	connect(ui->HomeButton, &QPushButton::clicked, this, &SignInWidget::on_HomeButton_clicked);
	//connect(ui->SignInToButton, &QPushButton::clicked, this, &SignInWidget::on_SignInToButton_clicked);

	ChatWidget = new Chat();

	ui->stackedWidget->insertWidget(1, ChatWidget);

	ui->PasswordLineEdit->setEchoMode(QLineEdit::Password);
}

void SignInWidget::on_HomeButton_clicked()
{
	emit HomeClicked();
}

void SignInWidget::on_SignInToButton_clicked()
{
	std::cout << "SIGN IN BUTTON CLICKED\n";

	QString login = ui->LoginLineEdit->text();
	QString password = ui->PasswordLineEdit->text();

	if (login.isEmpty() || password.isEmpty())
	{
		QMessageBox::critical(this, "Error", "Login and Password could not be empty!");
	}
	else if (login.size() > 255 || password.size() > 255)
	{
		QMessageBox::critical(this, "Message error", "Login and Password could not be greater than 255 symbols!");
	}
	else
	{
		if (client.SendUserInfoSignIn(login.toStdString(), password.toStdString()))
		{
			client.RecieveUsersInfo();
			ui->stackedWidget->setCurrentIndex(1);
			client.SetMessageInfoFrom(login.toStdString());
			std::cout << "FROM: " << client.GetMessageInfo().from;
		}
		else
		{
			QMessageBox::critical(this, "Error", "Login and Password!");
		}
	}
}

SignInWidget::~SignInWidget()
{
	delete ui;
	delete ChatWidget;
}