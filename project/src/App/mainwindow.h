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

	QMenu* menu_file_;																	//�˵����ļ���
	QMenu* menu_tool_;																	//�˵���������
	QMenu* menu_help_;																	//�˵���������
	QToolBar* toolbar_;																	//������
	QAction* action_new_;																//�½��ļ�
	QAction* action_open_;																//��md�ļ�
	QAction* action_save_;																//����md�ļ�
	QAction* action_saveas_;															//���Ϊmd�ļ�
	QAction* action_translate_;															//ת��md -> html
	QAction* action_save_html_;															//����html�ļ�
	//QAction* action_upload_;															//�ϴ�html�ļ�
	QAction* action_preview_;															//Ԥ��md�ļ�

	QTextEdit* text_edit_md_;															//��ʾ�ͱ༭md�ļ����ݵ��ı���
	QTextEdit* text_edit_html_;															//��ʾhtml�ļ����ݵ��ı���
	QTextEdit* text_edit_preview_;														//����Ԥ��

	QGroupBox* box_md_;																	//����ı�����Ҫ���������ڲ���
	QGroupBox* box_html_;
	QGroupBox* box_preview_;

	QVBoxLayout* md_layout_;
	QVBoxLayout* html_layout_;
	QVBoxLayout* preview_layout_;
	QGridLayout* box_layout_;

	QWidget* central_widget_;

	bool new_file_symbol_;																//���ڱ�ʶ��ǰ���ļ����½��Ļ������е�
	QString file_name_md_;																//��¼��ǰ�򿪵�MarkDown�ļ�������
	Translator* translator_md_;															//ת����

private:
	void CreateActions();																//�������ֹ���
	void CreateMenus();																	//�����˵���
	void CreateToolBars();																//����������
	void CreateStatusBar();																//����״̬��
	void CreateCentralWidget();															//���������ڲ�����
	void InitOther();																	//��ʣ��ı������г�ʼ��	

public slots:
	void New();																			//�½�һ��MarkDown�ļ�
	void Open();																		//��MarkDown�ļ�,ֻ֧��.md��ʽ
	void Save();																		//����һ�����е�MarkDown�ļ�
	void SaveAs();																		//����MarkDown�ļ�Ϊһ�����ļ�
	void SaveHTML();																	//�������ɵ�HTML�ļ�

	void Translate();																	//��MarkDownת��ΪHTML����ʾ��text_edit_html_��	
	void Preview();																		//��MarkDownԤ����text_edit_preview_��
	//void Upload();																

};
#endif // MAINWINDOW_H
