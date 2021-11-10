#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "translator.h"

Translator::Translator(QTextEdit* text_, QString file_name_)
{
	QStringList file_name_list_ = file_name_.split(QLatin1Char('/'));
	QString slim_file_name_ = file_name_list_.back().mid(0, file_name_list_.back().size() - 3);
	text_md_ = text_;
	grammar_tree_root_ = new GrammarTreeNode(blankline);
	//添加HTML开头部分
	text_html_ = "<!doctype html>\n<html>\n<head>\n";
	text_html_ += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width initial-scale=1'>\n";
	text_html_ = text_html_ + "<title>" + slim_file_name_ + "</title></head>\n<body>";

	//std::cout << "The file name is:" << std::string(slim_file_name_.toLocal8Bit()) << std::endl;
}


Translator::~Translator()
{
	if (grammar_tree_root_ != NULL) 
	{
		FreeTree(grammar_tree_root_);
	}
}


/*
函数Translate先对输入的Markdown文本每行进行预处理
将其转化为语法树，之后深度优先遍历该语法树得到对应的HTML代码
*/
void Translator::Translate() {
	QString text_md_temp_ = text_md_->document()->toPlainText();//获取MarkDown文本框中的内容
	QStringList text_list_md_ = text_md_temp_.split(QLatin1Char('\n'));//将内容按照换行符分割，并保留空行
	std::vector<std::pair<int, GrammarTreeNode*>>  list_node_list_;//用于层次列表
	size_t blank_line_num_ = 0;//表示空行的行数
	int index_ = -1;
	int pre_index_ = -1;

	for (size_t i = 0; i < text_list_md_.size(); i++)
	{
		if (text_list_md_[i].size() == 0)//如果是空行，看已经空了几行了
		{
			blank_line_num_++;
			if (blank_line_num_ > 1 && blank_line_num_ % 2 == 1)//说明至少上两行已经是空行
			{
				grammar_tree_root_->child_node_.push_back(new GrammarTreeNode(changeline));
			}
			continue;
		}

		blank_line_num_ = 0;//说明不是空行，复原
		pre_index_ = index_;
		index_ = PreTreatOneLine(text_list_md_[i]);
		std::string slim_line_ = text_list_md_[i].toStdString().substr(index_);//去除行首的空格

		//语法解析
		std::pair<int, int> type_ = JudgeGrammarType(slim_line_);

		//创建text结点
		if (type_.first == text) 
		{
			grammar_tree_root_->child_node_.push_back(new GrammarTreeNode(text));
			//逐字符插入
			Convert(grammar_tree_root_->child_node_.back(), slim_line_);
		}
		else if (type_.first >= h1 && type_.first <= h6)
		{
			if (index_ > 0)//这时会被作为text结点创建
			{
				grammar_tree_root_->child_node_.push_back(new GrammarTreeNode(text));
				Convert(grammar_tree_root_->child_node_.back(), slim_line_);
			}
			else//创建header结点
			{
				grammar_tree_root_->child_node_.push_back(new GrammarTreeNode(type_.first));
				Convert(grammar_tree_root_->child_node_.back(), slim_line_.substr(type_.second));
				//grammar_tree_root_->child_node_.back()->content = slim_line_.substr(type_.second);
			}
		}
		else if (type_.first == ul || type_.first == ol)//创建无序list结点
		{
			//判断是否为列表的第一项或者层次列表第一项
			//文档开始,或者语法树最后一个结点不是无序列表结点
			if (grammar_tree_root_->child_node_.empty() || (grammar_tree_root_->child_node_.back()->grammar_type_ != ul \
				&& grammar_tree_root_->child_node_.back()->grammar_type_ != ol))
			{
				GrammarTreeNode* tree_node_;

				//创建无序列表
				if (type_.first == ul)
				{
					tree_node_ = new GrammarTreeNode(ul);
				}
				else
				{
					tree_node_ = new GrammarTreeNode(ol);
				}
				
				grammar_tree_root_->child_node_.push_back(tree_node_);
				list_node_list_.clear();
				list_node_list_.push_back(std::make_pair(index_, tree_node_));
			}
			else if (index_ > list_node_list_.back().first)//说明是下一个层次的
			{
				GrammarTreeNode* tree_node_;

				if (type_.first == ul)
				{
					tree_node_ = new GrammarTreeNode(ul);
				}
				else
				{
					tree_node_ = new GrammarTreeNode(ol);
				}

				list_node_list_.back().second->child_node_.back()->child_node_.push_back(tree_node_);
				list_node_list_.push_back(std::make_pair(index_, tree_node_));
			}
			else if (index_ < list_node_list_.back().first)//说明是更高层次的
			{
				for (int i = list_node_list_.size() - 1; i >= 0; i--)
				{
					if (list_node_list_[i].first == index_)
					{
						list_node_list_.resize(i + 1);
					}
				}
			}

			//给无序列表加入列表子节点
			list_node_list_.back().second->child_node_.push_back(new GrammarTreeNode(li));
			//给列表子节点插入内容
			Convert(list_node_list_.back().second->child_node_.back(), slim_line_.substr(type_.second));
		}
		else if (type_.first == link)//创建link结点
		{
			grammar_tree_root_->child_node_.push_back(new GrammarTreeNode(link));
			Convert(grammar_tree_root_->child_node_.back(), slim_line_.substr(type_.second));
		}
		else if (type_.first == image)//创建image结点
		{
			grammar_tree_root_->child_node_.push_back(new GrammarTreeNode(image));
			Convert(grammar_tree_root_->child_node_.back(), slim_line_.substr(type_.second));
		}
	}
	
	DFS(grammar_tree_root_);//展开语法树,生成HTML文档
	text_html_ += "</body>\n</html>";//添加html结尾部分
	//std::cout << std::string(text_html_.toLocal8Bit()) << std::endl;
}


/*
函数DFS使用深度优先遍历的方法将语法树转换成HTML源代码
其中参数root表示语法树的根节点
*/
void Translator::DFS(GrammarTreeNode* root_)
{
	//插入前置标签
	if ((root_->grammar_type_ == ul || root_->grammar_type_ == ol) && text_html_[text_html_.size() - 1] != '\n')
	{
		text_html_ += '\n';
	}	

	text_html_ += html_start_[root_->grammar_type_];

	//link结点
	if (root_->grammar_type_ == link) 
	{
		text_html_ += QString::fromStdString(root_->path);
		text_html_ += "\'>";
		text_html_ += QString::fromStdString(root_->content);
	}
	//image结点
	else if (root_->grammar_type_ == image) 
	{
		text_html_ += QString::fromStdString(root_->path);
		if (root_->content[root_->content.size() - 1] == '>')//<path, name>形式
		{
			text_html_ += "\" alt=\"";
			root_->content.resize(root_->content.size() - 1);
		}
		else//![name](path)形式
		{
			text_html_ += "\" referrerpolicy=\"no-referrer\" alt=\"";

		}
		text_html_ += QString::fromStdString(root_->content);
	}
	else//text结点及其他结点
	{
		text_html_ += QString::fromStdString(root_->content);
	}

	//深度遍历孩子结点
	for (GrammarTreeNode* child_node_ : root_->child_node_)
	{
		DFS(child_node_);
	}

	//插入后置标签
	text_html_ += html_end_[root_->grammar_type_];
}


/*
函数PreTreatOneLine用来对一行进行预处理
其读入一行字符，去掉这一行最开头的空格
*/
size_t Translator::PreTreatOneLine(QString text_line_)
{
	size_t index = text_line_.size();
	for (size_t i = 0; i < text_line_.size(); i++)
	{
		if (text_line_.toStdString()[i] != ' ')
		{
			index = i;
			break;
		}
	}

	return index;
}


/*
Convert函数针对不同类型的行
将其转换为不同的HTML
其中参数current_node_是指当前所在的结点
line_是指该行的字符串内容
*/
void Translator::Convert(GrammarTreeNode* current_node_, std::string line_) 
{

	bool is_bold_ = false;//标识是否在粗体范围内
	bool is_italic_ = false;//标识是否在斜体范围内
	bool is_resetline_ = false;//标识是否在删除线范围内

	current_node_->child_node_.push_back(new GrammarTreeNode(blankline));//用于纯文本
	
	if (current_node_->grammar_type_ == image)//image
	{
		if (line_[line_.size() - 1] != '>')//![name](path)形式的
		{
			size_t i = 0;
			for (; line_[i] != ']'; i++)
			{
				current_node_->content += line_[i];
			}
			for (i = i + 2; line_[i] != ')'; i++) 
			{
				current_node_->path += line_[i];
			}
		}
		else//<path>形式的
		{
			size_t i = 0;
			for (; line_[i] != '"'; i++)
			{
				current_node_->path += line_[i];
			}
			i++;
			while (line_[i] != '"')
			{
				i++;
			}
			for (i = i + 1; line_[i] != '"'; i++)
			{
				current_node_->content += line_[i];
			}
			current_node_->content += '>';
		}
	}
	else if (current_node_->grammar_type_ == link)//link
	{
		//[name](path)形式
		size_t i = 0;
		for (; line_[i] != ']'; i++)
		{
			current_node_->content += line_[i];
		}
		for (i = i + 2; line_[i] != ')'; i++)
		{
			current_node_->path += line_[i];
		}
	}
	else
	{
		size_t i = 0;
		std::vector<GrammarTreeNode*> pre_node_list_;//记录之前的结点，用于记录深度
		GrammarTreeNode* tree_node_;

		pre_node_list_.push_back(current_node_);
		while(i < line_.size())
		{
			if (line_[i] == '*')
			{
				if (is_italic_)//斜体结束
				{
					current_node_->child_node_.push_back(new GrammarTreeNode(blankline));
					pre_node_list_.resize(pre_node_list_.size() - 1);
					is_italic_ = false;
				}
				else if (line_[i + 1] == '*')//粗体
				{
					if (is_bold_)//粗体结束
					{
						current_node_->child_node_.push_back(new GrammarTreeNode(blankline));
						pre_node_list_.resize(pre_node_list_.size() - 1);
						is_bold_ = false;
					}
					else//粗体开始
					{
						tree_node_ = new GrammarTreeNode(bold);
						pre_node_list_.back()->child_node_.push_back(tree_node_);
						pre_node_list_.push_back(tree_node_);
						is_bold_ = true;
					}
					i++;
				}
				else//斜体开始
				{
					tree_node_ = new GrammarTreeNode(italic);
					pre_node_list_.back()->child_node_.push_back(tree_node_);
					pre_node_list_.push_back(tree_node_);
					is_italic_ = true;
				}
				i++;
			}
			else if (line_[i] == '_')
			{
				if (is_italic_)//斜体结束
				{
					current_node_->child_node_.push_back(new GrammarTreeNode(blankline));
					pre_node_list_.resize(pre_node_list_.size() - 1);
					is_italic_ = false;
				}
				else if (line_[i + 1] == '_')//粗体
				{
					if (is_bold_)//粗体结束
					{
						current_node_->child_node_.push_back(new GrammarTreeNode(blankline));
						pre_node_list_.resize(pre_node_list_.size() - 1);
						is_bold_ = false;
					}
					else//粗体开始
					{
						tree_node_ = new GrammarTreeNode(bold);
						pre_node_list_.back()->child_node_.push_back(tree_node_);
						pre_node_list_.push_back(tree_node_);
						is_bold_ = true;
					}
					i++;
				}
				else//斜体开始
				{
					tree_node_ = new GrammarTreeNode(italic);
					pre_node_list_.back()->child_node_.push_back(tree_node_);
					pre_node_list_.push_back(tree_node_);
					is_italic_ = true;
				}
				i++;
			}
			else if (strncmp(line_.c_str() + i, "~~", 2) == 0)//删除线
			{
				if (is_resetline_)//删除线结束
				{
					current_node_->child_node_.push_back(new GrammarTreeNode(blankline));
					pre_node_list_.resize(pre_node_list_.size() - 1);
					is_resetline_ = false;
				}
				else//删除线开始
				{
					tree_node_ = new GrammarTreeNode(resetline);
					pre_node_list_.back()->child_node_.push_back(tree_node_);
					pre_node_list_.push_back(tree_node_);
					is_resetline_ = true;
				}
				i += 2;
			}
			else//普通文字或者下划线(下划线无需额外处理)
			{
				if (pre_node_list_.size() == 1)
				{
					current_node_->child_node_.back()->content += line_[i++];
				}
				else
				{
					pre_node_list_.back()->content += line_[i++];
				}
			}
		}
	}
}


/*
函数JudgeGrammarType对一行字符进行判断，其是text/header/image/link/list/中的哪一个
参数text_line_表示一行字符串，返回值为<语法类型，后续内容起始位置>
*/
std::pair<int, int> Translator::JudgeGrammarType(std::string text_line_) 
{
	int title_level_ = 0;//标题等级
	int index_ = 0;//后续内容开始的下标

	//判断header
	for (size_t i = 0; i < text_line_.size(); i++)
	{
		if (text_line_[i] == '#')
		{
			title_level_++;
		}
		else
		{
			index_ = i;
			break;
		}
	}
	//判断结果为header(# + 空格)
	if (text_line_[index_] == ' ' && title_level_ > 0 && title_level_ <= 6) 
	{
		return std::make_pair(h1 + title_level_ - 1, index_ + 1);
	}

	//判断list
	//判断结果为无序list(- + 空格/* + 空格)
	if (strncmp(text_line_.c_str(), "- ", 2) == 0 || strncmp(text_line_.c_str(), "* ", 2) == 0)
	{
		return std::make_pair(ul, 2);
	}

	//判断结果为有序list(数字 + "." + 空格)
	index_ = text_line_.find('.');
	bool is_digit = true;//判断是否为数字的标志
	if (index_ != text_line_.npos && text_line_[index_ + 1] == ' ')
	{
		for (size_t i = 0; i < index_; i++)
		{
			if (text_line_[i] > '9' || text_line_[i] < '0')
			{
				is_digit = false;
			}
		}
		if (is_digit == true)//在.之前都为数字
		{
			return std::make_pair(ol, index_ + 2);
		}
	}

	//判断image
	bool signal1_ = false;//标识'['是否存在
	bool signal2_ = false;//标识'('是否存在
	bool signal3_ = false;//标识')'是否存在
	//判断结果为image(![ + ] + ( + ))
	if (strncmp(text_line_.c_str(), "![", 2) == 0) 
	{
		for (size_t i = 2; i < text_line_.size(); i++)//找到对应的 ] 和 ( )
		{
			if (text_line_[i] == ']' && text_line_[i + 1] == '(')
			{
				signal1_ = true;
				signal2_ = true;
			}
			if (text_line_[i] == ')')
			{
				signal3_ = true;
				break;
			}
		}
		if (signal1_ && signal2_ && signal3_)
		{
			return std::make_pair(image, 2);
		}
	}

	//判断结果为image(<img src= + >)
	bool signal_ = false;//标识'>'是否存在
	if (strncmp(text_line_.c_str(), "<img src=\"", 10) == 0)
	{
		for (size_t i = 10; i < text_line_.size(); i++)//找到对应的 >
		{
			if (text_line_[i] == '>')
			{
				signal_ = true;
			}
		}
		if (signal_)
		{
			return std::make_pair(image, 10);
		}
	}

	//判断link
	//判断结果为link([ + ] + ( + ))
	if (text_line_[0] == '[')
	{
		for (size_t i = 1; i < text_line_.size(); i++)//找到对应的 ] 和 ( )
		{
			if (text_line_[i] == ']' && text_line_[i + 1] == '(')
			{
				signal1_ = true;
				signal2_ = true;
			}
			if (text_line_[i] == ')')
			{
				signal3_ = true;
				break;
			}
		}
		if (signal1_ && signal2_ && signal3_)
		{
			return std::make_pair(link, 1);
		}
	}

	//其他情况统一判断为text
	return std::make_pair(text, 0);
}


/*
函数free_tree的功能是递归的释放语法树
其参数root表示语法树的根
从根开始递归的自下向上释放树的节点
*/
void Translator::FreeTree(GrammarTreeNode* root_)
{
	if (root_ != NULL) 
	{
		for (GrammarTreeNode* child : root_->child_node_)
		{
			FreeTree(child);
		}
		delete root_;
	}
}