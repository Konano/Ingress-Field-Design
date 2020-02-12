#include <iostream>
#include <string>
#include <sstream>
#include "jsoncpp/json.h" // C++编译时默认包含此库

using namespace std;

int main()
{
	// 读入JSON
	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);

	// 分析自己收到的输入和自己过往的输出，并恢复状态
	string data = input["data"].asString(); // 该对局中，以前该Bot运行时存储的信息
	int turnID = input["responses"].size();
	for (int i = 0; i < turnID; i++)
	{
		istringstream in(input["requests"][i].asString()),
			out(input["responses"][i].asString());

		// 根据这些输入输出逐渐恢复状态到当前回合
	}

	// 看看自己本回合输入
	istringstream in(input["requests"][turnID].asString());

	// 做出决策存为myAction

	// 输出决策JSON
	Json::Value ret;
	ret["response"] = myAction;
	ret["data"] = myData; // 可以存储一些前述的信息，在整个对局中使用
	Json::FastWriter writer;
	cout << writer.write(ret) << endl;
	return 0;
}






Json::Reader arrayReader;
Json::Value arrayValue;
arrayReader.parse(jsonData, arrayValue);

Json::Value::Members arrayMember = arrayValue.getMemberNames(); //ayyayMember是成员名称的集合，即name,age;
for(Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter) //遍历json成员
{
    string member_name= *iter;
    string value_str = arrayValue[member_name].asString();  
    cout<<member_name<<":"<<value_str<<endl;
}
