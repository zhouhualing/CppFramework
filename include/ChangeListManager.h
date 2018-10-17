#ifndef INCLUDED_CHANGE_LIST_MANAGER_H
#define INCLUDED_CHANGE_LIST_MANAGER_H


namespace stc {
namespace framework {


class ChangeList;
class ChangeListManager {
public:

	//typedef int32_t ChangeListId;

	/// \brief Returns the singleton instance.
	static ChangeListManager& Instance();

	/// \brief Dtor
	~ChangeListManager();

	/// \brief Returns a new ChangeList
	ChangeList* CreateChangeList();

	/// \brief Sets the current active ChangeList.
	void SetActiveChangeList(ChangeList* cl);

	/// \brief Returns the current active ChangeList.
	ChangeList* GetActiveChangeList() const;

private:

	/// \brief Generates a new ChangeList id
	ChangeList* m_activeChangeList;

	ChangeListManager();
	ChangeListManager(const ChangeListManager&);
	ChangeListManager& operator =(const ChangeListManager&);
};

}
}

#endif // INCLUDED_CHANGE_LIST_MANAGER_H

