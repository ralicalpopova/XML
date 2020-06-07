#pragma once
#include <utility>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>

using namespace std;


class XMLFile
{
	struct Tag
	{
		map<string, string> attributes;
		string tag;
		string text;
		vector<Tag*> children;
		
		Tag() {};
		Tag(string name);
		void copy(Tag& const copyFrom);
		Tag(Tag& const copyFrom);
		Tag& operator=(Tag& const copyFrom);
		~Tag() {};
		
		string GetID();
		void AddAttr(string name, string value);
	};
	Tag * Root;
	string fileName;

	void AddId(Tag* elem);
	void printChildrenAttr(Tag * child);
	Tag* findTag(Tag * root, string id);
	void writeInternal(ostream& file, Tag* element, int countT);
	void readInternal(ifstream &file, Tag* parent, string elem);
	void xpathInternal(vector<Tag *> elements, string searched, string searchRest);
public:
	void open(string fileName);
	void close();
	void save();
	void saveAs(string fileName);
	void help();


	void print();
	void select(string id, string attr);
	void set(string id, string attr, string val);
	void children(string id);
	const Tag* child(string id, int n);
	string text(string id);
	void deleteAttr(string id, string attr);
	void newChild(string id);
	void xpath(string id, string xpathS);
};
