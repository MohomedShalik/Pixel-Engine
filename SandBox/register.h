
// engine
#include "Pixel-Engine.h"


// classes for register
#include "src/Player.h"
#include "src/Npc.h"

void register_classes()
{
	// register cpp objects
	REGISTER_CLASS(Player);
	REGISTER_CLASS(Npc);
}
