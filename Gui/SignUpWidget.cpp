#include "SignUpWidget.h"

SignUpWidget::SignUpWidget(QWidget* parent)
	: QWidget(parent)
{
	ui = new Ui::SignUpWidgetClass;
	ui->setupUi(this);

	ChatWidget = new Chat();

	ui->stackedWidget->insertWidget(1, ChatWidget);

	ui->PasswordLineEdit->setEchoMode(QLineEdit::Password);
}

void SignUpWidget::on_HomeButton_clicked()
{
	std::cout << "home button clicled\n";

	emit HomeClicked();
}

void SignUpWidget::on_SignUpToButton_clicked()
{
	std::cout << "SIGN UP BUTTON CLICKED\n";

	client.SendButtonInfo(TCPChat::Client::ButtonType::SIGN_UP);

	QString username = ui->UsernameLineEdit->text();
	QString login = ui->LoginLineEdit->text();
	QString password = ui->PasswordLineEdit->text();

	if (username.isEmpty() || login.isEmpty() || password.isEmpty())
	{
		QMessageBox::critical(this, "Message error", "Username, Login and Password could not be empty!");
	}
	else if (username.size() > 255 || login.size() > 255 || password.size() > 255)
	{
		QMessageBox::critical(this, "Message error", "Usename, Login and Password could not be greater than 255 symbols!");
	}
	else
	{
		if (client.SendUserInfoSignUp(username.toStdString(), login.toStdString(), password.toStdString()))
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

SignUpWidget::~SignUpWidget()
{
	delete ui;
	delete ChatWidget;
}