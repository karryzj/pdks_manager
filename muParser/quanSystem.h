#include"util.h"

namespace QuanSystem {
    Value ParseCMDPointer(ParamVec &params, void* oper, void* userdata);
    using PointerFunc = Value(*)(ParamVec &params, void *oper, void* userdata);

    enum funcType {
        UNKNOWN = -1,
        EQ = 0,
        LEQ = 1,
        GEQ = 2,
        LE =3,
        GE =4,
    };
    
    //struct miniFunc {

    //    std::vector<miniFunc*> gradFunc;

    //    miniFunc() : pFunc(nullptr), oper(nullptr), userdata(nullptr) {};
    //    Value callFunc(ParamVec & param_vec) {return pFunc(param_vec, oper, userdata);}
    //};

    struct Func {
        String description;

        PointerFunc pFunc;
        ParamVec params;
        int func_type;
        void* oper;
        void* userdata;
        std::vector<Func*> gradFunc;
        int funcOperUserType;

        Func() { description.clear(); params.clear(); gradFunc.clear(); pFunc = nullptr; oper = userdata = nullptr; func_type = funcType::EQ; funcOperUserType = -1; }
        Func(String a_sExpr) { description = a_sExpr; params.clear(); gradFunc.clear(); pFunc =nullptr; oper=userdata = nullptr; func_type = funcType::EQ; funcOperUserType = -1;}
        Value callFunc() { return pFunc(params, oper, userdata); }
        Value callFunc(ParamVec& params_) { return pFunc(params_, oper, userdata); }
        ~Func() {
            if (funcOperUserType == 0) {
                mu::ParserByteCode *pbytecode = (mu::ParserByteCode*)oper;
                delete pbytecode;
                oper = pbytecode = nullptr;
                userdata = nullptr;
                for (Func* pFunc : gradFunc) {delete pFunc;}
                gradFunc.clear();
            }
        }
    };

    class SubSystem
    {
    public:
        SubSystem(FuncVec& functions_); 
        SubSystem(FuncVec& functions_, ParamVec& params_, int mode = 0); //0 for input Y params, 1 for input X params
        void* userdata;


        int addYVar(Param& p);
        int addXFix(Param& p);
       
        void setVarLB(Param param, Value value) { m_varGuessLB[param] = value; }
        void setVarUB(Param param, Value value) { m_varGuessUB[param] = value; }

        FuncVec m_functions;
        ParamMapID m_varIdx;
        IDMapParam m_idxVar;
        int m_inequal_num;
        int m_Yvar_num;
        int m_Xfix_num;
        ParamMapValue m_varGuessLB;
        ParamMapValue m_varGuessUB;
    private:
    };

    struct RPoint {
        Value dependent;
        Value x;
        Value y;
    };

    struct ParamInf {
        Param param;
        Value lb;
        Value ub;
        ParamInf(Param param_, Value lb_, Value ub_) : param(param_), lb(lb_), ub(ub_) {};
    };

    struct Interval {
        double start;
        double end;
        Interval(double start_, double end_) : start(start_), end(end_) {};
    };
    struct LocalArea {
        Interval x_interval;
        Interval y_interval;
        LocalArea(Interval x_interval_, Interval y_interval_) : x_interval(x_interval_), y_interval(y_interval_) {};
    };

    class API_EXPORT_CXX SystemBase {
    public:
        SystemBase() {
            m_env = new mu::Parser();
            m_functions.clear();
            m_initialSet = 0; 
        };
        SystemBase(mu::Parser *env_) {
            m_env = env_;
            m_functions.clear();
            m_initialSet = 0;
        };
        std::vector<Func*> m_functions;
        ~SystemBase() {
            freeFuncExpressions();
            m_env = nullptr;
        }


        void addVar(const String& varname, Param& var);
        void addVar(const String& varname);
        void setVar(const String& varname, Value val);
        void addConst(const String& constname, Value val);

        void addFuncExpression(const String& func_name, int func_type=-1);
        void addFuncExpression(String& func_name, PointerFunc &p_func, ParamVec &params, int func_type = 0, void* oper = nullptr, void* udata = nullptr);
        void freeFuncExpressions();
        //f(x,y)=0;
        //To be completed
        //current, for given dependent_param t, x=g(t), y=h(t), x should increased with t increased.
        RPointList sample2DPointBasedFunc(FuncVec& functions, LocalArea& xyarea, Param xcoord, Param ycoord, Interval& dependent_interval, Param dependent_param);
        RPointList sample2DPoint(LocalArea& xyarea, Param xcoord, Param ycoord, Interval& dependent_interval,Param dependent_param);
        RPointList sample2DPoint(LocalArea& xyarea, Param xcoord, Param ycoord);
        RPointList sample2DPoint(LocalArea& xyarea, Param xcoord, Param ycoord, Interval& dependent_interval);

        const StringMapParam& getVar() const
        {
            return m_env->GetVar();
        }

        int& getSimpleCount() {return SampleCount;}
        double& getMinDistThreshold() { return MinDistThreshold; }
        double& getMinEndPointDiff() { return MinEndPointDiff; }
        double& getSampleStep() { return SampleStep; }
        mu::Parser & getEnv() { return *m_env; }
    protected:
        mu::Parser* m_env;
        int m_initialSet;
        int SampleCount = 4;
        double MinDistThreshold = 0.01;
        double MinEndPointDiff = 0.01;
        double SampleStep=0.01;

    private:
        int saveTestPoint(RPointListIter &start, RPointListIter &end, RPointList& potVec);
        void generate2DTestPoint(SubSystem* subsys, Param &xparam, Param& yparam, RPointListIter &start, RPointListIter &end, RPointList& potlist, Param &dependent_params);
        int sample2DEndPoint(SubSystem* subsys, RPointList& potList, Param &xcoord, Param &ycoord, Interval& dependent_interval, Param &dependent_param);

        virtual int solveSub(SubSystem* pSubsys) {
            return -1;
        }

        virtual int solve()
        {
            return -1;
        }

        //some bugs to be issued
        virtual int solveSub(std::vector<Func*> &subFunctions) {
            return -1;
        }
        


    };
}
