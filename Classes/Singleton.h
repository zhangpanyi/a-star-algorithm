#pragma once

#include <list>

class SingletonBase
{
	class InstanceTable : public std::list < SingletonBase * >
	{
	public:
		InstanceTable()
			: cleared_(false)
		{

		};

		~InstanceTable()
		{
			cleared_ = true;
			while (!empty())
			{
				delete back();
				pop_back();
			}
		}

	public:
		bool cleared_;
	};

protected:
	SingletonBase()
	{
		s_instance_table_.push_back(this);
	}

	virtual ~SingletonBase()
	{
		if (!s_instance_table_.cleared_)
		{
			s_instance_table_.remove(this);
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
		friend class Singleton<_class_>;

#define SINGLETON_DEFAULT(_class_)		\
	private:							\
		_class_() {};					\
		~_class_() {};					\
		friend class Singleton<_class_>;