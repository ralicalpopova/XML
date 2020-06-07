
//Note: No functionality is tested, only the help function.


#include "stdafx.h"
#include <iostream>
#include <string>
#include "XML.h"

using namespace std;

int main()
{
	string buffer, temp;
	getline(cin, buffer);
	bool isClose = 0;

	XMLFile mainFile;

	while (1)
	{
		if (!buffer.compare(0, 4, "open"))
		{
			mainFile.open(buffer.substr(5));
			if (isClose == 1)
				isClose = 0;
		}
		else if (isClose)
		{
			printf("Please enter command open.\n");
		}
		else if (!buffer.compare(0, 5, "close"))
		{
			mainFile.close();
			isClose = 1;
		}
		else if (!buffer.compare(0, 6, "saveas"))
		{
			mainFile.saveAs(buffer.substr(7));
		}
		else if (!buffer.compare(0, 4, "save"))
		{
			mainFile.save();
		}
		else if (!buffer.compare(0, 4, "help"))
		{
			mainFile.help();
		}
		else if (!buffer.compare(0, 4, "exit"))
		{
			cout << "Exiting the program...\n";
			break; 
		}
		else if (!buffer.compare(0, 5, "print"))
		{
			mainFile.print();
		}
		else if (!buffer.compare(0, 6, "select"))
		{
			string id, key;
			buffer = buffer.substr(7);
			if (buffer.find(" ") != string::npos)
			{
				id = buffer.substr(0, buffer.find(" "));
				buffer = buffer.substr(buffer.find(" ") + 1);
			}
			key = buffer;
			mainFile.select(id, key);
		}
		else if (!buffer.compare(0, 3, "set"))
		{
			string id, attr, val;
			buffer = buffer.substr(4);
			if (buffer.find(" ") != string::npos)
			{
				id = buffer.substr(0, buffer.find(" "));
				buffer = buffer.substr(buffer.find(" ") + 1);
			}
			if (buffer.find(" ") != string::npos)
			{
				attr = buffer.substr(0, buffer.find(" "));
				buffer = buffer.substr(buffer.find(" ") + 1);
			}
			val = buffer;
			mainFile.set(id, attr, val);
		}
		else if (!buffer.compare(0, 5, "child"))
		{
			string id;
			int nth = -1;
			buffer = buffer.substr(6);
			if (buffer.find(" ") != string::npos)
			{
				id = buffer.substr(0, buffer.find(" "));
				buffer = buffer.substr(buffer.find(" ") + 1);
			}
			nth = stoi(buffer);
			if(nth != -1)
				mainFile.child(id, nth);
		}
		else if (!buffer.compare(0, 8, "children"))
		{
			mainFile.children(buffer.substr(9));
		}
		else if (!buffer.compare(0, 4, "text"))
		{
			mainFile.text(buffer.substr(5));
		}
		else if (!buffer.compare(0, 6, "delete"))
		{
			string id, attr;
			buffer = buffer.substr(7);
			if (buffer.find(" ") != string::npos)
			{
				id = buffer.substr(0, buffer.find(" "));
				buffer = buffer.substr(buffer.find(" ") + 1);
			}
			attr = buffer;
			mainFile.deleteAttr(id, attr);
		}
		else if (!buffer.compare(0, 8, "newchild"))
		{
			mainFile.newChild(buffer.substr(9));
		}
		else if (!buffer.compare(0, 5, "xpath"))
		{
			string id, path;
			buffer = buffer.substr(6);
			if (buffer.find(" ") != string::npos)
			{
				id = buffer.substr(0, buffer.find(" "));
				buffer = buffer.substr(buffer.find(" ") + 1);
			}
			path = buffer;
			mainFile.xpath(id, path);
		}
		else
			cerr << "Please enter a vaid command. You can use the \"help\" command to check the valid ones\n";

		buffer.clear();
		getline(cin, buffer);
	}



    return 0;
}

