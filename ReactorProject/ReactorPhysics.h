#ifndef REACTOR_PHYSICS_H
#define REACTOR_PHYSICS_H

class ReactorPhysics {
public:
    ReactorPhysics();   // constructor (no arguments needed)
    void update();      // updates all internal values

    float macro;
    float reactionRate;
    float k;
};

#endif
