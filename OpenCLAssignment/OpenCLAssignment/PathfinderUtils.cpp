#include "PathfinderUtils.h"

std::deque<node*> PathfinderUtils::open = {};
std::deque<node*> PathfinderUtils::closed = {};
std::deque<node*> PathfinderUtils::finalPath = {};

std::deque<node*> PathfinderUtils::Astar(node* goal, node* start)
{
	node* currentNode;
	std::deque<node*>::iterator currentIT;
	open.push_back(start);


	while (open.size() != 0)
	{
		currentNode = open[0];
		currentIT = open.begin();
		//search for smallest F
		for (std::deque<node*>::iterator it = open.begin(); it != open.end(); ++it)
		{
			if ((*it)->F < currentNode->F)
			{
				currentNode = *it;
				currentIT = it;
			}
		}

		//switch to closed list
		closed.push_back(currentNode);
		open.erase(currentIT);
		currentNode->status = CLOSED;

		if (currentNode->type == GOAL)
		{
			break;
		}

		for each (node* connection in currentNode->connections)
		{
			if (connection->status != CLOSED)
			{
				switch (connection->type)
				{
					case WALL://ignore walls
						break;
					case SPACE:
						if (connection->status != OPEN)
						{
							open.push_back(connection);
							connection->status = OPEN;

							connection->parent = currentNode;
							connection->G = currentNode->G + 1;
							connection->F = connection->G + calculateHeuristic(goal, connection);
						}
						else
						{
							if (connection->G > (currentNode->G + 1))
							{
								connection->parent = currentNode;
								connection->G = currentNode->G + 1;
								connection->F = connection->G + calculateHeuristic(goal, connection);
							}
						}
						break;
					case GOAL:
						if (connection->status != OPEN)
						{
							open.push_back(connection);
							connection->status = OPEN;

							connection->parent = currentNode;
							connection->G = currentNode->G + 1;
							connection->F = connection->G + calculateHeuristic(goal, connection);
						}
						else
						{
							if (connection->G > (currentNode->G + 1))
							{
								connection->parent = currentNode;
								connection->G = currentNode->G + 1;
								connection->F = connection->G + calculateHeuristic(goal, connection);
							}
						}
						break;

				}
			}
		}


	}

	if (goal->status == CLOSED)
	{
		node* current = closed[closed.size() - 1];

		while (current->type != START)
		{
			finalPath.push_front(current);
			current = current->parent;
		}
		finalPath.push_front(current);//add the start node
	}
	return finalPath;
}

float PathfinderUtils::calculateHeuristic(node* goal, node* current)
{
	return (float)(abs(current->IDx - goal->IDx) + abs(current->IDy - goal->IDy));
}
