#ifndef CPPAD_CG_ARITHMETIC_INCLUDED
#define CPPAD_CG_ARITHMETIC_INCLUDED
/* --------------------------------------------------------------------------
 *  CppADCodeGen: C++ Algorithmic Differentiation with Source Code Generation:
 *    Copyright (C) 2012 Ciengis
 *
 *  CppADCodeGen is distributed under multiple licenses:
 *
 *   - Common Public License Version 1.0 (CPL1), and
 *   - GNU General Public License Version 2 (GPL2).
 *
 * CPL1 terms and conditions can be found in the file "epl-v10.txt", while
 * terms and conditions for the GPL2 can be found in the file "gpl2.txt".
 * ----------------------------------------------------------------------------
 * Author: Joao Leal
 */

namespace CppAD {

    template<class Base>
    CodeHandler<Base>* getHandler(const CG<Base> &left,
                                  const CG<Base> &right) {

        assert(!left.isParameter() || !right.isParameter());

        CodeHandler<Base>* handler;
        if (left.isParameter()) {
            handler = right.getCodeHandler();
        } else if (right.isParameter()) {
            handler = left.getCodeHandler();
        } else {
            if (left.getCodeHandler() != right.getCodeHandler()) {
                throw CGException("Attempting to use several source code generation handlers in the same source code generation");
            }
            handler = left.getCodeHandler();
        }
        return handler;
    }

    template<class Base>
    inline CG<Base> operator+(const CG<Base> &left, const CG<Base> &right) {
        if (left.isParameter() && right.isParameter()) {
            return CG<Base > (left.getParameterValue() + right.getParameterValue());

        } else {
            if (left.isParameter()) {
                if (left.IdenticalZero()) {
                    return right;
                }
            } else if (right.isParameter()) {
                if (right.IdenticalZero()) {
                    return left;
                }
            }

            CodeHandler<Base>* handler = getHandler(left, right);

            return CG<Base > (*handler, new SourceCodeFragment<Base > (CGAddOp, left.argument(), right.argument()));
        }
    }

    template<class Base>
    inline CG<Base> operator-(const CG<Base> &left, const CG<Base> &right) {
        if (left.isParameter() && right.isParameter()) {
            return CG<Base > (left.getParameterValue() - right.getParameterValue());

        } else {
            if (right.isParameter()) {
                if (right.IdenticalZero()) {
                    return left;
                }
            }

            CodeHandler<Base>* handler = getHandler(left, right);

            return CG<Base > (*handler, new SourceCodeFragment<Base > (CGSubOp, left.argument(), right.argument()));
        }
    }

    template<class Base>
    inline CG<Base> operator*(const CG<Base> &left, const CG<Base> &right) {
        if (left.isParameter() && right.isParameter()) {
            return CG<Base > (left.getParameterValue() * right.getParameterValue());

        } else {
            if (left.isParameter()) {
                if (left.IdenticalZero()) {
                    return CG<Base > (Base(0.0)); // does not consider the possibility of right being infinity
                } else if (left.IdenticalOne()) {
                    return right;
                }
            } else if (right.isParameter()) {
                if (right.IdenticalZero()) {
                    return CG<Base > (Base(0.0)); // does not consider the possibility of left being infinity
                } else if (right.IdenticalOne()) {
                    return left;
                }
            }

            CodeHandler<Base>* handler = getHandler(left, right);

            return CG<Base > (*handler, new SourceCodeFragment<Base > (CGMulOp, left.argument(), right.argument()));
        }
    }

    template<class Base>
    inline CG<Base> operator/(const CG<Base> &left, const CG<Base> &right) {
        if (left.isParameter() && right.isParameter()) {
            return CG<Base > (left.getParameterValue() / right.getParameterValue());

        } else {
            if (left.isParameter()) {
                if (left.IdenticalZero()) {
                    return CG<Base > (Base(0.0)); // does not consider the possibility of right being infinity or zero
                }
            } else if (right.isParameter()) {
                if (right.IdenticalOne()) {
                    return left;
                }
            }

            CodeHandler<Base>* handler = getHandler(left, right);

            return CG<Base > (*handler, new SourceCodeFragment<Base > (CGDivOp, left.argument(), right.argument()));
        }
    }

    template<class Base>
    inline CG<Base> operator+(const Base &left, const CG<Base> &right) {
        return CG<Base > (left) + right;
    }

    template<class Base>
    inline CG<Base> operator+(const CG<Base> &left, const Base &right) {
        return left + CG<Base > (right);
    }

    template<class Base>
    inline CG<Base> operator-(const Base &left, const CG<Base> &right) {
        return CG<Base > (left) - right;
    }

    template<class Base>
    inline CG<Base> operator-(const CG<Base> &left, const Base &right) {
        return left - CG<Base > (right);
    }

    template<class Base>
    inline CG<Base> operator/(const Base &left, const CG<Base> &right) {
        return CG<Base > (left) / right;
    }

    template<class Base>
    inline CG<Base> operator/(const CG<Base> &left, const Base &right) {
        return left / CG<Base > (right);
    }

    template<class Base>
    inline CG<Base> operator*(const Base &left, const CG<Base> &right) {
        return CG<Base > (left) * right;
    }

    template<class Base>
    inline CG<Base> operator*(const CG<Base> &left, const Base &right) {
        return left * CG<Base > (right);
    }

}

#endif

