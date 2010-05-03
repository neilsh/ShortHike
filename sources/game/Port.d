//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;

class Port
{
public:
  ~this() {}

  this(Vector position = Vector.ZERO, Quaternion orientation = Quaternion.IDENTITY)
  {
    setPosition(position);
    setOrientation(orientation);
  }


  bool
  canConnect(Port otherPort, Matrix myTransform, Matrix otherTransform)
  {
    const float MAGIC_PORT_ACCEPTANCE_DISTANCE = 0.1;

    //Only check positions for now ..
    Vector myPosition = position * myTransform;
    Vector otherPosition = otherPort.position * otherTransform;
    Vector displacement = myPosition - otherPosition;
    return cast(bool)(displacement.length() < MAGIC_PORT_ACCEPTANCE_DISTANCE);
  }


  //--------------------------------------------------------------------------------

  Vector
  getPosition()
  {
    return position;
  }

  Quaternion
  getOrientation()
  {
    return orientation;
  }


  void
  setPosition(Vector newPosition)
  {
    position = newPosition;
  }


  void
  setOrientation(Quaternion newOrientation)
  {
    orientation = newOrientation;
  }

  //--------------------------------------------------------------------------------

  Matrix
  getTransform()
  {
    return createMatrix(getPosition(), getOrientation());
  }


  void
  setTransform(Matrix transform)
  {
    setOrientation(transform.getOrientation());
    setPosition(transform.getPosition());
  }


private:
  Vector position;
  Quaternion orientation;
}
