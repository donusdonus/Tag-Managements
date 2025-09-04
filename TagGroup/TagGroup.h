#ifndef __TAG_GROUP__H___
#define __TAG_GROUP__H___

#include "Tag.h"
#include "Linklist.h"
#include <functional>

using namespace std;

class TagGroup 
{
private:
    /* data */

    LinkList<Tag> Tags;
public:

    TagGroup(/* args */);
    ~TagGroup();

    typedef std::function<void (Tag*,size_t)> func_search;

    bool Add(isType type,const char * name,size_t array_size=1,isMemory memtype = isMemory::RAM);

    bool Add(isType type,const char * name,void *addr,size_t array_size=1,isMemory memtype = isMemory::RAM);

    void ForEach(func_search func = nullptr);

    Tag * FindByIndex(size_t index);

    Tag * FindByName(char *name);

    int * FindIndexByName(const char *name);

    
void Remove(size_t index)
        {
            bool monitor;
            int count = 0;
            Tag **cur_prev = nullptr;
            Tag **cur = &_Item;


            while((cur != nullptr)&&((*cur) != nullptr))
            {
                Tag *tmp = nullptr;

                monitor = (count == index);
                if(monitor)
                {
                    tmp = (*cur)->_next;
                    (*cur)->Free();
                    free(*cur);
                    *cur = tmp;

                    if((cur_prev != nullptr) && ((*cur_prev) != nullptr))
                        (*cur_prev)->_next = *cur;

                    return ;
                }

                cur_prev = cur;
                cur = &(*cur)->_next; 
                count++;
            }
        }

void Remove(char * name)
        {
           int index = FindIndex(name);

           if(index == -1) return;

           Remove(index);
        }

void RemoveAll()
        {
            Tag **cur = &_Item;

            
            while((cur != nullptr) && ((*cur) != nullptr ))
            {
                Tag **tmp = nullptr;
                tmp = cur;
                *cur = (*cur)->_next;
                (*tmp)->Free();
                free(*tmp);
                *tmp = nullptr;
            }
        }

size_t Count()
        {
            Tag **cur = &_Item;
            size_t count = 0;
            
            while((cur != nullptr) && ((*cur) != nullptr))
            {   
                cur = &(*cur)->_next;
                count+=1;
            }

            return count;
        }    



};

TagGroup::TagGroup(/* args */)
{
}

TagGroup::~TagGroup()
{
}



#endif