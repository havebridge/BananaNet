#include "Chat.h"

Chat::Chat(QWidget* parent)
	: QWidget(parent)
{
	ui = new Ui::ChatClass;
	ui->setupUi(this);

	connect(ui->pushButton, &QPushButton::clicked, this, &Chat::CreateListWidget);
	connect(ui->Users_SearchLineUsers, &QLineEdit::textChanged, this, &Chat::SearchUsers);
	connect(ui->backButton, &QPushButton::clicked, this, &Chat::MoveBack);
	connect(ui->backButtonChat, &QPushButton::clicked, this, &Chat::MoveBack);
	connect(ui->ButtonChat, &QPushButton::clicked, this, &Chat::CreateListWidget);
	connect(ui->Message_SendButton, &QPushButton::clicked, this, &Chat::on_Message_SendButton_clicked);
	
	ui->pushButton->setStyleSheet("QPushButton { "
		"border-image: url(:/MainWindow/images/searchusers.png); "
		"}");
	ui->ButtonChat->setStyleSheet("QPushButton { "
		"border-image: url(:/MainWindow/images/searchusers.png); "
		"}");
	ui->backButton->setStyleSheet("QPushButton { "
		"border-image: url(:/MainWindow/images/back.png); "
		"}");
	ui->backButtonChat->setStyleSheet("QPushButton { "
		"border-image: url(:/MainWindow/images/back.png); "
		"}");
	ui->Message_SendButton->setStyleSheet("QPushButton { "
		"border-image: url(:/MainWindow/images/send.png); "
		"}");

	ui->listWidget->setStyleSheet(
		"QListWidget {"
		"border: none;"
		"border-radius: 5px;"
		"background-color: rgba(14, 14, 14, 1);" // Цвет фона
		"}"
		"QListWidget::item {"
		"padding: 5px;"
		"}"
		"QListWidget::item:hover {"
		"background-color: #c0c0c0;" // Цвет фона при наведении
		"}"
		"QPushButton {"
		"border: 1px solid gray;"
		"border-radius: 5px;"
		"padding: 5px;"
		"background-color: #ffffff;" // Цвет фона кнопки
		"}"
		"QPushButton:hover {"
		"background-color: #e0e0e0;" // Цвет фона кнопки при наведении
		"}"
		"QPushButton:pressed {"
		"background-color: #d0d0d0;" // Цвет фона кнопки при нажатии
		"}");

	ui->Chats_SearchLineChat->setStyleSheet("background-color: rgba(147, 147, 147, 1);"
		"border-radius: 10px;"
		"color: white;"
		"padding-right: 20px;"
		"text-align: center;");
	ui->Chats_SearchLineChat->setAlignment(Qt::AlignCenter);
	ui->Chats_SearchLineChat->setPlaceholderText("🔍 Search");

	ui->Users_SearchLineUsers->setStyleSheet("background-color: rgba(147, 147, 147, 1);"
		"border-radius: 10px;"
		"color: white;"
		"padding-right: 20px;"
		"text-align: center;");
	ui->Users_SearchLineUsers->setAlignment(Qt::AlignCenter);
	ui->Users_SearchLineUsers->setPlaceholderText("🔍 Search");

	ui->Message_SearchLineChats->setStyleSheet("background-color: rgba(147, 147, 147, 1);"
		"border-radius: 10px;"
		"color: white;"
		"padding-right: 20px;"
		"text-align: center;");
	ui->Message_SearchLineChats->setAlignment(Qt::AlignCenter);
	ui->Message_SearchLineChats->setPlaceholderText("🔍 Search");

	ui->Message_LineMessageEdit->setStyleSheet("background-color: rgba(28, 28, 28, 1);"
		"border-radius: 10px;"
		"color: white;"
		"padding-right: 20px;"
		"text-align: center;");
	//ui->Message_LineMessageEdit->setAlignment(Qt::AlignCenter);
	ui->Message_LineMessageEdit->setPlaceholderText("Message...");

	SearchUsers(ui->Users_SearchLineUsers->text());
}

void Chat::addButtonsFromVector(const std::vector<std::string>& buttonNames)
{
	for (const std::string& name : buttonNames)
	{
		QPushButton* button = new QPushButton(QString::fromStdString(name));
		QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
		ui->listWidget->addItem(item);
		ui->listWidget->setItemWidget(item, button);

		connect(button, &QPushButton::clicked, [this, name]() {
			OpenChatWithUser(name);
			});
	}
}

void Chat::CreateListWidget()
{
	std::cout << "Creating list widget\n";

	ui->stackedWidget->setCurrentWidget(ui->users);

	if (is_add_list_widget == false)
	{
		addButtonsFromVector(client.GetDto().usernames);
		is_add_list_widget = true;
	}

	ui->stackedWidget->setCurrentWidget(ui->listWidget);

	std::cout << "Created list widget\n";
}

void Chat::on_Message_SendButton_clicked()
{
	QString message = ui->Message_LineMessageEdit->text();

	if (!message.isEmpty())
	{
		std::cout << "message: " << message.toStdString() << " from(login): " << client.GetMessageInfo().from << " to(name): " << last_chat_name << '\n';
		client.SendMessageTest(message.toStdString(), client.GetMessageInfo().from, last_chat_name);
	}

	ui->Message_LineMessageEdit->clear();
}

void Chat::OpenChatWithUser(const std::string name)
{
	std::cout << "Open chat with user name: " << name << '\n';
	ui->stackedWidget->setCurrentWidget(ui->message);
	last_chat_name = name;
}

void Chat::MoveBack()
{
	qDebug() << "back clicked\n";
	ui->stackedWidget->setCurrentIndex(0);
}

void Chat::SearchUsers(const QString& name)
{
	for (int i = 0; i < ui->listWidget->count(); ++i)
	{
		QListWidgetItem* item = ui->listWidget->item(i);

		if (item) 
		{
			if (name.isEmpty())
			{
				item->setHidden(false);
			}
			else
			{
				bool containsText = item->text().contains(name, Qt::CaseInsensitive);
				item->setHidden(!containsText);
			}
		}
	}
}

Chat::~Chat()
{
	delete ui;
}