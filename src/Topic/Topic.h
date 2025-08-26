#ifndef __TOPIC_H__
#define __TOPIC_H__

#include <stdio.h>
#include "TagGroup.h"

class Topic
{
private:
    /* data */
    RawMemory _name;       /**< Name of the group */
    isOption _option;
    TagGroup * _group = nullptr;
    Topic *_first = nullptr;
    Topic *_next = nullptr;

public:
    Topic(/* args */);
    ~Topic();
};

Topic::Topic(/* args */)
{
}

Topic::~Topic()
{
}





#endif