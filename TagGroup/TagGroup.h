#ifndef __TAG_GROUP__H___
#define __TAG_GROUP__H___

#include "Tag.h"
#include "Linklist.h"

class TagGroup 
{
private:
    /* data */
    LinkList<Tag> tags;
public:
    TagGroup(/* args */);
    ~TagGroup();

Tag * Add(isType type,const char * name,size_t array_size=1,isMemory memtype = isMemory::RAM)
        {
            if(array_size == 0) 
                return nullptr;

            bool monitor;

            Tag *newItem = (Tag*)Allocator(memtype,array_size,sizeof(Tag));

            monitor = (newItem != nullptr);
            if(!monitor)
            return nullptr;

            newItem->_name.size = 0;
            newItem->_data.size = 0;

            newItem->_data.size = SchematicPoint[type].element_size * array_size;
            newItem->_option._memType = memtype;
            newItem->_option._type = type;

            monitor = newItem->SetName(name);
            if(!monitor)
            return nullptr;

            newItem->_data.value = (uint8_t*)Allocator(memtype,array_size,SchematicPoint[type].element_size);

            monitor = (newItem->_data.value != nullptr);
            if(!monitor)
            return nullptr;

            *cur = newItem;
            if(cur_first != nullptr)
                (*cur)->_first = (*cur_first);

            return *cur;
        }

Tag * Add(isType type,const char * name,void *addr,size_t array_size=1,isMemory memtype = isMemory::RAM)
        {
            if(array_size == 0)
                return nullptr;

            bool monitor;
            Tag **cur = &_Item;
            Tag **cur_first = &_Item;

            /* 1. Find last elements for connect */
            while ((cur != nullptr) && (*cur != nullptr))
            {
                cur = &(*cur)->_next;
            }

            Tag *newItem = (Tag*)Allocator(memtype,array_size,sizeof(Tag));

            monitor = (newItem != nullptr);
            if(!monitor)
            return nullptr;

            newItem->_name.size = 0;
            newItem->_data.size = 0;

            newItem->_data.size = SchematicPoint[type].element_size * array_size;
            newItem->_option._memType = memtype;
            newItem->_option._type = type;

            monitor = newItem->SetName(name);
            if(!monitor)
            return nullptr;

            newItem->_data.value = (uint8_t*)addr;

            monitor = (newItem->_data.value != nullptr);
            if(!monitor)
            return nullptr;

            *cur = newItem;
            (*cur)->_first = (*cur_first);

            return *cur;
        }

Tag * Find(const char *name)
        {
            bool monitor;
            Tag *cur = _Item;

            while(cur != nullptr)
            {
                monitor = (strcmp(name,(char*)cur->_name.value) == 0);
                if(monitor)
                    return cur;

                cur = cur->_next;
            }

            return nullptr;
        }

Tag * FindByIndex(size_t index)
        {
            bool monitor;
            size_t count = 0;
            Tag **cur = &_Item;

            while((cur != nullptr) && ((*cur) != nullptr))
            {
                monitor = (index == count);
                if(monitor)
                    return *cur;
                    
                cur = &(*cur)->_next;
                count+=1;
            }

            return nullptr;
        }

int FindIndex(const char *name)
        {
            int count = 0;
            bool monitor;
            Tag **cur = &_Item;

            while((cur != nullptr) && ((*cur) != nullptr))
            {
                monitor = (strcmp(name,(char*)(*cur)->_name.value) == 0);
                if(monitor)
                    return count;

                cur = &(*cur)->_next;
                count+=1;
            }

            return -1;
        }

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

void Remove(const char * name)
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