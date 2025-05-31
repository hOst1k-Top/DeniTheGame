#include "include/GameOverlayDialog.h"

GameOverlayDialog::GameOverlayDialog(QWidget *parent)
	: QDialog(parent),
	label(new QLabel(this)),
	okButton(new QPushButton(tr("OK"), this)),
	comboBox(new QComboBox(this)),
	confirmButton(new QPushButton(tr("Confirm"), this)),
	cancelButton(new QPushButton(tr("Candel"), this))
{
    setModal(true);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setStyleSheet("background-color: rgba(0, 0, 0, 200); color: white;");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(label, 0, Qt::AlignCenter);

    comboBox->hide();
    layout->addWidget(comboBox, 0, Qt::AlignCenter);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(confirmButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);

    layout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, [this]() {
        if (closeCallback) closeCallback();
        close();
    });
    connect(confirmButton, &QPushButton::clicked, this, &GameOverlayDialog::onConfirm);
    connect(cancelButton, &QPushButton::clicked, this, &GameOverlayDialog::onCancel);
}

GameOverlayDialog::~GameOverlayDialog()
{
	delete label;
	delete okButton;
	delete comboBox;
	delete confirmButton;
	delete cancelButton;
}

void GameOverlayDialog::showInfo(const QString& message, std::function<void()> onClose) {
    label->setText(message);
    comboBox->hide();
    confirmButton->hide();
    cancelButton->hide();
    okButton->show();
    closeCallback = std::move(onClose);
    showFullScreen();
}

void GameOverlayDialog::showSelection(const QString& title, const QStringList& items,
    std::function<void(const QString&)> onSelected,
    std::function<void()> onCancelled)
{
    label->setText(title);
    comboBox->clear();
    comboBox->addItems(items);
    comboBox->show();

    okButton->hide();
    confirmButton->show();
    cancelButton->show();

    selectionCallback = std::move(onSelected);
    cancelCallback = std::move(onCancelled);
    showFullScreen();
}

void GameOverlayDialog::onConfirm() {
    if (selectionCallback)
        selectionCallback(comboBox->currentText());
    close();
}

void GameOverlayDialog::onCancel() {
    if (cancelCallback)
        cancelCallback();
    close();
}