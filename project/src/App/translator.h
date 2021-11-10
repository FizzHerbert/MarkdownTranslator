#pragma once

#include <vector>
#include <QTextEdit>
#include <QString>
#include <string>
#include <iostream>


struct GrammarTreeNode 
{
	int grammar_type_;																//语法类型
	std::vector<GrammarTreeNode*> child_node_;										//孩子列表
	std::string content;															//转换的文本内容
	std::string path;																//转换的路径信息

	GrammarTreeNode(int type):grammar_type_(type), content(""), path(""){}
};

class Translator 
{

public:
	Translator(QTextEdit* text_, QString file_name_);
	~Translator();

public:
	QString text_html_;																//转换得到的HTML文本内容

private:
	GrammarTreeNode* grammar_tree_root_;											//语法树根节点
	QTextEdit* text_md_;															//MarkDown文本内容

public:
	void Translate();																//语法树转换成HTML源代码
	void DFS(GrammarTreeNode* tree_root_);											//深度优先遍历
	size_t PreTreatOneLine(QString text_line_);										//对一行文本进行预处理，去掉开头的空格
	void Convert(GrammarTreeNode* current_node_, std::string line_);				//对每一行内容进行转换
	std::pair<int, int> JudgeGrammarType(std::string text_line_); 					//判断该行文本的语法类型
	void FreeTree(GrammarTreeNode* tree_root_);										//释放语法树空间

private:
	enum grammar_types_
	{
		blankline = 0,																//空行
		text = 1,																	//文本
		link = 2,																	//链接
		ul = 3,																		//无序列表
		ol = 4,																		//有序列表
		li = 5,																		//列表
		italic = 6,																	//斜体
		bold = 7,																	//粗体
		resetline = 8,																//删除线
		image = 9,																	//图片
		h1 = 10,																	//一级标题
		h2 = 11,																	//二级标题
		h3 = 12,																	//三级标题
		h4 = 13,																	//四级标题
		h5 = 14,																	//五级标题
		h6 = 15,																	//六级标题
		changeline = 16,															//换行
	};

	std::vector<QString> html_start_ =
	{
		"",																			//空行开头语句
		"<p>",																		//文本开头语句
		"<p><a href=\'",															//链接开头语句
		"<ul>\n",																	//无序列表开头语句
		"<ol start=\'\' >\n",														//有序列表开头语句
		"<li>",																		//列表开头语句
		"<em>",																		//斜体开头语句
		"<strong>",																	//粗体开头语句
		"<del>",																	//删除线开头语句
		"<p><img src=\"", 															//图片开头语句
		"<h1>", 																	//一级标题开头语句
		"<h2>", 																	//二级标题开头语句
		"<h3>", 																	//三级标题开头语句
		"<h4>", 																	//四级标题开头语句
		"<h5>", 																	//五级标题开头语句
		"<h6>",																		//六级标题开头语句
		"<p>&nbsp"																	//换行开头语句
	};

	std::vector<QString> html_end_ =
	{
		"",																			//空行结尾语句
		"</p>\n", 																	//文本结尾语句
		"</a></p>\n",																//链接结尾语句
		"\n</ul>\n", 																//无序列表结尾语句
		"\n</ol>\n", 																//有序列表结尾语句
		"</li>\n", 																	//列表结尾语句
		"</em>",																	//斜体结尾语句
		"</strong>", 																//粗体结尾语句
		"</del>",																	//删除线结尾语句
		"\"></p>\n", 																//图片结尾语句
		"</h1>\n", 																	//一级标题结尾语句
		"</h2>\n",																	//二级标题结尾语句
		"</h3>\n", 																	//三级标题结尾语句
		"</h4>\n", 																	//四级标题结尾语句
		"</h5>\n", 																	//五级标题结尾语句
		"</h6>\n", 																	//六级标题结尾语句
		";</p>\n"																	//换行结尾语句
	};
};
