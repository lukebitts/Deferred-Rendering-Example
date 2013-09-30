#ifndef _IDEFERREDLIGHT_H_
#define _IDEFERREDLIGHT_H_

class Mesh;
class Program;

class IDeferredLight
{
    public:
        virtual const Mesh& mesh() = 0;
        virtual const Program& program() = 0;
        virtual ~IDeferredLight() {};
};

#endif // _IDEFERREDLIGHT_H_
