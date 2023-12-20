#include "SignUpWidget.h"

SignUpWidget::SignUpWidget(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::SignUpWidgetClass;
	ui->setupUi(this);

	connect(ui->HomeButton, &QPushButton::clicked, this, &SignUpWidget::on_HomeButton_clicked);
}

void SignUpWidget::on_HomeButton_clicked()
{
	emit HomeClicked();
}

SignUpWidget::~SignUpWidget()
{
	delete ui;
}