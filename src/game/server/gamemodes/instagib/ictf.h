#ifndef GAME_SERVER_GAMEMODES_INSTAGIB_ICTF_H
#define GAME_SERVER_GAMEMODES_INSTAGIB_ICTF_H

#include "ctf.h"

class CGameControllerICTF : public CGameControllerCTF
{
public:
	CGameControllerICTF(class CGameContext *pGameServer);
	~CGameControllerICTF();

	void OnCharacterSpawn(class CCharacter *pChr) override;
	void Tick() override;
};
#endif // GAME_SERVER_GAMEMODES_ICTF_H
