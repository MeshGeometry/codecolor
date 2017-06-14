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


void Init()
{
	ctx = new Context();
	fs = new FileSystem(ctx);
	ctx->RegisterSubsystem(fs);
	cc = new CodeColorer(ctx);
}


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

TEST(Base, FileColor)
{
	if (!ctx) {
		Init();
	}

	//file with rule defs
	String path = "../../../Source/cpp.json";

	//source file for testing
	String sPath = "../../../Source/CodeColorer.cpp";

	CodeColorer* cc = new CodeColorer(ctx);

	PODVector<char> code;
	PODVector<char> styles;
	cc->CreateColors(sPath, code, styles);

	//dump style map to file
	String outPath = "../../../Source/color_debug.style";
	File* out = new File(ctx, outPath, FILE_WRITE);
	out->Write(&styles[0], styles.Size());
	out->Close();
}