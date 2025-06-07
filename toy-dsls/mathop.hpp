#include <utility>

namespace mathop
{

template<typename T>
struct dummy
{
	T data{};
	dummy()=default;
	dummy(T const& d) : data(d) {}
	dummy(T const&& d) : data(std::move(d)) {}
	operator T&() { return data; }
};

template<typename T>
dummy<T>& operator+=(T const& obj, dummy<T>& d)
{
	d.data = obj;
	return d;
}

#define MATHOP_LOOP(IDX, END, EXPR, OP, NEUTRAL_ELEM) ({ \
	using T = decltype(END); \
	mathop::dummy<T> d{}; \
	auto& IDX += d; \
	T ret = NEUTRAL_ELEM; \
	for (; d <= END; ++d)\
		ret = ret OP (EXPR);\
	ret; \
})
#define Π(IDX, END, EXPR) MATHOP_LOOP(IDX, END, EXPR, *, 1)
#define Σ(IDX, END, EXPR) MATHOP_LOOP(IDX, END, EXPR, +, 0)

// Example: Σ(k=1, 10, (k+1)*(k+1));
// Todo: integration
}
