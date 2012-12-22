#ifndef CPPAD_CG_ARITHMETIC_ASSIGN_INCLUDED
#define CPPAD_CG_ARITHMETIC_ASSIGN_INCLUDED
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
    inline CG<Base>& CG<Base>::operator+=(const CG<Base> &right) {
        if (isParameter() && right.isParameter()) {
            *value_ += *right.value_;

        } else {
            CodeHandler<Base>* handler;
            if (isParameter()) {
                if (IdenticalZero()) {
                    *this = right;
                    return *this;
                }

                handler = right.getCodeHandler();

            } else if (right.isParameter()) {
                if (right.IdenticalZero()) {
                    return *this; // nothing to do
                }

                handler = getCodeHandler();

            } else {
                assert(getCodeHandler() == right.getCodeHandler());
                handler = getCodeHandler();
            }

            makeVariable(*handler, new SourceCodeFragment<Base>(CGAddOp, argument(), right.argument()));
        }

        return *this;
    }

    template<class Base>
    inline CG<Base>& CG<Base>::operator-=(const CG<Base> &right) {
        if (isParameter() && right.isParameter()) {
            *value_ -= *right.value_;

        } else {
            CodeHandler<Base>* handler;
            if (isParameter()) {
                handler = right.getCodeHandler();

            } else if (right.isParameter()) {
                if (right.IdenticalZero()) {
                    return *this; // nothing to do
                }

                handler = getCodeHandler();

            } else {
                assert(getCodeHandler() == right.getCodeHandler());
                handler = getCodeHandler();
            }

            makeVariable(*handler, new SourceCodeFragment<Base>(CGSubOp, argument(), right.argument()));
        }

        return *this;
    }

    template<class Base>
    inline CG<Base>& CG<Base>::operator*=(const CG<Base> &right) {
        if (isParameter() && right.isParameter()) {
            *value_ *= *right.value_;

        } else {
            CodeHandler<Base>* handler;
            if (isParameter()) {
                if (IdenticalZero()) {
                    return *this; // nothing to do (does not consider that right might be infinity)
                } else if (IdenticalOne()) {
                    *this = right;
                    return *this;
                }

                handler = right.getCodeHandler();

            } else if (right.isParameter()) {
                if (right.IdenticalZero()) {
                    makeParameter(Base(0.0)); // does not consider that left might be infinity
                    return *this;
                } else if (right.IdenticalOne()) {
                    return *this; // nothing to do
                }

                handler = getCodeHandler();

            } else {
                assert(getCodeHandler() == right.getCodeHandler());
                handler = getCodeHandler();
            }

            makeVariable(*handler, new SourceCodeFragment<Base>(CGMulOp, argument(), right.argument()));
        }

        return *this;
    }

    template<class Base>
    inline CG<Base>& CG<Base>::operator/=(const CG<Base> &right) {
        if (isParameter() && right.isParameter()) {
            *value_ /= *right.value_;

        } else {
            CodeHandler<Base>* handler;
            if (isParameter()) {
                if (IdenticalZero()) {
                    return *this; // nothing to do (does not consider that right might be infinity or zero)
                }

                handler = right.getCodeHandler();

            } else if (right.isParameter()) {
                if (right.IdenticalOne()) {
                    return *this; // nothing to do
                }

                handler = getCodeHandler();

            } else {
                assert(getCodeHandler() == right.getCodeHandler());
                handler = getCodeHandler();
            }

            makeVariable(*handler, new SourceCodeFragment<Base>(CGDivOp, argument(), right.argument()));
        }

        return *this;
    }

    template<class Base>
    inline CG<Base>& CG<Base>::operator+=(const Base &right) {
        return operator+=(CG<Base > (right));
    }

    template<class Base>
    inline CG<Base>& CG<Base>::operator-=(const Base &right) {
        return operator-=(CG<Base > (right));
    }

    template<class Base>
    inline CG<Base>& CG<Base>::operator/=(const Base &right) {
        return operator/=(CG<Base > (right));
    }

    template<class Base>
    inline CG<Base>& CG<Base>::operator*=(const Base &right) {
        return operator*=(CG<Base > (right));
    }

    template<class Base>
    template<class T>
    inline CG<Base>& CG<Base>::operator+=(const T &right) {
        return operator+=(CG<Base > (right));
    }

    template<class Base>
    template<class T>
    inline CG<Base>& CG<Base>::operator-=(const T &right) {
        return operator-=(CG<Base > (right));
    }

    template<class Base>
    template<class T>
    inline CG<Base>& CG<Base>::operator/=(const T &right) {
        return operator/=(CG<Base > (right));
    }

    template<class Base>
    template<class T>
    inline CG<Base>& CG<Base>::operator*=(const T &right) {
        return operator*=(CG<Base > (right));
    }

}

#endif

