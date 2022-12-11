#include "QtWidgetsApplication1.h"
#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include <QDialog>
#include <QDirModel>
#include <QStringList>
#include <QDirIterator>
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QMainWindow>
#include <QGridLayout>
#include <QSet>
#include <QScrollArea>
#include <QIcon>
#include <QProcess>
#include <QFileIconProvider>
#include <QIODevice>
#include <QTextStream>
#include <QtConcurrent/QtConcurrent>


QtWidgetsApplication1::QtWidgetsApplication1(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.gb->setVisible(false);

	 loadsettings(); // Loads the saved projects at the launch.



	 ui.gridLayout->setContentsMargins(10, 10, 10, 10);
	 ui.gridLayout->setHorizontalSpacing(10);
	 ui.gridLayout->setVerticalSpacing(10);

	ui.scrollArea->setWidgetResizable(true);
	ui.scrollArea->setVisible(true);


	QPushButton *addbutton1 = new QPushButton("Add Project", this);
	



	ui.horizontalLayout->addWidget(counter, 0);
	ui.horizontalLayout->addWidget(Calculate1, 0);
	ui.horizontalLayout->addWidget(addbutton1, 0);

	addbutton1->setFixedSize(171, 51);
	Calculate1->setFixedSize(171, 51);
	counter->setMinimumSize(800, 51);


	connect(addbutton1, SIGNAL(clicked()), this, SLOT(slot_add_new()));
	connect(Calculate1, SIGNAL(clicked()), this, SLOT(slot_Start_calculation()));

	addbutton1->setVisible(true);
	Calculate1->setVisible(true);
	counter->setVisible(true);


	font.setBold(true);
	font.setPointSize(16);
	counter->setFont(font);

}

// Loads the saved projects.
void QtWidgetsApplication1::loadsettings()
{

	QSettings setting(QApplication::applicationDirPath()+"/savefile.ini",QSettings::IniFormat);



	int size = setting.beginReadArray("Paths");
	for (int i = 0;i < size;i++)
	{
		setting.setArrayIndex(i);
		QString string = setting.value("Path").toString();
		add_new(string);
		pathlist->append(string);
		
	}

}

// Saves added projects and updates when a project is deleted.
void QtWidgetsApplication1::savesettings() 
{


	QSettings setting(QApplication::applicationDirPath() + "/savefile.ini", QSettings::IniFormat);

	setting.clear();

	setting.beginWriteArray("Paths");

	for (int i = 0;i < pathlist->size();i++)
	{
		setting.setArrayIndex(i);
		setting.setValue("Path",pathlist->at(i));
	}

	setting.endArray();
}






// Gets triggered when "Add Project" button is clicked
void QtWidgetsApplication1::slot_add_new()
{
	QString sPath = QFileDialog::getExistingDirectory(this, "File Explorer"); 
	if (sPath.isEmpty() == false)
	{
		
		add_new(sPath);

		pathlist->append(sPath);
		savesettings();		// Saves the file path
	}
}

void QtWidgetsApplication1::add_new(QString sPath)
{
	//To track the insertion point to the layout.
	static int x{};



	


		int index = sPath.lastIndexOf('/');

		QGroupBox *nbox = new QGroupBox(QString(sPath.mid(index + 1, sPath.length())), this); // Parent to a button, a treewidget and a delete button.

		nbox->setFixedSize(281, 371);

		QGridLayout *layout = new QGridLayout(nbox); // holds the groupboxes

		QPushButton* button = new QPushButton(QString(sPath.mid(index + 1, sPath.length())), this); // button to change tree visibility.
		button->setFixedSize(271, 71);

		layout->addWidget(button, 0, 0);

		QTreeWidget *tree = new QTreeWidget(this); // Displays all the items.



		QTreeWidgetItem *item = new QTreeWidgetItem(); // TreeWidgetItem displays files and folders.
		item->setText(0, sPath.mid(index + 1, sPath.length()));
		item->setCheckState(0, Qt::Unchecked);
		item->setIcon(0, dir_icon);

		tree->setHeaderHidden(true);
		tree->addTopLevelItem(item);
		tree->setFixedSize(271, 271);

		/*fill_item(item, sPath);*/
		QtConcurrent::run(this, &QtWidgetsApplication1::fill_item, item, sPath);


		layout->addWidget(tree, 1, 0);

		nbox->setLayout(layout);

		ui.gridLayout->addWidget(nbox, x / 4, x % 4);

		x++;

		ui.scrollArea->widget()->setLayout(ui.gridLayout);



		QPushButton* del_button = new QPushButton("X", nbox); // button to delete groupboxes.
		del_button->setMaximumSize(50, 20);
		del_button->move(230, 5);

		QString color = "QPushButton:hover { \nbackground-color: rgb(255, 0, 0);\ncolor: rgb(255, 255, 255); \n}";
		del_button->setStyleSheet(color);

	  connect(tree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(slot_ItemCheckStateChanged(QTreeWidgetItem*, int))); // triggers when TreeWidgetItem gets checked.


	  connect(button, &QPushButton::clicked, this, [=]() { // Changes tree visibility from visible to hidden.

		  if (tree->isHidden() == true)
		  {
			  tree->setVisible(true);
			  nbox->setFixedSize(281, 371);

		  }
		  else
		  {
			  tree->setHidden(true);
			  nbox->setFixedSize(281, 100);
		  }

	  });


	  connect(del_button, &QPushButton::clicked, this, [=]() { // deletes the parent groupbox and changes the savefile.

		  QMessageBox option;
		  option.setText("Do you want to delete " + sPath.mid(index + 1, sPath.length()) + " ?");
		  option.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		  int del = option.exec();

		  if (del == QMessageBox::Ok)
		  {
			  recursivedelete(item);
			  delete nbox;

			  pathlist->removeAll(sPath);
			  savesettings();

		  }
	  });
  
}


// Gets triggered when a TreeWidgetItem gets checked or unchecked.
void QtWidgetsApplication1::slot_ItemCheckStateChanged(QTreeWidgetItem * item, int column)
{
	recursivecheck(item);
}

// Recursively looks for the checkstate of the TreeWidgetItem and its children.
void QtWidgetsApplication1::recursivecheck(QTreeWidgetItem *item)
{

	if (item->checkState(0) == Qt::Checked)
	{
		if (item->text(2) == "file")
		{

			if (itemList->contains(item->text(1)) == false)
			{
			itemList->append(item->text(1));
            }


		
		}
	}
	else
	{
		if (item->text(2) == "file")
		{
			itemList->removeAll(item->text(1));
		}		
	}


	for (int i = 0;i < item->childCount();i++)
	{
		if (item->checkState(0) == Qt::Checked)
		{
			item->child(i)->setCheckState(0, Qt::Checked);
		}
		else
		{
			item->child(i)->setCheckState(0, Qt::Unchecked);
		}
		recursivecheck(item->child(i));

	}
}



// Gets triggered after deleting a project and unchecks, checked TreeWidgetItems and deletes the groupbox.
void QtWidgetsApplication1::recursivedelete(QTreeWidgetItem *item)
{

	if (item->checkState(0) == Qt::Checked)
	{
		if (item->text(2) == "file")
		{
				itemList->removeAll(item->text(1));
		}

		
	}


	for (int i = 0;i < item->childCount();i++)
	{
		
		recursivedelete(item->child(i));

	}
}



// Fills the tree with filepaths.
void QtWidgetsApplication1::fill_item(QTreeWidgetItem *item, QString sPath)
{



	QDirIterator it1(sPath, QDir::Dirs | QDir::NoDotAndDotDot); // Only looks for Directories.

	while (it1.hasNext())
	{
		QString name = it1.next();

		if (QDir(name).count()==2)
		{
			continue;
		}

		QFileInfo file_info(name);
		int index = name.lastIndexOf('/');
		QTreeWidgetItem *child = new QTreeWidgetItem();
		child->setText(0, name.mid(index + 1, name.length()));
		child->setText(1, name);
		child->setText(2, "folder");
		child->setCheckState(0, Qt::Unchecked);


		if (file_info.isDir() == true)
		{
			child->setIcon(0, dir_icon);
			fill_item(child, name);
/*			QtConcurrent::run(this, &QtWidgetsApplication1::fill_item, child,name);*/
		}

		item->addChild(child);

	}

	QDirIterator it(sPath, QStringList() << "*.cpp" << "*.cc" << "*.h" << "*.hpp" << "*.inl" << "*.cs" << "*.java" << "*.sql", QDir::Files); // Only looks for files.

	while (it.hasNext())
	{
		QString name = it.next();
		int index = name.lastIndexOf('/');

		QTreeWidgetItem *child = new QTreeWidgetItem();
		child->setText(0, name.mid(index + 1, name.length()));
		child->setText(1, name);
		child->setText(2, "file");
		child->setCheckState(0, Qt::Unchecked);
		child->setIcon(0, file_icon);

		item->addChild(child);
	}
}



void QtWidgetsApplication1::calculation()
{

	counter->setText("Total lines = " + QString::number(total) + ", SLOC-L = " + QString::number(logical) + ", Comment lines = " + QString::number(comment_l));
}







void QtWidgetsApplication1::slot_Start_calculation()
{

	logical = 0;
	total = 0;
	comment_l = 0;




	// used to be for running in seperate threads, but now it only runs in the main thread.
	for (int i = 0;i < itemList->size();i++)
	{
/*		QtConcurrent::run(this, &QtWidgetsApplication1::calculate, itemList->at(i));*/
		calculate(itemList->at(i));

	}



	calculation();



}




// Reads a file then, calculates Total lines of code, Logical lines of code and Comment lines of code.
void QtWidgetsApplication1::calculate(QString file_path)
{


	bool bracket = false, comment = false, semicolon = false, quote = false;

	QFile inputFile(file_path);
	inputFile.open(QIODevice::ReadOnly);

		QTextStream in(&inputFile);
		do
		{
			QString line = in.readLine();

			bracket = false;
			semicolon = false;
			quote = false;

			total++;

			if (comment == true)
			{
				comment_l++;
			}
				
			line = line.trimmed();  // removes whitespaces at the start and at the end of string.

			// search line by line.
			for (int i = 0;i < line.length();i++)
			{
				// if line is a comment skip the line.
				if (comment == true && line.contains("*/") == false)
				{
					break;
				}
				// Skip unnecessary chars.
				if ( line.at(i) == ' ' || (line.at(i)> 59 && line.at(i)<125 && line.at(i) != '\\') || (comment == true && line.contains("*/") && i<line.indexOf("*/")))
				{
					continue;
				}



				if (line.startsWith("//"))
				{

					comment_l++;

					break;
				}

				if (line.contains("//") && i== line.indexOf("//")) // if the comment in line isnt at the begginning.
				{
					break;
				}

				// if comment is true and */ is encountered, comment is no longer true
				if (line.contains("*/") && i == line.indexOf("*/") && comment == true) 
				{
					comment = false;
					continue;
				}

				// if comment is false and /* is encountered, comment is no longer false
				if (line.contains("/*") && i == line.indexOf("/*") && comment == false)
				{
					comment = true;
					if (line.contains("*/") == false)
					{
						comment_l++;
						break;
					}
					else if(line.endsWith("*/") && i<line.indexOf("*/"))
					{
						comment = false;
						comment_l++;
						break;

					}
					continue;
				}

				if (line.contains("*/") && line.contains("/*"))
				{
					if (i> line.indexOf("/*") && i < line.indexOf("*/")) {
						i = line.indexOf("*/");
						comment = false;
						continue;
						comment_l++;
					}

				}

				// only the last semicolon is counted.
				// if there are characters after the semicolon, check if they are in a comment or not. if they aren't decrement logical by one.
				if (semicolon == true)
				{
					if (line.contains("//") && i>=line.lastIndexOf("//"))
					{

						break;
					}
					else
					{
						logical--;
						semicolon = false;
					}
				}

	

				// when a string is declared bool quote changes
				if (line.at(i) == "\"" && line.count("\"") > 1)
				{
					if (quote == false)
					{
						quote = true;
					}
					else if (quote == true)
					{
						quote = false;
					}

				}
				
				
				



				// if semicolon is the last character other than characters inside a comment, increase logical by one.
				if ((line.at(i) == ';') && semicolon==false && comment == false && quote == false)
				{
					logical++;
					semicolon = true;

				}

				// if curly bracket isn't inside a comment and a quote, increase logical by one.
				if ((line.at(i) == '}') &&comment == false && quote == false)
				{
					if (semicolon == false)
					{
						logical++;
					}
					if (semicolon == true)
					{
						semicolon = false;
					}
					}



			}
		}while (!in.atEnd());
		inputFile.close();





}