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
		"background-color: rgba(14, 14, 14, 1);"
		"}"
		"QListWidget::item {"
		"padding: 5px;"
		"}"
		"QListWidget::item:hover {"
		"background-color: #c0c0c0;"
		"}"
		"QPushButton {"
		"border: 5px solid gray;"
		"border-radius: 5px;"
		"padding: 10px;"
		"background-color: gray;"

		"padding: 5px;"
		"background-color: gray;"
		"}"
		"QPushButton:hover {"
		"background-color: #e0e0e0;"
		"}"
		"QPushButton:pressed {"
		"background-color: #d0d0d0;"
		"}");

	ui->message_list->setStyleSheet(
		"QListWidget {"
		"border: none;}"); 
	ui->message_list->setResizeMode(QListView::Adjust);

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

		item->setSizeHint(QSize(300, 100));  

		button->setStyleSheet("QPushButton {"
			"    background-color: rgba(147, 147, 147, 1);"
			"    color: white;"
			"    border-radius: 4px;"
			"    padding: 8px;"
			"    font-size: 24px;"  
			"}"
			"QPushButton:hover {"
			"   background-color: rgba(147, 147, 147, 1);"
			"}");

		QIcon icon(":/MainWindow/images/avatar.png");
		button->setIcon(icon);
		button->setIconSize(QSize(48, 48));

		ui->listWidget->addItem(item);
		ui->listWidget->setItemWidget(item, button);

		connect(button, &QPushButton::clicked, [this, name]() {
			OpenChatWithUser(name);
			});
	}
}

void Chat::addMessage(const QString& message, bool alignRight)
{
	QListWidgetItem* item = new QListWidgetItem(ui->message_list);

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
	QListWidgetItem* message_item = new QListWidgetItem(message);

	if (!message.isEmpty())
	{
		std::cout << "message: " << message.toStdString() << " from(login): " << client.GetMessageInfo().from << " to(name): " << last_chat_name << '\n';
		client.SendMessageTest(message.toStdString(), client.GetMessageInfo().from, last_chat_name);

		QVBoxLayout* message_layout = new QVBoxLayout();
		QLabel* messageLabel = new QLabel(message);
		message_layout->addWidget(messageLabel);

		QListWidgetItem* item = new QListWidgetItem();
		item->setSizeHint(QSize(100, 50));
		ui->message_list->addItem(item);
		ui->message_list->setItemWidget(item, new QWidget);
		ui->message_list->itemWidget(item)->setLayout(message_layout);
	}


	

	ui->message_list->addItem(message_item);

	ui->Message_LineMessageEdit->clear();
}

void Chat::OpenChatWithUser(const std::string name)
{
	std::cout << "Open chat with user name: " << name << '\n';
	ui->stackedWidget->setCurrentWidget(ui->message);
	ui->user_name->setText(QString::fromStdString(name));
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
			QWidget* widget = ui->listWidget->itemWidget(item);

			if (widget && widget->inherits("QPushButton")) 
			{
				QPushButton* button = qobject_cast<QPushButton*>(widget);

				if (button)
				{
					QString buttonText = button->text();
					bool containsText = buttonText.contains(name, Qt::CaseInsensitive);
					item->setHidden(!containsText);
				}
			}
		}
	}
}

Chat::~Chat()
{
	delete ui;
}