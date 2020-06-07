#include "stdafx.h"
#include "XML.h"



XMLFile::Tag::Tag(string name)
{
	this->tag = name;
}

void XMLFile::Tag::copy(Tag& const copyFrom)
{
	this->tag = copyFrom.tag;
	this->text = copyFrom.text;
	this->attributes = copyFrom.attributes;
	this->children = copyFrom.children;
}

XMLFile::Tag::Tag(Tag& const copyFrom)
{
	if(&copyFrom != this)
		copy(copyFrom);
}

XMLFile::Tag& XMLFile::Tag::operator=(Tag& const copyFrom)
{
	if (&copyFrom != this)
		copy(copyFrom);
	return *this;
}

void XMLFile::Tag::AddAttr(string attr, string value)
{
	attributes.insert(make_pair(attr, value));
}

string XMLFile::Tag::GetID()
{
	return this->attributes.find("id")->second;
}

XMLFile::Tag* XMLFile::findTag(Tag* root, string id)
{
	if (!id.compare(root->GetID()))
		return root;
	else
	{
		if (root->children.empty())
			return nullptr;
		else
		{
			int size = root->children.size();
			for (int i = 0; i < size; i++)
			{
				Tag* temp = findTag(root->children[i], id);
				if (temp != nullptr)
					return temp;
			}
			return nullptr;
		}
	}
}

void XMLFile::printChildrenAttr(Tag* child)
{
	if (child == nullptr)
		return;

	int size = child->children.size();
	for (int i=0; i < size; i++)
	{
		printf("Tag with id %s has attributes: \n", child->children[i]->GetID().c_str());
		for (map<string, string>::iterator tempI = child->children[i]->attributes.begin(); tempI != child->children[i]->attributes.end(); ++tempI)
			printf("%s: %s\n", tempI->first.c_str(), tempI->second.c_str());
		printChildrenAttr(child->children[i]);
	}
}

void XMLFile::help()
{
	cout << "The following commands are supported :\n";
	cout << "open <file> \t\t\t opens <file>\n";
	cout << "close \t\t\t\t closes currently opened file\n";
	cout << "save \t\t\t\t saves the currently open file\n";
	cout << "saveas <file> \t\t\t saves the currently open file in <file>\n";
	cout << "help \t\t\t\t prints this information\n";
	cout << "exit \t\t\t\t exists the program\n";

	cout << "print \t\t\t\t prints the file\n";
	cout << "select <id> <key> \t\t prints the <key> attribute of <id>\n";
	cout << "set <id> <key> <value> \t\t sets the <value> of the <key> attribute of the <id>\n";
	cout << "children <id> \t\t\t prints a list with the children's attributes\n";
	cout << "child <id> <n> \t\t\t prints the n-th child of <id>\n";
	cout << "text <id> \t\t\t prints the text of <id>\n";
	cout << "delete <id> <key> \t\t deletes the <key> attribute of <id>\n";
	cout << "newchild <id> \t\t\t adds a new tag in the <id>\n";
	cout << "xpath <id> \t\t\t makes an xpath query\n";
}

void XMLFile::AddId(Tag* elem)
{
	if (elem->attributes.find("id") == elem->attributes.end())
	{
		string newID = to_string((int)this);
		elem->AddAttr("id", newID);
	}
	else
	{
		Tag* temp = findTag(Root, elem->attributes.find("id")->second);
		while (temp != nullptr)
		{
			if (temp == elem)
			{
				return;
			}
			else
			{
				string ID = temp->attributes.find("id")->second;
				temp->attributes.find("id")->second = ID + "_1";
				elem->attributes.find("id")->second = ID + "_2";
			}
		}
	}
}

void XMLFile::open(string name)
{
	fileName = name;
	ifstream temp(name);

	if (!temp.fail())
	{
		string buffer;

		getline(temp, buffer);
		if (!buffer.compare(0, 5, "<?xml"))
		{
			Root = new Tag("xml");
			string attr = "", val = "";
			buffer = buffer.substr(5);

			while (buffer.find("=") != string::npos)
			{
				attr = buffer.substr(1, buffer.find("=") - 1);
				buffer = buffer.substr(buffer.find("=") + 2);
				if (buffer.find("\"") != string::npos)
				{
					val = buffer.substr(0, buffer.find("\""));
					buffer = buffer.substr(buffer.find("\"") + 1);
					Root->AddAttr(attr, val);
				}
				attr.clear();
				val.clear();
			}

			AddId(Root);
			buffer.clear();
			getline(temp, buffer);
			while (!buffer.empty())
			{
				readInternal(temp, Root, buffer);
				buffer.clear();
				getline(temp, buffer);
			}
		}
		else
			cout << "The file is not a valid XML file!\n";
	}
	else
		cout << "There is no existing file with that name\n";
}

void XMLFile::readInternal(ifstream &file, Tag* parent, string elem)
{
	string attr = "", val = "";

	while (!elem.empty())
	{
		if (elem.find("<") != string::npos)
		{
			elem = elem.substr(elem.find("<") + 1);

			if (elem.find("=") != string::npos)   // if element have attributes
			{
				string tag = elem.substr(0, elem.find(" "));
				Tag* tempEl = new Tag(tag);

				elem = elem.substr(elem.find(" ") + 1);
				while (elem.find("=") != string::npos)
				{
					attr = elem.substr(0, elem.find("="));
					elem = elem.substr(elem.find("=") + 2);

					if (elem.find("\"") != string::npos)
					{
						val = elem.substr(0, elem.find("\""));
						elem = elem.substr(elem.find("\"") + 1);
						tempEl->AddAttr(attr, val);
					}
				}
				if (elem.find("/>") != string::npos) // if the element is self closing tag
				{
					parent->children.push_back(tempEl);
					AddId(tempEl);
				}
				else if (elem.find(">") != string::npos) // if the element is not self closing
				{
					parent->children.push_back(tempEl);
					AddId(tempEl);

					tag = "</" + tag + ">";
					elem = elem.substr(elem.find(">") + 1);
					if (!elem.empty())
					{
						while (!elem.empty())
						{
							if (elem.find(tag) != string::npos)
							{
								tempEl->text.append(elem.substr(0, elem.find(tag)));
								return;
							}
							else
							{
								tempEl->text.append(elem);
								tempEl->text.append("\n");
								elem.clear();
								getline(file, elem);
							}
						}
					}
					else  //if the element is not closed and we have children
					{
						getline(file, elem);
						while (elem.find(tag) == string::npos)
						{
							readInternal(file, tempEl, elem);
							elem.clear();
							getline(file, elem);
						}
						elem.clear();
					}
				}
			}



			else 
			{
				if (elem.find("/>") != string::npos)
				{
					Tag* tempEl = new Tag(elem.substr(0, elem.find("/>")));
					parent->children.push_back(tempEl);
					AddId(tempEl);
				}
				else if (elem.find(">") != string::npos)
				{
					string tag = elem.substr(0, elem.find(">"));
					Tag* tempEl = new Tag(tag);
					parent->children.push_back(tempEl);
					AddId(tempEl);

					tag = "</" + tag + ">";
					elem = elem.substr(elem.find(">") + 1);
					if (!elem.empty())
					{
						while (!elem.empty())
						{
							if (elem.find(tag) != string::npos)
							{
								tempEl->text.append(elem.substr(0, elem.find(tag)));
								return;
							}
							else
							{
								tempEl->text.append(elem);
								tempEl->text.append("\n");
								elem.clear();
								getline(file, elem);
							}
						}
					}
					else  //if the element is not closed and we have children
					{
						getline(file, elem);
						while (elem.find(tag) == string::npos)
						{
							readInternal(file, tempEl, elem);
							elem.clear();
							getline(file, elem);
						}
						elem.clear();
						//getline(file, elem);
					}
				}
			}
		}
		else
		{
			parent->text.append(elem);
			parent->text.append("\n");
			elem.clear();
		}
	}
}

void XMLFile::save()
{
	saveAs(fileName);
}

void XMLFile::close()
{
	save();
	fileName = "";
	Root->children.clear();
	delete Root;
	Root = nullptr;
}

void XMLFile::writeInternal(ostream& file, Tag* element, int countT)
{
	for (int i = 0; i < countT; i++)
		file << "\t";
	file << "<" << element->tag.c_str();
	for (map<string, string>::iterator tempI = element->attributes.begin(); tempI != element->attributes.end(); ++tempI)
		file << " " << tempI->first.c_str() << "=\"" << tempI->second.c_str() << "\"";
	file << ">";
	int size = element->children.size();
	if (size)
		file << "\n";
	else
		file << element->text;

	for (int i = 0; i < size; i++)
	{
		writeInternal(file, element->children[i], countT + 1);
	}
	if(size)
		for (int i = 0; i < countT; i++)
			file << "\t";
	file  << "</" << element->tag.c_str() << ">\n";
}

void XMLFile::saveAs(string name)
{
	string newName = name + ".xml";
	ofstream file(newName, std::ios::trunc);
	if (!file.fail())
	{
		file << "<?xml ";
		for (map<string, string>::iterator tempI = Root->attributes.begin(); tempI != Root->attributes.end(); ++tempI)
			file << tempI->first.c_str() << "=\"" << tempI->second.c_str() << "\" ";
		file << "?>\n";
		int size = Root->children.size();
		for (int i = 0; i < size; i++)
		{
			writeInternal(file, Root->children[i], 0);
		}
	}
	file.close();
}

void XMLFile::print()
{

	cout << "<?xml ";
	for (map<string, string>::iterator tempI = Root->attributes.begin(); tempI != Root->attributes.end(); ++tempI)
		cout << tempI->first.c_str() << "=\"" << tempI->second.c_str() << "\" ";
	cout << "?>\n";
	int size = Root->children.size();
	for (int i = 0; i < size; i++)
	{
		writeInternal(cout, Root->children[i], 0);
	}

}


void XMLFile::select(string id, string attr)
{
	Tag * temp = findTag(Root, id);
	
	if (temp)
	{
		if (temp->attributes.find(attr) != temp->attributes.end())
			printf("The attribute %s has value %s\n", attr.c_str(), temp->attributes.find(attr)->second.c_str());
		else
			printf("Element %s does not have attribute %s", id.c_str(), attr.c_str());
	}
	else
		printf("There is no element with id: %s", id.c_str());
}

void XMLFile::set(string id, string attr, string val)
{
	Tag* temp = findTag(Root, id);
	if (temp)
	{
		if (temp->attributes.find(attr) != temp->attributes.end())
			temp->attributes.find(attr)->second = val;
		else
			temp->attributes.insert(make_pair(attr, val));
	}
	else
		printf("There is no element with id %s\n", id.c_str());
}

void XMLFile::children(string id)
{
	Tag *temp = findTag(Root, id);
	if (temp)
		printChildrenAttr(temp);
	else
		printf("There is no element with id %s\n", id.c_str());
}

const XMLFile::Tag* XMLFile::child(string id, int n)
{
	Tag *temp = findTag(Root, id);
	if (temp)
	{
		int size = temp->children.size();
		if (n < size)
			return temp->children[n - 1];
		else
			printf("Element with id %s don't have element %d", id.c_str(), n);
	}
	else
		printf("There is no element with id %s\n", id.c_str());
}

string XMLFile::text(string id)
{
	Tag *temp = findTag(Root, id);
	if (temp)
	{
		return temp->text;
	}
	else
		printf("There is no element with id %s\n", id.c_str());
	return "";
}

void XMLFile::deleteAttr(string id, string attr)
{
	if (attr.compare("id") != 0)
	{
		Tag* temp = findTag(Root, id);
		if (temp)
		{
			if (temp->attributes.find(attr) != temp->attributes.end())
				temp->attributes.erase(attr);
			else
				printf("Element %s does not have attribute %s", id.c_str(), attr.c_str());
		}
		else
			printf("There is no element with id %s\n", id.c_str());
	}
	else
		printf("You can't delete the id of the tag\n");
}

void XMLFile::newChild(string id)
{
	Tag* temp = findTag(Root, id);
	if (temp)
	{
		string newID = to_string((int)temp);
		Tag tempChild("childTag");
		tempChild.AddAttr("id", newID);
		temp->children.push_back(&tempChild);
	}
}

void XMLFile::xpath(string id, string xpathS)
{
	vector <Tag*> tempStack;
	Tag* temp = findTag(Root, id);
	int size = xpathS.size();

	if (temp != nullptr)
	{
		tempStack.push_back(temp); //check

		if (xpathS.find("/") != string::npos)
		{
			xpathInternal(tempStack, xpathS.substr(0, xpathS.find("/")), xpathS.substr(xpathS.find("/") + 1));
		}
		else
		{
			xpathInternal(tempStack, xpathS, "");
		}
	}
}

void XMLFile::xpathInternal(vector<Tag*> elements, string searched, string searchRes)
{
	int stackSize = elements.size();
	vector<Tag*> newElements;
	int strSize = searched.size();
	string nth = "";
	int elNum = -1;
	string attr = "";
	string val = "";
	string tag = "";

	if (searchRes == "")
	{
		if (searched.find("[") != string::npos)
		{
			if (searched.find("]") != string::npos)
			{
				nth = searched.substr(searched.find("[") + 1, searched.find("]"));
				nth = nth.substr(0, nth.size() - 1);

				searched = searched.substr(0, searched.find("["));
			}
		}

		if (searched.find("(") != string::npos)
		{
			if (searched.find(")") != string::npos)
			{
				if (searched.find("@") != string::npos)
				{
					attr = searched.substr(searched.find("@") + 1, searched.find(")"));
					attr = attr.substr(0, attr.size() - 1);
					searched = searched.substr(0, searched.find("("));

				}
				else if (searched.find("=") != string::npos)
				{
					tag = searched.substr(0, searched.find("("));
					searched = searched.substr(searched.find("(") + 1);

					attr = searched.substr(0, searched.find("="));

					val = searched.substr(searched.find("=") + 2, searched.find(")"));
					val = val.substr(0, val.size() - 2);

					searched = tag;

				}

			}
		}

		for (int i = 0; i < stackSize; i++)
		{
			int childrenSize = elements[i]->children.size();
			if(nth != "")
				elNum = stoi(nth);
			for (int k = 0; k < childrenSize; k++)
			{
				if (!searched.compare(elements[i]->children[k]->tag))
				{
					if (elNum != -1)
					{
						if (elNum == 0)
						{
							printf("Tag %s with id %s\n", elements[i]->children[k]->tag.c_str(), elements[i]->children[k]->attributes.find("id")->second.c_str());
							return;
						}
						else
							elNum--;
					}
					else if (attr != "")
					{
						if (val == "")
						{
							if(elements[i]->children[k]->attributes.find(attr) != elements[i]->children[k]->attributes.end())
								printf("Tag %s with id %s has attribute %s\n", elements[i]->children[k]->tag.c_str(), elements[i]->children[k]->attributes.find("id")->second.c_str(), attr.c_str());
						}
						else
						{
							if (elements[i]->children[k]->attributes.find(attr) != elements[i]->children[k]->attributes.end())
								if (elements[i]->children[k]->attributes.find(attr)->second.compare(val) == 0)
									printf("Tag %s with id %s \n", elements[i]->children[k]->tag.c_str(), elements[i]->children[k]->attributes.find("id")->second.c_str());
						}
					}
					else
						printf("Tag %s with id %s\n", elements[i]->children[k]->tag.c_str(), elements[i]->children[k]->attributes.find("id")->second.c_str());
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < stackSize; i++)
		{
			int childrenSize = elements[i]->children.size();
			for (int k = 0; k < childrenSize; k++)
			{
				if (!searched.compare(elements[i]->children[k]->tag))
					newElements.push_back(elements[i]->children[k]);
			}
		}

		if (searchRes[0] == '/')
			searchRes = searchRes.substr(1);
		if (searchRes.find("/") != string::npos)
		{
			searched = searchRes.substr(1, searchRes.find("/"));
			searchRes = searchRes.substr(searchRes.find("/") + 1);
		}
		else
		{
			searched = searchRes;
			searchRes = "";
		}

		xpathInternal(newElements, searched, searchRes);
	}
}