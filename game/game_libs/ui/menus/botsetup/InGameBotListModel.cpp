#include "InGameBotListModel.h"

CInGameBotListModel::CInGameBotListModel() :
	CMenuBaseModel()
{
}

void CInGameBotListModel::Update()
{
}

int CInGameBotListModel::GetColumns() const
{
	return 1;
}

int CInGameBotListModel::GetRows() const
{
	return m_BotList.Count();
}

const char* CInGameBotListModel::GetCellText(int row, int column)
{
	if ( row < 0 || row >= m_BotList.Count() || column != 0 )
	{
		return nullptr;
	}

	return m_BotList[row].playerName.String();
}

bool CInGameBotListModel::AddEntry(const CUtlString& profileName, const CUtlString& playerName)
{
	if ( IsFull() )
	{
		return false;
	}

	m_BotList.AddToTail({profileName, playerName});
	return true;
}

const CInGameBotListModel::ListEntry* CInGameBotListModel::Entry(uint32_t index) const
{
	return index < static_cast<uint32_t>(m_BotList.Count()) ? &m_BotList[index] : nullptr;
}

void CInGameBotListModel::Clear()
{
	m_BotList.Purge();
}

bool CInGameBotListModel::IsFull() const
{
	return static_cast<size_t>(m_BotList.Count()) >= MAX_BOTS;
}

void CInGameBotListModel::OnDeleteEntry(int row)
{
	if ( row < 0 || row >= m_BotList.Count() )
	{
		return;
	}

	m_BotList.Remove(row);

	if ( m_ItemDelete )
	{
		m_ItemDelete();
	}
}

void CInGameBotListModel::SetItemDeleteCallback(const ItemDeleteCb& callback)
{
	m_ItemDelete = callback;
}
