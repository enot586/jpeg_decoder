#ifndef ZIGZAGMATRIX_H
#define ZIGZAGMATRIX_H

template<typename T, int M, int N>
class ZZMatrix
{
  private:
    const int sizeX = N;
    const int sizeY = M;

    T m[M][N];

    int x = 0;
    int y = 0;

    bool lastActionIsStep = false;
    bool direction = true;

  public:
    ZZMatrix();
    ~ZZMatrix();

    void Init(T init) {
      for (int i = 0; i<sizeY; ++i) {
        for (int j = 0; j<sizeX; ++j) {
          m[i][j] = init;
        }
      }
    }

    void Reset() {
      x = 0;
      y = 0;
      lastActionIsStep = false;
      direction = true;
    }

    void Push(T el);

    T Get(int y, int x) {
      return m[y][x];
    }

    void NextStep();

    bool IsTopBorderAchiev()    { return (y == 0); };
    bool IsLeftBorderAchive()   { return (x == 0); };
    bool IsRightBorderAchive()  { return (x == sizeX - 1); };
    bool IsBottomBorderAchive() { return (y == sizeY - 1); };

  protected:
    void GotoRight();
    void GotoDown();
    void GotoUpDiagonal();
    void GotoDownDiagonal();
};

template<typename T, int M, int N>
ZZMatrix<T,M,N>::ZZMatrix() {
  direction = true;
  Reset();
}

template<typename T, int M, int N>
ZZMatrix<T,M,N>::~ZZMatrix()
{
  //dtor
}

template<typename T, int M, int N>
void ZZMatrix<T,M,N>::GotoRight() {
  if ( x < sizeX-1 ) {
    ++x;
  }
}

template<typename T, int M, int N>
void ZZMatrix<T,M,N>::GotoDown() {
  if ( y < sizeY-1 ) {
    ++y;
  }
}

template<typename T, int M, int N>
void ZZMatrix<T,M,N>::GotoUpDiagonal() {
  if ( (y > 0) && (x < sizeX-1) ) {
    --y;
    ++x;
  }
}

template<typename T, int M, int N>
void ZZMatrix<T,M,N>::GotoDownDiagonal() {
  if ( (y < sizeY-1) && (x > 0) ) {
    ++y;
    --x;
  }
}

template<typename T, int M, int N>
void ZZMatrix<T,M,N>::NextStep() {
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

template<typename T, int M, int N>
void ZZMatrix<T,M,N>::Push(T el) {
  m[y][x] = el;

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




#endif // ZIGZAGMATRIX_H
