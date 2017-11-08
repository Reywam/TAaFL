#include "stdafx.h"
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>

using namespace std;

struct State
{
	string name = "";
	vector<string> transitions;
	vector<string> sygnals;

	bool operator == (const State &state) const {
		return this->name == state.name && this->transitions == state.transitions;
	}
};

struct EqualityClass 
{
	// Имя класса эквивалентности
	string name = "";

	// Состояния, которые в него входят
	vector<State> states;
};

void ReadMooreDataToMap(unordered_map<string, vector<int>> &map, ifstream &input)
{
	string sygnals = "";
	string dataString = "";

	// Считываем сигналы
	getline(input, sygnals);

	// Закидываем их в мапу, их индексы закидываем в значения
	string sygnal;
	vector<string> normIndexes;
	for (size_t i = 0; i < sygnals.size(); i++)
	{
		if (sygnals[i] != ' ') 
		{
			sygnal += sygnals[i];
		}
		else
		{
			normIndexes.push_back(sygnal);
			map[sygnal].push_back(normIndexes.size() - 1);
			sygnal = "";
		}
	}

	normIndexes.push_back(sygnal);
	map[sygnal].push_back(normIndexes.size() - 1);
}

vector<EqualityClass> CreateEqvClassesAndStates(unordered_map<string, vector<int>> &map, ifstream &input)
{
	vector<EqualityClass> classes;

	string dataString;
	vector<vector<string>> rows;

	// Считали строки матрицы
	while (getline(input, dataString))
	{
		vector<string> newCol;
		string str;
		for (size_t i = 0; i < dataString.size(); i++)
		{
			if (dataString[i] != ' ')
			{
				str += dataString[i];
			}
			else
			{
				newCol.push_back(str);
				str = "";
			}
		}
		newCol.push_back(str);
		rows.push_back(newCol);
	}

	// Создаём состояния и заключаем их в класс экв-сти
	for (auto it = map.begin(); it != map.end(); it++)
	{
		EqualityClass newClass;
		newClass.name = it->first;

		vector<int> indexes = it->second;
		// Собираем значения для состояния
		
		for (size_t i = 0; i < indexes.size(); i++)
		{
			State newState;
			newState.name = rows[0][indexes[i]];
			for (size_t row = 1; row < rows.size(); row++)
			{
				newState.transitions.push_back(rows[row][indexes[i]]);
			}
			newClass.states.push_back(newState);
		}
		classes.push_back(newClass);
	}

	return classes;
}

void RecalculateEqvClass(EqualityClass eqClass, vector<EqualityClass> &classes)
{
	while(!eqClass.states.empty())
	{
		EqualityClass newClass;
		newClass.name = to_string(classes.size() + 1);

		State currState = *(eqClass.states.begin());
		newClass.states.push_back(currState);
		eqClass.states.erase(eqClass.states.begin());
		// Проходимся по всем, сравниваем переходы, если равны - в один класс
		vector<State>::iterator it;
		for (it = eqClass.states.begin(); it != eqClass.states.end();)
		{
			State nextState = *it;
			if (currState.transitions == nextState.transitions)
			{
				newClass.states.push_back(nextState);
				it = eqClass.states.erase(it);
			}
			else
			{
				++it;
			}
		}
		classes.push_back(newClass);
	}
}

vector<EqualityClass> RecalculateEqvClasses(vector<EqualityClass> eqvClasses)
{
	vector<EqualityClass> allNewClasses;

	while(!eqvClasses.empty())
	{
		EqualityClass eqvClass = *eqvClasses.begin();
		RecalculateEqvClass(eqvClass, allNewClasses);
		eqvClasses.erase(eqvClasses.begin());
	}

	return allNewClasses;
}

unordered_map<string, string> FindEqClassOfStates(vector<EqualityClass> const &eqClasses)
{
	// Находим в каком классе эквивалентности находится состояние
	//Состояние-класс
	unordered_map<string, string> data;
	
	for (size_t i = 0; i < eqClasses.size(); i++)
	{
		for(size_t state = 0; state < eqClasses[i].states.size(); state++)
		{ 
			State currState = eqClasses[i].states[state];
			string stateName = currState.name;
			data[stateName] = "";
		}
	}

	for (auto it = data.begin(); it != data.end(); it++)
	{
		for (size_t i = 0; i < eqClasses.size(); i++)
		{
			vector<State> vertexes = eqClasses[i].states;
			for (auto vertex = vertexes.begin(); vertex != vertexes.end(); vertex++)
			{
				string vertexName = vertex->name;

				if (it->first == vertexName)
				{
					it->second = eqClasses[i].name;
					break;
				}
			}
		}
	}

	return data;
}

void ReplaceTransitions(vector<EqualityClass> &eqClasses, unordered_map<string, string> const &map)
{
	for (size_t i = 0; i < eqClasses.size(); i++)
	{
		vector<State> states = eqClasses[i].states;
		for (size_t state = 0; state < states.size(); state++)
		{
			vector<string> transitions = states[state].transitions;
			for (size_t transition = 0; transition < transitions.size(); transition++)
			{
				auto it = map.find(transitions[transition]);
				eqClasses[i].states[state].transitions[transition] = it->second;
			}
		}
	}
}

void ReplaceTransitions(vector<EqualityClass> &eqClasses,
	unordered_map<string, vector<string>> const &map)
{
	for (size_t i = 0; i < eqClasses.size(); i++)
	{
		vector<State> states = eqClasses[i].states;
		for (size_t state = 0; state < states.size(); state++)
		{
			auto newTransitions = map.find(states[state].name);
			eqClasses[i].states[state].transitions.clear();
			eqClasses[i].states[state].transitions = newTransitions->second;
		}
	}
}

vector<pair<string, State>> CreateNewTable(vector<EqualityClass> const &eqClasses,
	unordered_map<string, vector<int>> const &eqvIndexes)
{
	vector<pair<string, State>> table;

	for (size_t eqClass = 0; eqClass < eqClasses.size(); eqClass++)
	{
		State state = eqClasses[eqClass].states[0];
		bool found = false;
		for (auto it = eqvIndexes.begin(); it != eqvIndexes.end(); it++)
		{
			for (size_t vertexName = 0; vertexName < it->second.size(); vertexName++)
			{
				if (to_string(it->second[vertexName] + 1) == state.name)
				{
					table.push_back(make_pair(it->first, state));
					found = true;
					break;
				}
			}
			if (found)
			{
				break;
			}
		}
	}

	return table;
}

unordered_map<string, vector<string>> CreateTransitionMatrix(ifstream &input)
{
	string str;
	getline(input, str);

	unordered_map<string, vector<string>> transitionMatrix;
	
	// Считали состояния
	getline(input, str);
	string state;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] != ' ')
		{
			state += str[i];
		}
		else
		{
			transitionMatrix[state] = vector<string>();
			state = "";
		}
	}
	transitionMatrix[state] = vector<string>();

	string transition;
	while (getline(input, str))
	{
		unordered_map<string, vector<string>>::iterator it = transitionMatrix.begin();
		for (size_t i = 0; i < str.size(); i++)
		{
			if (str[i] != ' ')
			{
				transition += str[i];
			}
			else
			{
				it->second.push_back(transition);
				it++;
				transition = "";
			}
		}
		it->second.push_back(transition);
		transition = "";
	}

	return transitionMatrix;
}

vector<pair<int, int>> FindEdges(vector<pair<string, State>> const &graph)
{
	vector<pair<int, int>> edges;
	size_t stateNum = 0;
	for (auto it = graph.begin(); it != graph.end(); it++)
	{
		int firstElement = stateNum;

		for (size_t i = 0; i < it->second.transitions.size(); i++)
		{
			int secondElement = stoi(it->second.transitions[i]) - 1;
			edges.push_back(make_pair(firstElement, secondElement));
		}
		stateNum++;
	}
	
	return edges;
}

vector<double> CalculateWeights(vector<pair<string, State>> const &graph)
{
	vector<double> weights;

	for (auto it = graph.begin(); it != graph.end(); it++)
	{
		for (size_t i = 0; i < it->second.transitions.size(); i++)
		{
			weights.push_back(i + 1);
		}
	}

	return weights;
}

void DrawMooreGraph(ofstream &output, vector<pair<string, State>> const &graphMatrix)
{
	using Edge = std::pair<int, int>;
	using Graph = boost::adjacency_list<
		boost::vecS,
		boost::vecS, boost::directedS,
		boost::property<boost::vertex_name_t, std::string>,
		boost::property<boost::edge_weight_t, double>
	>;

	const size_t VERTEX_COUNT = graphMatrix.size();
	vector<Edge> edges = FindEdges(graphMatrix);
	std::vector<double> weights = CalculateWeights(graphMatrix);

	Graph graph(edges.begin(), edges.end(), weights.begin(),
		VERTEX_COUNT);

	//using VertexesNames = boost::property_map<Graph, boost::vertex_name_t>::type;
	//VertexesNames vertexesNames = boost::get(boost::vertex_name, graph);

	boost::dynamic_properties dp;
	dp.property("weight", boost::get(boost::edge_weight,
		graph));
	dp.property("label", boost::get(boost::edge_weight,
		graph));
	//dp.property("node_id", vertexesNames);
	dp.property("node_id", boost::get(boost::vertex_index, graph));

	boost::write_graphviz_dp(output, graph, dp);
}

vector<string> ParseMealeyInput(string input)
{
	string stateName;
	string sygnal;

	vector<string> result;

	size_t i = 0;

	while (input[i] != '/')
	{
		stateName += input[i];
		i++;
	}
	i++;
	while (i < input.size())
	{
		sygnal += input[i];
		i++;
	}

	result.push_back(stateName);
	result.push_back(sygnal);

	return result;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
	vector<std::string> tokens;
	string token;
	istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

vector<pair<string, vector<string>>> ReadMealeyDataToMap(ifstream &input)
{
	// Создать начальную таблицу со значениями
	vector<string> rows;

	string line;

	getline(input, line);

	vector<pair<string, vector<string>>> states;

	string stateName;
	for (size_t i = 0; i < line.size(); i++)
	{
		if (line[i] != ' ')
		{
			stateName += line[i];
		}
		else
		{
			states.push_back(make_pair(stateName, vector<string>()));
			stateName = "";
		}
	}
	states.push_back(make_pair(stateName, vector<string>()));
	while (getline(input, line))
	{
		rows.push_back(line);
	}

	for (string row : rows)
	{
		vector<string> transitionsAndSygnals = split(row, ' ');

		for (size_t i = 0; i < transitionsAndSygnals.size(); i++)
		{
			vector<string> transitionAndSygnal = ParseMealeyInput(transitionsAndSygnals[i]);
			string transition = transitionAndSygnal[0];
			string sygnal = transitionAndSygnal[1];
			states[i].second.push_back(transition);
			states[i].second.push_back(sygnal);
		}
	}

	return states;
}

vector<EqualityClass> CreateEqvClasses(vector<pair<string, vector<string>>> const &mealey)
{
	vector<EqualityClass> classes;
	// Класс эквивалентности =  состояния + переходы
	vector<State> states;
	vector<vector<string>> sygnals;
	for (auto state : mealey)
	{
		State newState;
		newState.name = state.first;
		vector<string> stateSygnals;
		for (size_t i = 0; i < state.second.size(); i++)
		{
			if (i % 2 == 0)
			{
				newState.transitions.push_back(state.second[i]);
			}
			else
			{
				stateSygnals.push_back(state.second[i]);
			}
		}
		sygnals.push_back(stateSygnals);
		states.push_back(newState);
	}

	size_t classNum = 0;
	while (!states.empty())
	{
		EqualityClass newClass;
		newClass.name = to_string(classNum);
		newClass.states.push_back(*states.begin());
		states.erase(states.begin());

		vector<string> currState = *sygnals.begin();
		sygnals.erase(sygnals.begin());

		vector<size_t> indexesForErase;
		vector<State> statesForErase;
		vector<vector<string>> sygnalsForErase;

		for (size_t i = 0; i < sygnals.size(); i++)
		{
			if (currState == sygnals[i])
			{
				newClass.states.push_back(*find(states.begin(), states.end(), states[i]));
				indexesForErase.push_back(i);
			}
		}

		for (size_t i = 0; i < indexesForErase.size(); i++)
		{
			statesForErase.push_back(states[indexesForErase[i]]);
			sygnalsForErase.push_back(sygnals[indexesForErase[i]]);
		}

		for (size_t i = 0; i < indexesForErase.size(); i++)
		{
			states.erase(
				find(states.begin(), states.end(), statesForErase[i])
			);

			sygnals.erase(
				find(sygnals.begin(), sygnals.end(), sygnalsForErase[i])
			);
		}

		indexesForErase.clear();
		classes.push_back(newClass);
		classNum++;
	}

	return classes;
}

unordered_map<string, vector<string>> CreateTransitionMatrix(vector<pair<string, vector<string>>> states)
{
	unordered_map<string, vector<string>> transitionMatrix;

	// Состояние-переходы
	for (auto state : states)
	{
		for(size_t i = 0; i < state.second.size(); i++)
		{
			if (i % 2 == 0)
			{
				transitionMatrix[state.first].push_back(state.second[i]);
			}
		}
	}

	return transitionMatrix;
}

bool isMealey(ifstream &input)
{
	string line;
	bool result = false;
	getline(input, line);
	getline(input, line);
	for (auto &ch : line)
	{
		if (ch == '/')
		{
			result = true;
			break;
		}
	}
	input.close();
	input.open("dataFile.txt");

	return result;
}

vector<pair<string, State>> CreateNewTable(vector<EqualityClass> const &eqClasses,
	vector<pair<string, vector<string>>> const &states)
{
	vector<pair<string, State>> table;

	for (auto eqClass: eqClasses)
	{
		State newState = eqClass.states[0];

		for (auto &state : states)
		{
			if (state.first == newState.name)
			{
				for (size_t i = 0; i < state.second.size(); i++)
				{
					if (i % 2 != 0)
					{
						newState.sygnals.push_back(state.second[i]);
					}
				}
			}
		}
		table.push_back(make_pair(eqClass.name, newState));
	}

	return table;
}

void DrawMealeyGraph(ofstream &output, vector<pair<string, State>> const &graphMatrix)
{
	using Edge = std::pair<int, int>;
	using Graph = boost::adjacency_list<
		boost::vecS,
		boost::vecS, boost::directedS,
		boost::property<boost::vertex_name_t, std::string>,
		boost::property<boost::edge_weight_t, double>
	>;

	const size_t VERTEX_COUNT = graphMatrix.size();
	vector<Edge> edges = FindEdges(graphMatrix);
	std::vector<double> weights = CalculateWeights(graphMatrix);

	Graph graph(edges.begin(), edges.end(), weights.begin(),
		VERTEX_COUNT);

	//using VertexesNames = boost::property_map<Graph, boost::vertex_name_t>::type;
	//VertexesNames vertexesNames = boost::get(boost::vertex_name, graph);

	boost::dynamic_properties dp;
	dp.property("weight", boost::get(boost::edge_weight,
		graph));
	dp.property("label", boost::get(boost::edge_weight,
		graph));
	//dp.property("node_id", vertexesNames);
	dp.property("node_id", boost::get(boost::vertex_index, graph));

	boost::write_graphviz_dp(output, graph, dp);
}

int main()
{
	ifstream input("dataFile.txt");
	ofstream output("out.dot");

	if (isMealey(input))
	{
		vector<pair<string, vector<string>>> states = ReadMealeyDataToMap(input);
		vector<EqualityClass> eqvClasses = CreateEqvClasses(states);

		size_t prevClassesCount = 0;
		size_t currClassesCount = eqvClasses.size();

		unordered_map<string, string> statesInEqClasses;

		unordered_map<string, vector<string>> startTransitionMatrix = CreateTransitionMatrix(states);

		while (prevClassesCount != currClassesCount)
		{
			statesInEqClasses = FindEqClassOfStates(eqvClasses);

			ReplaceTransitions(eqvClasses, startTransitionMatrix);
			ReplaceTransitions(eqvClasses, statesInEqClasses);

			eqvClasses = RecalculateEqvClasses(eqvClasses);

			prevClassesCount = currClassesCount;
			currClassesCount = eqvClasses.size();
		}

		vector<pair<string, State>> newTable = CreateNewTable(eqvClasses, states);
		size_t name = 1;
		for (size_t i = 0; i < newTable.size(); i++)
		{
			cout
				<< newTable[i].first << " "
				<< newTable[i].second.transitions[0] << "/"
				<< newTable[i].second.sygnals[0] << " "
				<< newTable[i].second.transitions[1] << "/"
				<< newTable[i].second.sygnals[1] <<
				endl;
			name++;
		}

		DrawMealeyGraph(output, newTable);
	}
	else
	{
		unordered_map<string, vector<int>> eqvIndexes;
		ReadMooreDataToMap(eqvIndexes, input);

		vector<EqualityClass> eqvClasses = CreateEqvClassesAndStates(eqvIndexes, input);

		size_t prevClassesCount = 0;
		size_t currClassesCount = eqvClasses.size();

		unordered_map<string, string> statesInEqClasses;

		input.close();
		input.open("dataFile.txt");
		unordered_map<string, vector<string>> startTransitionMatrix = CreateTransitionMatrix(input);

		while (prevClassesCount != currClassesCount)
		{
			statesInEqClasses = FindEqClassOfStates(eqvClasses);
			ReplaceTransitions(eqvClasses, startTransitionMatrix);
			ReplaceTransitions(eqvClasses, statesInEqClasses);

			eqvClasses = RecalculateEqvClasses(eqvClasses);

			prevClassesCount = currClassesCount;
			currClassesCount = eqvClasses.size();
		}

		vector<pair<string, State>> newTable = CreateNewTable(eqvClasses, eqvIndexes);
		size_t name = 1;
		for (size_t i = 0; i < newTable.size(); i++)
		{
			cout << newTable[i].first << ": " << name << " "
			<< newTable[i].second.transitions[0] << " " << newTable[i].second.transitions[1] << endl;
			name++;
		}

		DrawMooreGraph(output, newTable);
	}

    return 0;
}