/**
 * 单例模板类
 */

#pragma once

#include <set>

class singleton_base
{
	class instance_table : public std::set < singleton_base * >
	{
	public:
		instance_table()
			: is_cleared_(false)
		{

		};

		~instance_table()
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
	singleton_base()
	{
		s_instance_table_.insert(this);
	}

	virtual ~singleton_base()
	{
		if (!s_instance_table_.is_cleared_)
		{
			s_instance_table_.erase(this);
		}
	}

private:
	static instance_table s_instance_table_;
};

template <typename T>
class singleton : public singleton_base
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
	singleton() = default;

	virtual ~singleton()
	{
		s_singleton_ = nullptr;
	};

private:
	static T* s_singleton_;
};

template<typename T> T* singleton<T>::s_singleton_ = nullptr;

#define SINGLETON(_class_name_)					\
	private:									\
		_class_name_();							\
		~_class_name_();						\
		friend class Singleton<_class_name_>