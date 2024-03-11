#include "SignInWidget.h"

SignInWidget::SignInWidget(QWidget* parent)
	: QWidget(parent)
{
	ui = new Ui::SignInWidgetClass;

	ui->setupUi(this);

	ChatWidget = new Chat();

	ui->stackedWidget->insertWidget(1, ChatWidget);

	ui->PasswordLineEdit->setEchoMode(QLineEdit::Password);
}

void SignInWidget::on_HomeButton_clicked()
{
	std::cout << "home button clicled\n";

	emit HomeClicked();
}

void SignInWidget::on_SignInToButton_clicked()
{
	std::cout << "SIGN IN BUTTON CLICKED\n";

	client.SendButtonInfo(TCPChat::Client::ButtonType::SIGN_IN);

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
			ui->stackedWidget->setCurrentIndex(1);
			client.RecieveUsersInfo();
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