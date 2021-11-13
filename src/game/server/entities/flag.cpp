/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "character.h"
#include <game/server/gamecontext.h>

#include "flag.h"
CFlag::CFlag(CGameWorld *pGameWorld, int Team) :
	CEntity(pGameWorld, CGameWorld::ENTTYPE_FLAG)
{
	m_Team = Team;
	m_pCarrier = NULL;
	m_GrabTick = 0;

	Reset();
}

void CFlag::Reset()
{
	GameServer()->CreateSoundGlobal(SOUND_CTF_RETURN);

	m_pCarrier = NULL;
	m_AtStand = 1;
	m_Pos = m_StandPos;
	m_Vel = vec2(0, 0);
	m_GrabTick = 0;
}

void CFlag::Grab(CCharacter *pChar)
{
	m_pCarrier = pChar;
	if(m_AtStand)
		m_GrabTick = Server()->Tick();
	m_AtStand = false;
	GameServer()->CreateSoundGlobal(m_Team == TEAM_RED ? SOUND_CTF_GRAB_EN : SOUND_CTF_GRAB_PL);
}

void CFlag::Drop()
{
	m_pCarrier = 0;
	m_Vel = vec2(0, 0);
	m_DropTick = Server()->Tick();
}

void CFlag::TickDefered()
{
	if(m_pCarrier)
	{
		// update flag position
		m_Pos = m_pCarrier->GetPos();
	}
	else
	{
		// flag hits death-tile or left the game layer, reset it
		if((GameServer()->Collision()->GetCollisionAt(m_Pos.x, m_Pos.y) == TILE_DEATH) ||
			(GameServer()->Collision()->GetFCollisionAt(m_Pos.x, m_Pos.y) == TILE_DEATH) ||
			GameLayerClipped(m_Pos))
		{
			Reset();
			GameServer()->m_pController->OnFlagReturn(this);
		}

		if(!m_AtStand)
		{
			if(Server()->Tick() > m_DropTick + Server()->TickSpeed() * 30)
			{
				Reset();
				GameServer()->m_pController->OnFlagReturn(this);
			}
			else
			{
				m_Vel.y += GameWorld()->m_Core.m_Tuning[0].m_Gravity;
				GameServer()->Collision()->MoveBox(&m_Pos, &m_Vel, vec2(ms_PhysSize, ms_PhysSize), 0.5f);
			}
		}
	}
}

void CFlag::TickPaused()
{
	++m_DropTick;
	if(m_GrabTick)
		++m_GrabTick;
}

void CFlag::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CNetObj_Flag *pFlag = (CNetObj_Flag *)Server()->SnapNewItem(NETOBJTYPE_FLAG, m_Team, sizeof(CNetObj_Flag));
	if(!pFlag)
		return;

	pFlag->m_X = (int)m_Pos.x;
	pFlag->m_Y = (int)m_Pos.y;
	pFlag->m_Team = m_Team;
}
