#ifndef SESSION_H
#define SESSION_H

#include <QDialog>

namespace Ui {
class session;
}
class session : public QDialog
{
    Q_OBJECT

public:
    explicit session(QWidget *parent = nullptr);
    Ui::session *ui;
    ~session();

private slots:
    void checkPassword();

signals:
    void sessionTypeValue(const QString& whatSession);
    void timeOutValueClicked(const int& yesOrNoTimeOut);
};

#endif // SESSION_H
