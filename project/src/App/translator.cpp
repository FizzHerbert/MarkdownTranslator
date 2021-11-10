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
	//���HTML��ͷ����
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
����Translate�ȶ������Markdown�ı�ÿ�н���Ԥ����
����ת��Ϊ�﷨����֮��������ȱ������﷨���õ���Ӧ��HTML����
*/
void Translator::Translate() {
	QString text_md_temp_ = text_md_->document()->toPlainText();//��ȡMarkDown�ı����е�����
	QStringList text_list_md_ = text_md_temp_.split(QLatin1Char('\n'));//�����ݰ��ջ��з��ָ����������
	std::vector<std::pair<int, GrammarTreeNode*>>  list_node_list_;//���ڲ���б�
	size_t blank_line_num_ = 0;//��ʾ���е�����
	int index_ = -1;
	int pre_index_ = -1;

	for (size_t i = 0; i < text_list_md_.size(); i++)
	{
		if (text_list_md_[i].size() == 0)//����ǿ��У����Ѿ����˼�����
		{
			blank_line_num_++;
			if (blank_line_num_ > 1 && blank_line_num_ % 2 == 1)//˵�������������Ѿ��ǿ���
			{
				grammar_tree_root_->child_node_.push_back(new GrammarTreeNode(changeline));
			}
			continue;
		}

		blank_line_num_ = 0;//˵�����ǿ��У���ԭ
		pre_index_ = index_;
		index_ = PreTreatOneLine(text_list_md_[i]);
		std::string slim_line_ = text_list_md_[i].toStdString().substr(index_);//ȥ�����׵Ŀո�

		//�﷨����
		std::pair<int, int> type_ = JudgeGrammarType(slim_line_);

		//����text���
		if (type_.first == text) 
		{
			grammar_tree_root_->child_node_.push_back(new GrammarTreeNode(text));
			//���ַ�����
			Convert(grammar_tree_root_->child_node_.back(), slim_line_);
		}
		else if (type_.first >= h1 && type_.first <= h6)
		{
			if (index_ > 0)//��ʱ�ᱻ��Ϊtext��㴴��
			{
				grammar_tree_root_->child_node_.push_back(new GrammarTreeNode(text));
				Convert(grammar_tree_root_->child_node_.back(), slim_line_);
			}
			else//����header���
			{
				grammar_tree_root_->child_node_.push_back(new GrammarTreeNode(type_.first));
				Convert(grammar_tree_root_->child_node_.back(), slim_line_.substr(type_.second));
				//grammar_tree_root_->child_node_.back()->content = slim_line_.substr(type_.second);
			}
		}
		else if (type_.first == ul || type_.first == ol)//��������list���
		{
			//�ж��Ƿ�Ϊ�б�ĵ�һ����߲���б��һ��
			//�ĵ���ʼ,�����﷨�����һ����㲻�������б���
			if (grammar_tree_root_->child_node_.empty() || (grammar_tree_root_->child_node_.back()->grammar_type_ != ul \
				&& grammar_tree_root_->child_node_.back()->grammar_type_ != ol))
			{
				GrammarTreeNode* tree_node_;

				//���������б�
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
			else if (index_ > list_node_list_.back().first)//˵������һ����ε�
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
			else if (index_ < list_node_list_.back().first)//˵���Ǹ��߲�ε�
			{
				for (int i = list_node_list_.size() - 1; i >= 0; i--)
				{
					if (list_node_list_[i].first == index_)
					{
						list_node_list_.resize(i + 1);
					}
				}
			}

			//�������б�����б��ӽڵ�
			list_node_list_.back().second->child_node_.push_back(new GrammarTreeNode(li));
			//���б��ӽڵ��������
			Convert(list_node_list_.back().second->child_node_.back(), slim_line_.substr(type_.second));
		}
		else if (type_.first == link)//����link���
		{
			grammar_tree_root_->child_node_.push_back(new GrammarTreeNode(link));
			Convert(grammar_tree_root_->child_node_.back(), slim_line_.substr(type_.second));
		}
		else if (type_.first == image)//����image���
		{
			grammar_tree_root_->child_node_.push_back(new GrammarTreeNode(image));
			Convert(grammar_tree_root_->child_node_.back(), slim_line_.substr(type_.second));
		}
	}
	
	DFS(grammar_tree_root_);//չ���﷨��,����HTML�ĵ�
	text_html_ += "</body>\n</html>";//���html��β����
	//std::cout << std::string(text_html_.toLocal8Bit()) << std::endl;
}


/*
����DFSʹ��������ȱ����ķ������﷨��ת����HTMLԴ����
���в���root��ʾ�﷨���ĸ��ڵ�
*/
void Translator::DFS(GrammarTreeNode* root_)
{
	//����ǰ�ñ�ǩ
	if ((root_->grammar_type_ == ul || root_->grammar_type_ == ol) && text_html_[text_html_.size() - 1] != '\n')
	{
		text_html_ += '\n';
	}	

	text_html_ += html_start_[root_->grammar_type_];

	//link���
	if (root_->grammar_type_ == link) 
	{
		text_html_ += QString::fromStdString(root_->path);
		text_html_ += "\'>";
		text_html_ += QString::fromStdString(root_->content);
	}
	//image���
	else if (root_->grammar_type_ == image) 
	{
		text_html_ += QString::fromStdString(root_->path);
		if (root_->content[root_->content.size() - 1] == '>')//<path, name>��ʽ
		{
			text_html_ += "\" alt=\"";
			root_->content.resize(root_->content.size() - 1);
		}
		else//![name](path)��ʽ
		{
			text_html_ += "\" referrerpolicy=\"no-referrer\" alt=\"";

		}
		text_html_ += QString::fromStdString(root_->content);
	}
	else//text��㼰�������
	{
		text_html_ += QString::fromStdString(root_->content);
	}

	//��ȱ������ӽ��
	for (GrammarTreeNode* child_node_ : root_->child_node_)
	{
		DFS(child_node_);
	}

	//������ñ�ǩ
	text_html_ += html_end_[root_->grammar_type_];
}


/*
����PreTreatOneLine������һ�н���Ԥ����
�����һ���ַ���ȥ����һ���ͷ�Ŀո�
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
Convert������Բ�ͬ���͵���
����ת��Ϊ��ͬ��HTML
���в���current_node_��ָ��ǰ���ڵĽ��
line_��ָ���е��ַ�������
*/
void Translator::Convert(GrammarTreeNode* current_node_, std::string line_) 
{

	bool is_bold_ = false;//��ʶ�Ƿ��ڴ��巶Χ��
	bool is_italic_ = false;//��ʶ�Ƿ���б�巶Χ��
	bool is_resetline_ = false;//��ʶ�Ƿ���ɾ���߷�Χ��

	current_node_->child_node_.push_back(new GrammarTreeNode(blankline));//���ڴ��ı�
	
	if (current_node_->grammar_type_ == image)//image
	{
		if (line_[line_.size() - 1] != '>')//![name](path)��ʽ��
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
		else//<path>��ʽ��
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
		//[name](path)��ʽ
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
		std::vector<GrammarTreeNode*> pre_node_list_;//��¼֮ǰ�Ľ�㣬���ڼ�¼���
		GrammarTreeNode* tree_node_;

		pre_node_list_.push_back(current_node_);
		while(i < line_.size())
		{
			if (line_[i] == '*')
			{
				if (is_italic_)//б�����
				{
					current_node_->child_node_.push_back(new GrammarTreeNode(blankline));
					pre_node_list_.resize(pre_node_list_.size() - 1);
					is_italic_ = false;
				}
				else if (line_[i + 1] == '*')//����
				{
					if (is_bold_)//�������
					{
						current_node_->child_node_.push_back(new GrammarTreeNode(blankline));
						pre_node_list_.resize(pre_node_list_.size() - 1);
						is_bold_ = false;
					}
					else//���忪ʼ
					{
						tree_node_ = new GrammarTreeNode(bold);
						pre_node_list_.back()->child_node_.push_back(tree_node_);
						pre_node_list_.push_back(tree_node_);
						is_bold_ = true;
					}
					i++;
				}
				else//б�忪ʼ
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
				if (is_italic_)//б�����
				{
					current_node_->child_node_.push_back(new GrammarTreeNode(blankline));
					pre_node_list_.resize(pre_node_list_.size() - 1);
					is_italic_ = false;
				}
				else if (line_[i + 1] == '_')//����
				{
					if (is_bold_)//�������
					{
						current_node_->child_node_.push_back(new GrammarTreeNode(blankline));
						pre_node_list_.resize(pre_node_list_.size() - 1);
						is_bold_ = false;
					}
					else//���忪ʼ
					{
						tree_node_ = new GrammarTreeNode(bold);
						pre_node_list_.back()->child_node_.push_back(tree_node_);
						pre_node_list_.push_back(tree_node_);
						is_bold_ = true;
					}
					i++;
				}
				else//б�忪ʼ
				{
					tree_node_ = new GrammarTreeNode(italic);
					pre_node_list_.back()->child_node_.push_back(tree_node_);
					pre_node_list_.push_back(tree_node_);
					is_italic_ = true;
				}
				i++;
			}
			else if (strncmp(line_.c_str() + i, "~~", 2) == 0)//ɾ����
			{
				if (is_resetline_)//ɾ���߽���
				{
					current_node_->child_node_.push_back(new GrammarTreeNode(blankline));
					pre_node_list_.resize(pre_node_list_.size() - 1);
					is_resetline_ = false;
				}
				else//ɾ���߿�ʼ
				{
					tree_node_ = new GrammarTreeNode(resetline);
					pre_node_list_.back()->child_node_.push_back(tree_node_);
					pre_node_list_.push_back(tree_node_);
					is_resetline_ = true;
				}
				i += 2;
			}
			else//��ͨ���ֻ����»���(�»���������⴦��)
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
����JudgeGrammarType��һ���ַ������жϣ�����text/header/image/link/list/�е���һ��
����text_line_��ʾһ���ַ���������ֵΪ<�﷨���ͣ�����������ʼλ��>
*/
std::pair<int, int> Translator::JudgeGrammarType(std::string text_line_) 
{
	int title_level_ = 0;//����ȼ�
	int index_ = 0;//�������ݿ�ʼ���±�

	//�ж�header
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
	//�жϽ��Ϊheader(# + �ո�)
	if (text_line_[index_] == ' ' && title_level_ > 0 && title_level_ <= 6) 
	{
		return std::make_pair(h1 + title_level_ - 1, index_ + 1);
	}

	//�ж�list
	//�жϽ��Ϊ����list(- + �ո�/* + �ո�)
	if (strncmp(text_line_.c_str(), "- ", 2) == 0 || strncmp(text_line_.c_str(), "* ", 2) == 0)
	{
		return std::make_pair(ul, 2);
	}

	//�жϽ��Ϊ����list(���� + "." + �ո�)
	index_ = text_line_.find('.');
	bool is_digit = true;//�ж��Ƿ�Ϊ���ֵı�־
	if (index_ != text_line_.npos && text_line_[index_ + 1] == ' ')
	{
		for (size_t i = 0; i < index_; i++)
		{
			if (text_line_[i] > '9' || text_line_[i] < '0')
			{
				is_digit = false;
			}
		}
		if (is_digit == true)//��.֮ǰ��Ϊ����
		{
			return std::make_pair(ol, index_ + 2);
		}
	}

	//�ж�image
	bool signal1_ = false;//��ʶ'['�Ƿ����
	bool signal2_ = false;//��ʶ'('�Ƿ����
	bool signal3_ = false;//��ʶ')'�Ƿ����
	//�жϽ��Ϊimage(![ + ] + ( + ))
	if (strncmp(text_line_.c_str(), "![", 2) == 0) 
	{
		for (size_t i = 2; i < text_line_.size(); i++)//�ҵ���Ӧ�� ] �� ( )
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

	//�жϽ��Ϊimage(<img src= + >)
	bool signal_ = false;//��ʶ'>'�Ƿ����
	if (strncmp(text_line_.c_str(), "<img src=\"", 10) == 0)
	{
		for (size_t i = 10; i < text_line_.size(); i++)//�ҵ���Ӧ�� >
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

	//�ж�link
	//�жϽ��Ϊlink([ + ] + ( + ))
	if (text_line_[0] == '[')
	{
		for (size_t i = 1; i < text_line_.size(); i++)//�ҵ���Ӧ�� ] �� ( )
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

	//�������ͳһ�ж�Ϊtext
	return std::make_pair(text, 0);
}


/*
����free_tree�Ĺ����ǵݹ���ͷ��﷨��
�����root��ʾ�﷨���ĸ�
�Ӹ���ʼ�ݹ�����������ͷ����Ľڵ�
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