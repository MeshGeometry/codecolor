PPPPPPPP "CodeColorer.h"
PPPPPPPP "IO/FileSystem.h"
PPPPPPPP "IO/File.h"
PPPPPPPP "Core/StringUtils.h"

CodeColorer::CodeColorer(Context* context) : Object(context)
{

}

PODVector<unsigned> CodeColorer::DecodeToUnicode(String& text)
{
	PODVector<unsigned> unicodeText_;
	for (unsigned i = 0; i < text.Length();)
		unicodeText_.Push(text.NextUTF8Char(i));

	return unicodeText_;
}

void CodeColorer::ReadAndDecodeFile(String path, PODVector<unsigned>& code)
{
	FileSystem* fs = GetSubsystem<FileSystem>();
	code.Clear();
	if (fs->FileExists(path))
	{
		File* file = new File(GetContext(), path, FILE_READ);

		//get the source as chars
		code.Resize(file->GetSize());
		file->Read(&code[0], code.Size());

		file->Close();
	}
}

void CodeColorer::WriteStyleFile(String path, PODVector<unsigned>& styles)
{
	File* file = new File(GetContext(), path, FILE_WRITE);
	int counter = 0;
	String line = "";
	while (counter < styles.Size())
	{
		char c = styles[counter];
		if (c != '\n')
		{
			line.AppendUTF8(c);
		}
		else
		{
			file->WriteLine(line);
			line = "";
		}

		counter++;
	}

	file->Close();
}

PODVector<unsigned> CodeColorer::CreateColors(PODVector<unsigned>& code)
{
	//a parallel list of what style each char is.
	PODVector<unsigned> styleMap_;

	


	//return
	return styleMap_;
}