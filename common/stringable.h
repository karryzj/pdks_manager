#ifndef STRINGABLE_H
#define STRINGABLE_H

#endif // STRINGABLE_H

#include <iostream>

// HINT@leixunyong。这个文件好像没什么用。没有在任何地方用到。
namespace cm
{

class Stringable
{
public:
    Stringable();
    ~Stringable();

    // need implemented in the subclass. for logging or other use.
    virtual std::string to_str() = 0;
};

}
