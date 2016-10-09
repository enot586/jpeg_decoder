#ifndef ZZCONTAINERSTDMAT_H
#define ZZCONTAINERSTDMAT_H

#include "IZZContainer.h"

template<typename T, int M, int N>
class ZZStdMat : IZZContainer<T> {
  private:
    T m[M][N];

  public:
    ZZStdMat() {}
    virtual ~ZZStdMat() {}

    virtual void Set(int y, int x, T value);
    virtual T Get(int y, int x);
    virtual int GetSizeY();
    virtual int GetSizeX();
    virtual void Init(T init);
    virtual void Print();
};

template<typename T, int M, int N>
void ZZStdMat<T,M,N>::Init(T init) {
  for (int i = 0; i<M; ++i) {
    for (int j = 0; j<N; ++j) {
      Set(i, j, init);
    }
  }
}

template<typename T, int M, int N>
void ZZStdMat<T,M,N>::Set(int y, int x, T value) {
  m[y][x] = value;
}

template<typename T, int M, int N>
T ZZStdMat<T,M,N>::Get(int y, int x) {
  return m[y][x];
}

template<typename T, int M, int N>
int ZZStdMat<T,M,N>::GetSizeY() {
  return M;
}

template<typename T, int M, int N>
int ZZStdMat<T,M,N>::GetSizeX() {
  return N;
}

template<typename T, int M, int N>
void ZZStdMat<T,M,N>::Print() {

  for (int i = 0; i < GetSizeY(); ++i) {
    for (int j = 0; j < GetSizeX(); ++j) {
      std::cout << std::dec << static_cast<int16_t>(Get(i,j)) << "\t";
    }
    std::cout << std::endl;
  }

}
#endif // ZZCONTAINERSTDMAT_H
