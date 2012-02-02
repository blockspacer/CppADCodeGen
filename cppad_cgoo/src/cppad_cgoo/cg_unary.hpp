#ifndef CPPAD_CG_UNARY_INCLUDED
#define	CPPAD_CG_UNARY_INCLUDED
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2012 Ciengis

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */


namespace CppAD {

    template<class Base>
    inline CG<Base> CG<Base>::operator+() const {
        CPPAD_CG_CHECK_CG_THIS();

        return CG<Base > (*this); // nothing to do
    }

    template<class Base>
    inline CG<Base> CG<Base>::operator-() const {
        CPPAD_CG_CHECK_CG_THIS();

        if (isParameter()) {
            return CG<Base > (-getParameterValue());

        } else {
            std::string ops = "-";
            if (opTypes_ == NONE || opTypes_ == MULT_DIV || opTypes_ == FUNCTION) {
                ops += operations();
            } else {
                ops += "(" + operations() + ")";
            }

            CG<Base> result;
            result.makeTemporaryVariable(*handler_, ops, UNARY);
            return result;
        }
    }

}

#endif
