#ifndef SINGLETON_H
#define SINGLETON_H

#include <list>
#include <algorithm>

class SingletonBase
{
	class InstanceTable : public std::list < SingletonBase * >
	{
	public:
		InstanceTable()
			: m_isCleared(false)
		{

		};

		~InstanceTable()
		{
			m_isCleared = true;
			for_each(begin(), end(), destroyInstance);
		}

	public:
		static void destroyInstance(SingletonBase *pInstance)
		{
			delete pInstance;
		}

	public:
		bool m_isCleared;
	};

protected:
	SingletonBase()
	{
		s_instanceTable.push_back(this);
	}

	virtual ~SingletonBase()
	{
		if (!s_instanceTable.m_isCleared)
		{
			s_instanceTable.remove(this);
		}
	}

private:
	static InstanceTable s_instanceTable;
};

template <typename T>
class Singleton : public SingletonBase
{
public:
	static T* getInstance()
	{
		if (s_singleton == nullptr)
		{
			s_singleton = new (std::nothrow) T();
		}
		return s_singleton;
	}

protected:
	Singleton()
	{

	};

	virtual ~Singleton()
	{
		s_singleton = nullptr;
	};

private:
	static T* s_singleton;
};

template<typename T> T* Singleton<T>::s_singleton = nullptr;

#define SINGLETON(_class_)					\
	private:								\
		_class_();							\
		~_class_();							\
		friend class Singleton<_class_>;	\

#endif