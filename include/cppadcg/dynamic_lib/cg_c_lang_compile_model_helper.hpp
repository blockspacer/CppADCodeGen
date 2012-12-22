#ifndef CPPAD_CG_C_LANG_COMPILE_MODEL_HELPER_INCLUDED
#define CPPAD_CG_C_LANG_COMPILE_MODEL_HELPER_INCLUDED
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

    /**
     * Useful class for generating source code for the creation of a dynamic
     * library.
     * 
     * \author Joao Leal
     */
    template<class Base>
    class CLangCompileModelHelper {
        typedef CppAD::CG<Base> CGBase;
        typedef CppAD::AD<CGBase> ADCG;
    public:
        static const std::string FUNCTION_FORWAD_ZERO;
        static const std::string FUNCTION_JACOBIAN;
        static const std::string FUNCTION_HESSIAN;
        static const std::string FUNCTION_SPARSE_JACOBIAN;
        static const std::string FUNCTION_SPARSE_HESSIAN;
        static const std::string FUNCTION_JACOBIAN_SPARSITY;
        static const std::string FUNCTION_HESSIAN_SPARSITY;
        static const std::string FUNCTION_INFO;
    protected:
        static const std::string CONST;
    protected:
        ADFun<CGBase>* _fun; // the  model
        std::string _name; // the name of the model
        const std::string _baseTypeName;
        bool _zero;
        bool _jacobian;
        bool _hessian;
        bool _sparseJacobian;
        bool _sparseHessian;
        std::vector<size_t> _custom_jac_row;
        std::vector<size_t> _custom_jac_col;
        std::vector<size_t> _custom_hess_row;
        std::vector<size_t> _custom_hess_col;
        std::ostringstream _cache;
        size_t _maxAssignPerFunc; // maximum number of assignments per function (~ lines)
        bool _verbose;
        double _beginTime; // auxiliary variable to measure the elapsed time
    public:

        /**
         * Creates a new C language compilation helper for a model
         * 
         * \param fun The ADFun with the taped model
         * \param model The model name (must be a valid C function name)
         */
        CLangCompileModelHelper(ADFun<CppAD::CG<Base> >* fun, const std::string& model) :
            _fun(fun),
            _name(model),
            _baseTypeName(CLangCompileModelHelper<Base>::baseTypeName()),
            _zero(true),
            _jacobian(false),
            _hessian(false),
            _sparseJacobian(false),
            _sparseHessian(false),
            _maxAssignPerFunc(20000),
            _beginTime(0) {

            CPPADCG_ASSERT_KNOWN(_fun != NULL, "ADFun cannot be null");
            CPPADCG_ASSERT_KNOWN(!_name.empty(), "Model name cannot be empty");
            CPPADCG_ASSERT_KNOWN((_name[0] >= 'a' && _name[0] <= 'z') ||
                                 (_name[0] >= 'A' && _name[0] <= 'Z'),
                                 "Invalid model name character");
            for (size_t i = 1; i < _name.size(); i++) {
                char c = _name[i];
                CPPADCG_ASSERT_KNOWN((c >= 'a' && c <= 'z') ||
                                     (c >= 'A' && c <= 'Z') ||
                                     (c >= '0' && c <= '9') ||
                                     c == '_'
                                     , "Invalid model name character");
            }
        }

        inline const std::string& getName() const {
            return _name;
        }

        inline bool isCreateHessian() const {
            return _hessian;
        }

        inline void setCreateHessian(bool createFunction) {
            _hessian = createFunction;
        }

        inline bool isCreateJacobian() const {
            return _jacobian;
        }

        inline void setCreateJacobian(bool createFunction) {
            _jacobian = createFunction;
        }

        inline bool isCreateSparseHessian() const {
            return _sparseHessian;
        }

        inline void setCreateSparseHessian(bool createFunction) {
            _sparseHessian = createFunction;
        }

        inline bool isCreateSparseJacobian() const {
            return _sparseJacobian;
        }

        inline void setCreateSparseJacobian(bool createFunction) {
            _sparseJacobian = createFunction;
        }

        inline bool isCreateForwardZero() const {
            return _zero;
        }

        inline void setCreateForwardZero(bool createFunction) {
            _zero = createFunction;
        }

        inline void setCustomSparseJacobianElements(const std::vector<size_t>& row,
                                                    const std::vector<size_t>& col) {
            _custom_jac_row = row;
            _custom_jac_col = col;
        }

        inline void setCustomSparseHessianElements(const std::vector<size_t>& row,
                                                   const std::vector<size_t>& col) {
            _custom_hess_row = row;
            _custom_hess_col = col;
        }

        inline size_t getMaxAssignmentsPerFunc() const {
            return _maxAssignPerFunc;
        }

        inline void setMaxAssignmentsPerFunc(size_t maxAssignPerFunc) {
            _maxAssignPerFunc = maxAssignPerFunc;
        }

        inline virtual ~CLangCompileModelHelper() {
        };

        static inline std::string baseTypeName();

    protected:

        inline bool isVerbose() const {
            return _verbose;
        }

        inline void setVerbose(bool verbose) {
            _verbose = verbose;
        }

        virtual VariableNameGenerator<Base>* createVariableNameGenerator(const std::string& depName,
                                                                         const std::string& indepName,
                                                                         const std::string& tmpName);

        virtual void compileSources(CLangCompiler<Base>& compiler);

        virtual void generateInfoSource(std::map<std::string, std::string>& sources);

        virtual void generateZeroSource(std::map<std::string, std::string>& sources);

        virtual void generateJacobianSource(std::map<std::string, std::string>& sources);

        virtual void generateHessianSource(std::map<std::string, std::string>& sources);

        virtual void generateSparseJacobianSource(std::map<std::string, std::string>& sources);

        virtual void generateSparseHessianSource(std::map<std::string, std::string>& sources);

        virtual void generateSparsitySource(const std::string& function,
                                            const std::vector<size_t>& rows,
                                            const std::vector<size_t>& cols);

    private:
        void inline startingGraphCreation(const std::string& jobName);

        void inline finishedGraphCreation();

        CLangCompileModelHelper(const CLangCompileModelHelper&); // not implemented

        CLangCompileModelHelper& operator=(const CLangCompileModelHelper&); // not implemented

        friend class
        CLangCompileDynamicHelper<Base>;
    };

}

#endif
