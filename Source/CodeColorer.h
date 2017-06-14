#include "Core/Context.h"
#include "Core/Object.h"
#include "Container/Vector.h"
#include "Container/Str.h"
#include "Core/Variant.h"
#include "IO/Deserializer.h"
#include "IO/File.h"
#include "IO/FileSystem.h"

using namespace Urho3D;

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


URHO3D_API class CodeColorer : public Object
{
	URHO3D_OBJECT(CodeColorer, Object);

public:
	CodeColorer(Context* context);
	~CodeColorer() {};

	//the main functions
	PODVector<char> CreateColors(String& code);
	PODVector<char> CreateColors(PODVector<char>& code);
	bool CreateColors(String path, PODVector<char>& code, PODVector<char>& styles);

	//some helpers
	bool GetRulesFromFile(String path);
	void ApplyColorRule(ColorRule rule, const PODVector<char>& rawCode, PODVector<char>& styleMap);

protected:


	Vector<ColorRule> colorRules_;


};