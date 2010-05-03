//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


class Dimension
{
public:
  this() {setSize(0, 0);}
  this(int width, int height) {setSize(width, height);}
  
  int getWidth() {return w;}
  int getHeight() {return h;}
  void setSize(int width, int height) {w = width; h = height;}
  

  int opEquals(Dimension rhs)
  {
    if(w == rhs.w) {
      if(h == rhs.h) {
        return 0;
      }
      if(h < rhs.h)
        return -1;
      else
        return 1;
    }
    if(w < rhs.w)
      return -1;
    else
      return 1;
  }

  int w, h;
}

// inline std::ostream&
// operator<<(std::ostream& os, const Dimension& d)
// {
//   os << "Dimension [" << d.getWidth() << ", " << d.getHeight() <<  "]";
//   return os;
// }

// inline bool
// operator==(Dimension& first, Dimension& second)
// {
//   return first.w == second.w && first.h == second.h;
// }



