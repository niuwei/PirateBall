#ifndef _Singleton_H_
#define _Singleton_H_

template < class _Type >
class singleton
{
public:
	static _Type* getInstance()
	{
		if (!s_instance)
		{
			s_instance = new _Type();
		}
		return s_instance;
	}

protected:
	static _Type*  s_instance;
};

template<class _Type>
_Type*  singleton<_Type>::s_instance = 0;


#endif