#pragma once
#include <vector>
#include <deque>
enum STATUS{OPEN, CLOSED, UNVISITED};
enum NODE_TYPE{WALL, SPACE, GOAL, START};

struct node
{
	public:
		int IDx;
		int IDy;
		STATUS status;
		NODE_TYPE type;
		std::vector<node*> connections;
		node* parent;
		float F;
		float G;
};

class PathfinderUtils
{
	public:
		static PathfinderUtils& getInstance()
		{
			static PathfinderUtils _instance;
			return _instance;
		}

		static std::deque<node*> Astar(node* goal, node* start);
		

	private:
		PathfinderUtils() {}
		~PathfinderUtils() {}
		PathfinderUtils(const PathfinderUtils&);

		static float calculateHeuristic(node* goal, node* current);
		static std::deque<node*> open;
		static std::deque<node*> closed;
		static std::deque<node*> finalPath;
};