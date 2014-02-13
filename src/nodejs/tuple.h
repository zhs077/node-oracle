#ifndef TUPLE_H_
#define TUPLE_H_




class NullType{};

template<class T0,
	class T1 = NullType,
	class T2 = NullType,
	class T3 = NullType,
	class T4 = NullType,
	class T5 = NullType,
	class T6 = NullType,
	class T7 = NullType,
	class T8 = NullType,
	class T9 = NullType
	>
struct Pair
{
	typedef std::pair<T0, typename Pair<T1, T2, T3, T4, T5, T6, T7, T8, T9>::Base > Base;
	enum { SIZE = Pair<T1, T2, T3, T4, T5, T6, T7, T8, T9>::SIZE + 1 };
};

template <class T0, class T1>
struct Pair<T0, T1>
{
	typedef std::pair<T0, T1> Base;
	enum { SIZE = 2 };
};

/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
template <int N, class T, bool tag = (N > 0 && N <= T::SIZE)>
struct PairType
{
	typedef void ReturnType;
};

template <class T0, class T1>
struct PairType<1, Pair<T0, T1>, true>
{
	typedef T0 ReturnType;
};

template <class T0, class T1>
struct PairType<2, Pair<T0, T1>, true >
{
	typedef T1 ReturnType;
};

template<class T0,
		 class T1,
		 class T2,
		 class T3,
		 class T4,
		 class T5,
		 class T6,
		 class T7,
		 class T8,
		 class T9
		 >
struct PairType<1, Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>, true >
{
	typedef T0 ReturnType;
};

template <int N, 
		class T0,
		class T1,
		class T2,
		class T3,
		class T4,
		class T5,
		class T6,
		class T7,
		class T8,
		class T9
		>
struct PairType<N, Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>, true >
{
   typedef typename PairType<N-1, Pair<T1, T2, T3, T4, T5, T6, T7, T8, T9>, true >::ReturnType ReturnType;
};

/******************************************************\
 *
\******************************************************/

template <int N, class T>
struct PairValue
{
	static void get(T&){}
	static void get(const T&){}
};

template <class T0, class T1>
struct PairValue<1, Pair<T0, T1> >
{
	static T0& get(typename Pair<T0, T1>::Base& p)
	{
		return p.first;
	}
	static const T0& get(const typename Pair<T0, T1>::Base& p)
	{
		return p.first;
	}
};

template <class T0, class T1>
struct PairValue<2, Pair<T0, T1> >
{
	static T1& get(typename Pair<T0, T1>::Base& p)
	{
		return p.second;
	}
	static const T1& get(const typename Pair<T0, T1>::Base& p)
	{
		return p.second;
	}
};
template <class T0,
		 class T1,
		 class T2,
		 class T3,
		 class T4,
		 class T5,
		 class T6,
		 class T7,
		 class T8,
		 class T9
		 >
struct PairValue<1,Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >
{
	static T0& get(typename Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::Base& p)
	{
		return p.first;
	}
	static const T0& get(const typename Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::Base& p)
	{
		return p.first;
	}
};

template <int N, 
		 class T0,
		 class T1,
		 class T2,
		 class T3,
		 class T4,
		 class T5,
		 class T6,
		 class T7,
		 class T8,
		 class T9
		 >
struct PairValue<N, Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >
{
	static typename PairType<N, Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >::ReturnType& 
		get(typename Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::Base& p)
	{
		return PairValue<N-1, Pair<T1, T2, T3, T4, T5, T6, T7, T8, T9> >::get(p.second);
	}
	static const typename PairType<N, Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >::ReturnType& 
		get(const typename Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::Base& p)
	{
		return PairValue<N-1, Pair<T1, T2, T3, T4, T5, T6, T7, T8, T9> >::get(p.second);
	}
};

/***************************************************\
 *
\***************************************************/ 

template<class T0,
	class T1 = NullType,
	class T2 = NullType,
	class T3 = NullType,
	class T4 = NullType,
	class T5 = NullType,
	class T6 = NullType,
	class T7 = NullType,
	class T8 = NullType,
	class T9 = NullType
	>
class tuple : public Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::Base
{
private:
	typedef typename Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::Base duo0;
public:
	enum { SIZE = tuple< T1, T2, T3, T4, T5, T6, T7, T8, T9 >::SIZE + 1 };
public:
	tuple(){}
	tuple(const T0& t0,
		  const T1& t1 = T1(),
		  const T2& t2 = T2(),
		  const T3& t3 = T3(),
		  const T4& t4 = T4(),
		  const T5& t5 = T5(),
		  const T6& t6 = T6(),
		  const T7& t7 = T7(),
		  const T8& t8 = T8(),
		  const T9& t9 = T9()
		 )
	:duo0(t0, tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9>(t1, t2, t3, t4, t5, t6, t7, t8, t9))
	{
	}
	template <int N>
	typename PairType<N, Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >::ReturnType& get()
	{
		return PairValue<N, Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >::get(*this);
	}
	template <int N>
	const typename PairType<N, Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >::ReturnType& get() const
	{
		return PairValue<N, Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >::get(*this);
	}

};

template<class T0,class T1>
class tuple<T0, T1>	: public Pair<T0, T1>::Base
{
private:
	typedef typename Pair<T0, T1>::Base duo0;
public:
	enum { SIZE = 2 };
public:
	tuple(){}
	tuple(const T0& t0,const T1& t1 = T1(), ...):duo0(t0, t1)
	{
	}
	template <int N>
	typename PairType<N, Pair<T0, T1> >::ReturnType& get()
	{
		return PairValue<N, Pair<T0, T1> >::get(*this);
	}
	template <int N>
	const typename PairType<N, Pair<T0, T1> >::ReturnType& get() const
	{
		return PairValue<N, Pair<T0, T1> >::get(*this);
	}

};

template<class T>
class tuple<T>
{
private:
	T duo0;
public:
	enum { SIZE = 1 };
public:
	tuple(){}
	tuple(const T t):duo0(t)
	{
	}
	template <int N>
	T& get()
	{
		return duo0;
	}
	template <int N>
	const T& get() const
	{
		return duo0;
	}
	bool operator <(const tuple<T>& r) const
	{
		return duo0 < r.duo0;
	}
	bool operator ==(const tuple<T>& r) const
	{
		return duo0 == r.duo0;
	}

};

template <int N, 
		 class T0,
		 class T1,
		 class T2,
		 class T3,
		 class T4,
		 class T5,
		 class T6,
		 class T7,
		 class T8,
		 class T9
		 >
typename PairType<N, Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >::ReturnType& 
get(tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>& t)
{
	return PairValue<N, Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >::get(t);
}

template <int N, 
		 class T0,
		 class T1,
		 class T2,
		 class T3,
		 class T4,
		 class T5,
		 class T6,
		 class T7,
		 class T8,
		 class T9
		 >
const typename PairType<N, Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >::ReturnType& 
get(const tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>& t)
{
	return PairValue<N, Pair<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >::get(t);
}

template <int N, class T>
T get(tuple<T> & t)
{
	return t.get<N>();
}

template <int N, class T>
const T get(const tuple<T> & t)
{
	return t.get<N>();
}

// Make a tuple
template<typename T0>
inline
tuple<T0>
make_tuple(const T0& t0)
{
	return tuple<T0>(t0);
}

// Make a tuple
template<typename T0, typename T1>
inline
tuple<T0, T1>
make_tuple(const T0& t0, const T1& t1)
{
	return tuple<T0, T1>(t0, t1);
}

// Make a tuple
template<typename T0, typename T1, typename T2>
inline
tuple<T0, T1, T2>
make_tuple(const T0& t0, const T1& t1, const T2& t2)
{
	return tuple<T0, T1, T2>(t0, t1, t2);
}

// Make a tuple
template<typename T0, typename T1, typename T2, typename T3>
inline
tuple<T0, T1, T2, T3>
make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3)
{
	return tuple<T0, T1, T2, T3>(t0, t1, t2, t3);
}

// Make a tuple
template<typename T0, typename T1, typename T2, typename T3, typename T4>
inline
tuple<T0, T1, T2, T3, T4>
make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4)
{
	return tuple<T0, T1, T2, T3, T4>(t0, t1, t2, t3, t4);
}

// Make a tuple
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
inline
tuple<T0, T1, T2, T3, T4, T5>
make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
{
	return tuple<T0, T1, T2, T3, T4, T5>(t0, t1, t2, t3, t4, t5);
}

// Make a tuple
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline
tuple<T0, T1, T2, T3, T4, T5, T6>
make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6)
{
	return tuple<T0, T1, T2, T3, T4, T5, T6>(t0, t1, t2, t3, t4, t5, t6);
}

// Make a tuple
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline
tuple<T0, T1, T2, T3, T4, T5, T6, T7>
make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7)
{
	return tuple<T0, T1, T2, T3, T4, T5, T6, T7>(t0, t1, t2, t3, t4, t5, t6, t7);
}

// Make a tuple
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline
tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8>
make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8)
{
	return tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8>(t0, t1, t2, t3, t4, t5, t6, t7, t8);
}

// Make a tuple
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
inline
tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>
make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9)
{
	return tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>(t0, t1, t2, t3, t4, t5, t6, t7, t8, t9);
}

#endif
