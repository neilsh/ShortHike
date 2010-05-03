//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Dimension;
private import Point;
private import Value;
private import Library;

class Rect
{
  this() {setBounds(0, 0, 0, 0);}
  this(int x, int y, int w, int h) {setBounds(x, y, w, h);}
  this(int[4] values) {setBounds(values[0], values[1], values[2], values[3]); }
  
  int getX() {return x;}
  int getY() {return y;}
  int getWidth() {return w;}
  int getHeight() {return h;}
  
  Dimension getSize() {return new Dimension(w, h);}
  void setSize(int w, int h) {this.w = w; this.h = h;}
  void setSize(Dimension d) {setSize(d.w, d.h);}

  Point getPosition() {return new Point(x, y);}
  void setPosition(int xp, int yp) {x = xp; y = yp;}
  void setPosition(Point p) {x = p.x; y = p.y;}
  void translate(int dx, int dy) {x += dx; y += dy;}
    
  void setBounds(int x, int y, int w, int h) {this.x = x; this.y = y; this.h = h; this.w = w;}
  void setBounds(Point p, Dimension d) {setBounds(p.x, p.y, d.w, d.h);}


  bool contains(Point p) {return cast(bool)(p.x >= x && p.x < (x + w) && p.y >= y && p.y < (y + h));}

  // --------------------------------------------------------------------------------

  Rect
  intersection(Rect r1, Rect r2)
  {
    Rect res;

    int left = (r1.x > r2.x) ? r1.x : r2.x;
    int right = ((r1.x + r1.w) < (r2.x + r2.w)) ? (r1.x + r1.w) : (r2.x + r2.w);
    if (left > right) return res;

    int top = (r1.y > r2.y) ? r1.y : r2.y;
    int bottom = ((r1.y + r1.h) < (r2.y + r2.h)) ? (r1.y + r1.h) : (r2.y + r2.h);
    if (top > bottom) return res;

    res.x = left;
    res.w = right - left;
    res.y = top;
    res.h = bottom - top;
    return res;
  }

  // --------------------------------------------------------------------------------
  
  void
  flipX()
  {
    x = x + w;
    w = -w;
  }
  
  void
  flipY()
  {
    y = y + h;
    h = -h;
  }  

  // --------------------------------------------------------------------------------

  void
  addPoint(int pointX, int pointY)
  {
    if(pointX < x) {
      w += (x - pointX);
      x = pointX;
    }
    if((x + w) < pointX) {
      w = (pointX - x);
    }
    if(pointY < y) {
      h += (y - pointY);
      y = pointY;
    }
    if((y + h) < pointY) {
      h = (pointY - y);
    }  
  }

  int opEqual(Rect rhs)
  {
    //   return first.x == second.x && first.y == second.y && first.w == second.w && first.h == second.h;
    return 0;
  }

  void shrink(int pixels)
  {
    x += pixels;
    y += pixels;
    w -= 2 * pixels;
    h -= 2 * pixels;
  }
  
  void grow(int pixels)
  {
    x -= pixels;
    y -= pixels;
    w += 2 * pixels;
    h += 2 * pixels;
  }


  int x;
  int y;
  int w;
  int h;
};

// --------------------------------------------------------------------------------
// Custom ShortHike values

class RectValue : public Value
{
  this(Rect rect) {mRect = rect;}
  Type type()  {return Type.RECT;}
  Rect mRect;
};


  
Value
eRect(Environment env, Value[] args)
{
  assert(CHECK("Wrong number of arguments, 4 expected", args.length == 4));
  int[4] values;
  int i = 0;
  foreach(Value arg; args) {
    assert(CHECK("Numeric value", arg.isNumber()));
    values[i++] = arg.vInt();
  }
  return new RectValue(new Rect(values));
}

Value
eRectX(Environment env, Value[] args)
{
  assert(CHECK("Wrong number of arguments, 1 expected", args.length == 1));
  assert(CHECK("Rect expected as first argument", args[0].type() == Type.RECT));
  return new NumberValue((cast(RectValue)args[0]).mRect.x);
}

Value
eRectY(Environment env, Value[] args)
{
  assert(CHECK("Wrong number of arguments, 1 expected", args.length == 1));
  assert(CHECK("Rect expected as first argument", args[0].type() == Type.RECT));
  return new NumberValue((cast(RectValue)args[0]).mRect.y);
}

Value
eRectW(Environment env, Value[] args)
{
  assert(CHECK("Wrong number of arguments, 1 expected", args.length == 1));
  assert(CHECK("Rect expected as first argument", args[0].type() == Type.RECT));
  return new NumberValue((cast(RectValue)args[0]).mRect.w);
}

Value
eRectH(Environment env, Value[] args)
{
  assert(CHECK("Wrong number of arguments, 1 expected", args.length == 1));
  assert(CHECK("Rect expected as first argument", args[0].type() == Type.RECT));
  return new NumberValue((cast(RectValue)args[0]).mRect.h);
}

Value
eSetRectY(Environment env, Value[] args)
{
  assert(CHECK("Wrong number of arguments, 1 expected", args.length == 2));
  assert(CHECK("Rect expected as first argument", args[0].type() == Type.RECT));
  assert(CHECK("Number expected as second argument", args[1].isNumber()));
  Rect rect = (cast(RectValue)args[0]).mRect;
  rect.y = args[1].vInt;
  return new NumberValue(rect.y);
}


// std::ostream&
// operator<<(std::ostream& os, const Rect& r)
// {
//   os << "Rect [" << r.getX() << ", " << r.getY() << ", " << r.getWidth() << ", " << r.getHeight() << "]";
//   return os;
// }


