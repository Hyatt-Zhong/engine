#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <algorithm>
#include <bitset>
#include <cmath>
#include <queue>
#include <sstream>
#include <fstream>
#include <bitset>
#include <random>

#define SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
void DebugPrintf(const char* strOutputString, ...);
using std::endl;
using std::cout;
template <typename T, typename ...Args>
void print(T && arg, Args&&... args) {
	cout << arg << " ";
	if constexpr (sizeof...(args) > 0) {
		print(args...);
	}
	if constexpr (sizeof...(args) == 0) {
		cout << endl;
	}
}
inline std::string ReadFile(const std::string& path) {
	std::fstream fs;
	fs.open(path);
	std::stringstream ss;
	ss << fs.rdbuf();
	return ss.str();
}

template<class T>
bool sort_fun(T* A, T* B)
{
	return A->deep < B->deep;
}
template <typename T>
class single
{
private:
	static T* ins;
	friend typename T;
	single(){}
public:
	static T* Instance()
	{
		if (NULL == ins) { ins = new T; }
		return ins;
	}
	static void DelInstance()
	{
		SAFE_DELETE(ins);
	}
};

template <typename T>
T* single<T> ::ins = NULL;

