#pragma once

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
	static T* GetInstance()
	{
		if (s_singleton_ == nullptr)
		{
			s_singleton_ = new (std::nothrow) T();
		}
		return s_singleton_;
	}

	static void DestroyInstance()
	{
		if (s_singleton_)
		{
			delete s_singleton_;
		}
	}

protected:
	Singleton()
	{

	};

	virtual ~Singleton()
	{
		s_singleton_ = nullptr;
	};

private:
	static T* s_singleton_;
};

template<typename T> T* Singleton<T>::s_singleton_ = nullptr;

#define SINGLETON(_class_)				\
	private:							\
		_class_();						\
		~_class_();						\
		friend class Singleton<_class_>

#define SINGLETON_DEFAULT(_class_)		\
	private:							\
		_class_() {};					\
		~_class_() {};					\
		friend class Singleton<_class_>