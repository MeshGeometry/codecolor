#include "gtest/gtest.h"

#include "Core/Context.h"
#include "Core/Object.h"
#include "IO/File.h"
#include "IO/FileSystem.h"
#include "Core/StringUtils.h"

#include "JSONFile.h"

#include "CodeColorer.h"

#include  <regex>
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

TEST(Base, firstRegex)
{
	String codeLine = "#include \"Test.h\"";

	const char* c = codeLine.CString();
	std::cmatch m;
	//std::regex e("\\b(sub)([^ ]*)");   // matches words beginning by "sub"
	//std::regex e("\\#(\\w+)");
	std::regex e("\\#(\\w+)\\s\\K\"(.*?)\"");

	while (std::regex_search(c, m, e)) {
		for (auto x : m) std::cout << x << " ";
		std::cout << std::endl;
		c = m.suffix().first;
	}

}

TEST(Base, loadRegex)
{
	if (!ctx) {
		Init();
	}

	bool res = fs->FileExists("../../../Source/cpp.json");

	File* jSource = new File(ctx, "../../../Source/cpp.json", FILE_READ);
	JSONFile* jFile= new JSONFile(ctx);
	res = jFile->Load(*jSource);

	EXPECT_TRUE(res);

	File* source = new File(ctx, "../../../Source/CodeColorer.cpp", FILE_READ);

	EXPECT_TRUE(jFile != NULL);
	EXPECT_TRUE(source != NULL);

	//get the source as chars
	PODVector<unsigned char> buffer;
	buffer.Resize(source->GetSize());
	source->Read(&buffer[0], buffer.Size());

	//get the first regex
	const JSONValue& root = jFile->GetRoot();

	String test = root.Get("hello").GetString();

	const JSONValue& rVal = root.Get("rules");
	const JSONArray& rulesArr = rVal.GetArray();

	for (int i = 0; i < rulesArr.Size(); i++)
	{
		JSONValue currRule = rulesArr[i];
		String name = currRule["name"].GetString();
		String regex = currRule["regex"].GetString();
		Color color = ToColor(currRule["color"].GetString());

	}

}