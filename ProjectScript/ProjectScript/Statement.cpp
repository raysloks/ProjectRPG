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

	if (tokens.size()==0) {
		return;
	}

	if (tokens.size()==1) {
		*this = *tokens.front();
		if (token.lexeme.compare("return") == 0)
			keyword = 1;
		tokens.erase(tokens.begin());
		return;
	}

	if (!expression)
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
	}

	auto fp = tokens.end();
	bool found = true;
	while (found)
	{
		int depth = 0;
		int max_depth = 0;
		found = false;
		for (auto i=tokens.begin();i!=tokens.end();++i)
		{
			if ((*i)->token.lexeme.size())
			{
				if ((*i)->token.lexeme.front()=='(' && (*i)->token.lexeme.back()!=')')
				{
					if (depth==0)
						fp = i;
					++depth;
					if (depth>max_depth)
						max_depth=depth;
				}
				if ((*i)->token.lexeme.front()==')')
				{
					--depth;
					if (depth==0) {
						if (fp!=tokens.end()) {
							std::shared_ptr<Statement> p(new Statement(std::vector<std::shared_ptr<Statement>>(fp+1, i), true));
							if (max_depth==1)
								p->keyword = 5;
							if (fp!=tokens.begin())
								if ((*(fp-1))->token.lexeme.compare("if")==0 || (*(fp-1))->token.lexeme.compare("while")==0)
									p->keyword = 0;
							tokens.insert(tokens.erase(fp, i+1), p);
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
					--depth;
					if (depth==0) {
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
		if (depth>0)
			throw std::runtime_error("Missing '}'.");
	}

	found=true;
	while (found)
	{
		found=false;
		for (auto i=tokens.rbegin();i!=tokens.rend();++i)
		{
			if ((*i)->token.lexeme.size())
			{
				if ((*i)->token.lexeme.compare("while")==0)
				{
					auto b = i.base();
					auto bs = i.base();
					std::shared_ptr<Statement> p(new Statement(0, Token(), 0));
					if (b!=tokens.end())
					{
						p->lhs = *b;
						++b;
						if (b!=tokens.end())
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
				if ((*i)->token.lexeme.compare("if")==0)
				{
					auto b = i.base();
					auto bs = i.base();
					std::shared_ptr<Statement> p(new Statement(0, Token(), 0));
					if (b!=tokens.end())
					{
						p->lhs = *b;
						++b;
						if (b!=tokens.end())
						{
							++b;
							if (b!=tokens.end())
							{
								++b;
								if (b!=tokens.end())
								{
									if ((*(bs+2))->token.lexeme.compare("else")==0)
									{
										p->keyword = 3;
										p->rhs = std::shared_ptr<Statement>(new Statement(*(bs+1), Token(), *(bs+3)));
										//p->token.lexeme = "else"; <- this was silly
										std::shared_ptr<Statement> semi_colon(new Statement(0, Token(), 0));
										semi_colon->token.lexeme = ';';
										tokens.insert(tokens.insert(tokens.erase(bs-1, bs+4), semi_colon), p);
										found=true;
										break;
									}
								}
							}
							p->keyword = 2;
							p->rhs = *(bs+1);
							std::shared_ptr<Statement> semi_colon(new Statement(0, Token(), 0));
							semi_colon->token.lexeme = ';';
							tokens.insert(tokens.insert(tokens.erase(bs-1, bs+2), semi_colon), p);
							found=true;
							break;
						}
					}
					tokens.erase(bs-1, tokens.end());
					found=true;
					break;
				}
			}
		}
	}
	
	for (auto i=tokens.begin();i!=tokens.end();++i)
	{
		if ((*i)->token.lexeme.size())
		{
			if ((*i)->token.lexeme.front()==';')
			{
				std::shared_ptr<Statement> p(new Statement(std::vector<std::shared_ptr<Statement>>(tokens.begin(), i), true));
				*this = *p;
				tokens.erase(tokens.begin(), i+1);
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

Statement::~Statement(void)
{
}

void Statement::compile(ScriptCompile& comp)
{
	auto& ss = comp.ss;
	auto& sasm = comp.sasm;

	StreamAssignee<uint8_t> p(ss);
	StreamAssignee<uint8_t> po(ss);
	StreamAssignee<uint8_t> o(ss);
	StreamAssignee<uint8_t> dat8(ss);
	StreamAssignee<uint16_t> dat16(ss);
	StreamAssignee<uint32_t> dat32(ss);
	StreamAssignee<uint64_t> dat64(ss);
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
			if (comp.proto->ret.size == 0)
				throw std::runtime_error("Return type is 'void'.");
			if (comp.proto->ret != lhs->getType(comp))
				throw std::runtime_error("Return type mismatch.");

			if (comp.proto->ret.size <= 4)
			{
				comp.target = ScriptCompileMemoryTarget();

				lhs->compile(comp);
			}
			else
			{
				throw std::runtime_error("Return types larger than 32 bits are not supported yet.");
			}
		}
		else
		{
			if (comp.proto->ret.size > 0)
				throw std::runtime_error("Function needs to return a value.");
		}

		if (comp.stack > 0)
		{
			// add esp, comp.stack
			po = 0x81;
			o = 0b11000100;
			dat32 = comp.stack;
		}

		// leave
		po = 0xc9;
		
		// rets
		po = 0xc3;
	}
	return;
	case 2://if
	{
		lhs->compile(comp);

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
		lhs->compile(comp);

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
		if (code != 0)
		{
			comp.BeginScope();
			bool has_return = false;
			for (auto i = code->statements.begin(); i != code->statements.end(); ++i)
			{
				i->compile(comp);
				has_return |= i->hasReturn();
			}
			if (!has_return)
				throw std::runtime_error("Not all control paths return.");
			comp.EndScope();
		}
	}
	return;
	//keyword 5 is reserved for single depth parentheses' for some odd reason
	//aka I used it to make parsing a bit easier for me
	case 6://while
	{
		lhs->compile(comp);

		// jnz
		p = 0x0f;
		po = 0x84;
		rel32 = 0;

		size_t start = comp.ss.tellp();

		comp.BeginScope();
		rhs->compile(comp);

		comp.BeginScope();
		lhs->compile(comp);

		// jz
		p = 0x0f;
		po = 0x84;

		std::streamoff diff = start - comp.ss.tellp();
		diff -= sizeof(int32_t);

		rel32 = diff;

		comp.EndScope();
		comp.EndScope();
	}
	return;
	case 7://no real operator or keyword or something
	{
		if (lhs->keyword == 7)
		{
			if (lhs->lhs->token.lexeme.compare("class") == 0)
			{
				if (comp.proto == nullptr)
				{
					comp.SetClass(lhs->rhs->token.lexeme);
					rhs->compile(comp);
					comp.current_class.reset();
					return;
				}
			}
			if (rhs->code != nullptr && comp.current_class != nullptr)
			{
				comp.BeginScope();

				comp.BeginFunction();

				ScriptFunctionPrototype prototype;
				prototype.ret = lhs->getType(comp);

				std::vector<std::string> parameter_names;

				std::function<void(const std::shared_ptr<Statement>&)> add_arg;
				add_arg = [&](const std::shared_ptr<Statement>& v)
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
						prototype.params.push_back(v->getType(comp));
						parameter_names.push_back(v->rhs->token.lexeme);
					}
				};
				add_arg(rhs->lhs);
				
				size_t offset = 8;
				for (size_t i = 0; i < prototype.params.size(); ++i)
				{
					comp.AddParameter(parameter_names[i], prototype.params[i], offset);
					offset += prototype.params[i].size;
				}

				comp.current_class->AddFunction(lhs->rhs->token.lexeme, prototype, (char*)comp.base_pointer + ss.tellp());

				comp.proto.reset(new ScriptFunctionPrototype(prototype));
				rhs->compile(comp);
				comp.proto.reset();

				comp.EndScope();

				return;
			}
		}
		if (comp.proto != nullptr)
		{
			comp.PushVariable(rhs->token.lexeme, getType(comp));
			return;
		}
		if (comp.current_class != nullptr)
		{
			comp.current_class->AddMember(rhs->token.lexeme, getType(comp));
			return;
		}
	}
	return;
	default:
		break;
	}

	if (code != 0)
	{
		comp.BeginScope();
		for (auto i = code->statements.begin(); i != code->statements.end(); ++i)
		{
			i->compile(comp);
		}
		comp.EndScope();
	}

	if (lhs != 0) {
		auto lhs_type = lhs->getType(comp);
		if (rhs != 0) {
			auto rhs_type = lhs->getType(comp);
			if (lhs_type != rhs_type)
				throw std::runtime_error("Type mismatch.");
			switch (token.lexeme.front())
			{
			case '=':
			{
				if (lhs->keyword == 7)
				{
					auto rhs_target = sasm.FindRegister();
					comp.target = rhs_target;

					rhs->compile(comp);

					comp.PushVariable(lhs->rhs->token.lexeme, lhs_type, rhs_target);
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
				auto rhs_target = sasm.FindRegister(target);
				comp.target = rhs_target;

				rhs->compile(comp);

				sasm.Push(rhs_target);

				comp.target = target;

				lhs->compile(comp);

				sasm.Pop(rhs_target);

				sasm.Move(0x01, target, rhs_target);
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

				if (target.mod == 0b11)
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

				ScriptCompileMemoryTarget ebx_target;
				ebx_target.rm = 0b011;

				comp.target = ebx_target;
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

				// idiv ebx
				po = 0xf7;
				o = 0b11111011;

				if (target.mod != 0b11 || target.rm != 0b000)
					sasm.Move(0x89, target, eax_target);
			}
			break;
			case '(':
			{
				ScriptFunctionPrototype callee_proto;

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
				
				if (callee_proto.params.size() > 0)
				{
					if (callee_proto.params.size() == 1)
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
					else
					{
						for (int i = 0; i < params.size(); ++i)
						{
							params[i]->compile(comp);
						}
					}
				}

				ScriptCompileMemoryTarget esi_target;
				esi_target.rm = 0b110;

				comp.target = esi_target;
				comp.target.lvalue = true;
				lhs->compile(comp);

				// call comp.target
				sasm.Move(0xff, 2, comp.target);
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

				if (target.mod == 0b11)
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

					comp.target.mod = 0b01;
				}
				else
				{
					ScriptCompileMemoryTarget eax_target;

					comp.target = eax_target;
					lhs->compile(comp);

					comp.target.mod = 0b01;

					if (target.mod == 0b11)
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
					uint32_t ui;
					ss >> ui;
					if (!ss.fail())
					{
						if (target.mod == 0b11)
						{
							po = 0xb8 + target.rm;
							dat32 = ui;
						}
						else
						{
							sasm.Move(0xc7, 0, target);
							dat32 = ui;
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
					if (target.mod == 0b11)
					{
						sasm.Move(0x8b, target, var_target);
					}
					else
					{
						if (var_target.mod == 0b11)
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

	if (code != 0)
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
		break;
	case 5:
	case 7:
	{
		switch (lhs->keyword)
		{
		case 0:
			if (lhs->token.lexeme.compare("int") == 0)
			{
				ScriptTypeData typeData;
				typeData.size = 4;
				typeData.type = ST_INT;
				return typeData;
			}
			if (lhs->token.lexeme.compare("uint") == 0)
			{
				ScriptTypeData typeData;
				typeData.size = 4;
				typeData.type = ST_UINT;
				return typeData;
			}
		}
	}
	break;
	default:
		throw std::runtime_error("Was expecting an expression.");
	}

	if (lhs != 0) {
		auto lhs_type = lhs->getType(comp);
		if (rhs != 0) {
			switch (token.lexeme.front())
			{
			case '(':
			{
				
			}
			break;
			default:
			{
				if (rhs->getType(comp) != lhs_type)
					throw std::runtime_error("Type mismatch.");
			}
			break;
			}
			return lhs_type;
		} else {
			return lhs_type;
		}
	}
	else {
		if (rhs != 0) {
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
						typeData.size = 4;
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
						typeData.size = 4;
						typeData.type = ST_FLOAT;
						return typeData;
					}
				}
				if (token.lexeme.front() == '"' || token.lexeme.front() == '\'')
				{
					ScriptTypeData typeData;
					typeData.size = 4;
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
			if (code!=0)
			{
				str.clear();
				for (auto i=code->statements.begin();i!=code->statements.end();++i)
					str.append(i->output(indent+"    ")).append("\n");
			}
			else
			{
				str = indent + str;
			}
		}
		break;
	}
	str.append(std::to_string((long long)keyword));
	return str;
}