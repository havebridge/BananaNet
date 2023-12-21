#include "SignInWidget.h"

SignInWidget::SignInWidget(QWidget* parent)
	: QWidget(parent)
{
	ui = new Ui::SignInWidgetClass;
	ChatWidget = new Chat;
	client = &Instance::client;
	ui->setupUi(this);

	connect(ui->HomeButton, &QPushButton::clicked, this, &SignInWidget::on_HomeButton_clicked);
	connect(ui->SignInToButton, &QPushButton::clicked, this, &SignInWidget::on_SignInToButton_clicked);

	ui->stackedWidget->insertWidget(1, ChatWidget);

	ui->PasswordLineEdit->setEchoMode(QLineEdit::Password);
}

void SignInWidget::on_HomeButton_clicked()
{
	emit HomeClicked();
}

void SignInWidget::on_SignInToButton_clicked()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());

	QString login = ui->LoginLineEdit->text();
	QString password = ui->PasswordLineEdit->text();

	if (login.isEmpty() || password.isEmpty())
	{
		QMessageBox::critical(this, "Error", "Login and Password could not be empty!");
	}
	else
	{
		if (button)
		{
			if (client->SendUserInfoSignIn(login.toStdString(), password.toStdString()))
			{
				ui->stackedWidget->setCurrentIndex(1);
			}
			else
			{
				QMessageBox::critical(this, "Error", "Login and Password!");
			}
		}
	}
}

SignInWidget::~SignInWidget()
{
	delete ui;
	delete ChatWidget;
}