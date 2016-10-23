#ifndef ZIGZAGMATRIX_H
#define ZIGZAGMATRIX_H

#include <opencv2/core/core.hpp>
#include <iostream>

template<typename T>
class ZZMatrix
{
  private:
    const int sizeX;
    const int sizeY;

    std::vector< std::vector<T> > m;

    int x = 0;
    int y = 0;

    bool lastActionIsStep = false;
    bool direction = true;

  public:
    ZZMatrix(int M, int N);
    ~ZZMatrix();

    void Init(const T& init) {
      for (int i = 0; i<sizeY; ++i) {
        for (int j = 0; j<sizeX; ++j) {
          m.at(i).at(j) = init;
        }
      }
    }

    int Columns() { return sizeX; };
    int Rows()    { return sizeY; };

    void Print();

    void Reset() {
      x = 0;
      y = 0;
      lastActionIsStep = false;
      direction = true;
    }

    void Push(T el);

    T Get(int y, int x) {
      return m.at(y).at(x);
    }

    void NextStep();

    bool IsTopBorderAchiev()    { return (y == 0); };
    bool IsLeftBorderAchive()   { return (x == 0); };
    bool IsRightBorderAchive()  { return (x == sizeX - 1); };
    bool IsBottomBorderAchive() { return (y == sizeY - 1); };

    int GetCurrentY();
    int GetCurrentX();

    void ConvertTo_CV32FC1(cv::Mat& cvMatrix);

  protected:
    void GotoRight();
    void GotoDown();
    void GotoUpDiagonal();
    void GotoDownDiagonal();
};

template<typename T>
ZZMatrix<T>::ZZMatrix(int M, int N): sizeY(M),sizeX(N) {

  m.resize(sizeY);
  for (int i = 0; i < sizeY; ++i) {
    m[i].resize(sizeX);
  }

  direction = true;
  Reset();
}

template<typename T>
ZZMatrix<T>::~ZZMatrix()
{
  //dtor
}

template<typename T>
void ZZMatrix<T>::GotoRight() {
  if ( x < sizeX-1 ) {
    ++x;
  }
}

template<typename T>
void ZZMatrix<T>::GotoDown() {
  if ( y < sizeY-1 ) {
    ++y;
  }
}

template<typename T>
void ZZMatrix<T>::GotoUpDiagonal() {
  if ( (y > 0) && (x < sizeX-1) ) {
    --y;
    ++x;
  }
}

template<typename T>
void ZZMatrix<T>::GotoDownDiagonal() {
  if ( (y < sizeY-1) && (x > 0) ) {
    ++y;
    --x;
  }
}

template<typename T>
void ZZMatrix<T>::NextStep() {
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
void ZZMatrix<T>::Push(T el) {
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

template<typename T>
int ZZMatrix<T>::GetCurrentX() {
  return x;
}

template<typename T>
int ZZMatrix<T>::GetCurrentY() {
  return y;
}

template<typename T>
void ZZMatrix<T>::Print() {

  for (int i = 0; i < sizeY; ++i) {
    for (int j = 0; j < sizeX; ++j) {
      std::cout << std::dec << static_cast<int16_t>(m[i][j]) << "\t";
    }
    std::cout << std::endl;
  }

}

template<typename T>
void ZZMatrix<T>::ConvertTo_CV32FC1(cv::Mat& cvMatrix) {
  cvMatrix.create(sizeY, sizeX, CV_32FC1);

  for (int i = 0; i < sizeY; ++i) {
    for (int j = 0; j < sizeX; ++j) {
      cvMatrix.at<float>(i,j) = m[i][j];
    }
  }

}


#endif // ZIGZAGMATRIX_H
