/* lam.hpp - Lambda calculus in C++ */
#include <iostream>
#include <ostream>
#include <memory>
#include <functional>
#include <utility>
#include <variant>
#include <string>

namespace lam
{
class expr;
using ptr = std::shared_ptr<expr>;
using fun = std::function<expr(expr const&)>;

class expr
{
private:
	enum {
		IDENTIFIER, ABSTRACTION, APPLICATION
	} type_;
	std::variant<
		std::string,        // IDENTIFIER
		fun,                // ABSTRACTION
		std::pair<ptr, ptr> // APPLICATION
	> data;
public:
	decltype(type_) const& type = type_;
	
	expr(std::string const& id) : type_(IDENTIFIER), data(id) {}
	expr(fun f) : type_(ABSTRACTION), data(std::move(f)) {}
	expr(ptr lhs, ptr rhs) : type_(APPLICATION), data(std::pair<ptr, ptr>(std::move(lhs), std::move(rhs))) {}
	
	expr operator()(expr const& x) const
	{
		if (type == ABSTRACTION)
			return std::get<fun>(data)(x);
		return lam::expr(std::make_shared<expr>(*this), std::make_shared<expr>(x));
	}

	operator std::string() const
	{
		static thread_local int str_counter = 0;
		switch (type) {
			case IDENTIFIER: {
				auto const& id = std::get<std::string>(data);
				return id;
			}
			case APPLICATION: {
				auto const& [x, y] = std::get<std::pair<ptr, ptr> >(data);
				return "(" + std::string(*x) + " " + std::string(*y) + ")";
			}
			case ABSTRACTION: {
				auto k = std::to_string(str_counter++);
				auto const& f = std::get<fun>(data);
				return "(λ$" + k + "." + std::string(f("$" + k)) + ")";
			}
		}
		return "unknown";
	}

	expr()=default;
	~expr()=default;
};

std::ostream& operator<<(std::ostream& out, expr const& exp)
{
	return out << std::string(exp);
}

// Use this syntax to construct lamexps.
#define λ(x, y)          lam::expr([=](lam::expr const& x) -> lam::expr { return y; })
// Use this syntax to introduce identifiers.
#define LAM_ID(x)        lam::expr x(#x);
// If in global scope, use this macro instead of "auto name = \(x, ...);" because C++ complains about captures.
#define LAM_DEFINE(x, y) auto x = [](){ return y; }();

// "Standard library" (thanks google, stackoverflow, chatgpt, etc)
// Combinators
LAM_DEFINE(K,             λ(x,λ(y, x)))
LAM_DEFINE(S,             λ(x,λ(y,λ(z, x(z)(y(z))))))
LAM_DEFINE(I,             λ(x, x))
LAM_DEFINE(B,             λ(f,λ(g,λ(x, (f(g(x)))))))
LAM_DEFINE(C,             λ(f,λ(g,λ(x, f(x)(g)))))
LAM_DEFINE(Y,             λ(f, (λ(x,f(x)(x))) (λ(x,f(x)(x)))))

// Booleans
LAM_DEFINE(TRUE,          λ(t,λ(f, t)))
LAM_DEFINE(FALSE,         λ(t,λ(f, f)))
LAM_DEFINE(IF,            λ(cond,λ(t,λ(f, cond(t)(f)))))
// Church numerals
LAM_DEFINE(ZERO,          λ(f,λ(x, x)))
LAM_DEFINE(ONE,           λ(f,λ(x, f(x))))
LAM_DEFINE(SUCC,          λ(n,λ(f,λ(x, f(n(f)(x))))))
LAM_DEFINE(ADD,           λ(m,λ(n,λ(f, λ(x, m(f)(n(f)(x)))))))
LAM_DEFINE(MUL,           λ(m,λ(n,λ(f, m(n(f))))))
LAM_DEFINE(EXP,           λ(m,λ(n, n(m))))
LAM_DEFINE(IS_ZERO,       λ(n, n(λ(x,FALSE))(TRUE)))
// Pairs
LAM_DEFINE(PAIR,          λ(x,λ(y,λ(f, f(x)(y)))))
LAM_DEFINE(CAR,           λ(p, p(TRUE)))
LAM_DEFINE(CDR,           λ(p, p(FALSE)))
LAM_DEFINE(SWAP,          λ(p, PAIR(CDR(p))(CAR(p))))
// Lists
LAM_DEFINE(NIL,           λ(c,λ(n, n)))
LAM_DEFINE(CONS,          λ(h,λ(t,λ(c,λ(n, c(h)(t(c)(n)))))))
LAM_DEFINE(MAP,           λ(f,λ(l,λ(c,λ(n, l(λ(h,λ(r, c(f(h))(r))))(n))))))
LAM_DEFINE(FOLDR,         λ(f,λ(z,λ(l, l(f)(z)))))
LAM_DEFINE(FOLDL,         λ(f,λ(z,λ(l, l(λ(h,λ(r, f(r)(h))))(z)))))
LAM_DEFINE(IS_EMPTY_LIST, λ(l, l(λ(x,FALSE))(TRUE)))
LAM_DEFINE(HEAD,          λ(l, l(λ(h,λ(t,h))) (λ(x,x))))
LAM_DEFINE(TAIL,          λ(l, l(λ(h,λ(t,t))) (λ(x,x))))
LAM_DEFINE(FILTER,        λ(p,λ(l,λ(c,λ(n, l(λ(h, λ(r,p(h)(c(h)(r))(r))))(n))))))
}
