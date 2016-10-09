#ifndef IZZCONTAINER_H
#define IZZCONTAINER_H

template<typename T>
class IZZContainer
{
  public:
    IZZContainer() {};
    virtual ~IZZContainer() {};

    virtual void Set(int y, int x, T value) = 0;
    virtual T Get(int y, int x) = 0;

    virtual int GetSizeY() = 0;
    virtual int GetSizeX() = 0;
    virtual void Init(T init) = 0;
    virtual void Print() = 0;
};

#endif // IZZCONTAINER_H
