#pragma once

#include <memory>
#include <vector>
#include "Actor.h"
#include "Utils.h"

struct QuadtreeNode
{
   QuadtreeNode(float x, float z, float s)
   : 
     SWCornerX{x}, SWCornerZ{z}, size{s},
     SWChild{}, NWChild{}, NEChild{}, SEChild{},
     nodeActors{}
   {

   }
   
	int numberAsteroidsIntersected() const noexcept;
	void addIntersectingAsteroidsToList();
	void build(int currHeight);
    void drawObjects(const Area& area);
    std::vector<Actor*> getActorsCloseToArea(const Area& area) const;

private:

    Area getOwnArea() const noexcept
    {
        return {
            {SWCornerX, 0.f, SWCornerZ},
            {SWCornerX, 0.f, SWCornerZ - size},
            {SWCornerX + size, 0.f, SWCornerZ - size},
            {SWCornerX + size, 0.f, SWCornerZ}
        };
    }

    void getActorsCloseToAreaImpl(const Area& area, std::vector<Actor*>& closeActors, const QuadtreeNode& prevNode) const;

    bool isTimeToStopPartitioning(int height) const noexcept;

    void fillNodeActorsIn(std::vector<Actor*>& closeActors) const;


	float SWCornerX, SWCornerZ;
	float size;
    
    std::unique_ptr<QuadtreeNode> SWChild;
    std::unique_ptr<QuadtreeNode> NWChild;
    std::unique_ptr<QuadtreeNode> NEChild;
    std::unique_ptr<QuadtreeNode> SEChild;

	std::vector<Actor*> nodeActors;

	friend class Quadtree;
};

struct Quadtree
{
    Quadtree()
        :
        header{}
    {

    }
	
    void init(float x, float z, float s);
	void drawObjects(const Area& area);

    std::vector<Actor*> getActorsCloseToArea(const Area& area) const;

    private:
    std::unique_ptr<QuadtreeNode> header;
};



