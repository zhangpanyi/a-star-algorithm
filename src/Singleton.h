/**
 * 单例模板类
 */

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <list>
#include <algorithm>

class SingletonBase
{
	class InstanceTable : public std::list < SingletonBase * >
	{
	public:
		InstanceTable()
			: is_cleared_(false)
		{

		};

		~InstanceTable()
		{
			is_cleared_ = true;
			for (auto instance : *this)
			{
				delete instance;
			}
		}

	public:
		bool is_cleared_;
	};

protected:
	SingletonBase()
	{
		s_instance_table_.push_back(this);
	}

	virtual ~SingletonBase()
	{
		if (!s_instance_table_.is_cleared_)
		{
			InstanceTable::iterator itr = std::find(s_instance_table_.begin(), s_instance_table_.end(), this);
			if (itr != s_instance_table_.end())
			{
				s_instance_table_.erase(itr);
			}		
		}
	}

private:
	static InstanceTable s_instance_table_;
};

template <typename T>
class Singleton : public SingletonBase
{
public:
	static T* getInstance()
	{
		if (s_singleton_ == nullptr)
		{
			s_singleton_ = new (std::nothrow) T();
		}
		return s_singleton_;
	}

	static void destroy()
	{
		if (s_singleton_)
		{
			delete s_singleton_;
		}
	}

protected:
	Singleton() = default;

	virtual ~Singleton()
	{
		s_singleton_ = nullptr;
	};

private:
	static T* s_singleton_;
};

template<typename T> T* Singleton<T>::s_singleton_ = nullptr;

#define SINGLETON(_class_name_)					\
	private:									\
		_class_name_();							\
		~_class_name_();						\
		friend class Singleton<_class_name_>

#endif