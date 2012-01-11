// FrostTools.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "frostTools.h"

class Point: public Listening<Point>
{
	friend class Edge;
	int x;
	int y;
public:
	Point(int nx,int ny)
		:x(nx),y(ny)
	{}
	void set(int nx,int ny)
	{
		x=nx;
		y=ny;
		updateListeners();
	}
};
class P: public ListenerSingle<Point>
{
public:
	P(Point *pt)
		:ListenerSingle(pt)
	{}
	virtual void onUpdate(Point *c)
	{
	}
};
class Edge: public ListenerMultiple<Point>
{
	Point *p[2];
	float length;
public:
	Edge(Point *a,Point *b)
		:length(0)
	{
		p[0]=a;
		p[1]=b;
		a->attach(this);
		b->attach(this);		
		recalc();
	}
	void recalc()
	{
		length=sqrtf((p[0]->x-p[1]->x)*(p[0]->x-p[1]->x)+(p[0]->y-p[1]->y)*(p[0]->y-p[1]->y));
	}
	virtual void onUpdate(Point *c)
	{
		recalc();
	}
};

int listenerTest()
{
	NameManager<char> nameManager;
	IDManager<char*> idGenerator;
	std::string str1=nameManager.getName("name A");
	std::string str2=nameManager.getName("name A");
	std::string str3=nameManager.getName("name A");
	std::string str4=nameManager.getName("name A1");
	const int n=10;
	int id[n];
	for(int i=0;i<n;i++)
		id[i]=idGenerator.genID(NULL);

	idGenerator.freeID(4);
	int id1=idGenerator.genID(NULL);
	int id2=idGenerator.genID(NULL);

	Point a(1,4);
	Point b(2,5);
	Edge d(&a,&b);
	a.set(5,5);
	b.set(7,7);
	

	return 0;
}

