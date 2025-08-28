#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>

namespace CJMODAPI {

enum class PlaceholderType {
	Required,    // $! 必须
	Optional,    // $? 可选
	Unordered,   // $_ 无序
	Regular      // $  普通
};

struct AtomArg {
	std::string value;
	PlaceholderType type = PlaceholderType::Regular;
	std::function<std::string(const std::string&)> getter;

	void bind(std::function<std::string(const std::string&)> g) { getter = std::move(g); }
	void fillValue(const std::string& v) { value = v; }
	void fillValue(int v) { value = std::to_string(v); }
	
	// 占位符类型判断
	bool isRequired() const { return type == PlaceholderType::Required; }
	bool isOptional() const { return type == PlaceholderType::Optional; }
	bool isUnordered() const { return type == PlaceholderType::Unordered; }
};

class Arg {
public:
	Arg() = default;
	explicit Arg(const std::vector<std::string>& toks) {
		atoms_.reserve(toks.size());
		for (auto& t : toks) { 
			AtomArg a; 
			// 解析占位符类型
			if (t == "$!") a.type = PlaceholderType::Required;
			else if (t == "$?") a.type = PlaceholderType::Optional;
			else if (t == "$_") a.type = PlaceholderType::Unordered;
			else if (t == "$") a.type = PlaceholderType::Regular;
			a.value = t; 
			atoms_.push_back(a); 
		}
	}

	void print() const {
		std::cout << "[";
		for (size_t i = 0; i < atoms_.size(); ++i) {
			std::cout << '"' << atoms_[i].value << '"';
			if (i + 1 < atoms_.size()) std::cout << ", ";
		}
		std::cout << "]\n";
	}

	void bind(const std::string& token, std::function<std::string(const std::string&)> g) {
		for (auto& a : atoms_) if (a.value == token) a.bind(g);
	}

	void fillValue(const Arg& other) {
		size_t n = std::min(atoms_.size(), other.atoms_.size());
		for (size_t i = 0; i < n; ++i) atoms_[i].value = other.atoms_[i].value;
	}

	void transform(const std::string& out) { transformed_ = out; }
	const std::string& transformed() const { return transformed_; }

	AtomArg& operator[](size_t i) { return atoms_[i]; }
	const AtomArg& operator[](size_t i) const { return atoms_[i]; }
	size_t size() const { return atoms_.size(); }

private:
	std::vector<AtomArg> atoms_;
	std::string transformed_;
};

} // namespace CJMODAPI