#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "translator.h"

QT_BEGIN_NAMESPACE
    class MainWindow;
	class QAction;
	class QMenu;
	class QWebEngineView;
	class QTextEdit;
	class QGroupBox;
	class QToolBar;
	class QVBoxLayout;
	class QGridLayout;
	class QWidget;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

	QMenu* menu_file_;																	//菜单栏文件项
	QMenu* menu_tool_;																	//菜单栏工具项
	QMenu* menu_help_;																	//菜单栏帮助项
	QToolBar* toolbar_;																	//工具栏
	QAction* action_new_;																//新建文件
	QAction* action_open_;																//打开md文件
	QAction* action_save_;																//保存md文件
	QAction* action_saveas_;															//另存为md文件
	QAction* action_translate_;															//转换md -> html
	QAction* action_save_html_;															//保存html文件
	//QAction* action_upload_;															//上传html文件
	QAction* action_preview_;															//预览md文件

	QTextEdit* text_edit_md_;															//显示和编辑md文件内容的文本框
	QTextEdit* text_edit_html_;															//显示html文件内容的文本框
	QTextEdit* text_edit_preview_;														//用于预览

	QGroupBox* box_md_;																	//下面的变量主要用于主窗口布局
	QGroupBox* box_html_;
	QGroupBox* box_preview_;

	QVBoxLayout* md_layout_;
	QVBoxLayout* html_layout_;
	QVBoxLayout* preview_layout_;
	QGridLayout* box_layout_;

	QWidget* central_widget_;

	bool new_file_symbol_;																//用于标识单前的文件是新建的还是已有的
	QString file_name_md_;																//记录当前打开的MarkDown文件的名字
	Translator* translator_md_;															//转换器

private:
	void CreateActions();																//创建各种功能
	void CreateMenus();																	//创建菜单栏
	void CreateToolBars();																//创建工具栏
	void CreateStatusBar();																//创建状态栏
	void CreateCentralWidget();															//创建主窗口并布局
	void InitOther();																	//对剩余的变量进行初始化	

public slots:
	void New();																			//新建一个MarkDown文件
	void Open();																		//打开MarkDown文件,只支持.md形式
	void Save();																		//保存一个已有的MarkDown文件
	void SaveAs();																		//保存MarkDown文件为一个新文件
	void SaveHTML();																	//保存生成的HTML文件

	void Translate();																	//将MarkDown转换为HTML并显示在text_edit_html_上	
	void Preview();																		//将MarkDown预览在text_edit_preview_上
	//void Upload();																

};
#endif // MAINWINDOW_H
