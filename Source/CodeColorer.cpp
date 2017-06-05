#include "CodeColorer.h"

namespace
{
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

CodeColorer::CodeColorer(Context* context, String path) : Object(context)
{

}

PODVector<unsigned>CodeColorer::CreateColors(PODVector<unsigned> code)
{
	//a parallel list of what style each char is.
	PODVector<unsigned> styleMap_;
	
	if(styles_.Empty())
	{
		//no styles, no service
		return styleMap_;
	}

	//apply styles
	styleMap_.Clear();
	int numStyles = styles_.Size();

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
		if (c != '\n')
		{
			styleMap_.Push(0);
			counter++;
		}

		//keywords can be colored directly
		if (keywords_.Contains(word))
		{
			int wordLength = word.LengthUTF8();
			for (int j = wordLength; j > 0; j--)
			{
				int index = Max(0, counter - j);
				styleMap_[index] = 4; //TODO: look up different styles for different keywords
			}
		}

		//apply #
		if (tokenStack_.Front() == '#')
		{
			styleMap_[counter - 1] = 2;

			if (tokenStack_.Contains('\"') || tokenStack_.Contains('<'))
			{
				styleMap_[counter - 1] = 3;
			}

			continue;
		}

		//apply "
		if (tokenStack_.Contains('\"'))
		{
			styleMap_[counter - 1] = 3;
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
			styleMap_[counter - 1] = 1;
			if (tokenStack_.EndsWith("//"))
			{
				styleMap_[counter - 2] = 1;
			}
		}


		//return
		return styleMap_;
	}
}