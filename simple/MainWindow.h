#pragma once

#include <QMainWindow>
#include <QPlainTextEdit>

namespace Ui {
    class MainWindow;
}

class QtProperty;

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

  private slots:
    void onValueChanged(QtProperty *property);

    void onPopupMenu(QtProperty *property);

  private:
    void createProperties();

    Ui::MainWindow *ui = nullptr;
    QtProperty *root_ = nullptr;
    QMenu *popupMenu_ = nullptr;
    QPlainTextEdit *txtOutput = nullptr;
};
