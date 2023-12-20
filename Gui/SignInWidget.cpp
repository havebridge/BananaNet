#include "SignInWidget.h"

SignInWidget::SignInWidget(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::SignInWidgetClass;
	ui->setupUi(this);

	connect(ui->HomeButton, &QPushButton::clicked, this, &SignInWidget::on_HomeButton_clicked);
}

void SignInWidget::on_HomeButton_clicked()
{
	emit HomeClicked();
}

SignInWidget::~SignInWidget()
{}
