#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>

class GameOverlayDialog : public QDialog
{
	Q_OBJECT

public:
	enum DialogType {
		Info,
		Selection
	};

	GameOverlayDialog(QWidget *parent = nullptr);
	~GameOverlayDialog();

    void showInfo(const QString& message, std::function<void()> onClose = nullptr);
    void showSelection(const QString& title, const QStringList& items,
    std::function<void(const QString&)> onSelected,
    std::function<void()> onCancelled = nullptr);

private:
    QLabel* label;
    QPushButton* okButton;
    QComboBox* comboBox;
    QPushButton* confirmButton;
    QPushButton* cancelButton;

    std::function<void()> closeCallback;
    std::function<void(const QString&)> selectionCallback;
    std::function<void()> cancelCallback;

private slots:
    void onConfirm();
    void onCancel();

};
