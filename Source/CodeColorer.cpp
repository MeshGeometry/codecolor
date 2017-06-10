#include "CodeColorer.h"
#include "IO/FileSystem.h"
#include "IO/File.h"

namespace
{
	char formatTokens_[] = { ' ', '\n', '\t' };
	char codeTokens_[] = { '#', '<', '>', '"', ':', ';', '(', ')', '/' };
	char* keywords_[] = {
		"void",
		"int",
		"bool",
		"float",
		"vec3",
		"Vector3",
		"IntVector2",
		"varying",
		"class",
		"using",
		"namespace"
	};

	String tokenStack_;
	PODVector<unsigned> formatTokens_;// = { ' ', '\n', '\t' };
	PODVector<unsigned> codeTokens_;// = { '#', '<', '>', '"', ':', ';', '(', ')', '/' };
	Vector<String> keywords_;/* = {
		"void",
		"int",
		"bool",
		"float",
		"vec3",
		"Vector3",
		"IntVector2",
		"varying",
		"class",
		"using",
		"namespace"
	};
	*/

	PODVector<unsigned> styles_;
}

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
		while (!file->IsEof())
		{
			String line = file->ReadLine();
			line += "\n";
			code.Push(DecodeToUnicode(line));
		}
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


	//loop through text and update
	int counter = 0;
	String word;
	tokenStack_ = "";
	for (unsigned i = 0; i < code.Size(); i++)
	{
		char c = code[i];

		//update word and token state
		if (formatTokens_.Contains(c))
		{
			word = "";
			if (c == '\n')
			{
				tokenStack_ = "";
			}
		}
		else if (codeTokens_.Contains(c))
		{
			word = "";
			tokenStack_.AppendUTF8(c);
		}
		else
		{
			word.AppendUTF8(c);
		}

		//push default style to stack and adjust later
		styleMap_.Push(c);
		counter++;

		//keywords can be colored directly
		if (keywords_.Contains(word))
		{
			int wordLength = word.LengthUTF8();
			for (int j = wordLength; j > 0; j--)
			{
				int index = Max(0, counter - j);
				styleMap_[index] = 'K'; //TODO: look up different styles for different keywords
			}
		}

		//apply #
		if (tokenStack_.Front() == '#')
		{
			styleMap_[counter - 1] = 'I';

			if (tokenStack_.Contains('\"') || tokenStack_.Contains('<'))
			{
				styleMap_[counter - 1] = 'P';
			}

			continue;
		}

		//apply "
		if (tokenStack_.Contains('\"'))
		{
			styleMap_[counter - 1] = 'S';
			int first = tokenStack_.Find('\"');
			int last = tokenStack_.FindLast('\"');
			if (first != last)
			{
				tokenStack_.Replace(first, 1, ' ');
				tokenStack_.Replace(last, 1, ' ');
			}
		}

		//apply '//'
		if (tokenStack_.Contains("//"))
		{
			styleMap_[counter - 1] = 'C';
			if (tokenStack_.EndsWith("//"))
			{
				styleMap_[counter - 2] = 'C';
			}
		}
	}


	//return
	return styleMap_;
}