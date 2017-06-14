#include "CodeColorer.h"
#include "IO/FileSystem.h"
#include "IO/File.h"
#include "Core/StringUtils.h"

#include "JSONFile.h"

#include <regex>

CodeColorer::CodeColorer(Context* context) : Object(context)
{
	//default colors
	Color light(0.8f, 0.8f, 0.8f);
	Color blue(0.2f, 0.2f, 0.8f);
	Color red(0.9f, 0.2f, 0.2f);
	Color yellow(0.8f, 0.8f, 0.0f);
	Color green(0.2f, 0.8f, 0.2f);
	Color dark(0.3f, 0.3f, 0.3f);
	
	//create default rules (for cpp)
	ColorRule p("preprocessor", "\\#(\\w+)", dark);
	ColorRule l("literal", "\"(.*?)\"", yellow);
	ColorRule k("keyword", "\\b(void|return|int|unsigned|float|bool|virtual|class|namespace|using)", blue);
	ColorRule r("flow", "\\b(for|while|if|else)", red);
	ColorRule m("member", "([\\w]+)::([\\w]+)", green);
	ColorRule c("comment", "//(.*)", dark);

	//push
	colorRules_.Push(p);
	colorRules_.Push(l);
	colorRules_.Push(k);
	colorRules_.Push(r);
	colorRules_.Push(m);
	colorRules_.Push(c);
}

bool CodeColorer::GetRulesFromFile(String path)
{
	File* jSource = new File(GetContext(), path, FILE_READ);
	JSONFile* jFile = new JSONFile(GetContext());

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
	colorRules_.Clear();

	//loop through rules and fill vector
	for (int i = 0; i < rulesArr.Size(); i++)
	{
		JSONValue currRule = rulesArr[i];
		String name = currRule["name"].GetString();
		String regex = currRule["regex"].GetString();
		Color color = ToColor(currRule["color"].GetString());

		ColorRule cr(name, regex, color);

		colorRules_.Push(cr);

	}

	//close up
	jSource->Close();


	return res;
}


void CodeColorer::ApplyColorRule(ColorRule rule, const PODVector<char>& rawCode, PODVector<char>& styleMap)
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

PODVector<char> CodeColorer::CreateColors(PODVector<char>& code)
{
	//a parallel list of what style each char is.
	PODVector<char> styleMap;
	styleMap.Insert(0, code);
	
	//loop through styles and apply
	for (int i = 0; i < colorRules_.Size(); i++)
	{
		ColorRule r = colorRules_[i];
		ApplyColorRule(r, code, styleMap);
	}

	//return
	return styleMap;
}

bool CodeColorer::CreateColors(String path, PODVector<char>& code, PODVector<char>& styles)
{
	FileSystem* fs = GetSubsystem<FileSystem>();
	if (!fs->FileExists(path))
	{
		return false;
	}

	//dump file in to char vector
	PODVector<char> rawCode;
	File* source = new File(GetContext(), path, FILE_READ);
	rawCode.Resize(source->GetSize());
	source->Read(&rawCode[0], rawCode.Size());

	//create the colors
	styles = CreateColors(rawCode);

	return true;
}