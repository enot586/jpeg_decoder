#ifndef ZIGZAGMATRIX_H
#define ZIGZAGMATRIX_H

#include <iostream>
#include "IZZContainer.h"

template<typename T>
class ZZAdapter
{
  private:
    int x = 0;
    int y = 0;

    bool lastActionIsStep = false;
    bool direction = true;

    IZZContainer<T>& container;

  public:
    ZZAdapter(IZZContainer<T>& container_);
    ~ZZAdapter();

    void Print();

    void Reset() {
      x = 0;
      y = 0;
      lastActionIsStep = false;
      direction = true;
      container.Init();
    }

    void Push(T el);

    void NextStep();

    bool IsTopBorderAchiev()    { return (y == 0); };
    bool IsLeftBorderAchive()   { return (x == 0); };
    bool IsRightBorderAchive()  { return (x == container.GetSizeX() - 1); };
    bool IsBottomBorderAchive() { return (y == container.GetSizeY() - 1); };

    int GetCurrentY();
    int GetCurrentX();

  protected:
    void GotoRight();
    void GotoDown();
    void GotoUpDiagonal();
    void GotoDownDiagonal();
};

template<typename T>
ZZAdapter<T>::ZZAdapter(IZZContainer<T>& container_) : container(container_) {
  direction = true;
  Reset();
}

template<typename T>
ZZAdapter<T>::~ZZAdapter()
{
  //dtor
}

template<typename T>
void ZZAdapter<T>::GotoRight() {
  if ( x < container.GetSizeX()-1 ) {
    ++x;
  }
}

template<typename T>
void ZZAdapter<T>::GotoDown() {
  if ( y < container.GetSizeY()-1 ) {
    ++y;
  }
}

template<typename T>
void ZZAdapter<T>::GotoUpDiagonal() {
  if ( (y > 0) && (x < container.GetSizeX()-1) ) {
    --y;
    ++x;
  }
}

template<typename T>
void ZZAdapter<T>::GotoDownDiagonal() {
  if ( (y < container.GetSizeY()-1) && (x > 0) ) {
    ++y;
    --x;
  }
}

template<typename T>
void ZZAdapter<T>::NextStep() {
  if (IsTopBorderAchiev() && IsLeftBorderAchive()) {
    GotoRight();
    direction = false;
  } else if (IsLeftBorderAchive() && !IsBottomBorderAchive()) {
    GotoDown();
    direction = true;
  } else if (IsTopBorderAchiev() && !IsRightBorderAchive()) {
    GotoRight();
    direction = false;
  } else if (IsBottomBorderAchive() && !IsRightBorderAchive()) {
    GotoRight();
    direction = true;
  } else if (!IsBottomBorderAchive() && IsRightBorderAchive()) {
    GotoDown();
    direction = false;
  } else if (IsRightBorderAchive() && IsBottomBorderAchive()) {
    Reset();
  }
}

template<typename T>
void ZZAdapter<T>::Push(T el) {

  container.Set(y, x, el);

  if ( (IsTopBorderAchiev() || IsLeftBorderAchive() ||
        IsRightBorderAchive() || IsBottomBorderAchive()) && !lastActionIsStep ) {
    NextStep();
    lastActionIsStep = true;
  } else {
    lastActionIsStep = false;
    if (direction) {
      GotoUpDiagonal();
    } else {
      GotoDownDiagonal();
    }
  }
}

template<typename T>
int ZZAdapter<T>::GetCurrentX() {
  return x;
}

template<typename T>
int ZZAdapter<T>::GetCurrentY() {
  return y;
}


#endif // ZIGZAGMATRIX_H
