#include "fenetre.h"
#include "session.h"
#include "ui_fenetre.h"

#include <QtSql/QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDoubleValidator>
#include <vector>
#include <iostream>

using namespace std;

Fenetre::Fenetre(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Fenetre)
{
    ui->setupUi(this);

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("supermarche.db"); // set the database that we want to use
    m_db.open(); // boolean (1 or 0) ; 1 if the database has been succesfully opened ; 0 if the database couldn't be open

    // signals :
    QObject::connect(ui->pushButtonInsertToDB, &QPushButton::clicked, this, &Fenetre::insertToDB);
    QObject::connect(ui->pushButtonDeleteFromDB, &QPushButton::clicked, this, &Fenetre::deleteFromDB);
    QObject::connect(ui->pushButtonClearInputs, &QPushButton::clicked, this, &Fenetre::clearInputs);
    QObject::connect(ui->lineEditLibelle, &QLineEdit::textChanged, this, &Fenetre::checkInputs);
    QObject::connect(ui->lineEditMarque, &QLineEdit::textChanged, this, &Fenetre::checkInputs);
    QObject::connect(ui->lineEditDescription, &QLineEdit::textChanged, this, &Fenetre::checkInputs);
    QObject::connect(ui->lineEditPrix, &QLineEdit::textChanged, this, &Fenetre::checkInputs);
    QObject::connect(ui->comboBoxFeature, &QComboBox::currentIndexChanged, this, &Fenetre::selectFeature);
    QObject::connect(ui->checkBoxUpdateDB, &QCheckBox::stateChanged, this, &Fenetre::canUpdateDBOrNot);

    checkInputs();
    ui->lineEditPrix->setValidator(new QDoubleValidator(0, 10000, 2, this));
    ui->pushButtonInsertToDB->hide();
    ui->pushButtonDeleteFromDB->hide();
    ui->pushButtonClearInputs->hide();
    ui->labelMarque->hide();
    ui->labelDescription->hide();
    ui->labelPrix->hide();
    ui->lineEditMarque->hide();
    ui->lineEditDescription->hide();
    ui->lineEditPrix->hide();
    ui->lineEditLibelle->hide();
    ui->labelLibelle->hide();
    ui->tableWidgetDBTable->hide();
    ui->checkBoxUpdateDB->hide();

    ui->tableWidgetDBTable = new QTableWidget(this);

    ui->tableWidgetDBTable->hide();
    ui->tableWidgetDBTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QObject::connect(ui->tableWidgetDBTable, &QTableWidget::cellChanged, this, &Fenetre::updateDB);
}

void Fenetre::firstStatusUi(const QString &whatSession)
{
    if(whatSession=="Employé"){

        setWindowTitle("Supermarché - Employé");

        ui->comboBoxFeature->removeItem(3);
        ui->labelWhoAmI->setText("Connecté en tant qu'employé");
    }

    if(whatSession=="Client"){

        setWindowTitle("Supermarché - Client");

        ui->labelJeVeux->hide();
        ui->comboBoxFeature->hide();
        ui->labelWhoAmI->setText("Connecté en tant que client");
        ui->comboBoxFeature->removeItem(1);
        ui->comboBoxFeature->removeItem(1);

        createTable();
        ui->tableWidgetDBTable->setGeometry(60,50,510,500);
    }

    if(whatSession=="Directeur"){

        setWindowTitle("Supermarché - Directeur");
        ui->labelWhoAmI->setText("Connecté en tant que directeur");
    }
}
void Fenetre::checkInputs(){
    if((ui->lineEditLibelle->text().length()!=0)&&(ui->lineEditMarque->text().length()!=0)&&(ui->lineEditDescription->text().length()!=0)&&(ui->lineEditPrix->text().length()!=0)) // if all inputs aren't empty, returns true and enter the loop
        ui->pushButtonInsertToDB->setEnabled(true); // change pushButtonInsertToDB to clickable if all inputs are registered
    else
        ui->pushButtonInsertToDB->setEnabled(false);// change pushButtonInsertToDB to not clickable if one or more inputs are empty

    if(ui->lineEditLibelle->text().length()!=0)
        ui->pushButtonDeleteFromDB->setEnabled(true); // change pushButtonDeleteFromDB to clickable if libelle is registered
    else
        ui->pushButtonDeleteFromDB->setEnabled(false);// change pushButtonDeleteFromDB to not clickable if libelle is empty
}

void Fenetre::insertToDB(){
    //Variables (QString) containing lineEdits's values typed by the user :
    QString libelle = ui->lineEditLibelle->text();
    QString marque = ui->lineEditMarque->text();
    QString description = ui->lineEditDescription->text();
    QString prix = ui->lineEditPrix->text();

    float prixF = prix.toFloat(); // change QString prix to a float called prixF

    QSqlQuery queryCheckData;

    queryCheckData.prepare("SELECT Libelle from produits where Libelle = :Libelle"); //SQL Query that we want to execute
    queryCheckData.bindValue(":Libelle", libelle );

    queryCheckData.exec(); // execute the prepared query

    QMessageBox errorMessageAlreadyInDatabase;

    int nb = 0;
    while (queryCheckData.next())
        nb++;

    if (nb >0){
        errorMessageAlreadyInDatabase.setText("Deja dans la bdd");
        errorMessageAlreadyInDatabase.exec();
    }
    else{
        QSqlQuery sendToBDD;

        sendToBDD.prepare("INSERT INTO produits (Libelle, Marque, Description, Prix) VALUES (:Libelle, :Marque, :Description, :Prix)"); //SQL Query that we want to execute
        sendToBDD.bindValue(":Libelle", libelle ); // Libelle value will be lineEditLibelle's value
        sendToBDD.bindValue(":Marque", marque); // Marque value will be lineEditMarque's value
        sendToBDD.bindValue(":Description", description); // Description value will be lineEditDescription's value
        sendToBDD.bindValue(":Prix", prixF); // Prix value will be lineEditPrix's value

        sendToBDD.exec(); // execute the prepared query

        QMessageBox successMessage;
        successMessage.setText("Ajouté à la BDD avec succès");
        successMessage.exec();
    }
    clearInputs();
}

void Fenetre::deleteFromDB(){
    QString libelle = ui->lineEditLibelle->text(); // Strings containing lineEdit's value

    QSqlQuery queryCheckDataForDelete;

    queryCheckDataForDelete.prepare("SELECT Libelle from produits where Libelle = :Libelle"); //SQL Query that we want to execute
    queryCheckDataForDelete.bindValue(":Libelle", libelle );

    queryCheckDataForDelete.exec(); // execute the prepared query

    QMessageBox errorMessageNotInDatabase;

    int nb = 0;
    while (queryCheckDataForDelete.next())
        nb++;

    if (nb >0){
        QSqlQuery deleteFromDB;
        deleteFromDB.prepare("DELETE FROM produits where Libelle = :Libelle"); //SQL Query that we want to execute
        deleteFromDB.bindValue(":Libelle", libelle ); // Libelle value will be lineEditLibelle's value
        deleteFromDB.exec(); // execute the prepared query

        QMessageBox successMessage;
        successMessage.setText("Supprimé de la BDD avec succès");
        successMessage.exec();
    }
    else{
        errorMessageNotInDatabase.setText("Ce produit n'est pas dans la BDD");
        errorMessageNotInDatabase.exec();
    }
    clearInputs();
}

void Fenetre::clearInputs(){
    ui->lineEditPrix->setText("");
    ui->lineEditMarque->setText("");
    ui->lineEditLibelle->setText("");
    ui->lineEditDescription->setText("");
}

void Fenetre::selectFeature(){

    if(ui->comboBoxFeature->currentText()=="Ajouter un produit à la base de données"){ // Don't use currentIndex cz some might be deleted depending on the current session, so it might, consequently messed up indexing
        ui->pushButtonInsertToDB->show();
        ui->labelMarque->show();
        ui->labelDescription->show();
        ui->labelPrix->show();
        ui->lineEditMarque->show();
        ui->lineEditDescription->show();
        ui->lineEditPrix->show();
        ui->lineEditLibelle->show();
        ui->labelLibelle->show();
        ui->pushButtonDeleteFromDB->hide();
        ui->pushButtonClearInputs->show();
        ui->checkBoxUpdateDB->hide();
        ui->tableWidgetDBTable->hide();
    }

    if(ui->comboBoxFeature->currentText()=="Supprimer un produit de la base de données"){ // Don't use currentIndex cz some might be deleted depending on the current session, so it might, consequently messed up indexing

        ui->pushButtonInsertToDB->hide();
        ui->labelMarque->hide();
        ui->labelDescription->hide();
        ui->labelPrix->hide();
        ui->lineEditMarque->hide();
        ui->lineEditDescription->hide();
        ui->lineEditPrix->hide();
        ui->lineEditLibelle->show();
        ui->labelLibelle->show();
        ui->checkBoxUpdateDB->hide();
        ui->pushButtonDeleteFromDB->show();
        ui->tableWidgetDBTable->hide();

    }
    if(ui->comboBoxFeature->currentText()=="Consulter la base de données"){ // Don't use currentIndex cz some might be deleted depending on the current session, so it might, consequently messed up indexing

        ui->pushButtonInsertToDB->hide();
        ui->labelMarque->hide();
        ui->labelDescription->hide();
        ui->labelPrix->hide();
        ui->lineEditMarque->hide();
        ui->lineEditDescription->hide();
        ui->lineEditPrix->hide();
        ui->lineEditLibelle->hide();
        ui->labelLibelle->hide();
        ui->pushButtonDeleteFromDB->hide();
        ui->pushButtonClearInputs->hide();
        ui->tableWidgetDBTable->show();
        ui->checkBoxUpdateDB->show();
        createTable();
    }
}

void Fenetre::createTable()
{
    QSqlQuery showProduitsFromBDD;

    showProduitsFromBDD.prepare("SELECT * FROM produits"); //SQL Query that we want to execute
    showProduitsFromBDD.exec(); // execute the prepared query

    vector <QString> IDVector;
    vector <QString> libellesVector;
    vector <QString> marquesVector;
    vector <QString> descriptionsVector;
    vector <QString> prixVector;

    while(showProduitsFromBDD.next()){

        IDVector.push_back(showProduitsFromBDD.value(0).toString());
        libellesVector.push_back(showProduitsFromBDD.value(1).toString());
        marquesVector.push_back(showProduitsFromBDD.value(2).toString());
        descriptionsVector.push_back(showProduitsFromBDD.value(3).toString());
        prixVector.push_back(showProduitsFromBDD.value(4).toString());
    }

    ui->tableWidgetDBTable->setColumnCount(5);
    ui->tableWidgetDBTable->setRowCount(libellesVector.size());

    QTableWidgetItem* headerId = new QTableWidgetItem("Id");
    ui->tableWidgetDBTable->setHorizontalHeaderItem(0,headerId);

    QTableWidgetItem* headerLibelle = new QTableWidgetItem("Libelle");
    ui->tableWidgetDBTable->setHorizontalHeaderItem(1,headerLibelle);

    QTableWidgetItem* headerMarque = new QTableWidgetItem("Marque");
    ui->tableWidgetDBTable->setHorizontalHeaderItem(2,headerMarque);

    QTableWidgetItem* headerDescription = new QTableWidgetItem("Description");
    ui->tableWidgetDBTable->setHorizontalHeaderItem(3,headerDescription);

    QTableWidgetItem* headerPrix = new QTableWidgetItem("Prix en €");
    ui->tableWidgetDBTable->setHorizontalHeaderItem(4,headerPrix);

    for(unsigned int i=0;i<libellesVector.size();i++){

        QTableWidgetItem *IdCell=new QTableWidgetItem (IDVector[i]);
        ui->tableWidgetDBTable->setItem(i, 0, IdCell);

        QTableWidgetItem *libelleCell=new QTableWidgetItem (libellesVector[i]);
        ui->tableWidgetDBTable->setItem(i, 1, libelleCell);

        QTableWidgetItem *marqueCell=new QTableWidgetItem (marquesVector[i]);
        ui->tableWidgetDBTable->setItem(i, 2, marqueCell);

        QTableWidgetItem *descriptionCell=new QTableWidgetItem (descriptionsVector[i]);
        ui->tableWidgetDBTable->setItem(i, 3, descriptionCell);

        QTableWidgetItem *prixCell=new QTableWidgetItem (prixVector[i]);
        ui->tableWidgetDBTable->setItem(i, 4, prixCell);
    }

    ui->tableWidgetDBTable->resizeColumnsToContents();
    ui->tableWidgetDBTable->setGeometry(80,135,500,400);
    ui->tableWidgetDBTable->show();
}

void Fenetre::canUpdateDBOrNot(){

    if(ui->checkBoxUpdateDB->isChecked()){
        ui->tableWidgetDBTable->setEditTriggers(QAbstractItemView::DoubleClicked); // https://openclassrooms.com/forum/sujet/qt-empecher-l-edition-dans-un-qtableview-90842
        ui->tableWidgetDBTable->column(0);
    }

    else
        ui->tableWidgetDBTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // https://openclassrooms.com/forum/sujet/qt-empecher-l-edition-dans-un-qtableview-90842
}

void Fenetre::updateDB()
{
    int columnModified = ui->tableWidgetDBTable->currentColumn();
    int rowModified = ui->tableWidgetDBTable->currentRow();

    QString valeurModifiee= ui->tableWidgetDBTable->model()->data(ui->tableWidgetDBTable->model()->index(rowModified,columnModified)).toString(); //https://askcodez.com/qt-c-lobtention-de-donnees-a-un-certain-cellule-dans-un-qtableview.html


    int idDeLaValeurModifiee= ui->tableWidgetDBTable->model()->data(ui->tableWidgetDBTable->model()->index(rowModified,0)).toInt(); // https://askcodez.com/qt-c-lobtention-de-donnees-a-un-certain-cellule-dans-un-qtableview.html
    int attributDeLaValeurModifiee= ui->tableWidgetDBTable->currentColumn();

    QSqlQuery modificationFromConsultation ;
    if(attributDeLaValeurModifiee!=0){

        if(attributDeLaValeurModifiee==1)
            modificationFromConsultation.prepare("UPDATE produits set Libelle = :valeurModifiee WHERE id= :idDeLaValeurModifiee");
        if(attributDeLaValeurModifiee==2)
            modificationFromConsultation.prepare("UPDATE produits set Marque = :valeurModifiee WHERE id= :idDeLaValeurModifiee");
        if(attributDeLaValeurModifiee==3)
            modificationFromConsultation.prepare("UPDATE produits set Description = :valeurModifiee WHERE id= :idDeLaValeurModifiee");
        if(attributDeLaValeurModifiee==4)
            modificationFromConsultation.prepare("UPDATE produits set Prix = :valeurModifiee WHERE id= :idDeLaValeurModifiee");

        modificationFromConsultation.bindValue(":valeurModifiee", valeurModifiee);
        modificationFromConsultation.bindValue(":idDeLaValeurModifiee", idDeLaValeurModifiee);
        modificationFromConsultation.exec();
    }
    else{

        QMessageBox cannotChangeId;
        cannotChangeId.setText("Impossible de modifier l'Id");
        cannotChangeId.exec();
    }
}

void Fenetre::showMainWindow(){

    this->show();
}

Fenetre::~Fenetre(){

    delete ui;
}

