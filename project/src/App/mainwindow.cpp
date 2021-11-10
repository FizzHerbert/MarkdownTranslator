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
	action_new_ = new QAction(tr("&新建"), this);
	action_new_->setShortcut(QKeySequence::New);
	action_new_->setStatusTip(tr("创建一个新的MarkDown文件"));
	connect(action_new_, &QAction::triggered, this, &MainWindow::New);

	action_open_ = new QAction(tr("&导入"), this);
	action_open_->setShortcut(QKeySequence::Open);
	action_open_->setStatusTip(tr("打开一个已存在的MarkDown文件"));
	connect(action_open_, &QAction::triggered, this, &MainWindow::Open);

	action_save_ = new QAction(tr("&保存"), this);
	action_save_->setShortcut(QKeySequence::Save);
	action_save_->setStatusTip(tr("保存MarkDown文件"));
	connect(action_save_, &QAction::triggered, this, &MainWindow::Save);

	action_saveas_ = new QAction(tr("另存为"), this);
	action_saveas_->setShortcut(tr("ctrl+a"));
	action_saveas_->setStatusTip(tr("将MarkDown文件另存为"));
	connect(action_saveas_, &QAction::triggered, this, &MainWindow::SaveAs);

	action_save_html_ = new QAction(tr("导出HTML"), this);
	action_save_html_->setShortcut(tr("ctrl+h"));
	action_save_html_->setStatusTip(tr("将生成的HTML文档保存为HTML文件"));
	connect(action_save_html_, &QAction::triggered, this, &MainWindow::SaveHTML);

	action_translate_ = new QAction(tr("转换"), this);
	action_translate_->setShortcut(tr("ctrl+t"));
	action_translate_->setStatusTip(tr("将MarkDown文档转换为HTML文档"));
	connect(action_translate_, &QAction::triggered, this, &MainWindow::Translate);

	//action_upload_ = new QAction(tr("上传"), this);
	//action_upload_->setShortcut(tr("ctrl+u"));
	//action_upload_->setStatusTip(tr("将生成的HTML文档上传到FTP"));

	action_preview_ = new QAction(tr("预览"), this);
	action_preview_->setShortcut(tr("ctrl+p"));
	action_preview_->setStatusTip(tr("预览MarkDown文档"));
	connect(action_preview_, &QAction::triggered, this, &MainWindow::Preview);
}


void MainWindow::CreateMenus()
{
	//在文件菜单栏添加功能
	menu_file_ = this->menuBar()->addMenu(tr("文件"));								
	menu_file_->setStatusTip(tr("文件菜单"));
	menu_file_->addAction(action_new_);
	menu_file_->addAction(action_open_);
	menu_file_->addAction(action_save_);
	menu_file_->addAction(action_saveas_);
	menu_file_->addAction(action_save_html_);

	//在工具菜单栏添加功能
	menu_tool_ = this->menuBar()->addMenu(tr("工具"));								
	menu_tool_->setStatusTip(tr("工具菜单"));
	menu_tool_->addAction(action_translate_);
	//menu_tool_->addAction(action_upload_);
	menu_tool_->addAction(action_preview_);
}


void MainWindow::CreateToolBars()
{
	toolbar_ = this->addToolBar(tr("工具栏"));
	toolbar_->addAction(action_new_);
	toolbar_->addAction(action_open_);
	toolbar_->addAction(action_save_);
	toolbar_->addAction(action_saveas_);
	toolbar_->addAction(action_save_html_);

	//将文件相关功能与工具相关功能分隔开
	toolbar_->addSeparator();														
	toolbar_->addAction(action_translate_);
	//toolbar_->addAction(action_upload_);
	toolbar_->addAction(action_preview_);
}


void MainWindow::CreateStatusBar()
{
	//程序初始化后显示已就绪
	statusBar()->showMessage(tr("程序已就绪"));										
}


void MainWindow::CreateCentralWidget()
{
	//生成主窗口
	central_widget_ = new QWidget();

	box_md_ = new QGroupBox("MarkDown");
	box_html_ = new QGroupBox("HTML");
	box_preview_ = new QGroupBox("预览");
	//box_preview_ = new QGroupBox("预览");
	
	text_edit_md_ = new QTextEdit;
	text_edit_html_ = new QTextEdit;
	text_edit_preview_ = new QTextEdit;
	//web_engine_view_md_ = new QWebEngineView;

	md_layout_ = new QVBoxLayout(box_md_);
	html_layout_ = new QVBoxLayout(box_html_);
	preview_layout_ = new QVBoxLayout(box_preview_);
	box_layout_ = new QGridLayout();

	//相关的布局，其中md的text框在左，html的在右
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
	//清空两个文本框并设置新建文件标签为true
	text_edit_md_->clear();															
	text_edit_html_->clear();
	new_file_symbol_ = true;
	QMessageBox::information(this, "新建成功", "成功新建MarkDown文件！", QMessageBox::Ok);
}


void MainWindow::Open()
{
	QString file_name_ = QFileDialog::getOpenFileName(this, tr("打开MarkDown文件"), "../../../test/", tr("MarkDown(*.md)"));

	if (file_name_.isEmpty() == false)//打开文件并显示在text_edit_md_上
	{
		QFile md_file_(file_name_);

		if (md_file_.open(QFile::ReadOnly | QFile::Text) == false)
		{
			QMessageBox::warning(this, "Warning!", "MarkDown文件打开失败！");
		}
		else
		{
			text_edit_md_->setPlainText(md_file_.readAll());//读入MarkDown文件并显示在text_edit_md_上
			md_file_.close();
			new_file_symbol_ = false;//设置新建文件标签为false
			file_name_md_ = file_name_;//保存当前打开的文件名
			QMessageBox::information(this, "打开成功", "成功打开MarkDown文件:" + file_name_, QMessageBox::Ok);
		}
	}
}


void MainWindow::Save()
{
	if (new_file_symbol_ == true)//当文件是新建的
	{
		SaveAs();
	}
	else//当文件是已有打开的
	{
		QFile md_file_(file_name_md_);
		QTextStream text_stream_md_(&md_file_);

		md_file_.open(QFile::WriteOnly | QFile::Text);
		text_stream_md_.setCodec("UTF-8");
		text_stream_md_ << text_edit_md_->toPlainText();
		md_file_.close();
		QMessageBox::information(this, "保存成功", "成功保存MarkDown文件:" + file_name_md_, QMessageBox::Ok);
	}
}


void MainWindow::SaveAs()
{
	QString file_name_ = QFileDialog::getSaveFileName(this, tr("保存MarkDown文件"), "../../../test/", tr("MarkDown(*.md)"));

	if (file_name_.isNull() == false)//保存文件
	{
		QFile md_file_(file_name_);
		if (md_file_.open(QFile::WriteOnly | QFile::Text) == false)//检测文件是否打开
		{
			QMessageBox::warning(this, "Warning!", "MarkDown文件创建失败！");
		}
		else
		{
			QTextStream text_stream_md_(&md_file_);	
			text_stream_md_.setCodec("UTF-8");
			text_stream_md_ << text_edit_md_->toPlainText();//把text_edit_md_中的内容写入文件
			md_file_.close();
			new_file_symbol_ = false;//设置新建文件标签为false
			file_name_md_ = file_name_;//保存当前的文件名
			QMessageBox::information(this, "保存成功", "成功保存MarkDown文件:" + file_name_, QMessageBox::Ok);
		}
	}
}


void MainWindow::SaveHTML()
{
	QString file_name_ = QFileDialog::getSaveFileName(this, tr("保存HTML文件"), "../../../result/", tr("HTML(*.html)"));

	if (file_name_.isNull() == false)//保存文件
	{
		QFile html_file_(file_name_);
		if (html_file_.open(QFile::WriteOnly | QFile::Text) == false)//检测文件是否打开
		{
			QMessageBox::warning(this, "Warning!", "HTML文件创建失败！");
		}
		else
		{
			QTextStream text_stream_md_(&html_file_);
			text_stream_md_.setCodec("UTF-8");
			text_stream_md_ << translator_md_->text_html_;//把translator_md_->text_html_中的内容写入文件
			html_file_.close();
			QMessageBox::information(this, "保存成功", "成功保存HTML文件:" + file_name_, QMessageBox::Ok);
		}
	}
}


void MainWindow::Translate()
{
	translator_md_ = new Translator(text_edit_md_, file_name_md_);

	translator_md_->Translate();//进行转换
	text_edit_html_->setPlainText(translator_md_->text_html_);
	QMessageBox::information(this, "转换成功", "成功转换为HTML形式", QMessageBox::Ok);
}


void MainWindow::Preview()
{
	if (translator_md_ != NULL)
	{
		text_edit_preview_->setText(translator_md_->text_html_);
	}
}


