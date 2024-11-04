#include "quanSystem.h"
namespace QuanSystem
{


int GetLastFuncType(mu::ParserByteCode* m_vRPN)
{
    using namespace mu;
    const SToken* pTok = m_vRPN->GetLastBase();
    while(pTok->Cmd == cmEND) pTok--;
    switch (pTok->Cmd)
	{
    case  cmEQ:
        return funcType::EQ;
    case  cmASSIGN:
        return funcType::EQ;
    case  cmLE:
        return funcType::LEQ;
    case  cmGE:
        return funcType::GEQ;
    case  cmLT:
        return funcType::LE;
    case  cmGT:
        return funcType::GE;

    default:
		return funcType::UNKNOWN;
	}
    return funcType::UNKNOWN;
}

Value ParseCMDPointer(ParamVec &params, void* oper, void* userdata)
{
    using namespace mu;
    ParserByteCode* m_vRPN = (ParserByteCode*)oper;

    // Note: The check for nOffset==0 and nThreadID here is not necessary but
    //       brings a minor performance gain when not in bulk mode.
    int stack_size = (int)m_vRPN->GetMaxStackSize();
    value_type* stack = new value_type[stack_size * 16]();
    int m_nFinalResultIdx = m_vRPN->GetFinalResultIdx();
    std::vector<string_type> m_vStringBuf;
    string_type expr;
    std::tie(expr, m_vStringBuf) = m_vRPN->RestoreEnvironment();
    value_type buf;
    int sidx(0);
    int nOffset = 0;
    int nThreadID = 0;
    for (const SToken * pTok = m_vRPN->GetBase(); pTok->Cmd != cmEND; ++pTok)
    {
        switch (pTok->Cmd)
		{
        // built in binary operators
        //modifyed for inequality operators
        case  cmLE:
            --sidx;
            stack[sidx] = stack[sidx] - stack[sidx + 1];
            continue;
        case  cmGE:
            --sidx;
            stack[sidx] = stack[sidx] - stack[sidx + 1];
            continue;
        case  cmNEQ:
            --sidx;
            stack[sidx] = stack[sidx] != stack[sidx + 1];
            continue;
        case  cmEQ:
            --sidx;
            stack[sidx] = stack[sidx] - stack[sidx + 1];
            continue;
        case  cmLT:
            --sidx;
            stack[sidx] = stack[sidx] - stack[sidx + 1];
            continue;
        case  cmGT:
            --sidx;
            stack[sidx] = stack[sidx] - stack[sidx + 1];
            continue;
        case  cmADD:
            --sidx;
            stack[sidx] += stack[1 + sidx];
            continue;
        case  cmSUB:
            --sidx;
            stack[sidx] -= stack[1 + sidx];
            continue;
        case  cmMUL:
            --sidx;
            stack[sidx] *= stack[1 + sidx];
            continue;
        case  cmDIV:
            --sidx;
            stack[sidx] /= stack[1 + sidx];
            continue;

        case  cmPOW:
            --sidx;
            stack[sidx] = MathImpl<value_type>::Pow(stack[sidx], stack[1 + sidx]);
            continue;

        case  cmLAND:
            --sidx;
            stack[sidx] = stack[sidx] && stack[sidx + 1];
            continue;
        case  cmLOR:
            --sidx;
            stack[sidx] = stack[sidx] || stack[sidx + 1];
            continue;

        case  cmASSIGN:

            --sidx;
            stack[sidx] -= stack[1 + sidx];
            continue;

        case  cmIF:
            if (stack[sidx--] == 0)
			{
                MUP_ASSERT(sidx >= 0);
                pTok += pTok->Oprt.offset;
            }
            continue;

        case  cmELSE:
            pTok += pTok->Oprt.offset;
            continue;

        case  cmENDIF:
            continue;

        // value and variable tokens
        case  cmVAR:
            stack[++sidx] = *(params[pTok->Val.dataIdx]);
            continue;
        case  cmVAL:
            stack[++sidx] = pTok->Val.data2;
            continue;

        case  cmVARPOW2:
            buf = *(params[pTok->Val.dataIdx]);
            stack[++sidx] = buf * buf;
            continue;

        case  cmVARPOW3:
            buf = *(params[pTok->Val.dataIdx]);
            stack[++sidx] = buf * buf * buf;
            continue;

        case  cmVARPOW4:
            buf = *(params[pTok->Val.dataIdx]);
            stack[++sidx] = buf * buf * buf * buf;
            continue;

        case  cmVARMUL:
            stack[++sidx] = *(params[pTok->Val.dataIdx]) * pTok->Val.data + pTok->Val.data2;
            continue;

        // Next is treatment of numeric functions
        case  cmFUNC:
        {
            int iArgCount = pTok->Fun.argc;

            // switch according to argument count
            switch (iArgCount)
            {
            case 0:
                sidx += 1;
                stack[sidx] = pTok->Fun.cb.call_fun<0 >();
				continue;
            case 1:
                stack[sidx] = pTok->Fun.cb.call_fun<1 >(stack[sidx]);
				continue;
            case 2:
                sidx -= 1;
                stack[sidx] = pTok->Fun.cb.call_fun<2 >(stack[sidx], stack[sidx + 1]);
				continue;
            case 3:
                sidx -= 2;
                stack[sidx] = pTok->Fun.cb.call_fun<3 >(stack[sidx], stack[sidx + 1], stack[sidx + 2]);
				continue;
            case 4:
                sidx -= 3;
                stack[sidx] = pTok->Fun.cb.call_fun<4 >(stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3]);
                continue;
            case 5:
                sidx -= 4;
                stack[sidx] = pTok->Fun.cb.call_fun<5 >(stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3], stack[sidx + 4]);
                continue;
            case 6:
                sidx -= 5;
                stack[sidx] = pTok->Fun.cb.call_fun<6 >(stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3], stack[sidx + 4], stack[sidx + 5]);
                continue;
            case 7:
                sidx -= 6;
                stack[sidx] = pTok->Fun.cb.call_fun<7 >(stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3], stack[sidx + 4], stack[sidx + 5], stack[sidx + 6]);
                continue;
            case 8:
                sidx -= 7;
                stack[sidx] = pTok->Fun.cb.call_fun<8 >(stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3], stack[sidx + 4], stack[sidx + 5], stack[sidx + 6], stack[sidx + 7]);
                continue;
            case 9:
                sidx -= 8;
                stack[sidx] = pTok->Fun.cb.call_fun<9 >(stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3], stack[sidx + 4], stack[sidx + 5], stack[sidx + 6], stack[sidx + 7], stack[sidx + 8]);
                continue;
            case 10:
                sidx -= 9;
                stack[sidx] = pTok->Fun.cb.call_fun<10>(stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3], stack[sidx + 4], stack[sidx + 5], stack[sidx + 6], stack[sidx + 7], stack[sidx + 8], stack[sidx + 9]);
                continue;
            default:
                // function with variable arguments store the number as a negative value
                if (iArgCount > 0)
                    throw ParserError(ecINTERNAL_ERROR, -1, _T(""));

                sidx -= -iArgCount - 1;

                // <ibg 2020-06-08> From oss-fuzz. Happend when Multiarg functions and if-then-else are used incorrectly.
                // Expressions where this was observed:
                //		sum(0?1,2,3,4,5:6)			-> fixed
                //		avg(0>3?4:(""),0^3?4:(""))
                //
                // The final result normally lieas at position 1. If sixd is smaller there is something wrong.
                if (sidx <= 0)
                    throw ParserError(ecINTERNAL_ERROR, -1, _T(""));
                // </ibg>

                stack[sidx] = pTok->Fun.cb.call_multfun(&stack[sidx], -iArgCount);
                continue;
            }
        }

        // Next is treatment of string functions
        case  cmFUNC_STR:
        {
            sidx -= pTok->Fun.argc - 1;

            // The index of the string argument in the string table
            int iIdxStack = pTok->Fun.idx;
            if (iIdxStack < 0 || iIdxStack >= (int)m_vStringBuf.size())
                ParserError(ecINTERNAL_ERROR, 0, _T(""));

            switch (pTok->Fun.argc)  // switch according to argument count
			{
            case 0:
                stack[sidx] = pTok->Fun.cb.call_strfun<1>(m_vStringBuf[iIdxStack].c_str());
                continue;
            case 1:
                stack[sidx] = pTok->Fun.cb.call_strfun<2>(m_vStringBuf[iIdxStack].c_str(), stack[sidx]);
                continue;
            case 2:
                stack[sidx] = pTok->Fun.cb.call_strfun<3>(m_vStringBuf[iIdxStack].c_str(), stack[sidx], stack[sidx + 1]);
                continue;
            case 3:
                stack[sidx] = pTok->Fun.cb.call_strfun<4>(m_vStringBuf[iIdxStack].c_str(), stack[sidx], stack[sidx + 1], stack[sidx + 2]);
                continue;
            case 4:
                stack[sidx] = pTok->Fun.cb.call_strfun<5>(m_vStringBuf[iIdxStack].c_str(), stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3]);
                continue;
            case 5:
                stack[sidx] = pTok->Fun.cb.call_strfun<6>(m_vStringBuf[iIdxStack].c_str(), stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3], stack[sidx + 4]);
                continue;
            }

            continue;
        }

        case  cmFUNC_BULK:
        {
            int iArgCount = pTok->Fun.argc;

            // switch according to argument count
            switch (iArgCount)
            {
            case 0:
                sidx += 1;
                stack[sidx] = pTok->Fun.cb.call_bulkfun<0 >(nOffset, nThreadID);
                continue;
            case 1:
                stack[sidx] = pTok->Fun.cb.call_bulkfun<1 >(nOffset, nThreadID, stack[sidx]);
                continue;
            case 2:
                sidx -= 1;
                stack[sidx] = pTok->Fun.cb.call_bulkfun<2 >(nOffset, nThreadID, stack[sidx], stack[sidx + 1]);
                continue;
            case 3:
                sidx -= 2;
                stack[sidx] = pTok->Fun.cb.call_bulkfun<3 >(nOffset, nThreadID, stack[sidx], stack[sidx + 1], stack[sidx + 2]);
                continue;
            case 4:
                sidx -= 3;
                stack[sidx] = pTok->Fun.cb.call_bulkfun<4 >(nOffset, nThreadID, stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3]);
                continue;
            case 5:
                sidx -= 4;
                stack[sidx] = pTok->Fun.cb.call_bulkfun<5 >(nOffset, nThreadID, stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3], stack[sidx + 4]);
                continue;
            case 6:
                sidx -= 5;
                stack[sidx] = pTok->Fun.cb.call_bulkfun<6 >(nOffset, nThreadID, stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3], stack[sidx + 4], stack[sidx + 5]);
                continue;
            case 7:
                sidx -= 6;
                stack[sidx] = pTok->Fun.cb.call_bulkfun<7 >(nOffset, nThreadID, stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3], stack[sidx + 4], stack[sidx + 5], stack[sidx + 6]);
                continue;
            case 8:
                sidx -= 7;
                stack[sidx] = pTok->Fun.cb.call_bulkfun<8 >(nOffset, nThreadID, stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3], stack[sidx + 4], stack[sidx + 5], stack[sidx + 6], stack[sidx + 7]);
                continue;
            case 9:
                sidx -= 8;
                stack[sidx] = pTok->Fun.cb.call_bulkfun<9 >(nOffset, nThreadID, stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3], stack[sidx + 4], stack[sidx + 5], stack[sidx + 6], stack[sidx + 7], stack[sidx + 8]);
                continue;
            case 10:
                sidx -= 9;
                stack[sidx] = pTok->Fun.cb.call_bulkfun<10>(nOffset, nThreadID, stack[sidx], stack[sidx + 1], stack[sidx + 2], stack[sidx + 3], stack[sidx + 4], stack[sidx + 5], stack[sidx + 6], stack[sidx + 7], stack[sidx + 8], stack[sidx + 9]);
                continue;
            default:
                throw ParserError(ecINTERNAL_ERROR, 2, _T(""));
            }
        }

        default:
            throw ParserError(ecINTERNAL_ERROR, 3, _T(""));
        } // switch CmdCode
    } // for all bytecode tokens
    double ret = stack[m_nFinalResultIdx];
    delete[] stack;
    return ret;
}

SubSystem::SubSystem(FuncVec& functions_) : m_functions(functions_), userdata(nullptr)
{
    m_inequal_num = m_Yvar_num = m_Xfix_num = 0;
    for (Func * curFun : m_functions)
	{
        if (curFun->func_type != funcType::EQ)
            m_inequal_num++;
        for (Param curVar : curFun->params)
		{
            if (m_varIdx.find(curVar) == m_varIdx.end())
            {
                m_varIdx[curVar] = m_Yvar_num;
                m_idxVar[m_Yvar_num] = curVar;
                ++m_Yvar_num;
			}
		}
	}
}

SubSystem::SubSystem(FuncVec& functions_, ParamVec& params_, int mode) : m_functions(functions_), userdata(nullptr)
{
    m_inequal_num = m_Yvar_num = m_Xfix_num = 0;
    int curIdx;
    for (Param p : params_)
	{
        if (mode == 0)
		{
            curIdx = m_Yvar_num++;
		}
        else
        {
            curIdx = --m_Xfix_num;
		}
        m_varIdx[p] = curIdx;
        m_idxVar[curIdx] = p;
	}
    for (Func * curFun : m_functions)
    {
        if (curFun->func_type != funcType::EQ)
            m_inequal_num++;
        for (Param curVar : curFun->params)
        {
            if (m_varIdx.find(curVar) == m_varIdx.end())
            {
                if (mode == 0)
                {
                    curIdx = --m_Xfix_num;
                }
                else
                {
                    curIdx = m_Yvar_num++;
                }
                m_varIdx[curVar] = curIdx;
                m_idxVar[curIdx] = curVar;
            }
		}
	}
}

int SubSystem::addYVar(Param& p)
{
    if (m_varIdx.find(p) == m_varIdx.end())
	{
        m_varIdx[p] = m_Yvar_num;
        m_idxVar[m_Yvar_num] = p;
        m_Yvar_num++;
	}
    else if (m_varIdx[p] >= 0) return 0;
    else
	{
        m_varIdx[m_idxVar[m_Xfix_num]] = m_varIdx[p];
        m_idxVar[m_varIdx[p]] = m_idxVar[m_Xfix_num];
        m_idxVar.erase(m_Xfix_num);
        m_Xfix_num++;
        m_varIdx[p] = m_Yvar_num;
        m_idxVar[m_Yvar_num] = p;
        m_Yvar_num++;
	}
    return 1;
}

int SubSystem::addXFix(Param& p)
{
    if (m_varIdx.find(p) == m_varIdx.end())
	{
        m_Xfix_num--;
        m_varIdx[p] = m_Xfix_num;
        m_idxVar[m_Xfix_num] = p;
	}
    else if (m_varIdx[p] < 0) return 0;
    else
	{
        m_Yvar_num--;
        m_varIdx[m_idxVar[m_Yvar_num]] = m_varIdx[p];
        m_idxVar[m_varIdx[p]] = m_idxVar[m_Yvar_num];
        m_idxVar.erase(m_Yvar_num);
        m_Xfix_num--;
        m_varIdx[p] = m_Xfix_num;
        m_idxVar[m_Xfix_num] = p;
	}
    return 1;
}

void SystemBase::addVar(const String& varname, Param& var)
{
    m_env->DefineVar(varname, var);
}
void SystemBase::addVar(const String& varname)
{
    Param var = new Value;
    m_env->DefineVar(varname, var);
}
void SystemBase::setVar(const String& varname, Value val)
{
    StringMapParam variables = getVar();
    if (variables.find(varname) == variables.end())
	{
        printf("Error: Can't find the variable in the system");
        return;
	}
    *(variables[varname]) = val;
}
void SystemBase::addConst(const String& constname, Value val)
{
    m_env->DefineConst(constname, val);
}


void SystemBase::addFuncExpression(const String& func_name, int func_type)
{
    Func* cur_func = new Func(func_name);
    mu::ParserByteCode* rpn = new mu::ParserByteCode(m_env->GetFuncRPN(func_name, cur_func->params));
    cur_func->oper = (void*)rpn;
    cur_func->pFunc = ParseCMDPointer;
    if (func_type == -1)
        cur_func->func_type = GetLastFuncType(rpn);
    else cur_func->func_type = func_type;
    if (cur_func->func_type == -1)
	{
        printf("error func\n");
        throw std::runtime_error("Invalid input in curve.");
	}
    cur_func->funcOperUserType = 0;
    m_functions.push_back(cur_func);
}

void SystemBase::addFuncExpression(String& func_name, PointerFunc& p_func, ParamVec& params, int func_type, void* oper, void* udata)
{
    Func* cur_func = new Func(func_name);
    cur_func->pFunc = p_func;
    cur_func->oper = oper;
    cur_func->func_type = func_type;
    cur_func->userdata = udata;
    cur_func->params = params;
    m_functions.push_back(cur_func);
}

void SystemBase::freeFuncExpressions()
{
    for (Func * cur_func : m_functions)
	{
        delete cur_func;
	}
    m_functions.clear();
}

double DisPointToLine(RPoint& pt, RPoint& start, RPoint& end)
{
    double x0 = pt.x, x1 = start.x, x2 = end.x;
    double y0 = pt.y, y1 = start.y, y2 = end.y;
    double dx = x2 - x1;
    double dy = y2 - y1;
    double d = sqrt(dx * dx + dy * dy);  // line length
    double area = fabs(-x0 * dy + y0 * dx + x1 * y2 - x2 * y1);
    return area / d;
}
static double MaxDisToLine(RPointListIter&start, RPointListIter&end, RPointListIter&ft)
{
    double max_dist = 0;
    double x1 = start->x, x2 = end->x;
    double y1 = start->y, y2 = end->y;
    double dx = x2 - x1;
    double dy = y2 - y1;
    double d = sqrt(dx * dx + dy * dy);  // line length
    for (RPointListIter it = start; it != end;  it++)
	{
        double cur_dis = fabs(-it->x * dy + it->y * dx + x1 * y2 - x2 * y1) / d;
        if (cur_dis > max_dist)
        {
            max_dist = cur_dis;
            ft = it;
		}
	}
    return max_dist;
}



inline int disBetweenIter(RPointListIter&start, RPointListIter&end)
{
    int res = 0;
    for (std::list<RPoint>::iterator it = start; it != end; it++)
	{
        res++;
	}
    return res;
}
int SystemBase::saveTestPoint(RPointListIter&start, RPointListIter&end, RPointList &potVec)
{
    if (disBetweenIter(start, end) < 2)
        return 0;
    RPointListIter save = start;
    if (MaxDisToLine(start, end, save) < MinDistThreshold)
	{
        RPointListIter delStart = start;
        delStart++;
        potVec.erase(delStart, end);
		return 0;
	}
    else
    {
        saveTestPoint(start, save, potVec);
        saveTestPoint(save, end, potVec);
        return 1;
    }
    return 0;
}

void SystemBase::generate2DTestPoint(SubSystem* subsys, Param& xparam, Param& yparam, RPointListIter& start, RPointListIter& end, RPointList& potVec, Param& dependent_param)
{
    double dependent_start = start->dependent;
    double dependent_end = end->dependent;
    int kcount = 0;
    double a_step = SampleStep;
    if ((dependent_end - dependent_start) < a_step * SampleCount)
	{
        a_step = (dependent_end - dependent_start) / (SampleCount + 1);
        kcount = SampleCount;
    }
    else
    {
        kcount = (int)round(((dependent_end - dependent_start)) / a_step) - 1;
        a_step = (dependent_end - dependent_start) / (kcount + 1);
    }
    for (int i = 1; i <= kcount; i++)
    {
        *dependent_param = dependent_start + i * a_step;
        if (solveSub(subsys) != 1)
        {
            std::cout << "Discontinuous smaple point failed" << std::endl;
            exit(0);
        };
        potVec.insert(end, RPoint{ *dependent_param, *xparam, *yparam});
	}
}


int SystemBase::sample2DEndPoint(SubSystem* subsys, RPointList& potlist, Param &xcoord, Param &ycoord, Interval& dependent_interval, Param &dependent_param)
{
    int findPoint = 0;
    int sampleTCount = 1;
    int sampleBase = (int)(SampleCount + 1);
    while (!findPoint)
	{
        sampleTCount *= sampleBase;
        double interval = (dependent_interval.end - dependent_interval.start) / sampleTCount;
        for (int diffIdx = 0; diffIdx * 2 <= sampleTCount; diffIdx++)
        {
            *(dependent_param) = dependent_interval.start + diffIdx * interval;
            if (solveSub(subsys))
            {
                findPoint = 1;
                m_initialSet = 1;
                potlist.push_front(RPoint{*dependent_param, *xcoord, *ycoord });
            }

            *(dependent_param) = dependent_interval.end - diffIdx * interval;
            if (solveSub(subsys))
            {
                findPoint = 1;
                m_initialSet = 1;
                potlist.push_back(RPoint{ *dependent_param, *xcoord, *ycoord });
			}

            if(findPoint) break;
		}
        if (interval < SampleStep)
            break;
    }

    if (findPoint == 0)
    {
        return 0;
	}

    auto binarySearch2DEndPoint = [&](int direct)
	{
        double dependent_start = potlist.front().dependent;
        double dependent_end = dependent_interval.start;
        RPoint lastPoint = potlist.front();
        int update = 0;
        if (direct == 1)
        {
            dependent_start = potlist.back().dependent;
            dependent_end = dependent_interval.end;
            lastPoint = potlist.back();
		}
        while (fabs(dependent_start - dependent_end) > MinEndPointDiff)
        {
            double dependent_mid = (dependent_start + dependent_end) / 2;
            *(dependent_param) = dependent_mid;
            if (solveSub(subsys))
            {
                lastPoint = RPoint{ *dependent_param, *xcoord, *ycoord };
                update = 1;
                //if (fabs(xmid - lastPoint.x) > step) {
                //	if (direct == 0) {
                //		potlist.push_front(lastPoint);
                //	}
                //	else if (direct == 1) {
                //		potlist.push_back(lastPoint);
                //	}
                //}
                dependent_start = dependent_mid;
            }
            else
            {
                dependent_end = dependent_mid;
            }
		}
        if (update && direct == 0)
        {
            potlist.push_front(lastPoint);
		}
        else if (update && direct == 1)
        {
            potlist.push_back(lastPoint);
        }
    };

    binarySearch2DEndPoint(0);
    binarySearch2DEndPoint(1);

    return 1;
}


RPointList SystemBase::sample2DPointBasedFunc(FuncVec& functions, LocalArea& xyarea, Param xcoord, Param ycoord, Interval& dependent_interval, Param dependent_param)
{
    ParamVec YParamlist;
    if (dependent_param == xcoord)
    {
        YParamlist.emplace_back(ycoord);
	}
    else if (dependent_param == ycoord)
	{
        YParamlist.emplace_back(xcoord);
    }
    else
    {
        YParamlist.emplace_back(xcoord);
        YParamlist.emplace_back(ycoord);
	}

    SubSystem subsys(functions, YParamlist);
    subsys.setVarLB(xcoord, xyarea.x_interval.start);
    subsys.setVarLB(xcoord, xyarea.x_interval.end);
    subsys.setVarLB(ycoord, xyarea.y_interval.start);
    subsys.setVarUB(ycoord, xyarea.y_interval.end);

    RPointList potList;
    if (!sample2DEndPoint(&subsys, potList, xcoord, ycoord, dependent_interval, dependent_param))
	{
        std::cout << "Sample2DEndPoint failed" << std::endl;
        return potList;
	}

    for (RPointListIter it = potList.begin(); it != potList.end(); )
	{
        RPointListIter it_next = it;
        it_next++;
        if (it_next == potList.end()) break;
        generate2DTestPoint(&subsys, xcoord, ycoord, it, it_next, potList, dependent_param);
        if (!saveTestPoint(it, it_next, potList))
        {
            it++;
        }
    }
    m_initialSet = 0;
    return potList;
}

RPointList SystemBase::sample2DPoint(LocalArea& xyarea, Param xcoord, Param ycoord, Interval& dependent_interval, Param dependent_param)
{
    return sample2DPointBasedFunc(m_functions, xyarea, xcoord, ycoord, dependent_interval, dependent_param);
}

RPointList SystemBase::sample2DPoint(LocalArea& xyarea, Param xcoord, Param ycoord)
{
    return sample2DPointBasedFunc(m_functions, xyarea, xcoord, ycoord, xyarea.x_interval, xcoord);
}
RPointList SystemBase::sample2DPoint(LocalArea& xyarea, Param xcoord, Param ycoord, Interval& dependent_interval)
{
    return sample2DPointBasedFunc(m_functions, xyarea, xcoord, ycoord, dependent_interval, xcoord);
}
}

