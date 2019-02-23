#include "Statement.h"

#include "ScriptCode.h"

#include "BooleanVar.h"
#include "FloatVar.h"
#include "StringVar.h"
#include "FunctionVar.h"
#include "VectorVar.h"

#include "StreamAssignee.h"
#include "ScriptAssemblyHelper.h"

#include <sstream>

#include <iostream>

#include <set>

#include <functional>

void split(std::vector<std::shared_ptr<Statement>>& tokens, const std::set<std::string>& ops)
{
	for (auto i=tokens.begin();i!=tokens.end();++i)
	{
		if (ops.find((*i)->token.lexeme)!=ops.end() && (*i)->lhs==0 && (*i)->rhs==0)
		{
			std::shared_ptr<Statement> p;
			if (i==tokens.begin())
			{
				p = std::shared_ptr<Statement>(new Statement(
					std::shared_ptr<Statement>(new Statement(std::vector<std::shared_ptr<Statement>>(i+1, tokens.end()), true)),
					(*i)->token,
					0));
			}
			else
			{
				if (i + 1 == tokens.end())
				{
					p = std::shared_ptr<Statement>(new Statement(
						std::shared_ptr<Statement>(new Statement(std::vector<std::shared_ptr<Statement>>(tokens.begin(), i), true)),
						(*i)->token,
						0));
				}
				else
				{
					p = std::shared_ptr<Statement>(new Statement(
						std::shared_ptr<Statement>(new Statement(std::vector<std::shared_ptr<Statement>>(tokens.begin(), i), true)),
						(*i)->token,
						std::shared_ptr<Statement>(new Statement(std::vector<std::shared_ptr<Statement>>(i + 1, tokens.end()), true))));
				}
			}
			tokens.clear();
			tokens.push_back(p);
			break;
		}
	}
}

void split_reverse(std::vector<std::shared_ptr<Statement>>& tokens, const std::set<std::string>& ops)
{
	for (auto i=tokens.rbegin();i!=tokens.rend();++i)
	{
		if (ops.find((*i)->token.lexeme)!=ops.end() && (*i)->lhs==0 && (*i)->rhs==0)
		{
			std::shared_ptr<Statement> p;
			if (i==tokens.rbegin())
			{
				p = std::shared_ptr<Statement>(new Statement(
					std::shared_ptr<Statement>(new Statement(std::vector<std::shared_ptr<Statement>>(tokens.begin(), (i+1).base()), true)),
					(*i)->token,
					0));
			}
			else
			{
				if (i.base() == tokens.begin() + 1)
				{
					p = std::shared_ptr<Statement>(new Statement(
						std::shared_ptr<Statement>(new Statement(std::vector<std::shared_ptr<Statement>>(i.base(), tokens.end()), true)),
						(*i)->token,
						0));
				}
				else
				{
					p = std::shared_ptr<Statement>(new Statement(
						std::shared_ptr<Statement>(new Statement(std::vector<std::shared_ptr<Statement>>(tokens.begin(), (i + 1).base()), true)),
						(*i)->token,
						std::shared_ptr<Statement>(new Statement(std::vector<std::shared_ptr<Statement>>(i.base(), tokens.end()), true))));
				}
			}
			tokens.clear();
			tokens.push_back(p);
			break;
		}
	}
}

Statement::Statement(std::vector<std::shared_ptr<Statement>>& tokens, bool expression)
{
	/*std::map<std::string, int> depth;
	int i;
	for (i=0;i<tokens.size();++i)
	{
	if (tokens[i].lexeme.front()=='(' || tokens[i].lexeme.front()=='[' || tokens[i].lexeme.front()=='{')
	++depth[tokens[i].lexeme];
	if (tokens[i].lexeme.front()==')' || tokens[i].lexeme.front()==']' || tokens[i].lexeme.front()=='}')
	--depth[tokens[i].lexeme];
	for (std::map<std::string, int>::iterator it = depth.begin();it!=depth.end();++it) {
	if (it->second<0) {
	break;
	}
	}
	}*/

	/*for (auto i=tokens.begin();i!=tokens.end();++i)
	std::cout << (*i)->output();
	std::cout << std::endl;*/
	
	keyword = 0;

	if (tokens.size() == 0)
		return;

	if (tokens.size() == 1)
	{
		*this = *tokens.front();
		if (token.lexeme.compare("return") == 0)
			keyword = 1;
		tokens.erase(tokens.begin());
		return;
	}

	/*if (!expression) // was only necessary for the old script engine i believe
	{
		for (auto i=tokens.begin();i!=tokens.end();++i)
		{
			if ((*i)->token.lexeme.size())
			{
				if ((*i)->token.lexeme.front()=='.' && (*i)->keyword==0)
				{
					++i;
					if (i!=tokens.end())
					{
						if (isalpha((*i)->token.lexeme.front()) || (*i)->token.lexeme.front()=='_' && (*i)->keyword==0)
						{
							(*i)->token.lexeme.insert((*i)->token.lexeme.begin(), '"');
						}
					}
				}
			}
		}
	}*/

	auto fp = tokens.end();
	bool found = true;
	while (found)
	{
		int depth = 0;
		int max_depth = 0;
		found = false;
		for (auto i = tokens.begin(); i != tokens.end(); ++i)
		{
			if ((*i)->token.lexeme.size())
			{
				if ((*i)->token.lexeme.front() == '(' && (*i)->token.lexeme.back() != ')')
				{
					if (depth == 0)
						fp = i;
					++depth;
					if (depth > max_depth)
						max_depth = depth;
				}
				if ((*i)->token.lexeme.front()==')')
				{
					if (depth == 0)
						throw std::runtime_error("Unexpected ')'.");
					--depth;
					if (depth == 0) {
						if (fp != tokens.end()) {
							std::shared_ptr<Statement> p(new Statement(std::vector<std::shared_ptr<Statement>>(fp + 1, i), true));
							if (max_depth == 1)
								p->keyword = 5;
							if (fp!=tokens.begin())
								if ((*(fp - 1))->token.lexeme.compare("if")==0 || (*(fp - 1))->token.lexeme.compare("while")==0)
									p->keyword = 0;
							tokens.insert(tokens.erase(fp, i + 1), p);
							fp = tokens.end();
							found = true;
							break;
						}
					}
				}
			}
		}
		if (depth>0)
			throw std::runtime_error("Missing ')'.");
	}

	for (auto i=tokens.begin();i!=tokens.end();++i)
	{
		if ((*i)->token.lexeme.compare("if")==0 || (*i)->token.lexeme.compare("else")==0)
		{
			if ((*i)->token.lexeme.compare("if")==0)
				++i;
			if (i!=tokens.end())
			{
				if (i++!=tokens.end())
				{
					if ((*i)->token.lexeme.size())
					{
						if ((*i)->token.lexeme.front()!='{')
						{
							for (auto b=i;b!=tokens.end();++b)
							{
								if ((*b)->token.lexeme.front()==';')
								{
									std::shared_ptr<Statement> pBegin(new Statement(0, Token(), 0));
									pBegin->token.lexeme = '{';
									std::shared_ptr<Statement> pEnd(new Statement(0, Token(), 0));
									pEnd->token.lexeme = '}';
									tokens.insert(b+1, pEnd);
									tokens.insert(i, pBegin);
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	fp = tokens.end();
	found = true;
	while (found)
	{
		found = false;
		int depth = 0;
		for (auto i=tokens.begin();i!=tokens.end();++i)
		{
			if ((*i)->token.lexeme.size())
			{
				if ((*i)->token.lexeme.front()=='{')
				{
					if (i<fp)
						fp = i;
					++depth;
					found = true;
				}
				if ((*i)->token.lexeme.front()=='}')
				{
					if (depth == 0)
						throw std::runtime_error("Unexpected '}'.");
					--depth;
					if (depth == 0) {
						if (fp!=tokens.end()) {
							std::shared_ptr<Statement> p(new Statement(0, Token(), 0));
							p->code.reset(new ScriptCode(std::vector<std::shared_ptr<Statement>>(fp+1, i)));
							if (fp!=tokens.begin()) {
								if ((*(fp-1))->keyword==5) {
									p->lhs = *(fp-1);
									p->keyword = 4;
									tokens.insert(tokens.erase(fp-1, i+1), p);
									fp = tokens.end();
									break;
								}
							}
							tokens.insert(tokens.erase(fp, i+1), p);
							fp = tokens.end();
							break;
						}
					}
				}
			}
		}
		if (depth > 0)
			throw std::runtime_error("Missing '}'.");
	}

	found = true;
	while (found)
	{
		found=false;
		for (auto i = tokens.rbegin(); i != tokens.rend(); ++i)
		{
			if ((*i)->token.lexeme.size())
			{
				if ((*i)->token.lexeme.compare("while") == 0)
				{
					auto b = i.base();
					auto bs = i.base();
					std::shared_ptr<Statement> p(new Statement(0, Token(), 0));
					if (b != tokens.end())
					{
						p->lhs = *b;
						++b;
						if (b != tokens.end())
						{
							p->rhs = *b;
							p->keyword = 6;
							std::shared_ptr<Statement> semi_colon(new Statement(0, Token(), 0));
							semi_colon->token.lexeme = ';';
							tokens.insert(tokens.insert(tokens.erase(bs-1, bs+2), semi_colon), p);
							found=true;
							break;
						}
					}
				}
				if ((*i)->token.lexeme.compare("if") == 0)
				{
					auto b = i.base();
					auto bs = i.base();
					std::shared_ptr<Statement> p(new Statement(0, Token(), 0));
					if (b != tokens.end())
					{
						p->lhs = *b;
						++b;
						if (b != tokens.end())
						{
							++b;
							if (b != tokens.end())
							{
								++b;
								if (b != tokens.end())
								{
									if ((*(bs + 2))->token.lexeme.compare("else") == 0)
									{
										p->keyword = 3;
										p->rhs = std::shared_ptr<Statement>(new Statement(*(bs + 1), Token(), *(bs + 3)));
										//p->token.lexeme = "else"; <- this was silly
										std::shared_ptr<Statement> semi_colon(new Statement(0, Token(), 0));
										semi_colon->token.lexeme = ';';
										tokens.insert(tokens.insert(tokens.erase(bs - 1, bs + 4), semi_colon), p);
										found=true;
										break;
									}
								}
							}
							p->keyword = 2;
							p->rhs = *(bs + 1);
							std::shared_ptr<Statement> semi_colon(new Statement(0, Token(), 0));
							semi_colon->token.lexeme = ';';
							tokens.insert(tokens.insert(tokens.erase(bs - 1, bs + 2), semi_colon), p);
							found=true;
							break;
						}
					}
					tokens.erase(bs - 1, tokens.end());
					found=true;
					break;
				}
			}
		}
	}
	
	for (auto i = tokens.begin(); i != tokens.end(); ++i)
	{
		if ((*i)->token.lexeme.size())
		{
			if ((*i)->token.lexeme.front() == ';')
			{
				std::shared_ptr<Statement> p(new Statement(std::vector<std::shared_ptr<Statement>>(tokens.begin(), i), true));
				*this = *p;
				tokens.erase(tokens.begin(), i + 1);
				return;
			}
		}
	}

	//by this time you must have an isolated expression!

	if (expression)
	{

		if (tokens.front()->token.lexeme.size())
		{
			if (tokens.front()->token.lexeme.compare("return") == 0)
			{
				std::shared_ptr<Statement> p(new Statement(std::shared_ptr<Statement>(new Statement(std::vector<std::shared_ptr<Statement>>(tokens.begin()+1, tokens.end()), true)), Token(), 0));
				p->keyword = 1;
				tokens.erase(tokens.begin(), tokens.end());
				*this = *p;
				return;
			}
		}

		/*for (auto i=tokens.begin();i!=tokens.end();++i)
		{
		std::cout << (*i)->output();
		std::cout << std::endl;
		}*/

		std::set<std::string> ops;

		ops.insert("=");
		ops.insert("+=");
		ops.insert("-=");
		ops.insert("*=");
		ops.insert("/=");
		split(tokens, ops);

		ops.clear();
		ops.insert(",");
		split_reverse(tokens, ops);

		ops.clear();
		ops.insert("==");
		ops.insert("!=");
		ops.insert("<=");
		ops.insert(">=");
		ops.insert("<");
		ops.insert(">");
		split(tokens, ops);

		ops.clear();
		ops.insert("+");
		split(tokens, ops);

		ops.clear();
		ops.insert("-");
		split_reverse(tokens, ops);

		ops.clear();
		ops.insert("*");
		split(tokens, ops);

		ops.clear();
		ops.insert("/");
		split(tokens, ops);

		ops.clear();
		ops.insert("!");
		split(tokens, ops);

		ops.clear();
		ops.insert("&");
		split(tokens, ops);

		for (auto i=tokens.rbegin();i!=tokens.rend();++i)
		{
			if ((*i)->token.lexeme.compare(".")==0 && (*i)->lhs==0 && (*i)->rhs==0)
			{
				std::shared_ptr<Statement> p;
				if (i!=tokens.rbegin())
				{
					p = std::shared_ptr<Statement>(new Statement(
						std::shared_ptr<Statement>(new Statement(std::vector<std::shared_ptr<Statement>>(tokens.begin(), (i+1).base()), true)),
						(*i)->token,
						*i.base()));
				}
				tokens.insert(tokens.erase(tokens.begin(), i.base()+1), p);
				break;
			}
		}

		while(tokens.size()>1) {
			tokens[0] = std::shared_ptr<Statement>(new Statement(tokens[0], Token(), tokens[1]));
			if (tokens[1]->keyword == 5)
				tokens[0]->token.lexeme = "()";
			else
				tokens[0]->keyword = 7;
			tokens.erase(tokens.begin()+1);
		}

		if (tokens[0] != 0)
			*this = *tokens[0];
	}
	else
	{
		throw std::runtime_error("Missing ';'.");
	}
}

Statement::Statement(std::shared_ptr<Statement> l, const Token& o, std::shared_ptr<Statement> r) : lhs(l), rhs(r), token(o), keyword(0)
{
}

Statement::Statement(int key) : keyword(key)
{
}

Statement::~Statement(void)
{
}

void Statement::compile(ScriptCompile& comp)
{
	auto& ss = comp.ss;
	auto& sasm = comp.sasm;

	StreamAssignee<uint8_t> p(comp);
	StreamAssignee<uint8_t> po(comp);
	StreamAssignee<uint8_t> o(comp);
	StreamAssignee<uint8_t> dat8(comp);
	StreamAssignee<uint16_t> dat16(comp);
	StreamAssignee<uint32_t> dat32(comp);
	StreamAssignee<uint64_t> dat64(comp);
	StreamAssigneeRelative<int8_t> rel8(comp);
	StreamAssigneeRelative<int32_t> rel32(comp);

	ScriptCompileMemoryTarget target = comp.target;

	switch (keyword)
	{
	case 1://return
	{
		if (comp.proto == nullptr)
			throw std::runtime_error("'return' found outside of a function.");

		if (lhs != nullptr)
		{
			if (comp.proto->ret.GetSize() == 0)
				throw std::runtime_error("Return type is 'void'.");
			if (comp.proto->ret != lhs->getType(comp))
				throw std::runtime_error("Return type mismatch.");

			if (comp.proto->ret.size <= 8)
			{
				comp.target = ScriptCompileMemoryTarget();

				lhs->compile(comp);
			}
			else
			{
				throw std::runtime_error("Return types larger than 64 bits are not supported yet.");
			}
		}
		else
		{
			if (comp.proto->ret.size > 0)
				throw std::runtime_error("Function needs to return a value.");
		}

		if (comp.stack > 0)
		{
			// leave
			po = 0xc9;
		}
		else
		{
			// pop ebp
			ScriptCompileMemoryTarget ebp;
			ebp.regm = 0b101;
			sasm.Pop(ebp);
		}

		if (comp.proto->cc == CC_THISCALL)
		{
			// rets comp.proto->getParamsSize()
			po = 0xc2;
			dat16 = comp.proto->getParamsSize();
		}
		else
		{
			// rets
			po = 0xc3;
		}
	}
	return;
	case 2://if
	{
		comp.target = ScriptCompileMemoryTarget();
		lhs->compile(comp);

		// cmp
		po = 0x3d;
		dat32 = 0;

		// jz
		p = 0x0f;
		po = 0x84;
		rel32 = 0;

		comp.BeginScope();
		rhs->compile(comp);
		comp.EndScope();
	}
	return;
	case 3://if else
	{
		comp.target = ScriptCompileMemoryTarget();
		lhs->compile(comp);

		// cmp
		po = 0x3d;
		dat32 = 0;

		// jz
		p = 0x0f;
		po = 0x84;
		rel32 = 0;

		comp.BeginScope();

		rhs->lhs->compile(comp);

		// jmp
		p = 0xe9;
		rel32 = 0;

		comp.EndScope();

		comp.BeginScope();
		rhs->rhs->compile(comp);
		comp.EndScope();
	}
	return;
	case 4:
	{
		if (code)
		{
			comp.BeginScope();
			bool has_return = false;
			for (auto i = code->statements.begin(); i != code->statements.end(); ++i)
			{
				i->compile(comp);
				has_return |= i->hasReturn();
			}
			if (!has_return)
			{
				if (comp.proto->ret.GetSize() == 0)
					Statement(1).compile(comp);
				else
					throw std::runtime_error("Not all control paths return a value.");
			}
			comp.EndScope();
		}
	}
	return;
	//keyword 5 is reserved for single depth parentheses' for some odd reason
	//aka I used it to make parsing a bit easier for me
	case 6://while
	{
		comp.target = ScriptCompileMemoryTarget();
		lhs->compile(comp);

		// cmp
		po = 0x3d;
		dat32 = 0;

		// jz
		p = 0x0f;
		po = 0x84;
		rel32 = 0;

		size_t start = comp.ss.tellp();

		comp.BeginScope();
		rhs->compile(comp);

		comp.target = ScriptCompileMemoryTarget();
		lhs->compile(comp);

		// cmp
		po = 0x3d;
		dat32 = 0;

		// jnz
		p = 0x0f;
		po = 0x85;

		std::streamoff diff = start - comp.ss.tellp();
		diff -= sizeof(int32_t);

		rel32 = diff;
		comp.EndScope();
	}
	return;
	case 7://no real operator or keyword or something
	{
		if (rhs->keyword == 4)
		{
			if (rhs->code)
			{
				if (comp.proto)
					throw std::runtime_error("Function found inside function.");
				if (!comp.current_class)
					throw std::runtime_error("Function found outside class.");

				ScriptFunctionPrototype prototype;
				prototype.cc = CC_MICROSOFT_X64;
				prototype.ret = lhs->getType(comp);

				std::vector<std::string> parameter_names;

				std::function<void(const std::shared_ptr<Statement>&)> add_arg;
				add_arg = [&](const std::shared_ptr<Statement>& v)
				{
					if (v->token.lexeme.size() == 0 && v->keyword == 0)
						return;

					if (v->token.lexeme.size() > 0)
					{
						if (v->token.lexeme.front() == ',')
						{
							add_arg(v->lhs);
							add_arg(v->rhs);
							return;
						}
					}

					auto type = v->getType(comp);
					if (type.GetSize() != 0)
					{
						prototype.params.push_back(type);
						parameter_names.push_back(v->rhs->token.lexeme);
					}
				};
				add_arg(rhs->lhs);

				std::string func_name = lhs->token.lexeme;
				if (lhs->rhs)
					func_name = lhs->rhs->token.lexeme;

				ScriptFunctionCompileData function_comp;
				function_comp.class_ptr = comp.current_class;
				function_comp.name = func_name;
				function_comp.prototype = prototype;
				function_comp.parameter_names = parameter_names;
				function_comp.code = rhs;

				comp.function_code.push_back(function_comp);

				comp.current_class->AddFunction(func_name, prototype, nullptr);

				return;
			}
		}
		if (lhs->keyword == 7)
		{
			std::vector<std::string> tokens;
			std::function<void(const std::shared_ptr<Statement>&)> add_token;
			add_token = [&tokens, &add_token](const std::shared_ptr<Statement>& v)
			{
				if (v->keyword == 7)
				{
					add_token(v->rhs);
					add_token(v->lhs);
				}
				else
				{
					tokens.push_back(v->token.lexeme);
				}
			};
			add_token(lhs);

			bool dynamic = false;

			if (tokens.back().compare("dynamic") == 0)
			{
				dynamic = true;
				tokens.pop_back();
			}

			if (tokens.back().compare("class") == 0)
			{
				tokens.pop_back();

				if (comp.proto)
					throw std::runtime_error("'class' found inside function.");
				if (comp.current_class)
					throw std::runtime_error("'class' found inside class.");

				comp.SetClass(tokens.back());
				tokens.pop_back();

				if (!tokens.empty())
				{
					if (tokens.back().front() == ':')
					{
						auto parent = comp.classes.find(tokens.back().substr(1));
						if (parent == comp.classes.end())
							throw std::runtime_error("Cannot find class '" + tokens.back().substr(1) + "'.");
						comp.current_class->SetParent(parent->second);
					}
				}

				if (dynamic)
					comp.current_class->AddVirtualFunctionTable();

				rhs->compile(comp);

				comp.current_class.reset();

				return;
			}
		}
		if (comp.proto)
		{
			comp.PushVariable(rhs->token.lexeme, getType(comp));
			return;
		}
		if (comp.current_class)
		{
			comp.current_class->AddMember(rhs->token.lexeme, getType(comp));
			return;
		}
		if (lhs->token.lexeme.compare("include") == 0)
		{
			if (comp.proto)
				throw std::runtime_error("'include' found inside function.");
			if (comp.current_class)
				throw std::runtime_error("'include' found inside class.");

			//throw std::runtime_error("Cannot find '" + rhs->token.lexeme + "'.");
			throw std::runtime_error("'include' is not yet implemented.");
		}
	}
	return;
	default:
		break;
	}

	if (code)
	{
		comp.BeginScope();
		for (auto i = code->statements.begin(); i != code->statements.end(); ++i)
		{
			i->compile(comp);
		}
		comp.EndScope();
	}

	if (token.lexeme.size() > 0)
	{
		switch (token.lexeme.front())
		{
		case '(':
		{
			ScriptTypeData lhs_type = lhs->getType(comp);

			ScriptFunctionPrototype callee_proto;
			void * function_pointer = nullptr;

			if (lhs->keyword == 0)
			{
				if (comp.current_class)
				{
					for (auto& func : comp.current_class->functions)
					{
						if (func.first.compare(lhs->token.lexeme) == 0)
						{
							callee_proto = func.second.first;
							function_pointer = func.second.second;
						}
					}
				}
				auto lhs_type = lhs->getType(comp);
				if (lhs_type.function_prototype)
				{
					callee_proto = *lhs_type.function_prototype;
				}
			}

			std::vector<std::shared_ptr<Statement>> params;

			std::function<void(const std::shared_ptr<Statement>&)> add_arg;
			add_arg = [&params, &add_arg](const std::shared_ptr<Statement>& v)
			{
				if (v->token.lexeme.size() == 0 && v->keyword == 0)
					return;
				if (v->token.lexeme.front() == ',')
				{
					add_arg(v->lhs);
					add_arg(v->rhs);
				}
				else
				{
					params.push_back(v);
				}
			};
			add_arg(rhs);

			if (params.size() != callee_proto.params.size())
				throw std::runtime_error("Incorrect number of parameters.");

			std::vector<ScriptTypeData> params_type;
			params_type.resize(params.size());
			for (int i = 0; i < params.size(); ++i)
			{
				params_type[i] = params[i]->getType(comp);
				if (params_type[i] != callee_proto.params[i])
					throw std::runtime_error("Parameter type mismatch.");
			}

			size_t previous_stack = comp.stack;

			if (callee_proto.params.size() > 0)
			{
				/*if (callee_proto.params.size() == 1)
				{
				if (callee_proto.params.front().size == 4)
				{
				ScriptCompileMemoryTarget eax_target;
				comp.target = eax_target;
				rhs->compile(comp);
				}
				else
				{

				}
				}
				else*/
				{
					for (auto i = params.rbegin(); i != params.rend(); ++i)
					{
						ScriptCompileMemoryTarget eax_target;
						comp.target = eax_target;
						(*i)->compile(comp);
						sasm.Push(eax_target);
					}
				}
			}

			// call lhs (rel32)
			po = 0xe8;

			ScriptLinkData link;
			link.relative = true;
			link.location = ss.tellp();
			link.class_ptr = comp.current_class;
			link.function_name = lhs->token.lexeme;
			link.prototype = callee_proto;
			comp.links.push_back(link);

			dat32 = (char*)function_pointer - ((char*)comp.base_pointer + ss.tellp() + 4);

			if (callee_proto.cc != CC_THISCALL)
			{
				// add esp, params.size() * 4
				po = 0x81;
				o = 0b11000100;
				dat32 = params.size() * 4;
			}

			ScriptCompileMemoryTarget eax_target;
			if (target != eax_target)
				sasm.Move(0x89, target, eax_target);

			return;

			//ScriptCompileMemoryTarget esi_target;
			//esi_target.rm = 0b110;

			//comp.target = esi_target;
			//comp.target.lvalue = true;
			//lhs->compile(comp);

			//// call comp.target
			//sasm.Move(0xff, 2, comp.target);
		}
		break;
		default:
			break;
		}
	}

	if (lhs) {
		auto lhs_type = lhs->getType(comp);
		if (rhs) {
			auto rhs_type = lhs->getType(comp);
			if (lhs_type != rhs_type)
				throw std::runtime_error("Type mismatch.");
			switch (token.lexeme.front())
			{
			case '.':
			{
				auto lhs_type = lhs->getType(comp);
				if (lhs_type.type != ST_CLASS)
					throw std::runtime_error("'.' must be preceded by a class.");
				if (lhs_type.class_data)
				{
					for (auto func : lhs_type.class_data->functions)
					{
						if (rhs->token.lexeme.compare(func.first) == 0)
						{
							auto prototype = func.second.first;
							// TODO: implement thiscall
							return;
						}
					}
					for (auto member : lhs_type.class_data->members)
					{
						if (rhs->token.lexeme.compare(member.first) == 0)
						{
							comp.target = ScriptCompileMemoryTarget();
							comp.target.lvalue = true;
							
							lhs->compile(comp);

							comp.target.offset += member.second.target.offset;

							if (!target.lvalue)
							{
								if (target.mode != 0b11)
								{
									auto tmp_target = sasm.FindRegister({ target, comp.target });
									sasm.Move(0x8b, tmp_target, comp.target);
									sasm.Move(0x89, target, tmp_target);
								}
								else
								{
									sasm.Move(0x8b, target, comp.target);
								}
							}

							return;
						}
					}
					throw std::runtime_error("No member '" + rhs->token.lexeme + "' could be found in class '" + lhs->token.lexeme + "'."); // todo: change to class name probs
				}
				throw std::runtime_error("Class of '" + lhs->token.lexeme + "' could not be determined.");
			}
			case '=':
			{
				if (lhs->keyword == 7)
				{
					comp.PushVariable(lhs->rhs->token.lexeme, lhs_type);

					rhs->compile(comp);
				}
				else
				{
					comp.target.lvalue = true;
					lhs->compile(comp);
					rhs->compile(comp);
				}
			}
			break;
			case '+':
			{
				lhs->compile(comp);

				comp.target.lvalue = true;

				rhs->compile(comp);

				sasm.Move(0x01, target, comp.target);
			}
			break;
			case '-':
			{
				auto rhs_target = sasm.FindRegister(target);
				comp.target = rhs_target;

				rhs->compile(comp);

				sasm.Push(rhs_target);

				comp.target = target;

				lhs->compile(comp);

				sasm.Pop(rhs_target);

				sasm.Move(0x29, target, rhs_target);
			}
			break;
			case '*':
			{
				auto rhs_target = sasm.FindRegister(target);
				comp.target = rhs_target;

				rhs->compile(comp);

				sasm.Push(rhs_target);

				comp.target = target;

				lhs->compile(comp);

				sasm.Pop(rhs_target);

				if (target.mode == 0b11)
				{
					// imul target, rhs_target
					p = 0x0f;
					sasm.Move(0xaf, target, rhs_target);
				}
				else
				{
					// imul rhs_target, target
					p = 0x0f;
					sasm.Move(0xaf, rhs_target, target);

					// mov target, rhs_target
					sasm.Move(0x89, target, rhs_target);
				}
			}
			break;
			case '/':
			{
				ScriptCompileMemoryTarget eax_target;

				comp.target = eax_target;
				lhs->compile(comp);

				sasm.Push(eax_target);

				ScriptCompileMemoryTarget divisor_target = sasm.FindRegister({ target, eax_target });

				comp.target = divisor_target;
				rhs->compile(comp);

				sasm.Pop(eax_target);

				if (lhs_type.type == ST_INT)
				{
					// cdq
					po = 0x99;
				}
				else
				{
					// mov edx, 0
					po = 0xba;
					dat32 = 0;
				}

				// idiv divisor_target
				po = 0xf7;
				o = divisor_target.GetModRegRM(7);

				if (target.mode != 0b11 || target.regm != 0b000)
					sasm.Move(0x89, target, eax_target);
			}
			break;
			default:
			{
				throw std::runtime_error("Unexpected operand '" + token.lexeme + "'.");
			}
			break;
			}
		}
		else {
			switch (token.lexeme.front())
			{
			case '&':
			{
				comp.target.lvalue = true;
				lhs->compile(comp);

				if (target.mode == 0b11)
				{
					// lea target, comp.target
					sasm.Move(0x8d, comp.target, target);
				}
				else
				{
					ScriptCompileMemoryTarget eax_target;

					// lea eax, comp.target
					sasm.Move(0x8d, comp.target, eax_target);

					// mov target, eax
					sasm.Move(0x89, target, eax_target);
				}
			}
			break;
			case '*':
			{
				if (comp.target.lvalue)
				{
					ScriptCompileMemoryTarget eax_target;

					comp.target = eax_target;
					lhs->compile(comp);

					comp.target.mode = 0b01;
				}
				else
				{
					ScriptCompileMemoryTarget eax_target;

					comp.target = eax_target;
					lhs->compile(comp);

					comp.target.mode = 0b01;

					if (target.mode == 0b11)
					{
						// mov target, comp.target
						sasm.Move(0x8b, target, comp.target);
					}
					else
					{
						// mov eax, comp.target
						sasm.Move(0x8b, eax_target, comp.target);

						// mov target, eax
						sasm.Move(0x89, target, eax_target);
					}
				}
			}
			break;
			case '-':
			{
				lhs->compile(comp);
				sasm.Move(0xf7, 3, target);
			}
			break;
			default:
			{
				throw std::runtime_error("Unexpected operand '" + token.lexeme + "'.");
			}
			break;
			}
		}
	}
	else {
		if (rhs != 0) {
		}
		else {
			if (token.lexeme.size() != 0)
			{
				{
					std::istringstream ss(token.lexeme);
					uint64_t ui;
					ss >> ui;
					if (!ss.fail())
					{
						if (target.lvalue)
						{
							comp.target = sasm.FindRegister();
							uint8_t rex = 0b01001000;
							if (comp.target.regm > 7)
								rex |= 0b100;
							p = rex;
							po = 0xb8 + (comp.target.regm & 0b111);
							dat64 = ui;
						}
						else
						{
							if (target.mode == 0b11)
							{
								po = 0xb8 + target.regm;
								dat32 = ui;
							}
							else
							{
								sasm.Move(0xc7, 0, target);
								dat32 = ui;
							}
						}
						return;
					}
				}
				{
					std::istringstream ss(token.lexeme);
					float f;
					ss >> f;
					if (!ss.fail())
					{
						return;
					}
				}
				if (token.lexeme.front() == '"' || token.lexeme.front() == '\'')
				{
					return;
				}

				auto varData = comp.GetVariable(token.lexeme);

				ScriptCompileMemoryTarget var_target = varData.target;

				if (target.lvalue)
				{
					comp.target = var_target;
				}
				else
				{
					if (target == var_target)
					{
						// do anything here?
						// no?
					}
					else
					{
						if (target.mode == 0b11)
						{
							sasm.Move(0x8b, target, var_target);
						}
						else
						{
							if (var_target.mode == 0b11)
							{
								sasm.Move(0x89, target, var_target);
							}
							else
							{
								auto tmp_target = sasm.FindRegister({ target, var_target });
								sasm.Move(0x8b, tmp_target, var_target);
								sasm.Move(0x89, target, tmp_target);
							}
						}
					}
				}
			}
		}
	}
}

std::shared_ptr<Variable> Statement::run(const std::shared_ptr<ScriptMemory>& mem)
{
	switch (keyword)
	{
	case 1:
	{
		mem->return_var = lhs->run(mem);
		return 0; //return the return value here if you wanna do fancy stuff like "a = return b;"
	}
	break;
	case 2:
	{
		std::shared_ptr<BooleanVar> var = std::dynamic_pointer_cast<BooleanVar>(lhs->run(mem));
		if (var != 0)
		{
			if (var->b)
				rhs->run(mem);
		}
		return 0;
	}
	break;
	case 3:
	{
		std::shared_ptr<BooleanVar> var = std::dynamic_pointer_cast<BooleanVar>(lhs->run(mem));
		if (var != 0) {
			if (var->b)
				rhs->lhs->run(mem);
			else
				rhs->rhs->run(mem);
		}
		return 0;
	}
	break;
	case 4:
	{
		std::shared_ptr<FunctionVar> var(new FunctionVar());
		var->script_func.reset(new ScriptFunction());
		var->script_func->code = code;
		std::function<void(const std::shared_ptr<Statement>&)> add_arg;
		add_arg = [var, &add_arg](const std::shared_ptr<Statement>& v)
		{
			if (v->lhs == 0 && v->rhs == 0) {
				var->script_func->arg.push_back(v->token.lexeme);
				return;
			}
			if (v->lhs != 0)
				add_arg(v->lhs);
			if (v->rhs != 0)
				add_arg(v->rhs);
			return;
		};
		add_arg(lhs);
		return var;
	}
	break;
	//keyword 5 is reserved for single depth parentheses' for some odd reason
	//aka I used it to make parsing a bit easier for me
	case 6:
	{
		std::shared_ptr<BooleanVar> var;
		while (true)
		{
			var = std::dynamic_pointer_cast<BooleanVar>(lhs->run(mem));
			if (var == 0)
				break;
			if (!var->b)
				break;
			rhs->run(mem);
		}
	}
	break;
	default:
		break;
	}
	if (code!=0)
	{
		mem->push();
		code->run(mem);
		mem->pop();
		return 0;
	}
	if (lhs!=0) {
		if (rhs!=0) {
			std::shared_ptr<Variable> var = lhs->run(mem);
			if (var!=0)
				return var->operate(token.lexeme, rhs->run(mem));
		} else {
			std::shared_ptr<Variable> var = lhs->run(mem);
			if (var!=0)
				return var->operate(token.lexeme, 0);
		}
	} else {
		if (rhs!=0) {
			std::shared_ptr<Variable> var = rhs->run(mem);
			if (var!=0)
				return var->operate(token.lexeme, 0);
		} else {
			if (token.lexeme.size()!=0)
			{
				std::istringstream ss(token.lexeme);
				float f;
				ss >> f;
				if (!ss.fail())
					return std::shared_ptr<Variable>(new FloatVar(f));
				if (token.lexeme.front()=='"' || token.lexeme.front()=='\'')
					return std::shared_ptr<Variable>(new StringVar(std::string(token.lexeme.begin()+1, token.lexeme.end())));

				std::shared_ptr<Variable> var(mem->getVariable(token.lexeme));
				if (var!=0)
					return var;
				else
					return mem->setVariable(token.lexeme, std::shared_ptr<Variable>(new Variable()));
			}
		}
	}
	return 0;
}

bool Statement::hasReturn()
{
	switch (keyword)
	{
	case 1:
		return true;
	case 3:
		return lhs->hasReturn() && rhs->hasReturn();
	default:
		return false;
	}

	if (code)
	{
		for (auto i = code->statements.begin(); i != code->statements.end(); ++i)
		{
			if (i->hasReturn())
				return true;
		}
	}

	return false;
}

//TODO add evaluation of constant expressions
bool Statement::isConstant(ScriptCompile & comp)
{
	switch (keyword)
	{
	case 0:
		break;
	default:
		return false;
	}

	if (lhs != 0) {
		if (rhs != 0) {
			return lhs->isConstant(comp) && rhs->isConstant(comp);
		}
		else {
			return lhs->isConstant(comp);
		}
	}
	else {
		if (rhs != 0) {
		}
		else {
			if (token.lexeme.size() != 0)
			{
				{
					std::istringstream ss(token.lexeme);
					uint32_t ui;
					ss >> ui;
					if (!ss.fail())
					{
						return true;
					}
				}
				{
					std::istringstream ss(token.lexeme);
					float f;
					ss >> f;
					if (!ss.fail())
					{
						return true;
					}
				}
				if (token.lexeme.front() == '"' || token.lexeme.front() == '\'')
				{
					return true;
				}
				auto varData = comp.GetVariable(token.lexeme);
				return false;
			}
		}
	}
	return false;
}

ScriptTypeData Statement::getType(ScriptCompile & comp)
{
	switch (keyword)
	{
	case 0:
		if (token.lexeme.compare("int") == 0)
		{
			ScriptTypeData typeData;
			typeData.size = 8;
			typeData.type = ST_INT;
			return typeData;
		}
		if (token.lexeme.compare("uint") == 0)
		{
			ScriptTypeData typeData;
			typeData.size = 8;
			typeData.type = ST_UINT;
			return typeData;
		}
		for (auto c : comp.classes)
		{
			if (token.lexeme.compare(c.first) == 0)
			{
				ScriptTypeData typeData;
				typeData.size = c.second->size;
				typeData.type = ST_CLASS;
				typeData.class_data = c.second;
				return typeData;
			}
		}
		break;
	case 5:
		if (!lhs)
			break;
	case 7:
	{
		if (lhs->token.lexeme.compare("static") == 0)
			return rhs->getType(comp);
		return lhs->getType(comp);
	}
	break;
	default:
		throw std::runtime_error("Was expecting an expression.");
	}

	if (lhs) {
		if (rhs) {
			switch (token.lexeme.front())
			{
			case '.':
			{
				auto lhs_type = lhs->getType(comp);
				if (lhs_type.type != ST_CLASS)
					throw std::runtime_error("'.' must be preceded by a class.");
				if (lhs_type.class_data)
				{
					for (auto func : lhs_type.class_data->functions)
					{
						if (rhs->token.lexeme.compare(func.first) == 0)
						{
							ScriptTypeData typeData;
							typeData.size = 8;
							typeData.type = ST_FUNCTION;
							typeData.function_prototype.reset(new ScriptFunctionPrototype(func.second.first));
							return typeData;
						}
					}
					for (auto member : lhs_type.class_data->members)
					{
						if (rhs->token.lexeme.compare(member.first) == 0)
						{
							return member.second.type;
						}
					}
					throw std::runtime_error("No member '" + rhs->token.lexeme + "' could be found in class '" + lhs->token.lexeme + "'."); // todo: change to class name probs
				}
				throw std::runtime_error("Class of '" + lhs->token.lexeme + "' could not be determined.");
			}
			case '(':
			{
				if (lhs->keyword == 0)
				{
					if (lhs->rhs)
					{
						auto lhs_type = lhs->getType(comp);
						if (lhs_type.function_prototype)
							return lhs_type.function_prototype->ret;
					}
					if (comp.current_class)
					{
						for (auto func : comp.current_class->functions)
						{
							if (func.first.compare(lhs->token.lexeme) == 0)
							{
								return func.second.first.ret;
							}
						}
					}
				}
			}
			default:
			{
				if (lhs->token.lexeme.compare("static") == 0)
					return rhs->getType(comp);
				auto lhs_type = lhs->getType(comp);
				if (rhs->getType(comp) != lhs_type)
					throw std::runtime_error("Type mismatch.");
				return lhs_type;
			}
			break;
			}
		}
		auto lhs_type = lhs->getType(comp);
		return lhs_type;
	}
	else {
		if (rhs) {
		} else {
			if (token.lexeme.size() != 0)
			{
				{
					std::istringstream ss(token.lexeme);
					uint32_t ui;
					ss >> ui;
					if (!ss.fail() && token.lexeme.find('.') == std::string::npos)
					{
						ScriptTypeData typeData;
						typeData.size = 8;
						typeData.type = ST_UINT;
						return typeData;
					}
				}
				{
					std::istringstream ss(token.lexeme);
					float f;
					ss >> f;
					if (!ss.fail())
					{
						ScriptTypeData typeData;
						typeData.size = 8;
						typeData.type = ST_FLOAT;
						return typeData;
					}
				}
				if (token.lexeme.front() == '"' || token.lexeme.front() == '\'')
				{
					ScriptTypeData typeData;
					typeData.size = 8;
					return typeData;
				}
				if (comp.current_class)
				{
					for (auto func : comp.current_class->functions)
					{
						if (token.lexeme.compare(func.first) == 0)
						{
							ScriptTypeData typeData;
							typeData.size = 8;
							typeData.type = ST_FUNCTION;
							typeData.function_prototype.reset(new ScriptFunctionPrototype(func.second.first));
							return typeData;
						}
					}
				}
				if (token.lexeme.compare("void") == 0)
				{
					ScriptTypeData typeData;
					typeData.size = 0;
					typeData.type = ST_VOID;
					return typeData;
				}
				auto varData = comp.GetVariable(token.lexeme);
				return varData.type;
			}
		}
	}
	return ScriptTypeData();
}

std::string Statement::output(const std::string& indent)
{
	std::string str;
	if (lhs != 0 && rhs != 0)
		str = std::string("(") + lhs->output() + token.lexeme + rhs->output() + ")";
	if (lhs != 0 && rhs == 0)
		str = std::string("(") + token.lexeme + lhs->output() + ")";
	if (lhs == 0 && rhs == 0)
		str = std::string("(") + token.lexeme + ")";
	switch(keyword)
	{
	case 1:
		{
			str = indent + std::string("return ") + (lhs != 0 ? lhs->output() : "");
		}
		break;
	case 2:
		{
			str = indent + std::string("if (").append(lhs->output()).append(")\n")
				.append(indent).append("{\n")
				.append(rhs->output(indent))
				.append(indent).append("}");
		}
		break;
	case 3:
		{
			str = std::string("if (").append(lhs->output()).append(")\n")
				.append(indent).append("{\n")
				.append(rhs->lhs->output(indent))
				.append(indent).append("}\n")
				.append(indent).append("else\n")
				.append(indent).append("{\n")
				.append(rhs->rhs->output(indent))
				.append(indent).append("}");
		}
		break;
	case 4:
		{
		}
		break;
	default:
		{
			str = indent + str;
		}
		break;
	}
	if (code)
	{
		str = "\n";
		for (auto i = code->statements.begin(); i != code->statements.end(); ++i)
			str.append(i->output(indent + "    ")).append("\n");
	}
	str.append(std::to_string((long long)keyword));
	return str;
}