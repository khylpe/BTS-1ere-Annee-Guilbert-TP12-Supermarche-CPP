#ifndef FENETRE_H
#define FENETRE_H

#include <QWidget>
#include <QSqlDatabase>
#include <session.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Fenetre; }
QT_END_NAMESPACE

class Fenetre : public QWidget
{
    Q_OBJECT

public:
    Fenetre(QWidget *parent = nullptr);
    ~Fenetre();

private:
    QSqlDatabase m_db;
    Ui::Fenetre *ui;

private slots :
    void checkInputs();
    void insertToDB();
    void deleteFromDB();
    void clearInputs();
    void selectFeature();
    void createTable();
    void updateDB();
    void canUpdateDBOrNot();
    void showMainWindow();
    void firstStatusUi(const QString& whatSession);
    void timeOut(const int& yesOrNoTimeOut);

};

#endif // FENETRE_H
