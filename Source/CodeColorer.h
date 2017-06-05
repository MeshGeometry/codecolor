#include "Core/Context.h"
#include "Core/Object.h"
#include "Container/Vector.h"
#include "Container/Str.h"
#include "Core/Variant.h"
#include "IO/Deserializer.h"
#include "IO/File.h"
#include "IO/FileSystem.h"

using namespace Urho3D;

URHO3D_API class CodeColorer : public Object
{
	URHO3D_OBJECT(CodeColorer, Object);

public:
	CodeColorer(Context* context, String path);
	~CodeColorer() {};

	PODVector<unsigned> CreateColors(PODVector<unsigned> code);
};