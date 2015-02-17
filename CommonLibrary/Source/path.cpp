#include "path.h"


Path::Path(Clearing *c1, Clearing *c2, bool isHidden){
	hidden = isHidden;
    border = false;
	clearing1 = c1;
	clearing2 = c2;
}

//for a path that leaves a tile.
Path::Path(Clearing* c1)
{
    //border paths cannot be secret passages or hidden paths
    hidden = false;
    border = true;
    clearing1 = c1;
    clearing2 = NULL;
}

Path::~Path()
{
    cout << "Destructor called for Path" << endl;
    if(clearing1)
        clearing1->removePath(this);
    if(clearing2)
        clearing2->removePath(this);

}

void Path::getBothEndPoints(Clearing* c1, Clearing* c2)
{
    if(c1 == NULL || c2 == NULL)
        cout << "WARN: getBothEndPoints(c1, c2) c1 || c2 not Null, overwriting pointer" << endl; 
    c1 = clearing1;
    c2 = clearing2;
}

Clearing* Path::getEnd(Clearing* startPoint)
{
     return startPoint == clearing1 ? clearing2 : clearing1;
}

bool Path::isHidden()
{
    return hidden;
}