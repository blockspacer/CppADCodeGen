#ifndef CPPAD_CG_CODE_HANDLER_INCLUDED
#define CPPAD_CG_CODE_HANDLER_INCLUDED
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
    class CG;

    /**
     * Helper class to analyze the operation graph and generate source code
     * for several languages
     * 
     * \author Joao Leal
     */
    template<class Base>
    class CodeHandler {
    protected:
        // counter used to generate variable IDs
        size_t _idCount;
        // the independent variables
        std::vector<SourceCodeFragment<Base> *> _independentVariables;
        // all the source code blocks created with the CG<Base> objects
        std::vector<SourceCodeFragment<Base> *> _codeBlocks;
        /** the order for the variable creation in the source code
         * (variables which are used more than once) */
        std::vector<SourceCodeFragment<Base> *> _variableOrder;
        // all the source code blocks created for the optimized operation graph
        std::vector<SourceCodeFragment<Base> *> _codeBlocksOpt;
        /** the order for the optimized variable creation in the source code
         * (variables which are used more than once) */
        std::vector<SourceCodeFragment<Base> *> _variableOrderOpt;
        // Maps regular variables to their optimized versions
        std::map<SourceCodeFragment<Base> *, SourceCodeFragment<Base> *> _regular2OptcodeBlocks;
        // a flag indicating if this handler was previously used to generate code
        bool _used;
        // a flag indicating whether or not to reuse the IDs of destroyed variables
        bool _reuseIDs;
        /* a flag indicating whether or not to attempt to reduce the number of
         * operations in the operation graph */
        bool _optimize;
        // the language used for source code generation
        Language<Base>* _lang;
        // the lowest ID used for temporary variables
        size_t _minTemporaryVarID;
        //
        bool _verbose;
    public:

        CodeHandler(size_t varCount = 50) :
        _idCount(0),
        _used(false),
        _reuseIDs(true),
        _optimize(true),
        _lang(NULL),
        _minTemporaryVarID(0),
        _verbose(false) {
            _codeBlocks.reserve(varCount);
            _variableOrder.reserve(1 + varCount / 3);
        }

        inline void setReuseVariableIDs(bool reuse) {
            _reuseIDs = reuse;
        }

        inline bool isReuseVariableIDs() const {
            return _reuseIDs;
        }

        inline void setOptimize(bool optimize) {
            _optimize = optimize;
        }

        inline bool isOptimize() const {
            return _optimize;
        }

        inline void makeVariables(std::vector<CG<Base> >& variables) {
            for (typename std::vector<CG<Base> >::iterator it = variables.begin(); it != variables.end(); ++it) {
                makeVariable(*it);
            }
        }

        inline void makeVariables(std::vector<AD<CG<Base> > >& variables) {
            for (typename std::vector<AD<CG<Base> > >::iterator it = variables.begin(); it != variables.end(); ++it) {
                CG<Base> v;
                makeVariable(v); // make it a codegen variable
                *it = v; // variable[i] id now the same as v
            }
        }

        inline void makeVariable(CG<Base>& variable) {
            _independentVariables.push_back(new SourceCodeFragment<Base > (CGInvOp));
            variable.makeVariable(*this, _independentVariables.back());
        }

        size_t getIndependentVariableSize() const {
            return _independentVariables.size();
        }

        size_t getIndependentVariableIndex(const SourceCodeFragment<Base>& var) const throw (CGException) {
            assert(var.operation_ == CGInvOp);

            typename std::vector<SourceCodeFragment<Base> *>::const_iterator it =
                    std::find(_independentVariables.begin(), _independentVariables.end(), &var);
            if (it == _independentVariables.end()) {
                throw CGException("Variable not found in the independent variable vector");
            }

            return it - _independentVariables.begin();
        }

        inline size_t getMaximumVariableID() const {
            return _idCount;
        }

        inline bool isVerbose() const {
            return _verbose;
        }

        inline void setVerbose(bool verbose) {
            _verbose = verbose;
        }

        /**
         * Creates the source code from the operations registered so far.
         * 
         * \param out The output stream where the source code is to be printed.
         * \param lang The targeted language.
         * \param dependent The dependent variables for which the source code
         *                  should be generated. By defining this vector the 
         *                  number of operations in the source code can be 
         *                  reduced and thus providing a more optimized code.
         * \param nameGen Provides the rules for variable name creation.
         */
        virtual void generateCode(std::ostream& out,
                CppAD::Language<Base>& lang,
                std::vector<CG<Base> >& dependent,
                VariableNameGenerator<Base>& nameGen,
                const std::string& jobName = "source") {
            double beginTime;
            if (_verbose) {
                std::cout << "generating source for '" << jobName << "' ... ";
                std::cout.flush();
                beginTime = system::currentTime();
            }

            _lang = &lang;
            _idCount = 0;

            if (_used) {
                _variableOrder.clear();
                _variableOrderOpt.clear();
                _regular2OptcodeBlocks.clear();

                for (typename std::vector<SourceCodeFragment<Base> *>::const_iterator it = _codeBlocks.begin(); it != _codeBlocks.end(); ++it) {
                    SourceCodeFragment<Base>* block = *it;
                    block->resetHandlerCounters();
                }
            }
            _used = true;

            /**
             * the first variable IDs are for the independent variables
             */
            for (typename std::vector<SourceCodeFragment<Base> *>::iterator it = _independentVariables.begin(); it != _independentVariables.end(); ++it) {
                (*it)->setVariableID(++_idCount);
            }

            for (typename std::vector<CG<Base> >::iterator it = dependent.begin(); it != dependent.end(); ++it) {
                if (it->getSourceCodeFragment() != NULL && it->getSourceCodeFragment()->variableID() == 0) {
                    it->getSourceCodeFragment()->setVariableID(++_idCount);
                }
            }

            _minTemporaryVarID = _idCount + 1;

            // determine the number of times each variable is used
            for (typename std::vector<CG<Base> >::iterator it = dependent.begin(); it != dependent.end(); ++it) {
                CG<Base>& var = *it;
                if (var.getSourceCodeFragment() != NULL) {
                    SourceCodeFragment<Base>& code = *var.getSourceCodeFragment();
                    markCodeBlockUsed(code);
                }
            }

            // determine the variable creation order
            for (typename std::vector<CG<Base> >::iterator it = dependent.begin(); it != dependent.end(); ++it) {
                CG<Base>& var = *it;
                if (var.getSourceCodeFragment() != NULL) {
                    SourceCodeFragment<Base>& code = *var.getSourceCodeFragment();
                    if (code.use_count_ == 0) {
                        // dependencies not visited yet
                        checkVariableCreation(code);

                        // make sure new temporary variables are NOT created for
                        // the independent variables and that a dependency did
                        // not use it first
                        if ((code.variableID() == 0 || !isIndependent(code)) && code.use_count_ == 0) {
                            addToEvaluationQueue(code);
                        }
                    }
                    code.use_count_++;
                }
            }

            assert(_idCount == _variableOrder.size() + _independentVariables.size());

            std::vector<CG<Base> > dependentOpt;
            //_optimize = false;
            if (_optimize) {
                // optimize the operation graph
                optimizeOperationGraph();
                // determine the list of optimized dependent variables
                dependentOpt = dependent;
                for (size_t i = 0; i < dependentOpt.size(); i++) {
                    if (dependentOpt[i].isVariable() && !isIndependent(*dependentOpt[i].sourceCode_)) {
                        dependentOpt[i].sourceCode_ = _regular2OptcodeBlocks.at(dependent[i].sourceCode_);
                    }
                }
            }

            std::vector<CG<Base> >& dependent2 = _optimize ? dependentOpt : dependent;

            if (_reuseIDs) {
                reduceTemporaryVariables(dependent2);
            }

            nameGen.setTemporaryVariableID(_minTemporaryVarID, _idCount);

            /**
             * Creates the source code for a specific language
             */
            LanguageGenerationData<Base> info(_independentVariables, dependent2,
                    _minTemporaryVarID,
                    _optimize ? _variableOrderOpt : _variableOrder,
                    nameGen, _reuseIDs);
            lang.generateSourceCode(out, info);

            if (_verbose) {
                double endTime = system::currentTime();
                std::cout << "done [" << std::fixed << std::setprecision(3)
                        << (endTime - beginTime) << "]" << std::endl;
            }
        }

        virtual void reset() {
            typename std::vector<SourceCodeFragment<Base> *>::iterator itc;
            for (itc = _codeBlocks.begin(); itc != _codeBlocks.end(); ++itc) {
                delete *itc;
            }
            _codeBlocks.clear();
            for (itc = _codeBlocksOpt.begin(); itc != _codeBlocksOpt.end(); ++itc) {
                delete *itc;
            }
            _codeBlocksOpt.clear();
            _regular2OptcodeBlocks.clear();
            _independentVariables.clear();
            _idCount = 0;
            _used = false;
        }

        /**
         * Solves an expression (e.g. f(x, y) == 0) for a given variable (e.g. x)
         * The variable can appear only once in the expression.
         * 
         * \param expression  The original expression (f(x, y))
         * \param code  The variable to solve for
         * \return  The expression for variable
         */
        inline CG<Base> solveFor(SourceCodeFragment<Base>* expression,
                SourceCodeFragment<Base>* code) throw (CGException);

        /**
         * Eliminates an independent variable by substitution using the provided
         * dependent variable which is assumed to be a residual of an equation.
         * If successful the model will contain one less independent variable.
         * 
         * \param indep The independent variable to eliminate.
         * \param dep The dependent variable representing a residual
         */
        inline void substituteIndependent(const CG<Base>& indep,
                const CG<Base>& dep) throw (CGException);

        inline void substituteIndependent(SourceCodeFragment<Base>* indep,
                SourceCodeFragment<Base>* dep) throw (CGException);

        virtual ~CodeHandler() {
            reset();
        }

    protected:

        virtual void manageSourceCodeBlock(SourceCodeFragment<Base>* code) {
            assert(std::find(_codeBlocks.begin(), _codeBlocks.end(), code) == _codeBlocks.end());

            if (_codeBlocks.capacity() == _codeBlocks.size()) {
                _codeBlocks.reserve((_codeBlocks.size()*3) / 2 + 1);
            }

            _codeBlocks.push_back(code);
        }

        virtual void markCodeBlockUsed(SourceCodeFragment<Base>& code) {
            code.total_use_count_++;

            if (code.total_use_count_ == 1) {
                // first time this operation is visited

                const std::vector<Argument<Base> >& args = code.arguments_;

                typename std::vector<Argument<Base> >::const_iterator it;
                for (it = args.begin(); it != args.end(); ++it) {
                    if (it->operation() != NULL) {
                        SourceCodeFragment<Base>& arg = *it->operation();
                        markCodeBlockUsed(arg);
                    }
                }
            }
        }

        virtual void checkVariableCreation(SourceCodeFragment<Base>& code) {
            const std::vector<Argument<Base> >& args = code.arguments_;

            typename std::vector<Argument<Base> >::const_iterator it;

            for (it = args.begin(); it != args.end(); ++it) {
                if (it->operation() != NULL) {
                    SourceCodeFragment<Base>& arg = *it->operation();

                    if (arg.use_count_ == 0) {
                        // dependencies not visited yet
                        checkVariableCreation(arg);

                        // make sure new temporary variables are NOT created for
                        // the independent variables and that a dependency did
                        // not use it first
                        if ((arg.variableID() == 0 || !isIndependent(arg)) && arg.use_count_ == 0) {

                            size_t argIndex = it - args.begin();
                            if (_lang->createsNewVariable(arg) ||
                                    _lang->requiresVariableArgument(code.operationCode(), argIndex)) {
                                addToEvaluationQueue(arg);
                                if (arg.variableID() == 0) {
                                    arg.setVariableID(++_idCount);
                                }
                            }
                        }
                    }

                    arg.use_count_++;
                }
            }

        }

        inline void addToEvaluationQueue(SourceCodeFragment<Base>& arg) {
            if (_variableOrder.size() == _variableOrder.capacity()) {
                _variableOrder.reserve((_variableOrder.size()*3) / 2 + 1);
            }

            _variableOrder.push_back(&arg);
            arg.setEvaluationOrder(_variableOrder.size());

            dependentAdded2EvaluationQueue(arg);
        }

        inline void reduceTemporaryVariables(std::vector<CG<Base> >& dependent) {

            std::vector<SourceCodeFragment<Base> *>& varOrder = _optimize ? _variableOrderOpt : _variableOrder;
            /**
             * determine the last line where each temporary variable is used
             */
            resetUsageCount();

            for (typename std::vector<CG<Base> >::iterator it = dependent.begin(); it != dependent.end(); ++it) {
                CG<Base>& var = *it;
                if (var.getSourceCodeFragment() != NULL) {
                    SourceCodeFragment<Base>& code = *var.getSourceCodeFragment();
                    if (code.use_count_ == 0) {
                        // dependencies not visited yet
                        determineLastTempVarUsage(code);
                    }
                    code.use_count_++;
                }
            }

            // location of where temporary variables can be released
            std::vector<std::vector<SourceCodeFragment<Base>* > > tempVarRelease(varOrder.size());
            for (size_t i = 0; i < varOrder.size(); i++) {
                SourceCodeFragment<Base>* var = varOrder[i];
                if (isTemporary(*var)) {
                    tempVarRelease[var->getLastUsageEvaluationOrder() - 1].push_back(var);
                }
            }


            /**
             * Redefine temporary variable IDs
             */
            std::vector<size_t> freedVariables; // variable IDs no longer in use
            _idCount = _minTemporaryVarID - 1;

            for (size_t i = 0; i < varOrder.size(); i++) {
                SourceCodeFragment<Base>& var = *varOrder[i];

                const std::vector<SourceCodeFragment<Base>* >& released = tempVarRelease[i];
                for (size_t r = 0; r < released.size(); r++) {
                    freedVariables.push_back(released[r]->variableID());
                }

                if (isTemporary(var)) {
                    if (freedVariables.empty()) {
                        var.setVariableID(++_idCount);
                    } else {
                        size_t id = freedVariables.back();
                        freedVariables.pop_back();
                        var.setVariableID(id);
                    }
                }
            }
        }

        inline void determineLastTempVarUsage(SourceCodeFragment<Base>& code) {
            const std::vector<Argument<Base> >& args = code.arguments_;

            typename std::vector<Argument<Base> >::const_iterator it;

            /**
             * count variable usage
             */
            for (it = args.begin(); it != args.end(); ++it) {
                if (it->operation() != NULL) {
                    SourceCodeFragment<Base>& arg = *it->operation();

                    if (arg.use_count_ == 0) {
                        // dependencies not visited yet
                        determineLastTempVarUsage(arg);
                    }

                    arg.use_count_++;

                    if (arg.getLastUsageEvaluationOrder() < code.getEvaluationOrder()) {
                        arg.setLastUsageEvaluationOrder(code.getEvaluationOrder());
                    }
                }
            }
        }

        inline void resetUsageCount() {
            typename std::vector<SourceCodeFragment<Base> *>::const_iterator it;
            for (it = _codeBlocks.begin(); it != _codeBlocks.end(); ++it) {
                SourceCodeFragment<Base>* block = *it;
                block->use_count_ = 0;
            }
            for (it = _codeBlocksOpt.begin(); it != _codeBlocksOpt.end(); ++it) {
                SourceCodeFragment<Base>* block = *it;
                block->use_count_ = 0;
            }
        }

        /**
         * Defines the evaluation order for code fragments that do not
         * create variables
         * \param code The operation just added to the evaluation order
         */
        inline void dependentAdded2EvaluationQueue(SourceCodeFragment<Base>& code) {
            const std::vector<Argument<Base> >& args = code.arguments_;

            typename std::vector<Argument<Base> >::const_iterator it;

            for (it = args.begin(); it != args.end(); ++it) {
                if (it->operation() != NULL) {
                    SourceCodeFragment<Base>& arg = *it->operation();
                    if (arg.getEvaluationOrder() == 0) {
                        arg.setEvaluationOrder(code.getEvaluationOrder());
                        dependentAdded2EvaluationQueue(arg);
                    }
                }
            }
        }

        inline bool isIndependent(const SourceCodeFragment<Base>& arg) const {
            size_t id = arg.variableID();
            return id > 0 && id <= _independentVariables.size();
        }

        inline bool isTemporary(const SourceCodeFragment<Base>& arg) const {
            return arg.variableID() >= _minTemporaryVarID;
        }

        /**
         * Reduces the number of operations
         */
        inline void optimizeOperationGraph() throw (CGException) {
            _variableOrderOpt.reserve(_variableOrder.size());
            _codeBlocksOpt.reserve(_codeBlocks.size());

            typename std::vector<SourceCodeFragment<Base> *>::const_iterator it;
            for (it = _variableOrder.begin(); it != _variableOrder.end(); ++it) {
                SourceCodeFragment<Base>* opOpt = optimizeOperationGraph(**it);
                _variableOrderOpt.push_back(opOpt);
                _regular2OptcodeBlocks[*it] = opOpt;
            }
        }

        /**
         * Reduces the number of operations
         * \param op graph operation node to optimize
         * \return  The new optimized graph operation node
         */
        inline SourceCodeFragment<Base>* optimizeOperationGraph(SourceCodeFragment<Base>& op) throw (CGException) {
            if (op.variableID() > 0) {
                if (isTemporary(op)) {
                    typename std::map<SourceCodeFragment<Base> *, SourceCodeFragment<Base> *>::const_iterator it = _regular2OptcodeBlocks.find(&op);
                    if (it != _regular2OptcodeBlocks.end()) {
                        // already done
                        return it->second;
                    }
                } else if (isIndependent(op)) {
                    return &op; // independent variable (the originals can be used)
                }
            }

            const std::vector<Argument<Base> >& args = op.arguments();

            /**
             * optimize the arguments
             */
            std::vector<Argument<Base> > argsOpt(args.size());
            for (size_t i = 0; i < args.size(); i++) {
                const Argument<Base>* arg = &args[i];
                while (arg->operation() != NULL && arg->operation()->operationCode() == CGAliasOp) {
                    arg = &arg->operation()->arguments()[0];
                }

                if (arg->operation() != NULL) {
                    argsOpt[i] = Argument<Base > (*optimizeOperationGraph(*arg->operation()));
                } else {
                    argsOpt[i] = Argument<Base > (*arg->parameter());
                }
            }

            /**
             * optimize op
             */
            SourceCodeFragment<Base>* opOpt = NULL;

            CGOpCode opCode = op.operationCode();

            if (op.operationCode() == CGAddOp || op.operationCode() == CGSubOp) {
                opOpt = optimizeOperationGraphAddSub(op, argsOpt);
            } else if (op.operationCode() == CGDivOp || op.operationCode() == CGMulOp) {
                opOpt = optimizeOperationGraphDivMul(op, argsOpt);
            }

            if (opOpt == NULL) {
                std::vector<CGOpCodeExtra> operationInfo;
                opOpt = new SourceCodeFragment<Base > (opCode,
                        argsOpt,
                        operationInfo,
                        op);
                _codeBlocksOpt.push_back(opOpt); // for memory management
            }

            return opOpt;
        }

        /**
         * Combines and sorts addition/subtraction operations
         * 
         * \param op The operation to be optimized
         * \param argsOpt The optimized arguments for this operation
         * \return the optimized operation
         */
        inline SourceCodeFragment<Base>* optimizeOperationGraphAddSub(SourceCodeFragment<Base>& op,
                const std::vector<Argument<Base> >& argsOpt) throw (CGException) {
            CGOpCode opCode = op.operationCode();
            SourceCodeFragment<Base>* left = argsOpt[0].operation();
            SourceCodeFragment<Base>* right = argsOpt[1].operation();

            if (left != NULL && left->variableID() == 0 && left->operationCode() == CGAddSubOp) {
                // the left operation is already a combined addition/subtraction: just include this new operation there

                left->operationInfo_.push_back(opCode == CGAddOp ? CGExtraAddOp : CGExtraSubOp);
                if (right != NULL && right->variableID() == 0 && right->operationCode() == CGAddSubOp) {
                    // the right operation is also an addition/subtraction: merge operations
                    if (opCode == CGAddOp) {
                        // a + (b - c)  ->  a + b - c
                        left->operationInfo_.insert(left->operationInfo_.end(), right->operationInfo_.begin(), right->operationInfo_.end());
                    } else {
                        // a - (b - c)  ->  a - b + c
                        left->operationInfo_.reserve(left->operationInfo_.size() + right->operationInfo_.size());
                        for (std::vector<CGOpCodeExtra>::const_iterator it = right->operationInfo_.begin(); it != right->operationInfo_.end(); ++it) {
                            left->operationInfo_.push_back(*it == CGExtraAddOp ? CGExtraSubOp : CGExtraAddOp);
                        }
                    }
                    left->arguments_.insert(left->arguments_.end(), right->arguments_.begin(), right->arguments_.end());
                } else {
                    left->arguments_.push_back(argsOpt[1]);
                }

                left->cloneInfo(op);

                return left;

            } else if (right != NULL && right->variableID() == 0 && right->operationCode() == CGAddSubOp) {
                // the right operation is already a combined addition/subtraction: just include this new operation there

                if (opCode == CGSubOp) {
                    // a - (b - c)  ->  a - b + c
                    for (std::vector<CGOpCodeExtra>::iterator it = right->operationInfo_.begin(); it != right->operationInfo_.end(); ++it) {
                        *it = (*it == CGExtraAddOp) ? CGExtraSubOp : CGExtraAddOp;
                    }
                }

                right->operationInfo_.insert(right->operationInfo_.begin(), opCode == CGSubOp ? CGExtraSubOp : CGExtraAddOp);
                right->arguments_.insert(right->arguments_.begin(), argsOpt[0]);

                right->cloneInfo(op);

                return right;

            } else {
                std::vector<CGOpCodeExtra> operationInfo(1);
                operationInfo[0] = opCode == CGAddOp ? CGExtraAddOp : CGExtraSubOp;

                SourceCodeFragment<Base>* opOpt = new SourceCodeFragment<Base > (CGAddSubOp,
                        argsOpt,
                        operationInfo,
                        op);
                _codeBlocksOpt.push_back(opOpt); // for memory management

                return opOpt;
            }
        }

        /**
         * Combines and sorts division/multiplication operations
         * 
         * \param op The operation to be optimized
         * \param argsOpt The optimized arguments for this operation
         * \return the optimized operation
         */
        inline SourceCodeFragment<Base>* optimizeOperationGraphDivMul(SourceCodeFragment<Base>& op,
                const std::vector<Argument<Base> >& argsOpt) throw (CGException) {
            CGOpCode opCode = op.operationCode();
            SourceCodeFragment<Base>* left = argsOpt[0].operation();
            SourceCodeFragment<Base>* right = argsOpt[1].operation();

            if (left != NULL && left->variableID() == 0 && left->operationCode() == CGDivMulOp) {
                // the left operation is already a combined division/multiplication: just include this new operation there

                left->operationInfo_.push_back(opCode == CGDivOp ? CGExtraDivOp : CGExtraMulOp);
                if (right != NULL && right->variableID() == 0 && right->operationCode() == CGDivMulOp) {
                    // the right operation is also a division/multiplication: merge operations 

                    if (opCode == CGMulOp) {
                        // a * (b / c)  ->  a * b / c
                        left->operationInfo_.insert(left->operationInfo_.end(), right->operationInfo_.begin(), right->operationInfo_.end());
                    } else {
                        // a / (b / c)  ->  a / b * c
                        left->operationInfo_.reserve(left->operationInfo_.size() + right->operationInfo_.size());
                        for (std::vector<CGOpCodeExtra>::const_iterator it = right->operationInfo_.begin(); it != right->operationInfo_.end(); ++it) {
                            left->operationInfo_.push_back((*it == CGExtraMulOp) ? CGExtraDivOp : CGExtraMulOp);
                        }
                    }

                    left->arguments_.insert(left->arguments_.end(), right->arguments_.begin(), right->arguments_.end());
                } else {
                    left->arguments_.push_back(argsOpt[1]);
                }

                left->cloneInfo(op);

                return left;

            } else if (right != NULL && right->variableID() == 0 && right->operationCode() == CGDivMulOp) {
                // the right operation is already a combined division/multiplication: just include this new operation there

                if (opCode == CGDivOp) {
                    // a / (b / c)  ->  a / b / c
                    for (std::vector<CGOpCodeExtra>::iterator it = right->operationInfo_.begin(); it != right->operationInfo_.end(); ++it) {
                        *it = (*it == CGExtraMulOp) ? CGExtraDivOp : CGExtraMulOp;
                    }
                }

                right->operationInfo_.insert(right->operationInfo_.begin(), opCode == CGDivOp ? CGExtraDivOp : CGExtraMulOp);
                right->arguments_.insert(right->arguments_.begin(), argsOpt[0]);

                right->cloneInfo(op);

                return right;

            } else {
                std::vector<CGOpCodeExtra> operationInfo(1);
                operationInfo[0] = opCode == CGDivOp ? CGExtraDivOp : CGExtraMulOp;

                SourceCodeFragment<Base>* opOpt = new SourceCodeFragment<Base > (CGDivMulOp,
                        argsOpt,
                        operationInfo,
                        op);
                _codeBlocksOpt.push_back(opOpt); // for memory management

                return opOpt;
            }
        }

    private:

        CodeHandler(const CodeHandler&); // not implemented

        CodeHandler& operator=(const CodeHandler&); // not implemented

        friend class CG<Base>;

    };

}
#endif

