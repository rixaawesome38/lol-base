#pragma once
#include "Offsets.h"
#include "Utils.h"
#include "Vector.h"
#include "Hooks.h"
#include "CObjectManager.h"


#define me Engine::GetLocalObject()

class Engine {
public:
	static Vector GetMouseWorldPosition() {
		DWORD MousePtr = (DWORD)GetModuleHandle(NULL) + oHudInstance;
		auto aux1 = *(DWORD*)MousePtr;
		aux1 += 0x14;
		auto aux2 = *(DWORD*)aux1;
		aux2 += 0x1C;

		float X = *(float*)(aux2 + 0x0);
		float Y = *(float*)(aux2 + 0x4);
		float Z = *(float*)(aux2 + 0x8);

		return Vector{ X, Y, Z };
	}

	static float GetGameTime() {
		return *(float*)(baseAddr + oGameTime);
	}

	static CObject* GetLocalObject() {
		auto retaddr = *(DWORD*)(baseAddr + oLocalPlayer);
		if (retaddr == NULL)
			return NULL;

		return (CObject*)retaddr;
	}
	static void PrintChat(const char* Message) {
		typedef void(__thiscall* tPrintChat)(DWORD ChatClient, const char* Message, int Color);
		tPrintChat fnPrintChat = (tPrintChat)(baseAddr + oPrintChat);
		fnPrintChat(*(DWORD*)(baseAddr + oChatClientPtr), Message, 1);
	}

	static CObject* Engine::GetObjectByID(int ID)
	{
		if (ObjManager != NULL && ID >= 0 && ID <= 10000) {
			//return ObjManager->objectArray[ID];
		}
	}

	static void MoveTo(Vector* pos) {
		Functions.IssueOrder(GetLocalObject(), 2, pos, NULL, false, false, false);

	}
		


	static void AttackTarget(CObject* obj) {
		Functions.IssueOrder(GetLocalObject(), 3, &obj->GetPos(), obj, true, false, false);
	}


	static void Engine::CastSpellSelf(int SlotID) {
		if (me->IsAlive()) {
			auto spellbook = (DWORD)me + oObjSpellBook;
			auto spellslot = me->GetSpellBook()->GetSpellSlotByID(SlotID);
			Functions.CastSpell(spellbook, spellslot, SlotID, &me->GetPos(), &me->GetPos(), 0);
		}
	}

	static void Engine::CastSpellPos(int SlotID, Vector pos) {
		auto spellbook = (DWORD)me + oObjSpellBook;
		auto spellslot = me->GetSpellBook()->GetSpellSlotByID(SlotID);
		auto targetpos = &pos;

		Functions.CastSpell(spellbook, spellslot, SlotID, targetpos, new Vector(0, 0, 0), 0x0);
	}

	static void Engine::CastSpellPos(int SlotID, float x, float y, float z, CObject* obj) {
		auto spellbook = (DWORD)me + oObjSpellBook;
		auto spellslot = me->GetSpellBook()->GetSpellSlotByID(SlotID);
		Vector* pos = new Vector(x, y, z);
		Functions.CastSpell(spellbook, spellslot, SlotID, &obj->GetPos(), &me->GetPos(), obj->GetNetworkID());
		delete pos;
	}

	static void Engine::CastSpellTargetted(int SlotID, CObject* obj) {
		auto spellbook = (DWORD)me + oObjSpellBook;
		auto spellslot = me->GetSpellBook()->GetSpellSlotByID(SlotID);

		Functions.CastSpell(spellbook, spellslot, SlotID, &obj->GetPos(), &me->GetPos(), obj->GetNetworkID());
	}

	static float Engine::getCD(int slot, CObject* obj) {

		//Console.print("CD : %f", obj->GetSpellBook()->GetSpellSlotByID(slot)->GetCD() - Engine::GetGameTime());
		return obj->GetSpellBook()->GetSpellSlotByID(slot)->GetCD();
	}

	static bool Engine::IsReady(int slot, CObject* obj) {
		//Console.print("LEVEL : %i", obj->GetSpellBook()->GetSpellSlotByID(slot)->GetLevel());
		return obj->GetSpellBook()->GetSpellSlotByID(slot)->GetLevel() >= 1 && getCD(slot, obj) == 0.0f;
	}
};

