#ifndef SINGLETON_H
#define SINGLETON_H

#include <list>

class SingletonBase
{
	class InstanceTable : public std::list < SingletonBase * >
	{
	public:
		InstanceTable()
			: bCleared_(false)
		{

		};

		~InstanceTable()
		{
			bCleared_ = true;
			while (!empty())
			{
				delete back();
				pop_back();
			}
		}

	public:
		bool bCleared_;
	};

protected:
	SingletonBase()
	{
		sInstanceTable_.push_back(this);
	}

	virtual ~SingletonBase()
	{
		if (!sInstanceTable_.bCleared_)
		{
			sInstanceTable_.remove(this);
		}
	}

private:
	static InstanceTable sInstanceTable_;
};

template <typename T>
class Singleton : public SingletonBase
{
public:
	static T* getInstance()
	{
		if (sSingleton_ == nullptr)
		{
			sSingleton_ = new (std::nothrow) T();
		}
		return sSingleton_;
	}

	static void destroyInstance()
	{
		if (sSingleton_)
		{
			delete sSingleton_;
		}
	}

protected:
	Singleton()
	{

	};

	virtual ~Singleton()
	{
		sSingleton_ = nullptr;
	};

private:
	static T* sSingleton_;
};

template<typename T> T* Singleton<T>::sSingleton_ = nullptr;

#define SINGLETON(_class_)					\
	private:								\
		_class_();							\
		~_class_();							\
		friend class Singleton<_class_>;	\

#endif