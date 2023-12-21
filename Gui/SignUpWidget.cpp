#include "SignUpWidget.h"

SignUpWidget::SignUpWidget(QWidget* parent)
	: QWidget(parent)
{
	ui = new Ui::SignUpWidgetClass;
	ChatWidget = new Chat;
	client = &Instance::client;
	ui->setupUi(this);

	connect(ui->HomeButton, &QPushButton::clicked, this, &SignUpWidget::on_HomeButton_clicked);
	connect(ui->SignUpToButton, &QPushButton::clicked, this, &SignUpWidget::on_SignUpToButton_clicked);

	ui->stackedWidget->insertWidget(1, ChatWidget);

	ui->PasswordLineEdit->setEchoMode(QLineEdit::Password);
}

void SignUpWidget::on_HomeButton_clicked()
{
	emit HomeClicked();
}

void SignUpWidget::on_SignUpToButton_clicked()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());

	QString username = ui->UsernameLineEdit->text();
	QString login = ui->LoginLineEdit->text();
	QString password = ui->PasswordLineEdit->text();

	if (username.isEmpty() || login.isEmpty() || password.isEmpty())
	{
		QMessageBox::critical(this, "Error", "Username, login and password could not be empty!");
	}
	else
	{
		if (button)
		{
			if (client->SendUserInfoSignUp(username.toStdString(), login.toStdString(), password.toStdString()))
			{
				ui->stackedWidget->setCurrentIndex(1);
			}
		}
	}
}

SignUpWidget::~SignUpWidget()
{
	delete ui;
	delete ChatWidget;
}