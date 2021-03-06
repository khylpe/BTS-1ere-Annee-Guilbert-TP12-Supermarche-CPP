#include "session.h"
#include "fenetre.h"
#include "ui_session.h"
#include "QMessageBox"
#include <QDebug>

// https://stackoverflow.com/questions/10896056/qt-signals-and-slots-in-different-classes

using namespace std;

session::session(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::session)
{
    ui->setupUi(this);
    QObject::connect(ui->pushButtonOk, &QPushButton::clicked, this, &session::checkPassword);
    ui->lineEditMDP->setEchoMode(QLineEdit::Password); // https://stackoverflow.com/questions/5834412/using-qlineedit-for-passwords
    setWindowTitle("Supermarché - Connexion");
}

void session::checkPassword(){
    QMessageBox msgBox;
    msgBox.setText("Souhaitez-vous que la session soit fermée après une inactivité de 5 minutes ?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    QMessageBox wrongPassword;
    wrongPassword.setText("Mauvais mot de passe");
    if(ui->comboBox->currentIndex()==1){
        if((ui->comboBox->currentIndex()==1) && (ui->lineEditMDP->text()=="Employé")){
            int timeOutValueClickedmsg = msgBox.exec();
            emit timeOutValueClicked(timeOutValueClickedmsg);
            this->hide();
            emit sessionTypeValue("Employé");
        }
        else
            wrongPassword.exec();
    }

    if(ui->comboBox->currentIndex()==2){
        if((ui->comboBox->currentIndex()==2) && (ui->lineEditMDP->text()=="Client")){
            this->hide();
            emit sessionTypeValue("Client");
        }

        else
            wrongPassword.exec();
    }

    if(ui->comboBox->currentIndex()==3){
        if((ui->comboBox->currentIndex()==3) && (ui->lineEditMDP->text()=="Directeur")){
            this->hide();
            emit sessionTypeValue("Directeur");
        }

        else
            wrongPassword.exec();
    }
}

session::~session(){
    delete ui;
}
