#include "Statement.h"

#include "ScriptCode.h"

#include "BooleanVar.h"
#include "FloatVar.h"
#include "StringVar.h"
#include "FunctionVar.h"
#include "VectorVar.h"

#include "StreamAssignee.h"
#include "ScriptAssemblyHelper.h"
#include "ScriptRegisterGuard.h"

#include <sstream>

#include <iostream>

#include <set>

#include <functional>

#include <algorithm>

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

	for (size_t i = 0; i < tokens.size(); ++i)
	{
		if (tokens[i]->token.lexeme.compare("if")==0 || tokens[i]->token.lexeme.compare("else")==0)
		{
			if (tokens[i]->token.lexeme.compare("if")==0)
				++i;
			if (i < tokens.size())
			{
				if (++i < tokens.size())
				{
					if (tokens[i]->token.lexeme.size())
					{
						if (tokens[i]->token.lexeme.front()!='{')
						{
							for (size_t b = i; b < tokens.size(); ++b)
							{
								if (tokens[b]->token.lexeme.front()==';')
								{
									std::shared_ptr<Statement> pBegin(new Statement(0, Token(), 0));
									pBegin->token.lexeme = '{';
									std::shared_ptr<Statement> pEnd(new Statement(0, Token(), 0));
									pEnd->token.lexeme = '}';
									tokens.insert(tokens.begin() + b + 1, pEnd);
									tokens.insert(tokens.begin() + i, pBegin);
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

std::vector<std::string> boolean_comparators = { "==", "!=", "<", ">", "<=", ">=" };

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

		// add rsp, 0
		p = 0b01001000;
		po = 0x81;
		o = 0b11000100;
		comp.AddStackDependant();
		dat32 = 0;

		sasm.Pop(ScriptCompileMemoryTarget(0b101));

		sasm.Move(0x89, ScriptCompileMemoryTarget(0b011), ScriptCompileMemoryTarget(0b01, 0b100, 8));

		// rets
		po = 0xc3;
	}
	return;
	case 2://if
	case 3://if else
	{
		auto cmp = std::find(boolean_comparators.begin(), boolean_comparators.end(), lhs->token.lexeme);
		if (cmp != boolean_comparators.end())
		{
			lhs->compile(comp);
		}
		else
		{
			comp.target.lvalue = true;
			lhs->compile(comp);

			// TODO check type

			// cmp
			sasm.MoveR(0x81, 7, comp.target);
			dat32 = 0;

			// jz
			p = 0x0f;
			po = 0x84;
			rel32 = 0;
		}

		size_t rel_index = comp.rel32.size() - 1;
		size_t rel = comp.rel32[rel_index];

		if (keyword == 2)
		{
			comp.BeginScope();
			rhs->compile(comp);
			comp.EndScopeAggresive();
		}
		else
		{
			comp.BeginScope();

			rhs->lhs->compile(comp);

			if (!rhs->lhs->hasReturn())
			{
				// jmp
				p = 0xe9;
				rel32 = 0;
			}

			comp.EndScopeAggresive();

			comp.rel32.erase(comp.rel32.cbegin() + rel_index);

			comp.BeginScope();
			rhs->rhs->compile(comp);
			comp.EndScopeAggresive();

			comp.rel32.insert(comp.rel32.cbegin() + rel_index, rel);
		}
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

				if (func_name.compare("~" + comp.current_class->class_name) == 0)
				{
					func_name = "~";
				}

				ScriptFunctionCompileData function_comp;
				function_comp.class_ptr = comp.current_class;
				function_comp.name = func_name;
				function_comp.prototype = prototype;
				function_comp.parameter_names = parameter_names;
				function_comp.code = rhs;

				comp.function_code.push_back(function_comp);

				comp.current_class->AddFunction(func_name, prototype, nullptr);

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

				if (std::find(tokens.begin(), tokens.end(), "virtual") != tokens.end())
					comp.current_class->AddVirtualFunction(func_name, prototype);

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

				if (comp.current_class->GetMember("_vfptr") != ScriptVariableData())
				{
					comp.current_class->vftable = (char*)comp.base_pointer + ss.tellp();
					std::vector<ScriptVirtualFunctionData> vfs;
					vfs.reserve(comp.current_class->GetVirtualFunctionCount());
					comp.current_class->GetVirtualFunctionList(vfs);
					for (auto vf : vfs)
					{
						ScriptLinkData link;
						link.relative = false;
						link.location = ss.tellp();
						link.class_ptr = comp.current_class;
						link.function_name = vf.name;
						link.prototype = vf.prototype;
						comp.links.push_back(link);

						dat64 = 0;
					}
				}

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
			ScriptFunctionPrototype callee_proto;

			std::vector<std::shared_ptr<Statement>> params;

			std::function<void(const std::shared_ptr<Statement>&)> add_arg;
			add_arg = [&params, &add_arg](const std::shared_ptr<Statement>& v)
			{
				if (v->token.lexeme.size() == 0 && (v->keyword == 0 || v->keyword == 5))
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

			callee_proto.params.resize(params.size());
			std::transform(params.begin(), params.end(), callee_proto.params.begin(), [&](auto a) { return a->getType(comp); });

			std::string function_name;
			std::shared_ptr<ScriptClassData> function_class;
			if (lhs->token.lexeme.compare(".") == 0)
			{
				function_name = lhs->rhs->token.lexeme;
				function_class = lhs->lhs->getType(comp).class_data;
				if (!function_class)
					throw std::runtime_error("'.' must be preceded by an instance of a class.");

				comp.target = ScriptCompileMemoryTarget(0b001);
				lhs->lhs->compile(comp);
			}
			else
			{
				function_name = lhs->token.lexeme;
				function_class = comp.current_class;

				// mov rcx, rbx
				sasm.Move(0x89, ScriptCompileMemoryTarget(0b001), ScriptCompileMemoryTarget(0b011));
			}

			if (lhs->keyword == 0)
			{
				auto full_proto = function_class->GetFunctionFullPrototype(function_name, callee_proto);
				if (full_proto)
					callee_proto = full_proto.value();
				else
					throw std::runtime_error("Could not find function '" + function_name + "'.");
			}

			bool has_this = true;

			comp.BeginScope();

			size_t shadow_space = std::max(32ull, params.size() * 8 + (has_this ? 8 : 0));
			comp.stack += shadow_space;

			size_t temp_space = 0;

			for (size_t i = 0; i < params.size(); ++i)
			{
				ScriptCompileMemoryTarget param_target;
				size_t real_index = has_this ? i + 1 : i;
				switch (real_index)
				{
				case 0:
					param_target.regm = 0b001;
					break;
				case 1:
					param_target.regm = 0b010;
					break;
				case 2:
					param_target.regm = 8;
					break;
				case 3:
					param_target.regm = 9;
					break;
				default:
					param_target.regm = 0b10101;
					param_target.mode = 0b10;
					param_target.offset = -128 + real_index * 8;
					break;
				}

				if (callee_proto.params[i].GetSize() <= 8)
				{
					comp.target = param_target;
					params[i]->compile(comp);
				}
				else
				{
					comp.target = ScriptCompileMemoryTarget(0b10, 0b10101, -128 + shadow_space + temp_space);
					temp_space += callee_proto.params[i].GetSize();
					params[i]->compile(comp);
					sasm.Move(0x8d, param_target, comp.target);
				}
			}

			comp.stack += temp_space;

			off_t vfi = function_class->GetVirtualFunctionIndex(lhs->token.lexeme, callee_proto);
			if (vfi >= 0)
			{
				auto tmp_target = sasm.FindRegister();
				auto vfptr_target = function_class->GetMember("_vfptr").target;
				sasm.Move(0x89, tmp_target, vfptr_target);

				tmp_target.offset = vfi * 8;
				tmp_target.mode = 0b10;

				sasm.MoveR(0xff, 2, tmp_target);
			}
			else
			{
				auto tmp_target = sasm.FindRegister();
				uint8_t rex = 0b01001000;
				if (tmp_target.regm & 0b1000)
					rex |= 0b001;
				p = rex;
				po = 0xb8 + (tmp_target.regm & 0b111);

				ScriptLinkData link;
				link.relative = false;
				link.location = ss.tellp();
				link.class_ptr = function_class;
				link.function_name = function_name;
				link.prototype = callee_proto;
				comp.links.push_back(link);
				
				dat64 = 0;

				sasm.MoveR(0xff, 2, tmp_target);

				//// call lhs (rel32)
				//po = 0xe8;

				//ScriptLinkData link;
				//link.relative = true;
				//link.location = ss.tellp();
				//link.class_ptr = comp.current_class;
				//link.function_name = lhs->token.lexeme;
				//link.prototype = callee_proto;
				//comp.links.push_back(link);

				//dat32 = 0;
			}

			comp.EndScope();

			if (callee_proto.ret.GetSize() > 0 && callee_proto.ret.GetSize() <= 8)
			{
				ScriptCompileMemoryTarget eax_target;
				if (target.lvalue)
				{
					comp.target = eax_target;
				}
				else
				{
					if (target != eax_target)
						sasm.Move(0x89, target, eax_target);
				}
			}

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

			auto cmp = std::find(boolean_comparators.begin(), boolean_comparators.end(), token.lexeme);
			if (cmp != boolean_comparators.end())
			{
				comp.target.lvalue = true;
				lhs->compile(comp);
				target = comp.target;
				ScriptRegisterGuard guard(&comp);

				comp.target.lvalue = true;
				rhs->compile(comp);

				sasm.Move(0x39, target, comp.target);

				switch (cmp - boolean_comparators.begin())
				{
				case 0:
					// jnz
					p = 0x0f;
					po = 0x85;
					rel32 = 0;
					return;
				case 1:
					// jz
					p = 0x0f;
					po = 0x84;
					rel32 = 0;
					return;
				case 2:
					// jge
					p = 0x0f;
					po = 0x8d;
					rel32 = 0;
					return;
				case 3:
					// jle
					p = 0x0f;
					po = 0x8e;
					rel32 = 0;
					return;
				case 4:
					// jg
					p = 0x0f;
					po = 0x8f;
					rel32 = 0;
					return;
				case 5:
					// jl
					p = 0x0f;
					po = 0x8c;
					rel32 = 0;
					return;
				}
			}

			/*if (target.lvalue)
			{
				comp.target = sasm.FindRegister();
				target = comp.target;
			}*/
			switch (token.lexeme.front())
			{
			case '.':
			{
				auto lhs_type = lhs->getType(comp);
				if (lhs_type.type != ST_CLASS)
					throw std::runtime_error("'.' must be preceded by an instance of a class.");
				if (lhs_type.class_data)
				{
					for (auto func : lhs_type.class_data->functions)
					{
						if (rhs->token.lexeme.compare(func.first) == 0)
						{
							auto prototype = func.second.first;

							return;
						}
					}
					for (auto member : lhs_type.class_data->members)
					{
						if (rhs->token.lexeme.compare(member.first) == 0)
						{
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
					throw std::runtime_error("No member '" + rhs->token.lexeme + "' could be found in class '" + lhs_type.class_data->class_name + "'.");
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
				if (target.lvalue)
					target = comp.target;
				ScriptRegisterGuard guard(&comp);

				comp.target.lvalue = true;
				rhs->compile(comp);

				sasm.Move(0x01, target, comp.target);
				comp.target = target;
			}
			break;
			case '-':
			{
				lhs->compile(comp);
				if (target.lvalue)
					target = comp.target;
				ScriptRegisterGuard guard(&comp);

				comp.target.lvalue = true;
				rhs->compile(comp);

				sasm.Move(0x29, target, comp.target);
				comp.target = target;
			}
			break;
			case '*':
			{
				auto rax_target = ScriptCompileMemoryTarget();
				auto rdx_target = ScriptCompileMemoryTarget(0b010);

				bool rax_busy = comp.IsBusy(rax_target.regm);
				if (rax_busy)
					sasm.Push(rax_target);

				comp.target = rax_target;
				lhs->compile(comp);
				ScriptRegisterGuard guard(&comp);

				comp.target.lvalue = true;
				rhs->compile(comp);

				bool rdx_busy = comp.IsBusy(rdx_target.regm);
				if (rdx_busy)
					sasm.Push(rdx_target);

				sasm.MoveR(0xf7, 4, comp.target);

				if (rdx_busy)
					sasm.Pop(rdx_target);

				if (rax_busy)
				{
					if (target.lvalue)
					{
						comp.target = sasm.FindRegister();
						sasm.Move(0x89, comp.target, rax_target);
					}
					else
					{
						sasm.Move(0x89, target, rax_target);
					}
					sasm.Pop(rax_target);
				}
				else
				{
					if (target.lvalue)
						comp.target = rax_target;
					else
						if (target != rax_target)
							sasm.Move(0x89, target, rax_target);
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

				if (comp.target.mode == 0b11)
				{
					throw std::runtime_error("Cannot get pointer to register.");
				}
				else
				{
					if (target.lvalue)
					{
						target = sasm.FindRegister();

						// lea target, comp.target
						sasm.Move(0x8d, target, comp.target);

						comp.target = target;
					}
					else
					{
						// lea target, comp.target
						sasm.Move(0x8d, target, comp.target);
					}
				}
			}
			break;
			case '*':
			{
				if (lhs_type.indirection == 0)
					throw std::runtime_error("Operand of '*' must be a pointer.");
				--lhs_type.indirection;

				comp.target.lvalue = true;
				lhs->compile(comp);

				if (comp.target.mode == 0b11)
				{
					comp.target.mode = 0b00;
				}
				else
				{
					auto reg_target = sasm.FindRegister();
					sasm.Move(0x8b, reg_target, comp.target);
					comp.target = reg_target;
					comp.target.mode = 0b00;
				}

				if (!target.lvalue)
				{
					size_t size = lhs_type.GetSize();
					if (size <= 8)
					{
						sasm.Move(0x89, target, comp.target);
					}
					else
					{
						auto rcx_target = ScriptCompileMemoryTarget(0b001);
						bool rcx_busy = comp.IsBusy(rcx_target.regm);
						if (rcx_busy)
							sasm.Push(rcx_target);

						sasm.MoveR(0x8d, 0b110, comp.target);
						sasm.MoveR(0x8d, 0b111, target);

						// mov rcx, size
						p = 0b01001000;
						po = 0xc7;
						o = rcx_target.GetModRegRM(0);
						dat32 = size;

						// cld
						po = 0xfc;

						// rep movs
						p = 0xf3;
						p = 0b01001000;
						po = 0xa5;

						if (rcx_busy)
							sasm.Pop(rcx_target);
					}
				}
			}
			break;
			case '-':
			{
				lhs->compile(comp);
				sasm.MoveR(0xf7, 3, comp.target);
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
							if (comp.target.regm & 0b1000)
								rex |= 0b001;
							p = rex;
							po = 0xb8 + (comp.target.regm & 0b111);
							dat64 = ui;
						}
						else
						{
							if (target.mode == 0b11)
							{
								uint8_t rex = 0b01001000;
								if (target.regm & 0b1000)
									rex |= 0b001;
								p = rex;
								po = 0xb8 + (target.regm & 0b111);
								dat64 = ui;
							}
							else
							{
								auto tmp_target = sasm.FindRegister({ target });
								uint8_t rex = 0b01001000;
								if (tmp_target.regm & 0b1000)
									rex |= 0b001;
								p = rex;
								po = 0xb8 + (tmp_target.regm & 0b111);
								dat64 = ui;

								sasm.Move(0x89, target, tmp_target);
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
		return rhs->lhs->hasReturn() && rhs->rhs->hasReturn();
	default:
		break;
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
		if (lhs)
			return lhs->getType(comp);
		break;
	case 7:
	{
		if (rhs->keyword == 4)
		{
			ScriptFunctionPrototype callee_proto;

			std::vector<std::shared_ptr<Statement>> params;

			std::function<void(const std::shared_ptr<Statement>&)> add_arg;
			add_arg = [&params, &add_arg](const std::shared_ptr<Statement>& v)
			{
				if (v->token.lexeme.size() == 0 && (v->keyword == 0 || v->keyword == 5))
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

			callee_proto.params.resize(params.size());
			std::transform(params.begin(), params.end(), callee_proto.params.begin(), [&](auto a) { return a->getType(comp); });

			if (lhs->keyword == 0)
			{
				auto full_proto = comp.current_class->GetFunctionFullPrototype(lhs->token.lexeme, callee_proto);
				if (full_proto)
					return full_proto.value().ret;
			}
		}
		if (lhs->token.lexeme.compare("static") == 0)
			return rhs->getType(comp);
		if (lhs->token.lexeme.compare("virtual") == 0)
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
					throw std::runtime_error("'.' must be preceded by an instance of a class.");
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
					throw std::runtime_error("No member '" + rhs->token.lexeme + "' could be found in class '" + lhs_type.class_data->class_name + "'.");
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