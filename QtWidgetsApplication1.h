#pragma once

#include <QtWidgets/QWidget>
#include <QtCore>
#include <QtGui>
#include <QLabel>
#include <QMessageBox>
#include <QDialog>
#include "ui_QtWidgetsApplication1.h"

#include <QDirModel>
#include <QFileSystemModel>


class QtWidgetsApplication1 : public QWidget
{
	Q_OBJECT


public:
	QtWidgetsApplication1(QWidget *parent = Q_NULLPTR);


	QStringList *itemList = new QStringList; // Holds the information of all checked TreeWidgetItems.

	QStringList *pathlist = new QStringList; // Holds the paths of projects.

	QLabel *counter = new QLabel("Code count will appear here", this); // Displays the count.

	QPushButton *Calculate1 = new QPushButton("Calculate", this); // Button to calculate the lines of code.

	int logical = 0, total = 0, comment_l = 0; 

	QFont font = counter->font(); 

	// Icons for TreeWidgetItems.
	QFileIconProvider item_icon;
	QIcon dir_icon = item_icon.icon(QFileIconProvider::Folder); // Default Folder icon.
	QIcon file_icon = item_icon.icon(QFileIconProvider::File);	// Default File icon.





private:
	Ui::QtWidgetsApplication1Class ui;



public:
	void recursivecheck(QTreeWidgetItem *item); // recursively checks TreeWidgetItems.
	void recursivedelete(QTreeWidgetItem *item);// recursively deletes TreeWidgetItems.
	void fill_item(QTreeWidgetItem *item, QString sPath); // recursively fills the TreeWidget.
	void calculate(QString file_path); // Calculates the Lines of code.
	void calculation(); // Displays the lines of code.
	void loadsettings(); // Loads the saved projects on startup.
	void savesettings(); // saves the projects in an ini file.
	void add_new(QString sPath); // adds the groupboxes that contain buttons and a tree, initializes the tree and connects the buttons.
	

public slots:

	void slot_Start_calculation(); // starts calculation when "Calculate" button is pressed.
	void slot_add_new();  // adds new projects when "Add Project" button is clicked.
	void slot_ItemCheckStateChanged(QTreeWidgetItem *item, int column); // gets trigerred when a TreeWidgetItem gets selected.



									
};
