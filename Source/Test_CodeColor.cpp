#include "gtest/gtest.h"

#include "Core/Context.h"
#include "Core/Object.h"
#include "IO/File.h"
#include "IO/FileSystem.h"

#include "CodeColorer.h"

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