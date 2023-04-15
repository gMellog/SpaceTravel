        #include "Quadtree.h"
        #include "SpaceTravel.h"
        #include "Intersections.h"
        #include "SimpleAsteroid.h"
        #include "GoldenAsteroid.h"
        #include "SpaceTravel.h"

        int QuadtreeNode::numberAsteroidsIntersected() const noexcept
        {
            int r{};

             for (const auto& actor : SpaceTravel::actors)
             {
                for (const auto& tag : actor->tags)
                {
                    if (tag != Spacecraft::tag)
                    {
                        float R;

                        if(tag == Tags::AsteroidTag)
                        {
                            R = static_cast<SimpleAsteroid*>(actor.get())->getRadius();
                        }
                        else if(tag == Tags::GoldenAsteroidTag)
                        {
                            R = static_cast<GoldenAsteroid*>(actor.get())->getRadius();
                        }

                        if(Intersections::checkDiscRectangleIntersection( 
                                {SWCornerX, 0.f, SWCornerZ}, {SWCornerX + size, 0.f, SWCornerZ - size}, 
                                actor->getTransform().translation, R))
                        {
                            ++r;
                        }
                    }
                }
            }

            return r;          
        }

        void QuadtreeNode::addIntersectingAsteroidsToList()
        {
             for (const auto& actor : SpaceTravel::actors)
             {
                for (const auto& tag : actor->tags)
                {
                    if (tag != Spacecraft::tag)
                    {
                        float R;

                        if(tag == Tags::AsteroidTag)
                        {
                            R = static_cast<SimpleAsteroid*>(actor.get())->getRadius();
                        }
                        else if(tag == Tags::GoldenAsteroidTag)
                        {
                            R = static_cast<GoldenAsteroid*>(actor.get())->getRadius();
                        }
                        else
                        {
                            std::cerr << "Not recognized object!\n";
                            return;
                        }

                        if(Intersections::checkDiscRectangleIntersection( 
                                {SWCornerX, 0.f, SWCornerZ}, {SWCornerX + size, 0.f, SWCornerZ - size}, 
                                actor->getTransform().translation, R))
                        {
                            auto nodeActor = actor.get();
                            nodeActors.push_back(nodeActor);

                            for(const auto& tag : nodeActor->tags)
                            {
                                if(tag == Tags::GoldenAsteroidTag)
                                {
                                    auto goldenAsteroid = static_cast<GoldenAsteroid*>(nodeActor);
                                    goldenAsteroid->onDie(
                                        [nodeActor, this]() 
                                    {
                                        auto it = std::remove_if( std::begin(nodeActors), std::end(nodeActors), 
                                            [nodeActor]
                                            (const Actor* actor)
                                            {
                                                return nodeActor == actor;
                                            });

                                        nodeActors.erase(it, std::end(nodeActors));      
                                    }
                                    );
                                    break;
                                }
                            }
                        }
                    }
                }
            }

        }
        
        bool QuadtreeNode::isTimeToStopPartitioning(int height) const noexcept
        {
            return (numberAsteroidsIntersected() <= (SpaceTravel::getMinStoreActors())) || 
                   ( height >= SpaceTravel::getMaxQuadtreeHeight());
        }

        void QuadtreeNode::build(int height)
        {
            if(isTimeToStopPartitioning(height)) 
            {
                addIntersectingAsteroidsToList();
            }
            else
            {
                SWChild = std::make_unique<QuadtreeNode>(SWCornerX, SWCornerZ, size / 2.0);
                NWChild = std::make_unique<QuadtreeNode>(SWCornerX, SWCornerZ - size / 2.0, size / 2.0);
                NEChild = std::make_unique<QuadtreeNode>(SWCornerX + size / 2.0, SWCornerZ - size / 2.0, size / 2.0);
                SEChild = std::make_unique<QuadtreeNode>(SWCornerX + size / 2.0, SWCornerZ, size / 2.0);

                SWChild->build(height + 1);
                NWChild->build(height + 1);
                NEChild->build(height + 1);
                SEChild->build(height + 1);
            }
        }

        std::vector<Actor*> QuadtreeNode::getActorsCloseToArea(const Area& area) const
        {
            std::vector<Actor*> r;

            if(SWChild != nullptr)
            {
                SWChild->getActorsCloseToAreaImpl(area, r, *this);
                NWChild->getActorsCloseToAreaImpl(area, r, *this);
                NEChild->getActorsCloseToAreaImpl(area, r, *this);
                SEChild->getActorsCloseToAreaImpl(area, r, *this);
            }
            else
            {
                fillNodeActorsIn(r);
            }

            return r;
        }

        void QuadtreeNode::getActorsCloseToAreaImpl(const Area& area, std::vector<Actor*>& closeActors, const QuadtreeNode& prevNode) const
        {
            if(Intersections::checkQuadrilateralsIntersection(area, getOwnArea()))
            {   
                if(SWChild == nullptr)
                {
                    fillNodeActorsIn(closeActors);
                }
                else
                {
                    SWChild->getActorsCloseToAreaImpl(area, closeActors, *this);
                    NWChild->getActorsCloseToAreaImpl(area, closeActors, *this);
                    NEChild->getActorsCloseToAreaImpl(area, closeActors, *this);
                    SEChild->getActorsCloseToAreaImpl(area, closeActors, *this);
                }
            }
            else
            {
                prevNode.fillNodeActorsIn(closeActors);
            }
        }

        void QuadtreeNode::fillNodeActorsIn(std::vector<Actor*>& closeActors) const
        {
            for(auto nodeActor : nodeActors)
            {
                closeActors.push_back(nodeActor);
            }
        }

        void QuadtreeNode::drawObjects(const Area& area)
        {
            if(Intersections::checkQuadrilateralsIntersection(area, getOwnArea()))
            {
                if(SWChild == nullptr)
                {
                    for(auto nodeActor : nodeActors)
                    {
                        if(!nodeActor->isTickCalled())
                            nodeActor->tick(SpaceTravel::getDeltaTime());
                    }
                }
                else
                {
                    SWChild->drawObjects(area);
                    NWChild->drawObjects(area);
                    NEChild->drawObjects(area);
                    SEChild->drawObjects(area);
                }
            }
        }

        void Quadtree::init(float x, float z, float s)
        {
            header = std::make_unique<QuadtreeNode>(x, z, s);
            
            const int startupTreeHeight = 1;
            header->build(startupTreeHeight);
        }

        void Quadtree::drawObjects(const Area& area)
        {
            header->drawObjects(area);
        }

        std::vector<Actor*> Quadtree::getActorsCloseToArea(const Area& area) const
        {
            return header->getActorsCloseToArea(area);
        }
