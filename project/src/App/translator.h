#pragma once

#include <vector>
#include <QTextEdit>
#include <QString>
#include <string>
#include <iostream>


struct GrammarTreeNode 
{
	int grammar_type_;																//�﷨����
	std::vector<GrammarTreeNode*> child_node_;										//�����б�
	std::string content;															//ת�����ı�����
	std::string path;																//ת����·����Ϣ

	GrammarTreeNode(int type):grammar_type_(type), content(""), path(""){}
};

class Translator 
{

public:
	Translator(QTextEdit* text_, QString file_name_);
	~Translator();

public:
	QString text_html_;																//ת���õ���HTML�ı�����

private:
	GrammarTreeNode* grammar_tree_root_;											//�﷨�����ڵ�
	QTextEdit* text_md_;															//MarkDown�ı�����

public:
	void Translate();																//�﷨��ת����HTMLԴ����
	void DFS(GrammarTreeNode* tree_root_);											//������ȱ���
	size_t PreTreatOneLine(QString text_line_);										//��һ���ı�����Ԥ����ȥ����ͷ�Ŀո�
	void Convert(GrammarTreeNode* current_node_, std::string line_);				//��ÿһ�����ݽ���ת��
	std::pair<int, int> JudgeGrammarType(std::string text_line_); 					//�жϸ����ı����﷨����
	void FreeTree(GrammarTreeNode* tree_root_);										//�ͷ��﷨���ռ�

private:
	enum grammar_types_
	{
		blankline = 0,																//����
		text = 1,																	//�ı�
		link = 2,																	//����
		ul = 3,																		//�����б�
		ol = 4,																		//�����б�
		li = 5,																		//�б�
		italic = 6,																	//б��
		bold = 7,																	//����
		resetline = 8,																//ɾ����
		image = 9,																	//ͼƬ
		h1 = 10,																	//һ������
		h2 = 11,																	//��������
		h3 = 12,																	//��������
		h4 = 13,																	//�ļ�����
		h5 = 14,																	//�弶����
		h6 = 15,																	//��������
		changeline = 16,															//����
	};

	std::vector<QString> html_start_ =
	{
		"",																			//���п�ͷ���
		"<p>",																		//�ı���ͷ���
		"<p><a href=\'",															//���ӿ�ͷ���
		"<ul>\n",																	//�����б�ͷ���
		"<ol start=\'\' >\n",														//�����б�ͷ���
		"<li>",																		//�б�ͷ���
		"<em>",																		//б�忪ͷ���
		"<strong>",																	//���忪ͷ���
		"<del>",																	//ɾ���߿�ͷ���
		"<p><img src=\"", 															//ͼƬ��ͷ���
		"<h1>", 																	//һ�����⿪ͷ���
		"<h2>", 																	//�������⿪ͷ���
		"<h3>", 																	//�������⿪ͷ���
		"<h4>", 																	//�ļ����⿪ͷ���
		"<h5>", 																	//�弶���⿪ͷ���
		"<h6>",																		//�������⿪ͷ���
		"<p>&nbsp"																	//���п�ͷ���
	};

	std::vector<QString> html_end_ =
	{
		"",																			//���н�β���
		"</p>\n", 																	//�ı���β���
		"</a></p>\n",																//���ӽ�β���
		"\n</ul>\n", 																//�����б��β���
		"\n</ol>\n", 																//�����б��β���
		"</li>\n", 																	//�б��β���
		"</em>",																	//б���β���
		"</strong>", 																//�����β���
		"</del>",																	//ɾ���߽�β���
		"\"></p>\n", 																//ͼƬ��β���
		"</h1>\n", 																	//һ�������β���
		"</h2>\n",																	//���������β���
		"</h3>\n", 																	//���������β���
		"</h4>\n", 																	//�ļ������β���
		"</h5>\n", 																	//�弶�����β���
		"</h6>\n", 																	//���������β���
		";</p>\n"																	//���н�β���
	};
};
