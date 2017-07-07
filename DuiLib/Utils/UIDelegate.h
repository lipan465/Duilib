#ifndef __UIDELEGATE_H__
#define __UIDELEGATE_H__

#pragma once
#ifndef __USE_STD_FUNCTION_
namespace DuiLib {

	class UILIB_API CDelegateBase	 
	{
	public:
		CDelegateBase(void* pObject, void* pFn);
		CDelegateBase(const CDelegateBase& rhs);
		virtual ~CDelegateBase();
		bool Equals(const CDelegateBase& rhs) const;
		bool operator() (void* param);
		virtual CDelegateBase* Copy() const = 0; // add const for gcc

	protected:
		void* GetFn();
		void* GetObject();
		virtual bool Invoke(void* param) = 0;

	private:
		void* m_pObject;
		void* m_pFn;
	};

	class CDelegateStatic: public CDelegateBase
	{
		typedef bool (*Fn)(void*);
	public:
		CDelegateStatic(Fn pFn) : CDelegateBase(NULL, pFn) { } 
		CDelegateStatic(const CDelegateStatic& rhs) : CDelegateBase(rhs) { } 
		virtual CDelegateBase* Copy() const { return new CDelegateStatic(*this); }

	protected:
		virtual bool Invoke(void* param)
		{
			Fn pFn = (Fn)GetFn();
			return (*pFn)(param); 
		}
	};

	template <class O, class T>
	class CDelegate : public CDelegateBase
	{
		typedef bool (T::* Fn)(void*);
	public:
		CDelegate(O* pObj, Fn pFn) : CDelegateBase(pObj, &pFn), m_pFn(pFn) { }
		CDelegate(const CDelegate& rhs) : CDelegateBase(rhs) { m_pFn = rhs.m_pFn; } 
		virtual CDelegateBase* Copy() const { return new CDelegate(*this); }

	protected:
		virtual bool Invoke(void* param)
		{
			O* pObject = (O*) GetObject();
			return (pObject->*m_pFn)(param); 
		}  

	private:
		Fn m_pFn;
	};

	template <class O, class T>
	CDelegate<O, T> MakeDelegate(bool (T::* pFn)(void*),O* pObject)
	{
		return CDelegate<O, T>(pObject, pFn);
	}

	inline CDelegateStatic MakeDelegate(bool (*pFn)(void*))
	{
		return CDelegateStatic(pFn); 
	}

	class UILIB_API CEventSource
	{
		typedef bool (*FnType)(void*);
	public:
		~CEventSource();
		operator bool();
		void operator+= (const CDelegateBase& d); // add const for gcc
		void operator+= (FnType pFn);
		void operator-= (const CDelegateBase& d);
		void operator-= (FnType pFn);
		bool operator() (void* param);
		void Clear();

	protected:
		CStdPtrArray m_aDelegates;
	};
} // namespace DuiLib
#else
#include <functional>
namespace DuiLib {
	typedef std::function<bool(void*)> FnType;

class UILIB_API CDelegateBase	 
{	
public:
	CDelegateBase(FnType* pFn)
	{
		m_pfn = new FnType;
		*m_pfn = *pFn;
	};
	~CDelegateBase()
	{
		delete m_pfn;
	}
	bool Run(void* parm){return ((*m_pfn))(parm);}
	bool Equals(const CDelegateBase& rhs) const
	{
		return m_pfn == rhs.m_pfn;
	};
	CDelegateBase* Copy() const 
	{
		return new CDelegateBase(m_pfn);
	};
private:
	FnType* m_pfn;
};

	//#define CC_CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
	//inline CDelegateBase MakeDelegate(std::function<bool(void*)> pFn)
	//{
	//	return CDelegateBase(&pFn);
	//}
	//#define MakeDelegate(__selector__,__target__, ...) std::bind(__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
	template <class O, class T>
	CDelegateBase MakeDelegate(bool (T::* pFn)(void*),O* pObject)
	{
		FnType fn = std::bind(pFn,pObject, std::placeholders::_1);
		return CDelegateBase(&fn);
	}

	class UILIB_API CEventSource
	{
	public:
		~CEventSource();
		operator bool();
		void operator+= (const CDelegateBase& d); // add const for gcc
		void operator+= (FnType pFn);
		void RemoveAll();
		bool operator() (void* param);
	protected:
		CStdPtrArray m_aDelegates;
	};
}
#endif
#endif // __UIDELEGATE_H__