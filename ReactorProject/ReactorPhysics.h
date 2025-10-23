#ifndef REACTOR_PHYSICS_H
#define REACTOR_PHYSICS_H

// Reactor physics class. Needed to make instances of each reactor. 
// Still ahve to implement in real time changing of the reactor and having 
// independent values for each reactor. Will investigate later
class ReactorPhysics {
public:
    ReactorPhysics();   // constructor (no arguments needed)
    void update();      // updates all internal values

    float macro;
    float reactionRate;
    float k;
    //float power;
};

#endif
