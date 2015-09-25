#ifndef simple_node_h
#define simple_node_h

#include <iostream>


class simple_node {
private:
    int id;
    int rootId;
public:
    simple_node()
    :id(0),
            rootId(0)
    {
    }
    simple_node(int id_)
    {
        id = id_;
        rootId = 0;
    }
    simple_node(int id_,int rootId_)
    {
        id = id_;
        rootId = rootId_;
    }
    int getRootId()
    {
        return rootId;
    }
    simple_node &operator=(const simple_node& other)
    {
        id = other.id;
        rootId = other.rootId;
        return *this;
    }
    friend bool operator<(const simple_node& lhs,const simple_node& rhs);
    friend bool operator>(const simple_node& lhs,const simple_node& rhs);
    friend bool operator==(const simple_node& lhs,const simple_node& rhs);
    friend bool operator>=(const simple_node& lhs,const simple_node& rhs);
    friend std::ostream &operator<<(std::ostream &output,const simple_node& sn);
    friend std::istream &operator>>(std::istream &input,simple_node& sn);
};

bool operator<(const simple_node& lhs,const simple_node& rhs)
{
    return (lhs.id < rhs.id);
}
bool operator>(const simple_node& lhs,const simple_node& rhs)
{
    return (lhs.id > rhs.id);
}
bool operator==(const simple_node& lhs,const simple_node& rhs)
{
    return (lhs.id == rhs.id);
}
bool operator>=(const simple_node& lhs,const simple_node& rhs)
{
    return !(lhs < rhs);
}
bool operator<=(const simple_node& lhs,const simple_node& rhs)
{
    return !(lhs > rhs);
}
std::ostream
        &operator<<(std::ostream &output,const simple_node& sn)
{
    output << "(" << sn.id << "," << sn.rootId << ")";
    return output;
}
std::istream &operator>>(std::istream &input,simple_node& sn)
{
    input >> sn.id >> sn.rootId;
    return input;
}

#endif