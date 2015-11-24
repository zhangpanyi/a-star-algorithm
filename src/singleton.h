/**
 * 单例模板类
 */

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <set>

class SingletonBase
{
	class InstanceTable : public std::set < SingletonBase * >
	{
	public:
		InstanceTable()
			: is_cleared_(false)
		{

		};

		~InstanceTable()
		{
			is_cleared_ = true;
			for (auto instance_ptr : *this)
			{
				delete instance_ptr;
			}
		}

	public:
		bool is_cleared_;
	};

protected:
	SingletonBase()
	{
		s_instance_table_.insert(this);
	}

	virtual ~SingletonBase()
	{
		if (!s_instance_table_.is_cleared_)
		{
			s_instance_table_.erase(this);
		}
	}

private:
	static InstanceTable s_instance_table_;
};

template <typename T>
class Singleton : public SingletonBase
{
public:
	static T* instance()
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