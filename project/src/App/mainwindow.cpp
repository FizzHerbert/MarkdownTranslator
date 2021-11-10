#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QMessageBox>
#include <iostream>
#include <QGroupBox>
#include <QAction>
#include <QToolBar>
#include <QWebEngineView>
#include <QTextEdit>
#include <QWidget>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CreateActions();
    CreateMenus();
    CreateToolBars();
    CreateStatusBar();
	CreateCentralWidget();
	InitOther();

    this->resize(1800, 900);
    this->setWindowTitle("MarkDown Translator");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::CreateActions()
{
	action_new_ = new QAction(tr("&�½�"), this);
	action_new_->setShortcut(QKeySequence::New);
	action_new_->setStatusTip(tr("����һ���µ�MarkDown�ļ�"));
	connect(action_new_, &QAction::triggered, this, &MainWindow::New);

	action_open_ = new QAction(tr("&����"), this);
	action_open_->setShortcut(QKeySequence::Open);
	action_open_->setStatusTip(tr("��һ���Ѵ��ڵ�MarkDown�ļ�"));
	connect(action_open_, &QAction::triggered, this, &MainWindow::Open);

	action_save_ = new QAction(tr("&����"), this);
	action_save_->setShortcut(QKeySequence::Save);
	action_save_->setStatusTip(tr("����MarkDown�ļ�"));
	connect(action_save_, &QAction::triggered, this, &MainWindow::Save);

	action_saveas_ = new QAction(tr("���Ϊ"), this);
	action_saveas_->setShortcut(tr("ctrl+a"));
	action_saveas_->setStatusTip(tr("��MarkDown�ļ����Ϊ"));
	connect(action_saveas_, &QAction::triggered, this, &MainWindow::SaveAs);

	action_save_html_ = new QAction(tr("����HTML"), this);
	action_save_html_->setShortcut(tr("ctrl+h"));
	action_save_html_->setStatusTip(tr("�����ɵ�HTML�ĵ�����ΪHTML�ļ�"));
	connect(action_save_html_, &QAction::triggered, this, &MainWindow::SaveHTML);

	action_translate_ = new QAction(tr("ת��"), this);
	action_translate_->setShortcut(tr("ctrl+t"));
	action_translate_->setStatusTip(tr("��MarkDown�ĵ�ת��ΪHTML�ĵ�"));
	connect(action_translate_, &QAction::triggered, this, &MainWindow::Translate);

	//action_upload_ = new QAction(tr("�ϴ�"), this);
	//action_upload_->setShortcut(tr("ctrl+u"));
	//action_upload_->setStatusTip(tr("�����ɵ�HTML�ĵ��ϴ���FTP"));

	action_preview_ = new QAction(tr("Ԥ��"), this);
	action_preview_->setShortcut(tr("ctrl+p"));
	action_preview_->setStatusTip(tr("Ԥ��MarkDown�ĵ�"));
	connect(action_preview_, &QAction::triggered, this, &MainWindow::Preview);
}


void MainWindow::CreateMenus()
{
	//���ļ��˵�����ӹ���
	menu_file_ = this->menuBar()->addMenu(tr("�ļ�"));								
	menu_file_->setStatusTip(tr("�ļ��˵�"));
	menu_file_->addAction(action_new_);
	menu_file_->addAction(action_open_);
	menu_file_->addAction(action_save_);
	menu_file_->addAction(action_saveas_);
	menu_file_->addAction(action_save_html_);

	//�ڹ��߲˵�����ӹ���
	menu_tool_ = this->menuBar()->addMenu(tr("����"));								
	menu_tool_->setStatusTip(tr("���߲˵�"));
	menu_tool_->addAction(action_translate_);
	//menu_tool_->addAction(action_upload_);
	menu_tool_->addAction(action_preview_);
}


void MainWindow::CreateToolBars()
{
	toolbar_ = this->addToolBar(tr("������"));
	toolbar_->addAction(action_new_);
	toolbar_->addAction(action_open_);
	toolbar_->addAction(action_save_);
	toolbar_->addAction(action_saveas_);
	toolbar_->addAction(action_save_html_);

	//���ļ���ع����빤����ع��ָܷ���
	toolbar_->addSeparator();														
	toolbar_->addAction(action_translate_);
	//toolbar_->addAction(action_upload_);
	toolbar_->addAction(action_preview_);
}


void MainWindow::CreateStatusBar()
{
	//�����ʼ������ʾ�Ѿ���
	statusBar()->showMessage(tr("�����Ѿ���"));										
}


void MainWindow::CreateCentralWidget()
{
	//����������
	central_widget_ = new QWidget();

	box_md_ = new QGroupBox("MarkDown");
	box_html_ = new QGroupBox("HTML");
	box_preview_ = new QGroupBox("Ԥ��");
	//box_preview_ = new QGroupBox("Ԥ��");
	
	text_edit_md_ = new QTextEdit;
	text_edit_html_ = new QTextEdit;
	text_edit_preview_ = new QTextEdit;
	//web_engine_view_md_ = new QWebEngineView;

	md_layout_ = new QVBoxLayout(box_md_);
	html_layout_ = new QVBoxLayout(box_html_);
	preview_layout_ = new QVBoxLayout(box_preview_);
	box_layout_ = new QGridLayout();

	//��صĲ��֣�����md��text������html������
	md_layout_->addWidget(text_edit_md_);											
	html_layout_->addWidget(text_edit_html_);
	preview_layout_->addWidget(text_edit_preview_);
	box_layout_->addWidget(box_md_, 0, 0, 1, 1);									
	box_layout_->addWidget(box_html_, 0, 1, 1, 1);
	box_layout_->addWidget(box_preview_, 0, 2, 1, 1);

	this->setCentralWidget(central_widget_);
	central_widget_->setLayout(box_layout_);
}


void MainWindow::InitOther()
{
	new_file_symbol_ = true;
	translator_md_ = NULL;
}


void MainWindow::New()
{
	//��������ı��������½��ļ���ǩΪtrue
	text_edit_md_->clear();															
	text_edit_html_->clear();
	new_file_symbol_ = true;
	QMessageBox::information(this, "�½��ɹ�", "�ɹ��½�MarkDown�ļ���", QMessageBox::Ok);
}


void MainWindow::Open()
{
	QString file_name_ = QFileDialog::getOpenFileName(this, tr("��MarkDown�ļ�"), "../../../test/", tr("MarkDown(*.md)"));

	if (file_name_.isEmpty() == false)//���ļ�����ʾ��text_edit_md_��
	{
		QFile md_file_(file_name_);

		if (md_file_.open(QFile::ReadOnly | QFile::Text) == false)
		{
			QMessageBox::warning(this, "Warning!", "MarkDown�ļ���ʧ�ܣ�");
		}
		else
		{
			text_edit_md_->setPlainText(md_file_.readAll());//����MarkDown�ļ�����ʾ��text_edit_md_��
			md_file_.close();
			new_file_symbol_ = false;//�����½��ļ���ǩΪfalse
			file_name_md_ = file_name_;//���浱ǰ�򿪵��ļ���
			QMessageBox::information(this, "�򿪳ɹ�", "�ɹ���MarkDown�ļ�:" + file_name_, QMessageBox::Ok);
		}
	}
}


void MainWindow::Save()
{
	if (new_file_symbol_ == true)//���ļ����½���
	{
		SaveAs();
	}
	else//���ļ������д򿪵�
	{
		QFile md_file_(file_name_md_);
		QTextStream text_stream_md_(&md_file_);

		md_file_.open(QFile::WriteOnly | QFile::Text);
		text_stream_md_.setCodec("UTF-8");
		text_stream_md_ << text_edit_md_->toPlainText();
		md_file_.close();
		QMessageBox::information(this, "����ɹ�", "�ɹ�����MarkDown�ļ�:" + file_name_md_, QMessageBox::Ok);
	}
}


void MainWindow::SaveAs()
{
	QString file_name_ = QFileDialog::getSaveFileName(this, tr("����MarkDown�ļ�"), "../../../test/", tr("MarkDown(*.md)"));

	if (file_name_.isNull() == false)//�����ļ�
	{
		QFile md_file_(file_name_);
		if (md_file_.open(QFile::WriteOnly | QFile::Text) == false)//����ļ��Ƿ��
		{
			QMessageBox::warning(this, "Warning!", "MarkDown�ļ�����ʧ�ܣ�");
		}
		else
		{
			QTextStream text_stream_md_(&md_file_);	
			text_stream_md_.setCodec("UTF-8");
			text_stream_md_ << text_edit_md_->toPlainText();//��text_edit_md_�е�����д���ļ�
			md_file_.close();
			new_file_symbol_ = false;//�����½��ļ���ǩΪfalse
			file_name_md_ = file_name_;//���浱ǰ���ļ���
			QMessageBox::information(this, "����ɹ�", "�ɹ�����MarkDown�ļ�:" + file_name_, QMessageBox::Ok);
		}
	}
}


void MainWindow::SaveHTML()
{
	QString file_name_ = QFileDialog::getSaveFileName(this, tr("����HTML�ļ�"), "../../../result/", tr("HTML(*.html)"));

	if (file_name_.isNull() == false)//�����ļ�
	{
		QFile html_file_(file_name_);
		if (html_file_.open(QFile::WriteOnly | QFile::Text) == false)//����ļ��Ƿ��
		{
			QMessageBox::warning(this, "Warning!", "HTML�ļ�����ʧ�ܣ�");
		}
		else
		{
			QTextStream text_stream_md_(&html_file_);
			text_stream_md_.setCodec("UTF-8");
			text_stream_md_ << translator_md_->text_html_;//��translator_md_->text_html_�е�����д���ļ�
			html_file_.close();
			QMessageBox::information(this, "����ɹ�", "�ɹ�����HTML�ļ�:" + file_name_, QMessageBox::Ok);
		}
	}
}


void MainWindow::Translate()
{
	translator_md_ = new Translator(text_edit_md_, file_name_md_);

	translator_md_->Translate();//����ת��
	text_edit_html_->setPlainText(translator_md_->text_html_);
	QMessageBox::information(this, "ת���ɹ�", "�ɹ�ת��ΪHTML��ʽ", QMessageBox::Ok);
}


void MainWindow::Preview()
{
	if (translator_md_ != NULL)
	{
		text_edit_preview_->setText(translator_md_->text_html_);
	}
}


