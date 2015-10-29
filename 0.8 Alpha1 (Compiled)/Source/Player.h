#include "RakNet/NetworkIDObject.h"
#include "SUtil/Common.h"
#include <string>

using namespace std;

class PlayerGameObject : public NetworkIDObject{
	private:
		wstring name;
		ulonglong objid;
};