#include "gtest/gtest.h"

#include "Core/Context.h"
#include "Core/Object.h"
#include "IO/File.h"
#include "IO/FileSystem.h"
#include "Core/StringUtils.h"

#include "JSONFile.h"

#include "CodeColorer.h"

#include <regex>
#include <string>

using namespace Urho3D;

Context* ctx;
FileSystem* fs;
CodeColorer* cc;

struct Token
{
	String name_;
	String regex_;
	Color color_;
	Vector<Pair<int, int>> ranges_;

	Token::Token() {};
	Token::Token(String name, String regex, Color col):
		name_(name),
		regex_(regex),
		color_(col)
	{
		
	}
};

struct ColorRule
{
	String name_;
	String regex_;
	Color color_;

	ColorRule::ColorRule() {};
	ColorRule::ColorRule(String name, String regex, Color col) :
		name_(name),
		regex_(regex),
		color_(col)
	{

	}
};

void Init()
{
	ctx = new Context();
	fs = new FileSystem(ctx);
	ctx->RegisterSubsystem(fs);
	cc = new CodeColorer(ctx);
}




TEST(Base, FileParse)
{
	if (!ctx) {
		Init();
	}

	String dir = fs->GetCurrentDir();
	String path = "../../../Source/CodeColorer.cpp";
	PODVector<unsigned> code;
	cc->ReadAndDecodeFile(path, code);

	PODVector<unsigned> styles = cc->CreateColors(code);

	String out = "../../../CodeColorerStyle.txt";
	cc->WriteStyleFile(out, styles);

}

//TEST(Base, firstRegex)
//{
//	String codeLine = "#include \"Test.h\"";
//
//	const char* c = codeLine.CString();
//	std::cmatch m;
//	//std::regex e("\\b(sub)([^ ]*)");   // matches words beginning by "sub"
//	//std::regex e("\\#(\\w+)");
//	std::regex e("\\#(\\w+)\\s\\K\"(.*?)\"");
//
//	while (std::regex_search(c, m, e)) {
//		for (auto x : m) std::cout << x << " ";
//		std::cout << std::endl;
//		c = m.suffix().first;
//	}
//
//}

//TEST(Base, loadRegex)
//{
//	if (!ctx) {
//		Init();
//	}
//
//	bool res = fs->FileExists("../../../Source/cpp.json");
//
//	File* jSource = new File(ctx, "../../../Source/cpp.json", FILE_READ);
//	JSONFile* jFile= new JSONFile(ctx);
//	res = jFile->Load(*jSource);
//
//	EXPECT_TRUE(res);
//
//	File* source = new File(ctx, "../../../Source/CodeColorer.cpp", FILE_READ);
//
//	EXPECT_TRUE(jFile != NULL);
//	EXPECT_TRUE(source != NULL);
//
//	//get the source as chars
//	PODVector<char> buffer;
//	buffer.Resize(source->GetSize());
//	source->Read(&buffer[0], buffer.Size());
//
//	//get tokens from the rule list
//	const JSONValue& root = jFile->GetRoot();
//
//	String test = root.Get("hello").GetString();
//
//	const JSONValue& rVal = root.Get("rules");
//	const JSONArray& rulesArr = rVal.GetArray();
//
//	Vector<Token> tokens;
//
//	for (int i = 0; i < rulesArr.Size(); i++)
//	{
//		JSONValue currRule = rulesArr[i];
//		String name = currRule["name"].GetString();
//		String regex = currRule["regex"].GetString();
//		Color color = ToColor(currRule["color"].GetString());
//
//		Token t(name, regex, color);
//
//		tokens.Push(t);
//
//	}
//
//	jSource->Close();
//
//	//now iterate over the tokens and match the regex
//	Token t = tokens[0];
//
//	//not sure if a conversion needs to happen here
//	//String rawCodeString;
//	//BufferToString(rawCodeString, &buffer[0], buffer.Size());
//	const char* rawCode = &buffer[0];
//	std::cmatch m;
//	const char* eChars = t.regex_.CString();
//	std::regex e(eChars);
//
//	int startCounter = 0;
//	while (std::regex_search(rawCode, m, e)) {
//
//		int start = m.position() + startCounter;
//		int end = start + m.length();
//		Pair<int, int> currRange(start, end);
//		t.ranges_.Push(currRange);
//
//		//iterate
//		rawCode = m.suffix().first;
//		startCounter = end;
//	}
//
//	//test
//	for (int i = 0; i < t.ranges_.Size(); i++)
//	{
//		Pair<int, int> r = t.ranges_[i];
//
//		String word = "";
//		for (int j = r.first_; j < r.second_; j++)
//		{
//			word.AppendUTF8(buffer[j]);
//		}
//
//		std::cout << word.CString() << std::endl;
//	}
//
//}

bool GetRulesFromFile(String path, Vector<ColorRule>& rules)
{
	File* jSource = new File(ctx, path, FILE_READ);
	JSONFile* jFile = new JSONFile(ctx);

	bool res = jFile->Load(*jSource);

	if (!res)
	{
		return res;
	}

	//get tokens from the rule list
	const JSONValue& root = jFile->GetRoot();
	const JSONValue& rVal = root.Get("rules");
	const JSONArray& rulesArr = rVal.GetArray();


	//clear the rules
	rules.Clear();

	//loop through rules and fill vector
	for (int i = 0; i < rulesArr.Size(); i++)
	{
		JSONValue currRule = rulesArr[i];
		String name = currRule["name"].GetString();
		String regex = currRule["regex"].GetString();
		Color color = ToColor(currRule["color"].GetString());

		ColorRule cr(name, regex, color);

		rules.Push(cr);

	}

	//close up
	jSource->Close();


	return res;
}

void ApplyColorRule(ColorRule rule, const PODVector<char>& rawCode, PODVector<char>& styleMap)
{
	//define regex stuff
	std::cmatch m;
	std::regex e(rule.regex_.CString());

	//var name
	char v = rule.name_.ToUpper().At(0);

	//get pointer into raw code
	const char* c = &rawCode[0];

	//perform the regex
	int startCounter = 0;
	while (std::regex_search(c, m, e)) {

		//remap the start/end from local to original positions
		int start = m.position() + startCounter;
		int end = start + m.length();
		
		//apply to styleMap
		for (int i = start; i < end; i++)
		{
			styleMap[i] = v;
		}

		//iterate
		c = m.suffix().first;
		startCounter = end;
	}
}

TEST(Base, BetterParse)
{
	if (!ctx) {
		Init();
	}
	
	//the coloring rules
	Vector<ColorRule> colorRules;

	//file with rule defs
	String path = "../../../Source/cpp.json";

	//source file for testing
	String sPath = "../../../Source/CodeColorer.cpp";

	//parse rules from file
	GetRulesFromFile(path, colorRules);

	//dump file in to char vector
	PODVector<char> rawCode;
	File* source = new File(ctx, sPath, FILE_READ);
	rawCode.Resize(source->GetSize());
	source->Read(&rawCode[0], rawCode.Size());

	//create a parallel vector for styles map
	PODVector<char> styleMap;
	styleMap.Insert(0, rawCode);

	//loop through styles and apply
	for (int i = 0; i < colorRules.Size(); i++)
	{
		ColorRule r = colorRules[i];
		ApplyColorRule(r, rawCode, styleMap);
	}

	//dump style map to file
	String outPath = "../../../Source/color_debug.style";
	File* out = new File(ctx, outPath, FILE_WRITE);
	out->Write(&styleMap[0], styleMap.Size());
	out->Close();

}